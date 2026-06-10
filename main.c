#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.c"
#include "foggates.c"

#include "miniz.h"

#define COMPRESSION_ALLOC_SIZE 10*1024*1024

#define OUT_FOLDER_PATH "output/"


// [          FILE        ]
// [Header][ModelInfo][...]
//   16        16
//
// [        Header         ]
// [magic][ ][ ][ ][ ][ ][ ]
//    4    4  4  1  1  1  1
//
// [                              ModelInfo                             ]
// [Version][offset_count][name_offset][entry_offsets][next_param_offset]
//    4           4            8             8                8
//
// [                  Entry                         ]
// [name_offset][type][ ][id][ ][type_data_offset][ ]
//      8         1    1  2   4         8          4
//
//
// [Header]     = [4][4][4][1][1][1][1]
// [ModelInfo]  = [4][4][8][8][8]
// [Entry]      = [8][1][1][2][4][8][4]

const char* FORMAT = "MSB ";
const char* map_begin = NULL;

#define read_utf_16(sv) (utf16_t *)(sv)->data;

#define assert_item(type, sv, val)\
    do {\
        assert(*(type *)(sv)->data == (val));\
        (sv)->data += sizeof(type);\
    } while (0)\

#define _read_item(type, sv)        *(type *)(sv)->data
#define _read_item_ptr(type, sv)    (type *)(sv)->data
#define read_item(type, sv)         _read_item(type, sv);     (sv)->data += sizeof(type); (sv)->count -= sizeof(type);
#define read_item_ptr(type, sv, n)  _read_item_ptr(type, sv); (sv)->data += sizeof(type)*(n)

#define assert_val_ret(item, val) if ((item) != (val)) {nob_log(ERROR, "Invalid value"); return false;}
#define assert_val(item, val) if ((item) != (val)) {nob_log(ERROR, "Invalid value"); assert(false);}

#define VERSION 5

typedef enum {
	MT_MapPiece = 0,
	MT_Object = 1,
	MT_Collision = 3,
	MT_Navmesh = 4,
} ModelType;

typedef enum {
	ET_Light = 1,
	ET_Shadow = 2,
	ET_Fog = 3,
	ET_BGColor = 4,
	ET_MapOffset = 5,
	ET_Warp = 6,
	ET_CheapMode = 7,
} EventType;

typedef enum {
	RT_Region0 = 0,
	RT_Light = 3,
	RT_StartPoint = 5,
	RT_Sound = 7,
	RT_SFX = 9,
	RT_Wind = 13,
	RT_EnvLight = 14,
	RT_Fog = 15,
} RegionType;

typedef enum
{
	PT_MapPiece = 0,
	PT_Object = 1,
	PT_Collision = 3,
	PT_Navmesh = 4,
	PT_ConnectCollision = 5,
} PartType;

typedef packed_struct {
	varint name_offset;
	int8_t type;
	int8_t val3;
	int16_t id;
	int32_t val5;
	int64_t type_data_offset;
	varint val7;
} __Entry;

typedef struct {
	ModelType type;
	bool has_type_data;
	__Entry entry;
	utf16_t *name;
} Entry;

DA(Entry, Model);

void entry_copy(Entry *copy, const Entry *src, const char *buf, size_t size) {
	size_t str_len = wcslen(src->name);
	assert(size > str_len);
	wchar_t *dst = (wchar_t *)(buf);
	*copy = *src;
	assert(wcscpy_s(dst, size, src->name) == 0);
	copy->name = dst;
}

typedef packed_struct {
	int32_t version;
	int32_t offset_count;
	int64_t name_offset;
	varint *entry_offsets;
	varint next_param_offset;
} ParamSt;

typedef struct {
	ParamSt param;
	utf16_t *name;
    Model MapPieces;
    Model Objects;
    Model Collisions;
    Model Navmeshes;
} ModelParam;

typedef packed_struct {
	uint8_t r, g, b, a;
} Color;

typedef uint8_t byte;

#define LightPatternSize 0x3B
typedef packed_struct {
	byte UnkT00;
	byte val2; // 0
	int16_t val3; // -1
	float UnkT04;
	float UnkT08;
	Color ColorT0C;
	Color ColorT10;
	int32_t val8; // 0
	int32_t val9; // 0
	float UnkT1C;
	float UnkT20;
	Color ColorT24;
	Color ColorT28;
	int32_t val14; // 0
	int32_t val15; // 0
	Color ColorT34;
	Color ColorT38;
	Color ColorT3C;
	float UnkT40;
	byte UnkT44;
	char pattern[LightPatternSize]; //0x00
} Light;

#define ShadowPatternSize 0x18
typedef packed_struct {
	int32_t val1; //0
	float UnkT04;
	float UnkT08;
	float UnkT0C;
	int32_t val5; //0
	float UnkT14;
	float UnkT18;
	int32_t val8; //0
	float UnkT20;
	Color ColorT24;
	char pattern[ShadowPatternSize]; // 0x00
} Shadow;

#define FogPatternSize 0x11
typedef packed_struct {
	byte UnkT00;
	byte val2; //0
	byte val3; //0
	byte val4; //0
	Color ColorT04;
	float UnkT08;
	float UnkT0C;
	float UnkT10;
	byte UnkT14;
	byte UnkT15;
	byte UnkT16;
	char pattern[FogPatternSize]; // 0x00
} Fog;

#define BGColorPatternSize 0x24
typedef packed_struct {
	Color color;
	char pattern[BGColorPatternSize]; // 0x00
} BGColor;

static float normalize_angle(float angle) {
	angle = fmodf(angle, 360);
	if (angle < 0) {
		angle += 360;
	}
	return angle;
}

V3f v3f_flip_y(V3f rotation) {
	return (V3f) {
		rotation.x, 
		normalize_angle(rotation.y + 180),
		rotation.z
	};
}

V3f v3f_move(V3f position, V3f rotation, float distance) {
	float yaw = rotation.y * (float)(M_PI / 180); // Rotate around y-axis
	float x = (float)(sinf(yaw) * -distance);
	float z = (float)(cosf(yaw) * -distance);
	return (V3f){position.x + x, position.y, position.z + z};
}

V3f v3f_add(V3f a, V3f b) {
	return (V3f) { a.x + b.x, a.y + b.y, a.z + b.z};
}

V3f v3f_sub(V3f a, V3f b) {
	return (V3f) { a.x - b.x, a.y - b.y, a.z - b.z};
}

typedef packed_struct {
	V3f translation;
	int32_t degree; // 0
} MapOffset;

typedef packed_struct {
	byte UnkT00;
	byte val2; // 0
	byte val3; // 0
	byte val4; // 0
	V3f position;
} Warp;

#define CheapModePatternSize 0xE
typedef packed_struct {
	int16_t UnkT00;
	char pattern[CheapModePatternSize]; // 0x00
} CheapMode;

typedef packed_struct {
	varint name_offset;
	int32_t event_id;
	byte type;
	byte val4; // 0
	int16_t id;
	varint type_data_offset;
} __Event;

typedef struct {
	__Event event;
	utf16_t* name;
	EventType type;
	union {
		Light light;
		Shadow shadow;
		Fog fog;
		BGColor bgcolor;
		MapOffset map_offset;
		Warp warp;
		CheapMode cheap_mode;
	} type_data;
} Event;

DA(Event, Events);

typedef struct {
	ParamSt param;
	utf16_t *name;
	Events Lights;
	Events Shadows;
	Events Fogs;
	Events BGColors;
	Events MapOffsets;
	Events Warps;
	Events CheapModes;
} EventParam;

typedef uint32_t uint;

typedef enum {
	ST_None = 0xFFFFFFFF,
	ST_Point = 0,
	ST_Circle = 1,
	ST_Sphere = 2,
	ST_Cylinder = 3,
	ST_Rectangle = 4,
	ST_Box = 5,
	ST_Composite = 6,
} ShapeType;

bool has_shape_data(ShapeType type) {
	switch (type) {
	case ST_Point: return false;
	case ST_Circle:
	case ST_Sphere:
	case ST_Cylinder:
	case ST_Rectangle:
	case ST_Box:
	case ST_Composite: return true;
	case ST_None:
	default: UNREACHABLE("ShapeType");
	}
}

#define RegionPatternSize 0x24
typedef packed_struct { 
	int64_t name_offset;
	int16_t Unk08;
	byte type;
	byte shape_type;
	int16_t id;
	int16_t Unk0E;
	V3f position;
	V3f rotation;
	varint unkOffsetA;
	varint unkOffsetB;
	int32_t val11; // -1
	char pattern[RegionPatternSize]; // 0x00
	varint shape_data_offset;
	varint type_data_offset;
	int64_t val15; // 0
	int64_t val16; // 0
} __Region;

#define RegionLightPatternSize 0x10
#define RegionSoundPatternSize 0x14
#define RegionSFXPatternSize 0x18
#define RegionEnvLightPatternSize 0x14
#define RegionFogPatternSize 0x18
typedef union {
	packed_struct {
		int32_t UnkT00;
		Color ColorT04;
		Color ColorT08;
		float UnkT0C;
		char pattern[RegionLightPatternSize]; // 0x00
		int32_t val4; // 0
	} light;
	packed_struct {
		int32_t UnkT00;
		int32_t sound_id;
		int32_t UnkT08;
		char pattern[RegionSoundPatternSize]; // 0x00
	} sound;
	packed_struct {
		int32_t effect_id;
		int32_t UnkT04;
		char pattern[RegionSFXPatternSize]; // 0x00
	} sfx;
	packed_struct {
		int32_t UnkT00;
		float UnkT04;
		float UnkT08;
		float UnkT0C;
		float UnkT10;
		float UnkT14;
		float UnkT18;
		int32_t val9; // 0
	} wind;
	packed_struct {
		int32_t UnkT00;
		float   UnkT04;
		float   UnkT08;
		char pattern[RegionEnvLightPatternSize]; // 0x00
	} env_light;
	packed_struct {
		int32_t UnkT00;
		int32_t UnkT04;
		char pattern[RegionFogPatternSize]; // 0x00
		int32_t val4; // 0
	} fog;
} RegionTypeData;

typedef packed_struct {
	int32_t region_index;
	int32_t Unk04;
} Child;

#define N_SHAPE_CHILDREN 8
typedef struct {
	ShapeType type;
	bool has_shape_data;
	union {
		float radius;
		V2f v2;
		V3f v3;
		Child children[N_SHAPE_CHILDREN];
	};
} Shape;

typedef struct {
	__Region region;
	Shape shape;
	utf16_t *name;
	bool has_type_data;
	RegionTypeData type_data;
} Region;

DA(Region, Regions);

typedef struct {
	ParamSt param;
	utf16_t *name;
	Regions Regions;
	Regions Lights;
	Regions StartPoints;
	Regions Sounds;
	Regions SFXs;
	Regions Winds;
	Regions EnvLights;
	Regions Fogs;
} PointParam;

typedef struct {
	ParamSt param;
	utf16_t *name;
} RouteParam;

typedef RouteParam LayerParam;

#define PartCollisionPatternSize 0x10
#define PartNavmeshPatternSize 0x10
typedef packed_struct {
	varint name_offset;
	byte type;
	byte val3; //0
	int16_t id;
	int16_t model_index; // model_name is derived from model_index in MSB2.ModelParam.MapPieces.items[model_index].name
	int16_t val6; //0
	V3f position;
	V3f rotation;
	V3f scale;
	int32_t draw_groups[4];
	int32_t Unk44;
	int32_t Unk48;
	int32_t Unk4C;
	int32_t Unk50;
	int32_t disp_groups[4];
	int32_t Unk64;
	int32_t val16; //0
	byte Unk6C;
	byte val18; //0
	byte Unk6E;
	byte val20; //0
	varint type_data_offset;
	int64_t val22; //0
} __Part;

typedef union {
	packed_struct {
		int16_t UnkT00;
		byte UnkT02;
		byte val3; // 0
		int32_t val4; //0
	} map_piece;
	packed_struct {
		int32_t map_object_instance_param_id;
		int16_t UnkT04;
		int16_t val3; // 0
		int32_t val4; // 0
		int32_t val5; // 0
	} object;
	packed_struct {
		int32_t UnkT00;
		int32_t UnkT04;
		int32_t UnkT08;
		int32_t UnkT0C;
		byte UnkT10;
		byte UnkT11;
		byte UnkT12;
		byte UnkT13;
		byte UnkT14;
		byte UnkT15;
		byte val11; //0
		byte UnkT17;
		int32_t UnkT18;
		int32_t UnkT1C;
		int32_t UnkT20;
		int16_t val16; //0
		byte UnkT26;
		byte UnkT27;
		int32_t UnkT28;
		byte UnkT2C;
		byte val21; //0
		int16_t UnkT2E;
		int32_t UnkT30;
		byte val24; //0
		byte UnkT35;
		int16_t UnkT36;
		int32_t val27; //0
		int32_t UnkT3C;
		byte UnkT40;
		byte val30; //0
		byte val31; //0
		byte val32; //0
		int32_t UnkT44;
		char pattern[PartCollisionPatternSize]; // 0x00
	} collision;
	packed_struct {
		int32_t UnkT00;
		int32_t UnkT04;
		int32_t UnkT08;
		int32_t UnkT0C;
		char pattern[PartNavmeshPatternSize]; // 0x00
	} navmesh;
	packed_struct {
		int32_t collision_index;
		byte MapID;
		int32_t UnkT08;
		byte UnkT0C;
		byte val5; //0
		byte val6; //0
		byte val7; //0
	} connect_collision;
} PartTypeData;

typedef struct {
	__Part part;
	utf16_t* name;
	PartType type;
	PartTypeData type_data;
} Part;

void part_copy(Part *copy, const Part *src, const char *buf, size_t size) {
	size_t str_len = wcslen(src->name);
	assert(size > str_len);
	wchar_t *dst = (wchar_t *)(buf);
	*copy = *src;
	assert(wcscpy_s(dst, size, src->name) == 0);
	copy->name = dst;
}


DA(Part, Parts);

typedef struct {
	ParamSt param;
	utf16_t *name;
	Parts MapPieces;
	Parts Objects;
	Parts Collisions;
	Parts Navmeshes;
	Parts ConnectCollisions;
} PartParam;

typedef packed_struct {
	int32_t NameIndex;
	V3f Translation;
	V3f Rotation;
	V3f Scale;
} Bone;

DA(Bone, Bones);
DA(Bones, BoneGroup);

typedef packed_struct {
	int16_t PartIndex;
	int16_t boneCount;
	int32_t val3; // 0
	varint bonesOffset;
} PartPoseSt;

DA(PartPoseSt, PartPoseSts);

typedef struct {
	ParamSt param;
	utf16_t *name;
	PartPoseSts poses;
	BoneGroup bones;
} PartPoses;

DA(utf16_t*, Names);

typedef struct {
	ParamSt param;
	utf16_t *name;
	Names names;
} BoneNames;

typedef packed_struct {
	char magic[4]; // `MSB `
	int32_t val1;  // 1
	int32_t val2;  // 0x10
	bool val3;     // false
	bool val4;     // false
	char val5;     // 1
	char val6;     // (char)0xFF => -1
} Header;

bool assert_header(const char *map_path, Header* h) {
	char magic[5];
	for (int i=0; i<4; i++) {
		magic[i] = h->magic[i];
	}
	magic[4] = 0;
    if (strcmp(magic, FORMAT) != 0) {
        nob_log(ERROR, "Invalid file format for %s: Expected `%s` but got `%s`\n", map_path, FORMAT, h->magic);
        return false;
    }
	assert_val_ret(h->val1, 1);
	assert_val_ret(h->val2, 0x10);
	assert_val_ret(h->val3, false);
	assert_val_ret(h->val4, false);
	assert_val_ret(h->val5, 1);
	assert_val_ret(h->val6, (char)0xFF);
#ifdef LOG_GENERATED_FILES
    nob_log(INFO, "MSB Header verified");
#endif
	return true;
}

void print_model_type(ModelType type) {
	printf("Model Type = ");
	switch (type) {
	case MT_MapPiece: printf("MT_MapPiece"); break;
	case MT_Object: printf("MT_Object"); break;
	case MT_Collision: printf("MT_Collision"); break;
	case MT_Navmesh: printf("MT_Navmesh"); break;
	default: UNREACHABLE(temp_sprintf("Unknown Model_Type %d", type));
	}
	printf("\n");
}

void read_entry(String_View* sv, Entry *entry) {
	const char *start = sv->data;
	entry->entry = *(__Entry *)sv->data;
	assert_val(entry->entry.type, (int8_t)entry->type);
	assert_val(entry->entry.val3, 0);
	assert_val(entry->entry.val5, 0);
	assert_val(entry->entry.val7, 0);
	sv->data += entry->entry.name_offset;
	entry->name = (utf16_t *)sv->data;
	if (entry->has_type_data) {
		sv->data = start + entry->entry.type_data_offset;
		assert_item(varint, sv, 0);
	}
}

typedef struct {
	Header header;
	ModelParam models;
	EventParam events;
	PointParam regions;
	RouteParam routes;
	LayerParam layers;
	PartParam parts;
	PartPoses part_poses;
	BoneNames bone_names;
} MSB2;

typedef packed_struct {
	int32_t version; // 5
	int32_t offset_count;
	int64_t name_offset;
	varint *entry_offsets; // n = offset_count - 1
} ModalParamSTBegin;

ParamSt read_param_ex(String_View *sv, int32_t version) {
	ParamSt param = {0};
	param.version = read_item(int32_t, sv);
	assert(param.version == version);

    param.offset_count = read_item(int32_t, sv);
    param.name_offset  = read_item(int64_t, sv);

    param.entry_offsets = read_item_ptr(varint, sv, param.offset_count-1);
    param.next_param_offset = read_item(varint, sv);
	return param;
}

#define read_param(sv) read_param_ex((sv), VERSION)

utf16_t* get_param_name(const ParamSt *param) {
    return (utf16_t *)(map_begin + param->name_offset);
}

void* get_param_entry(const ParamSt *param, size_t i) {
	return (void*)(map_begin + param->entry_offsets[i]);
}

ModelParam read_model_param(String_View *sv) {
    ModelParam model = {0};
	model.param = read_param(sv);

    model.name = get_param_name(&model.param);
	utf16_t *required = L"MODEL_PARAM_ST";
	assert(wcscmp(model.name, required) == 0);

	for (int i=0; i<model.param.offset_count-1; i++) {
		Entry entry = {0};
		sv->data = get_param_entry(&model.param, i);
		ModelType model_type = *(int8_t *)(sv->data + sizeof(varint));
		entry.type = model_type;
		switch (model_type) {
		case MT_MapPiece: {
			entry.has_type_data = false;
			read_entry(sv, &entry);
			da_append(&model.MapPieces, entry);
	    } break;
		case MT_Object: {
			entry.has_type_data = true;
			read_entry(sv, &entry);
			da_append(&model.Objects, entry);
		} break;
		case MT_Collision: {
			entry.has_type_data = false;
			read_entry(sv, &entry);
			da_append(&model.Collisions, entry);
		} break;
		case MT_Navmesh: {
			entry.has_type_data = false;
			read_entry(sv, &entry);
			da_append(&model.Navmeshes, entry);
		} break;
		default: UNREACHABLE("Model_Type");
		}
	}
#ifdef LOG_GENERATED_FILES
	nob_log(INFO, "Read %zu map pieces.", model.MapPieces.count);
	nob_log(INFO, "Read %zu objects.", model.Objects.count);
	nob_log(INFO, "Read %zu collisions.", model.Collisions.count);
	nob_log(INFO, "Read %zu navmeshes.", model.Navmeshes.count);
#endif
	sv->data = map_begin + model.param.next_param_offset;

	return model;
}

__Event read_event(String_View *sv) {
	__Event event = *(__Event *)sv->data;
	assert(event.val4 == (byte)0);
	sv->data += sizeof(__Event);
	return event;
}

Event read_light(String_View *sv) {
	const char* start = sv->data;
	__Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	sv->data = start + event.type_data_offset;
	Light light = *(Light *)sv->data;
	assert(light.val2 == 0);
	assert(light.val3 == -1);
	assert(light.val8 == 0);
	assert(light.val9 == 0);
	assert(light.val14 == 0);
	assert(light.val15 == 0);
	for (size_t i=0; i<LightPatternSize; i+=1) {
		assert(light.pattern[i] == 0x00);
	}
	sv->data += sizeof(Light);
	return (Event) {
		.event = event,
		.name = name,
		.type_data.light = light,
	};
}

Event read_shadow(String_View *sv) {
	const char* start = sv->data;
	__Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	sv->data = start + event.type_data_offset;
	Shadow shadow = *(Shadow *)sv->data;
	assert(shadow.val1 == 0);
	assert(shadow.val5 == 0);
	assert(shadow.val8 == 0);
	for (size_t i=0; i<ShadowPatternSize; i+=1) {
		assert(shadow.pattern[i] == 0x00);
	}
	sv->data += sizeof(Shadow);
	return (Event) {
		.event = event,
		.name = name,
		.type_data.shadow = shadow,
	};
}

Event read_fog(String_View *sv) {
	const char* start = sv->data;
	__Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	sv->data = start + event.type_data_offset;
	Fog fog = *(Fog *)sv->data;
	assert(fog.val2 == 0);
	assert(fog.val3 == 0);
	assert(fog.val4 == 0);
	for (size_t i=0; i<FogPatternSize; i+=1) {
		assert(fog.pattern[i] == 0x00);
	}
	sv->data += sizeof(Fog);
	return (Event) {
		.event = event,
		.name = name,
		.type_data.fog = fog,
	};
}

Event read_bgcolor(String_View *sv) {
	const char* start = sv->data;
	__Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	sv->data = start + event.type_data_offset;
	BGColor bgcolor = *(BGColor *)sv->data;
	for (size_t i=0; i<BGColorPatternSize; i+=1) {
		assert(bgcolor.pattern[i] == 0x00);
	}
	sv->data += sizeof(BGColor);
	return (Event) {
		.event = event,
		.name = name,
		.type_data.bgcolor = bgcolor,
	};
}

Event read_map_offset(String_View *sv) {
	const char* start = sv->data;
	__Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	sv->data = start + event.type_data_offset;
	MapOffset map_offset = *(MapOffset *)sv->data;
	assert(map_offset.degree == 0);
	sv->data += sizeof(MapOffset);
	return (Event) {
		.event = event,
		.name = name,
		.type_data.map_offset = map_offset,
	};
}

Event read_warp(String_View *sv) {
	const char* start = sv->data;
	__Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	sv->data = start + event.type_data_offset;
	Warp warp = *(Warp *)sv->data;
	assert(warp.val2 == 0);
	assert(warp.val3 == 0);
	assert(warp.val4 == 0);
	sv->data += sizeof(Warp);
	return (Event) {
		.event = event,
		.name = name,
		.type_data.warp = warp,
	};
}

Event read_cheap_mode(String_View *sv) {
	const char* start = sv->data;
	__Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	sv->data = start + event.type_data_offset;
	CheapMode cheap_mode = *(CheapMode *)sv->data;
	for (size_t i=0; i<CheapModePatternSize; i+=1) {
		assert(cheap_mode.pattern[i] == 0);
	}
	sv->data += sizeof(CheapMode);
	return (Event) {
		.event = event,
		.name = name,
		.type_data.cheap_mode = cheap_mode,
	};
}

EventParam read_event_param(String_View *sv) {
	EventParam events = {0};
	ParamSt param = read_param(sv);
	events.param = param;
    utf16_t *name = get_param_name(&param);
	events.name = name;
	utf16_t *required = L"EVENT_PARAM_ST";
	assert(wcscmp(name, required) == 0);

	for (int i=0; i<param.offset_count-1; i++) {
		sv->data = get_param_entry(&param, i);
		EventType event_type = *(int8_t *)(sv->data + sizeof(varint) + 4);
		switch (event_type) {
	    case ET_Light: {
			Event light = read_light(sv);
			light.type = event_type;
			da_append(&events.Lights, light);
		} break;
	    case ET_Shadow: {
			Event shadow = read_shadow(sv);
			shadow.type = event_type;
			da_append(&events.Shadows, shadow);
		} break;
	    case ET_Fog: {
			Event fog = read_fog(sv);
			fog.type = event_type;
			da_append(&events.Fogs, fog);
		} break;
	    case ET_BGColor: {
			Event bgcolor = read_bgcolor(sv);
			bgcolor.type = event_type;
			da_append(&events.BGColors, bgcolor);
		} break;
	    case ET_MapOffset: {
			Event map_offset = read_map_offset(sv);
			map_offset.type = event_type;
			da_append(&events.MapOffsets, map_offset);
		} break;
	    case ET_Warp: {
			Event warp = read_warp(sv);
			warp.type = event_type;
			da_append(&events.Warps, warp);
		} break;
	    case ET_CheapMode: {
			Event cheap_mode = read_cheap_mode(sv);
			cheap_mode.type = event_type;
			da_append(&events.CheapModes, cheap_mode);
		} break;
		default: UNREACHABLE("model_type");
		}
	}
#ifdef LOG_GENERATED_FILES
	nob_log(INFO, "Read %zu Events.Lights", events.Lights.count);
	nob_log(INFO, "Read %zu Events.Shadows", events.Shadows.count);
	nob_log(INFO, "Read %zu Events.Fogs", events.Fogs.count);
	nob_log(INFO, "Read %zu Events.BGColors", events.BGColors.count);
	nob_log(INFO, "Read %zu Events.MapOffsets", events.MapOffsets.count);
	nob_log(INFO, "Read %zu Events.Warps", events.Warps.count);
	nob_log(INFO, "Read %zu Events.CheapModes", events.CheapModes.count);
#endif
	sv->data = map_begin + param.next_param_offset;
	return events;
}

Child new_child(String_View *sv) {
	Child child = *(Child *)sv->data;
	sv->data += sizeof(Child);
	return child;
}

Shape create_shape(ShapeType type) {
	switch (type) {
	case ST_Point: {
		return (Shape) {
			.type = type,
			.has_shape_data = has_shape_data(type),
		};
	} break;
	case ST_Circle: {
		return (Shape) {
			.type = type,
			.has_shape_data = has_shape_data(type),
			.radius = 1.0f,
		};
	} break;
	case ST_Sphere: {
		return (Shape) {
			.type = type,
			.has_shape_data = has_shape_data(type),
			.radius = 1.0f,
		};
	} break;
	case ST_Cylinder: {
		return (Shape) {
			.type = type,
			.has_shape_data = has_shape_data(type),
			.v2 = (V2f){1.0f, 1.0f},
		};
	} break;
	case ST_Rectangle: {
		return (Shape) {
			.type = type,
			.has_shape_data = has_shape_data(type),
			.v2 = (V2f){1.0f, 1.0f},
		};
	} break;
	case ST_Box: {
		return (Shape) {
			.type = type,
			.has_shape_data = has_shape_data(type),
			.v3 = (V3f){1.0f, 1.0f, 1.0f},
		};
	} break;
	case ST_Composite: {
		Shape shape = {0};
		shape.type = type;
		shape.has_shape_data = has_shape_data(type);
		return shape;
	} break;
	case ST_None:
	default: UNREACHABLE("ShapeType");
	}
}

void read_shape_data(Shape* shape, String_View *sv) {
	switch (shape->type) {
	case ST_Point: return;
	case ST_Circle: 
	case ST_Sphere: {
		shape->radius = read_item(float, sv);
	} break;
	case ST_Rectangle:
	case ST_Cylinder: {
		shape->v2.x = read_item(float, sv);
		shape->v2.y = read_item(float, sv);
	} break;
	case ST_Box: {
		shape->v3.x = read_item(float, sv);
		shape->v3.y = read_item(float, sv);
		shape->v3.z = read_item(float, sv);
	} break;
	case ST_Composite: {
		for (size_t i=0; i<N_SHAPE_CHILDREN; i++) {
			shape->children[i] = new_child(sv);
		}
	} break;
	case ST_None:
	default: UNREACHABLE("ShapeType");
	}
}

RegionTypeData read_regiontype_data(String_View *sv, RegionType type) {
	RegionTypeData data = {0};
	switch (type) {
	case RT_Light: {
		memcpy(&data.light, sv->data, sizeof(data.light));
		for (size_t i=0; i<RegionLightPatternSize; i+=1) {
			assert(data.light.pattern[i] == 0x00);
		}
		assert(data.light.val4 == 0);
	} break;
	case RT_Sound: {
		memcpy(&data.sound, sv->data, sizeof(data.sound));
		for (size_t i=0; i<RegionSoundPatternSize; i+=1) {
			assert(data.sound.pattern[i] == 0x00);
		}
	} break;
	case RT_SFX: {
		memcpy(&data.sfx, sv->data, sizeof(data.sfx));
		for (size_t i=0; i<RegionSFXPatternSize; i+=1) {
			assert(data.sfx.pattern[i] == 0x00);
		}
	} break;
	case RT_Wind: {
		memcpy(&data.wind, sv->data, sizeof(data.wind));
		assert(data.wind.val9 == 0);
	} break;
	case RT_EnvLight: {
		memcpy(&data.env_light, sv->data, sizeof(data.env_light));
		for (size_t i=0; i<RegionEnvLightPatternSize; i+=1) {
			assert(data.env_light.pattern[i] == 0x00);
		}
	} break;
	case RT_Fog: {
		memcpy(&data.fog, sv->data, sizeof(data.fog));
		for (size_t i=0; i<RegionFogPatternSize; i+=1) {
			assert(data.fog.pattern[i] == 0x00);
		}
		assert(data.fog.val4 == 0);
	} break;
	case RT_StartPoint:
	case RT_Region0:
	default: UNREACHABLE("RegionType");
	}
	return data;
}

__Region read_region_(String_View *sv) {
	const char *start = sv->data;
	__Region region = *(__Region *)sv->data;
	assert(region.val11 == -1);
	for (size_t i=0; i<RegionPatternSize; i+=1) {
		assert(region.pattern[i] == 0x00);
	}
	assert(region.val15 == 0);
	assert(region.val16 == 0);
	sv->data += sizeof(__Region);

	assert(region.name_offset != 0);
	assert(*(int32_t *)(start + region.unkOffsetA) == 0);
	assert(*(int32_t *)(start + region.unkOffsetB) == 0);

	// TODO: this assertion is incorrect
	// assert((shape.has_shape_data^region.shape_data_offset) == 0);

	return region;
}

Region read_region(String_View *sv, bool has_type_data) {
	const char *start = sv->data;
	Region region;
	region.has_type_data = has_type_data;
	region.region = read_region_(sv);

	// NOTE: this assertion is incorrect
	// assert((has_type_data^region.type_data_offset) == 0);

	region.shape = create_shape(region.region.shape_type);
	region.name = (utf16_t *)(start + region.region.name_offset);

	if (region.shape.has_shape_data) {
		sv->data = start + region.region.shape_data_offset;
		read_shape_data(&region.shape, sv);
	}

	if (has_type_data) {
		sv->data = start + region.region.type_data_offset;
		region.type_data = read_regiontype_data(sv, region.region.type);
	}

	return region;
}

PointParam read_point_param(String_View *sv) {
	PointParam regions = {0};
	ParamSt param = read_param(sv);
	regions.param = param;
    utf16_t *name = get_param_name(&param);
	regions.name = name;
	utf16_t *required = L"POINT_PARAM_ST";
	assert(wcscmp(name, required) == 0);
	for (int i=0; i<param.offset_count-1; i++) {
		sv->data = get_param_entry(&param, i);
		RegionType region_type = *(byte *)(sv->data + sizeof(varint) + 2);
		switch (region_type) {
		case RT_Region0: {
			Region region = read_region(sv, false);
			da_append(&regions.Regions, region);
		} break; 
		case RT_Light: {
			Region light = read_region(sv, true);
			da_append(&regions.Lights, light);
		} break; 
		case RT_StartPoint: {
			Region start_point = read_region(sv, false);
			da_append(&regions.StartPoints, start_point);
		} break; 
		case RT_Sound: {
			Region sound = read_region(sv, true);
			da_append(&regions.Sounds, sound);
		} break; 
		case RT_SFX: {
			Region sfx = read_region(sv, true);
			da_append(&regions.SFXs, sfx);
		} break; 
		case RT_Wind: {
			Region wind = read_region(sv, true);
			da_append(&regions.Winds, wind);
		} break; 
		case RT_EnvLight: {
			Region env_light = read_region(sv, true);
			da_append(&regions.EnvLights, env_light);
		} break; 
		case RT_Fog: {
			Region fog = read_region(sv, true);
			da_append(&regions.Fogs, fog);
		} break; 
		default: UNREACHABLE("region_type");
		}
	}
#ifdef LOG_GENERATED_FILES
	nob_log(INFO, "Read %zu Regions.Region", regions.Regions.count);
	nob_log(INFO, "Read %zu Regions.Light", regions.Lights.count);
	nob_log(INFO, "Read %zu Regions.StartPoint", regions.StartPoints.count);
	nob_log(INFO, "Read %zu Regions.Sound", regions.Sounds.count);
	nob_log(INFO, "Read %zu Regions.SFX", regions.SFXs.count);
	nob_log(INFO, "Read %zu Regions.Wind", regions.Winds.count);
	nob_log(INFO, "Read %zu Regions.EnvLight", regions.EnvLights.count);
	nob_log(INFO, "Read %zu Regions.Fog", regions.Fogs.count);
#endif
	sv->data = map_begin + param.next_param_offset;
	return regions;
}

RouteParam read_route_param(String_View *sv) {
	RouteParam route = {0};
	route.param = read_param(sv);
    utf16_t *name = get_param_name(&route.param);
	route.name = name;
	utf16_t *required = L"ROUTE_PARAM_ST";
	assert(wcscmp(name, required) == 0);
	for (int i=0; i<route.param.offset_count-1; i++) {
		UNREACHABLE("route params are empty in ds2");
	}
	sv->data = map_begin + route.param.next_param_offset;
	return route;
}

LayerParam read_layer_param(String_View *sv) {
	LayerParam layer = {0};
	ParamSt param = read_param(sv);
	layer.param = param;
    utf16_t *name = get_param_name(&param);
	layer.name = name;
	utf16_t *required = L"LAYER_PARAM_ST";
	assert(wcscmp(name, required) == 0);
	for (int i=0; i<param.offset_count-1; i++) {
		UNREACHABLE("layer params are empty in ds2");
	}
	sv->data = map_begin + param.next_param_offset;
	return layer;
}

PartTypeData read_part_type_data(String_View *sv, PartType type) {
	PartTypeData data = {0};
	switch (type) {
	case PT_MapPiece: {
		memcpy(&data.map_piece, sv->data, sizeof(data.map_piece));
	} break;
	case PT_Object: {
		memcpy(&data.object, sv->data, sizeof(data.object));
	} break;
	case PT_Collision: {
		memcpy(&data.collision, sv->data, sizeof(data.collision));
	} break;
	case PT_Navmesh: {
		memcpy(&data.navmesh, sv->data, sizeof(data.navmesh));
	} break;
	case PT_ConnectCollision: {
		memcpy(&data.connect_collision, sv->data, sizeof(data.connect_collision));
	} break;
	default: UNREACHABLE("part_type");
	}
	return data;
}

Part new_part(String_View *sv, PartType type) {
	const char *start = sv->data;
	Part part = {0};
	part.type = type;
	part.part = *(__Part *)sv->data;
	assert(part.part.name_offset != 0);
	assert(part.part.type_data_offset != 0);
	sv->data = start + part.part.name_offset;
	part.name = (utf16_t *)sv->data;
	sv->data = start + part.part.type_data_offset;
	part.type_data = read_part_type_data(sv, type);
	return part;
}

PartParam read_parts_param(String_View *sv) {
	PartParam parts = {0};
	ParamSt param = read_param(sv);
	parts.param = param;
    utf16_t *name = get_param_name(&param);
	parts.name = name;
	utf16_t *required = L"PARTS_PARAM_ST";
	assert(wcscmp(name, required) == 0);
	for (int i=0; i<param.offset_count-1; i++) {
		sv->data = get_param_entry(&param, i);
		PartType part_type = *(byte *)(sv->data + sizeof(varint));
		Part part = new_part(sv, part_type);
		switch (part_type) {
		case PT_MapPiece: {
			da_append(&parts.MapPieces, part);
		} break;
		case PT_Object: {
			da_append(&parts.Objects, part);
		} break;
		case PT_Collision: {
			da_append(&parts.Collisions, part);
		} break;
		case PT_Navmesh: {
			da_append(&parts.Navmeshes, part);
		} break;
		case PT_ConnectCollision: {
			da_append(&parts.ConnectCollisions, part);
		} break;
		default: UNREACHABLE("region_type");
		}
	}
#ifdef LOG_GENERATED_FILES
	nob_log(INFO, "Read %zu MapPiece", parts.MapPieces.count);
	nob_log(INFO, "Read %zu Object", parts.Objects.count);
	nob_log(INFO, "Read %zu Collision", parts.Collisions.count);
	nob_log(INFO, "Read %zu Navmesh", parts.Navmeshes.count);
	nob_log(INFO, "Read %zu ConnectCollision", parts.ConnectCollisions.count);
#endif
	sv->data = map_begin + param.next_param_offset;
	return parts;
}

Bone new_bone(String_View *sv) {
	return *(Bone *)sv->data;
}

PartPoses read_part_poses(String_View *sv) {
	PartPoses pp = {0};
	ParamSt param = read_param_ex(sv, 0);
	pp.param = param;
    utf16_t *name = get_param_name(&param);
	pp.name = name;
	utf16_t *required = L"MAPSTUDIO_PARTS_POSE_ST";
	assert(wcscmp(name, required) == 0);
	for (int i=0; i<param.offset_count-1; i++) {
		sv->data = get_param_entry(&param, i);
		const char *start = sv->data;
		PartPoseSt pose = *(PartPoseSt *)sv->data;
		assert(pose.val3 == 0);
		da_append(&pp.poses, pose);
		sv->data = start + pose.bonesOffset;
		Bones bones = {0};
		for (int j = 0; j < pose.boneCount; j++) {
			Bone bone = new_bone(sv);
			da_append(&bones, bone);
			sv->data += sizeof(Bone);
		}
		da_append(&pp.bones, bones);
	}
#ifdef LOG_GENERATED_FILES
	nob_log(INFO, "Read %zu PartPoses", pp.bones.count);
#endif
	sv->data = map_begin + param.next_param_offset;
	return pp;
}

BoneNames read_bone_names(String_View *sv) {
	BoneNames names = {0};
	ParamSt param = read_param_ex(sv, 0);
	names.param = param;
    utf16_t *name = get_param_name(&param);
	names.name = name;
	utf16_t *required = L"MAPSTUDIO_BONE_NAME_STRING";
	assert(wcscmp(name, required) == 0);
	for (int i=0; i<param.offset_count-1; i++) {
		sv->data = get_param_entry(&param, i);
		utf16_t *name = (utf16_t *)sv->data;
		da_append(&names.names, name);
	}
#ifdef LOG_GENERATED_FILES
	nob_log(INFO, "Read %zu BoneNames", names.count);
#endif
	sv->data = map_begin + param.next_param_offset;
	return names;
}


void free_models(ModelParam *param) {
	da_free(param->MapPieces);
	da_free(param->Navmeshes);
	da_free(param->Collisions);
	da_free(param->Objects);
}

void free_events(EventParam *param) {
	da_free(param->BGColors);
	da_free(param->CheapModes);
	da_free(param->Fogs);
	da_free(param->Lights);
	da_free(param->MapOffsets);
	da_free(param->Shadows);
	da_free(param->Warps);
}

void free_regions(PointParam *param) {
	da_free(param->Lights);
	da_free(param->Fogs);
	da_free(param->EnvLights);
	da_free(param->Regions);
	da_free(param->SFXs);
	da_free(param->Sounds);
	da_free(param->Winds);
	da_free(param->StartPoints);
}

void free_parts(PartParam *param) {
	da_free(param->Objects);
	da_free(param->Collisions);
	da_free(param->Navmeshes);
	da_free(param->MapPieces);
	da_free(param->ConnectCollisions);
}

void free_part_poses(PartPoses *param) {
	da_free(param->poses);
	for (size_t i=0; i<param->bones.count; i++) {
		da_free(param->bones.items[i]);
	}
	da_free(param->bones);
}

void free_bone_names(BoneNames *param) {
	da_free(param->names);
}

void free_map(MSB2 *entries) {
	free_models(&entries->models);
	free_events(&entries->events);
	free_regions(&entries->regions);
	free_parts(&entries->parts);
	free_part_poses(&entries->part_poses);
	free_bone_names(&entries->bone_names);
}

#define map_path(name) "./map/" name "/" name ".msb"
#define eventlocation_path(name) "./Param/eventlocation_" name ".param"
#define eventparam_path(name) "./Param/eventparam_" name ".param"
#define map_object_instance_param_path(name) "./Param/mapobjectinstanceparam_" name ".param"
#define esd_path(name) "./ezstate/event_" name ".esd"

MSB2 load_map(const char* map_path, String_View map_sv) {
	Header h = *(Header *)map_sv.data;
	map_begin = map_sv.data;
	if (!assert_header(map_path, &h)) {
		exit(1);
	}
	map_sv.data += sizeof(Header);

	MSB2 map = {0};
	map.header = h;
	map.models = read_model_param(&map_sv);
	map.events = read_event_param(&map_sv);
	map.regions = read_point_param(&map_sv);
	map.routes = read_route_param(&map_sv);
	map.layers = read_layer_param(&map_sv);
	map.parts = read_parts_param(&map_sv);
	map.part_poses = read_part_poses(&map_sv);
	map.bone_names = read_bone_names(&map_sv);

	assert(map_sv.data == map_begin);

	// TODO: maybe this is unnecessary if the model names are unique
	// disambiguate_model_names(&map.models);

	return map;
}

void write_header(String_Builder *sb, Header *h) {
	size_t length = sizeof(Header);
	nob_sb_append_buf(sb, h, length);
}

void __sb_write(String_Builder *sb, void *data, size_t length) {
	nob_sb_append_buf(sb, data, length);
}

#define sb_write(sb, data) __sb_write((sb), &(data), sizeof(data));

void sb_patch_varint(String_Builder *sb, size_t position, varint data) {
	size_t cur_pos = sb->count;
	sb->count = position;
	sb_write_varint(sb, data);
	sb->count = cur_pos;
}

void sb_patch_int64_t(String_Builder *sb, size_t position, int64_t data) {
	size_t cur_pos = sb->count;
	sb->count = position;
	sb_write_int64t(sb, data);
	sb->count = cur_pos;
}

void sb_patch_uint32t(String_Builder *sb, size_t position, uint32_t data) {
	size_t cur_pos = sb->count;
	sb->count = position;
	sb_write_uint32t(sb, data);
	sb->count = cur_pos;
}

void sb_patch_int32t(String_Builder *sb, size_t position, int32_t data) {
	size_t cur_pos = sb->count;
	sb->count = position;
	sb_write_int32t(sb, data);
	sb->count = cur_pos;
}


void sb_patch_int64t(String_Builder *sb, size_t position, int64_t data) {
	size_t cur_pos = sb->count;
	sb->count = position;
	sb_write_int64t(sb, data);
	sb->count = cur_pos;
}

void sb_patch_int16t(String_Builder *sb, size_t position, int16_t data) {
	size_t cur_pos = sb->count;
	sb->count = position;
	sb_write_int16t(sb, data);
	sb->count = cur_pos;
}

void write_model_type_data(String_Builder *sb, ModelType type) {
	switch (type) {
		case MT_Object: sb_write_varint(sb, 0); break;
		default:
			UNREACHABLE("Not Implemented");
	}
}

void write_event_type_data(String_Builder *sb, Event *event) {
	switch (event->type) {
	case ET_Light: {
		size_t length = sizeof(Light);
		sb_append_buf(sb, &event->type_data.light, length);
	} break;
	case ET_Shadow: {
		size_t length = sizeof(Shadow);
		sb_append_buf(sb, &event->type_data.shadow, length);
	} break;
	case ET_Fog: {
		size_t length = sizeof(Fog);
		sb_append_buf(sb, &event->type_data.fog, length);
	} break;
	case ET_BGColor: {
		size_t length = sizeof(BGColor);
		sb_append_buf(sb, &event->type_data.bgcolor, length);
	} break;
	case ET_MapOffset: {
		size_t length = sizeof(MapOffset);
		sb_append_buf(sb, &event->type_data.map_offset, length);
	} break;
	case ET_Warp: {
		size_t length = sizeof(Warp);
		sb_append_buf(sb, &event->type_data.warp, length);
	} break;
	case ET_CheapMode: {
		size_t length = sizeof(CheapMode);
		sb_append_buf(sb, &event->type_data.cheap_mode, length);
	} break;
	default: UNREACHABLE("Not Implemented");
	}
}

void sb_append_sint32(String_Builder *sb, int32_t value) {
	size_t length = sizeof(value);
	sb_append_buf(sb, &value, length);
}

void sb_append_float(String_Builder *sb, float value) {
	size_t length = sizeof(float);
	sb_append_buf(sb, &value, length);
}

#define sb_pad sb_reserve
void write_model(String_Builder *sb, Model *model, DArrz *offset_positions, size_t *entries_count) {
	for (size_t i=0; i<model->count; i++) {
		size_t begin = sb->count;
		assert(*entries_count < offset_positions->count);
		size_t param_pos = offset_positions->items[*entries_count];
		sb_patch_varint(sb, param_pos, (varint)sb->count); // add the location of param

		size_t name_offset = sb->count;
		sb_reserve(sb, sizeof(varint)); // name_offset


		size_t length = offsetof(__Entry, id) - offsetof(__Entry, type);
		sb_append_buf(sb, &model->items[i].entry.type, length);

		int16_t id = i;
		length = sizeof(int16_t);
		sb_append_buf(sb, &id, length);

		sb_append_sint32(sb, model->items[i].entry.val5);

		size_t type_data_offset = sb->count;
		sb_reserve(sb, sizeof(varint)); // type_data_offset
		sb_write_varint(sb, 0);

		sb_patch_varint(sb, name_offset, sb->count - begin);
		sb_append_utf16t(sb, model->items[i].name);
		sb_pad_align(sb, sizeof(varint));

		if (model->items[i].has_type_data) {
			sb_patch_varint(sb, type_data_offset, sb->count - begin);
			write_model_type_data(sb, model->items[i].type);
		} else {
			sb_patch_varint(sb, type_data_offset, 0);
		}

		sb_pad_align(sb, sizeof(varint));
		(*entries_count)++;
	}
}

void write_events(String_Builder *sb, Events *model, DArrz *offset_positions, size_t *entries_count) {
	for (size_t i=0; i<model->count; i++) {
		size_t begin = sb->count;
		assert(*entries_count < offset_positions->count);
		size_t param_pos = offset_positions->items[*entries_count];
		sb_patch_varint(sb, param_pos, (varint)sb->count); // add the location of param

		size_t name_offset = sb->count;
		sb_reserve(sb, sizeof(varint)); // name_offset

		size_t length = offsetof(__Event, id) - offsetof(__Event, event_id);
		nob_sb_append_buf(sb, &model->items[i].event.event_id, length);

		int16_t id = i;
		length = sizeof(int16_t);
		sb_append_buf(sb, &id, length);

		size_t type_data_offset = sb->count;
		sb_reserve(sb, sizeof(varint)); // type_data_offset

		sb_patch_varint(sb, name_offset, sb->count - begin);
		sb_append_utf16t(sb, model->items[i].name);
		sb_pad_align(sb, sizeof(varint));

		sb_patch_varint(sb, type_data_offset, sb->count - begin);
		write_event_type_data(sb, &model->items[i]);

		sb_pad_align(sb, sizeof(varint));
		(*entries_count)++;
	}
}

void write_regions(String_Builder *sb, Regions *model, DArrz *offset_positions, size_t *entries_count) {
	for (size_t i=0; i<model->count; i++) {
		size_t begin = sb->count;
		assert(*entries_count < offset_positions->count);
		size_t param_pos = offset_positions->items[*entries_count];
		sb_patch_varint(sb, param_pos, (varint)sb->count); // add the location of param

		size_t name_offset = sb->count;
		sb_reserve(sb, sizeof(varint)); // name_offset

		size_t length = offsetof(__Region, id) - offsetof(__Region, Unk08);
		nob_sb_append_buf(sb, &model->items[i].region.Unk08, length);

		int16_t id = (int16_t)i;
		length = sizeof(int16_t);
		sb_append_buf(sb, &id, length);

		length = offsetof(__Region, unkOffsetA) - offsetof(__Region, Unk0E);
		nob_sb_append_buf(sb, &model->items[i].region.Unk0E, length);

		size_t unkOffsetA = sb->count;
		sb_reserve(sb, sizeof(varint));
		size_t unkOffsetB = sb->count;
		sb_reserve(sb, sizeof(varint));

		length = offsetof(__Region, shape_data_offset) - offsetof(__Region, val11);
		nob_sb_append_buf(sb, &model->items[i].region.val11, length);

		size_t shape_data_offset = sb->count;
		sb_reserve(sb, sizeof(varint));
		size_t type_data_offset = sb->count;
		sb_reserve(sb, sizeof(varint)); // type_data_offset

		sb_write_int64t(sb, 0);
		sb_write_int64t(sb, 0);

		sb_patch_varint(sb, name_offset, sb->count - begin);
		sb_append_utf16t(sb, model->items[i].name);
		sb_pad_align(sb, 4);

		sb_patch_varint(sb, unkOffsetA, sb->count - begin);
		sb_write_int32t(sb, 0);
		sb_patch_varint(sb, unkOffsetB, sb->count - begin);
		sb_write_int32t(sb, 0);
		sb_pad_align(sb, sizeof(varint));

		if (model->items[i].shape.has_shape_data) {
			sb_patch_varint(sb, shape_data_offset, sb->count - begin);

			switch (model->items[i].shape.type) {
			case ST_Circle: 
			case ST_Sphere:
				sb_append_float(sb, model->items[i].shape.radius);
				break;
			case ST_Cylinder: 
			case ST_Rectangle:
				sb_append_float(sb, model->items[i].shape.v2.x);
				sb_append_float(sb, model->items[i].shape.v2.y);
				break;
			case ST_Box: 
				sb_append_float(sb, model->items[i].shape.v3.x);
				sb_append_float(sb, model->items[i].shape.v3.y);
				sb_append_float(sb, model->items[i].shape.v3.z);
				break;
			case ST_Composite:
				length = sizeof(model->items[i].shape.children);
				sb_append_buf(sb, model->items[i].shape.children, length);
				break;
			case ST_None:
			case ST_Point:
			default: UNREACHABLE("this ShapeType should not have shape data!!");
			}
		} else {
			sb_patch_varint(sb, shape_data_offset, 0);
		}

		if (model->items[i].has_type_data) {
			sb_patch_varint(sb, type_data_offset, sb->count - begin);
			switch((RegionType)model->items[i].region.type) {
			case RT_Light:
				length = sizeof(model->items[i].type_data.light);
				sb_append_buf(sb, &model->items[i].type_data.light, length);
			break;
			case RT_Sound:
				length = sizeof(model->items[i].type_data.sound);
				sb_append_buf(sb, &model->items[i].type_data.sound, length);
			break;
			case RT_SFX: 
				length = sizeof(model->items[i].type_data.sfx);
				sb_append_buf(sb, &model->items[i].type_data.sfx, length);
			break;
			case RT_Wind: 
				length = sizeof(model->items[i].type_data.wind);
				sb_append_buf(sb, &model->items[i].type_data.wind, length);
			break;
			case RT_EnvLight: 
				length = sizeof(model->items[i].type_data.env_light);
				sb_append_buf(sb, &model->items[i].type_data.env_light, length);
			break;
			case RT_Fog: 
				length = sizeof(model->items[i].type_data.fog);
				sb_append_buf(sb, &model->items[i].type_data.fog, length);
			break;
			case RT_Region0: 
			case RT_StartPoint: 
			default: UNREACHABLE("this RegionType should not have type data!!");
			}
		} else {
			sb_patch_varint(sb, type_data_offset, 0);
		}

		sb_pad_align(sb, sizeof(varint));
		(*entries_count)++;
	}
}

void write_parts(String_Builder *sb, Parts *parts, DArrz *offset_positions, size_t *entries_count) {
	for (size_t i=0; i<parts->count; i++) {
		size_t begin = sb->count;
		assert(*entries_count < offset_positions->count);
		size_t param_pos = offset_positions->items[*entries_count];
		sb_patch_varint(sb, param_pos, (varint)sb->count); // add the location of param

		size_t name_offset = sb->count;
		sb_reserve(sb, sizeof(varint)); // name_offset

		size_t length = offsetof(__Part, id) - offsetof(__Part, type);
		nob_sb_append_buf(sb, &parts->items[i].part.type, length);

		int16_t id = (int16_t)i;
		length = sizeof(int16_t);
		sb_append_buf(sb, &id, length);

		length = offsetof(__Part, type_data_offset) - offsetof(__Part, model_index);
		nob_sb_append_buf(sb, &parts->items[i].part.model_index, length);

		size_t type_data_offset = sb->count;
		sb_reserve(sb, sizeof(varint)); // type_data_offset

		sb_write_int64t(sb, parts->items[i].part.val22);

		size_t name_start = sb->count;
		int name_pad = 0x20;
		sb_patch_varint(sb, name_offset, sb->count - begin);
		sb_append_utf16t(sb, parts->items[i].name);

		for (size_t j=0; j<name_start+name_pad-sb->count; j++) {
			sb_append(sb, 0);
		}

		sb_pad_align(sb, sizeof(varint));

		sb_patch_varint(sb, type_data_offset, sb->count - begin);

		switch((PartType)parts->items[i].type) {
		case PT_MapPiece: 
			length = sizeof(parts->items[i].type_data.map_piece);
			sb_append_buf(sb, &parts->items[i].type_data.map_piece, length);
			break;
		case PT_Object: 
			length = sizeof(parts->items[i].type_data.object);
			sb_append_buf(sb, &parts->items[i].type_data.object, length);
			break;
		case PT_Collision: 
			length = sizeof(parts->items[i].type_data.collision);
			sb_append_buf(sb, &parts->items[i].type_data.collision, length);
			break;
		case PT_Navmesh: 
			length = sizeof(parts->items[i].type_data.navmesh);
			sb_append_buf(sb, &parts->items[i].type_data.navmesh, length);
			break;
		case PT_ConnectCollision: 
			length = sizeof(parts->items[i].type_data.connect_collision);
			sb_append_buf(sb, &parts->items[i].type_data.connect_collision, length);
			break;
		default: UNREACHABLE("PartType");
		}

		sb_pad_align(sb, sizeof(varint));
		(*entries_count)++;
	}
}

size_t write_param_header(String_Builder *sb, ParamSt *p, utf16_t *name, DArrz *offset_positions) {
	size_t length = offsetof(ParamSt, name_offset);
	nob_sb_append_buf(sb, p, length);
	size_t name_offset = sb->count;
	sb_write(sb, p->name_offset);

	for (int i=0; i<p->offset_count-1; i++) {
		da_append(offset_positions, sb->count);
		sb_reserve(sb, sizeof(varint));
	}
	size_t next_param_offset = sb->count;
	sb_write(sb, p->next_param_offset);

	sb_patch_varint(sb, name_offset, (varint)sb->count);
	sb_append_utf16t(sb, name);
	sb_pad_align(sb, sizeof(varint));
	return next_param_offset;
}

void write_model_param(String_Builder *sb, ModelParam *param) {
	DArrz offset_positions = {0};
	size_t total_entries = param->MapPieces.count + param->Objects.count + param->Collisions.count + param->Navmeshes.count;
	param->param.offset_count = total_entries + 1;
	size_t next_param_offset = write_param_header(sb, &param->param, param->name, &offset_positions);

	size_t entries_count = 0;
	write_model(sb, &param->MapPieces, &offset_positions, &entries_count);
	write_model(sb, &param->Objects, &offset_positions, &entries_count);
	write_model(sb, &param->Collisions, &offset_positions, &entries_count);
	write_model(sb, &param->Navmeshes, &offset_positions, &entries_count);
	sb_patch_varint(sb, next_param_offset, sb->count);
	da_free(offset_positions);
}

void write_event_param(String_Builder *sb, EventParam *param) {
	DArrz offset_positions = {0};
	size_t next_param_offset = write_param_header(sb, &param->param, param->name, &offset_positions);

	size_t entries_count = 0;
	write_events(sb, &param->Lights, &offset_positions, &entries_count);
	write_events(sb, &param->Shadows, &offset_positions, &entries_count);
	write_events(sb, &param->Fogs, &offset_positions, &entries_count);
	write_events(sb, &param->BGColors, &offset_positions, &entries_count);
	write_events(sb, &param->MapOffsets, &offset_positions, &entries_count);
	write_events(sb, &param->Warps, &offset_positions, &entries_count);
	write_events(sb, &param->CheapModes, &offset_positions, &entries_count);
	sb_patch_varint(sb, next_param_offset, sb->count);
	da_free(offset_positions);
}

void write_region_param(String_Builder *sb, PointParam *param) {
	DArrz offset_positions = {0};
	size_t next_param_offset = write_param_header(sb, &param->param, param->name, &offset_positions);

	size_t entries_count = 0;
	write_regions(sb, &param->Regions, &offset_positions, &entries_count);
	write_regions(sb, &param->Lights, &offset_positions, &entries_count);
	write_regions(sb, &param->StartPoints, &offset_positions, &entries_count);
	write_regions(sb, &param->Sounds, &offset_positions, &entries_count);
	write_regions(sb, &param->SFXs, &offset_positions, &entries_count);
	write_regions(sb, &param->Winds, &offset_positions, &entries_count);
	write_regions(sb, &param->EnvLights, &offset_positions, &entries_count);
	write_regions(sb, &param->Fogs, &offset_positions, &entries_count);
	sb_patch_varint(sb, next_param_offset, sb->count);
	da_free(offset_positions);
}

void write_route_param(String_Builder *sb, RouteParam *param) {
	DArrz offset_positions = {0};
	size_t next_param_offset = write_param_header(sb, &param->param, param->name, &offset_positions);
	sb_patch_varint(sb, next_param_offset, sb->count);
	da_free(offset_positions);
}

void write_layer_param(String_Builder *sb, LayerParam *param) {
	DArrz offset_positions = {0};
	size_t next_param_offset = write_param_header(sb, &param->param, param->name, &offset_positions);
	sb_patch_varint(sb, next_param_offset, sb->count);
	da_free(offset_positions);
}

void write_parts_param(String_Builder *sb, PartParam *param) {
	DArrz offset_positions = {0};
	size_t total_entries = param->MapPieces.count + param->Objects.count + param->Collisions.count +\
						   param->Navmeshes.count + param->ConnectCollisions.count;
	param->param.offset_count = total_entries + 1;
	size_t next_param_offset = write_param_header(sb, &param->param, param->name, &offset_positions);

	size_t entries_count = 0;
	write_parts(sb, &param->MapPieces, &offset_positions, &entries_count);
	write_parts(sb, &param->Objects, &offset_positions, &entries_count);
	write_parts(sb, &param->Collisions, &offset_positions, &entries_count);
	write_parts(sb, &param->Navmeshes, &offset_positions, &entries_count);
	write_parts(sb, &param->ConnectCollisions, &offset_positions, &entries_count);
	sb_patch_varint(sb, next_param_offset, sb->count);

	da_free(offset_positions);
}

void write_part_poses(String_Builder *sb, PartPoses *param) {
	DArrz offset_positions = {0};
	size_t next_param_offset = write_param_header(sb, &param->param, param->name, &offset_positions);

	size_t entries_count = 0;

	assert(param->poses.count == param->bones.count);

	for (size_t i=0; i<param->poses.count; i++) {
		size_t begin = sb->count;
		assert(entries_count < offset_positions.count);
		size_t param_pos = offset_positions.items[entries_count];
		sb_patch_varint(sb, param_pos, (varint)sb->count); // add the location of param

		PartPoseSt *poses = &param->poses.items[i];
		Bones *bones = &param->bones.items[i];

		size_t length = offsetof(PartPoseSt, bonesOffset);
		sb_append_buf(sb, poses, length);

		size_t bonesOffset = sb->count;
		sb_reserve(sb, sizeof(varint)); // bonesOffset

		sb_patch_varint(sb, bonesOffset, sb->count - begin);

		length = sizeof(Bone);
		for (size_t j=0; j<bones->count; j++) {
			Bone *bone = &bones->items[j];
			sb_append_buf(sb, bone, length);
		}

		sb_pad_align(sb, sizeof(varint));
		entries_count++;
	}
	sb_patch_varint(sb, next_param_offset, sb->count);
	da_free(offset_positions);
}

void write_bone_names(String_Builder *sb, BoneNames *names) {
	DArrz offset_positions = {0};
	size_t next_param_offset = write_param_header(sb, &names->param, names->name, &offset_positions);

	size_t entries_count = 0;

	for (size_t i=0; i<names->names.count; i++) {
		assert(entries_count < offset_positions.count);
		size_t param_pos = offset_positions.items[entries_count];
		sb_patch_varint(sb, param_pos, (varint)sb->count); // add the location of param

		sb_append_utf16t(sb, names->names.items[i]);

		sb_pad_align(sb, sizeof(varint));
		entries_count++;
	}
	sb_patch_varint(sb, next_param_offset, 0);
	da_free(offset_positions);
}


bool write_map(String_Builder *sb, MSB2 *entries) {
	write_header(sb, &entries->header);
	write_model_param(sb, &entries->models);
	write_event_param(sb, &entries->events);
	write_region_param(sb, &entries->regions);
	write_route_param(sb, &entries->routes);
	write_layer_param(sb, &entries->layers);
	write_parts_param(sb, &entries->parts);
	write_part_poses(sb, &entries->part_poses);
	write_bone_names(sb, &entries->bone_names);

	// Immediate checking
	String_View sv = sb_to_sv(*sb);
	Header h = *(Header *)sv.data;
	map_begin = sv.data;
	if (!assert_header("", &h)) {
		return false;
	}
	sv.data += sizeof(Header);

	MSB2 map = {0};
	map.models = read_model_param(&sv);
	map.events = read_event_param(&sv);
	map.regions = read_point_param(&sv);
	map.routes = read_route_param(&sv);
	map.layers = read_layer_param(&sv);
	map.parts = read_parts_param(&sv);
	map.part_poses = read_part_poses(&sv);
	map.bone_names = read_bone_names(&sv);

	free_map(&map);

	return true;
}

int test_loading_all_maps() {
	const char *map_names[] = {
		map_path("m10_02_00_00"),
		map_path("m10_04_00_00"),
		map_path("m10_10_00_00"),
		map_path("m10_14_00_00"),
		map_path("m10_15_00_00"),
		map_path("m10_16_00_00"),
		map_path("m10_17_00_00"),
		map_path("m10_18_00_00"),
		map_path("m10_19_00_00"),
		map_path("m10_23_00_00"),
		map_path("m10_25_00_00"),
		map_path("m10_27_00_00"),
		map_path("m10_29_00_00"),
		map_path("m10_30_00_00"),
		map_path("m10_31_00_00"),
		map_path("m10_32_00_00"),
		map_path("m10_33_00_00"),
		map_path("m10_34_00_00"),
		map_path("m20_10_00_00"),
		map_path("m20_11_00_00"),
		map_path("m20_21_00_00"),
		map_path("m20_24_00_00"),
		map_path("m20_26_00_00"),
		map_path("m40_03_00_00"),
		map_path("m50_35_00_00"),
		map_path("m50_36_00_00"),
		map_path("m50_37_00_00"),
		map_path("m50_38_00_00"),
	};
	String_Builder sb = {0};
	String_Builder rsb = {0};
	for (size_t i=0; i<ARRAY_LEN(map_names); i++) {
		sb.count = 0;
		rsb.count = 0;
		const char *map_path = map_names[i];
		if (!nob_read_entire_file(map_path, &sb)) {
			return 1;
		}
		printf("Read %zu bytes from %s\n", sb.count, map_path);
		String_View sv = sb_to_sv(sb);

		MSB2 map = load_map(map_path, sv);

		if (!write_map(&rsb, &map)) {
			return 1;
		}

		assert(sb.count == rsb.count);
		for (size_t i=0; i<sb.count; i++) {
			assert(sb.items[i] == rsb.items[i]);
		}

		free_map(&map);
	}
	da_free(sb);
	da_free(rsb);
	return 0;
}

bool is_dcx(String_View *sv) {
	if (sv->count < 4) return false;
	char magic[5];
	memcpy(magic, sv->data, 4);
	magic[4] = 0;
	if (strcmp(magic, "DCP\0") == 0 || strcmp(magic, "DCX\0") == 0) return true;
	return false;
}

// byte-sized
typedef enum {
	FF1_None = 0,
	FF1_Flag01 = 1<<0,
	FF1_IntDataOffset = 1<<1, /// Expanded header with 32-bit data offset.
	FF1_LongDataOffset = 1<<2, /// Expanded header with 64-bit data offset.
	FF1_Flag08 = 1<<3, /// Unused?
	FF1_Flag10 = 1<<4, /// Unused?
	FF1_Flag20 = 1<<5, /// Unused?
	FF1_Flag40 = 1<<6, /// Unused?
	FF1_OffsetParamType = 1<<7, /// Param type string is written separately instead of fixed-width in the header.
} FormatFlags1;

// byte-sized
typedef enum {
	FF2_None = 0,
	FF2_UnicodeRowNames = 1<<0, /// Row names are written as UTF-16.
	FF2_Flag02 = 1<<1, /// Unknown.
	FF2_Flag04 = 1<<2, /// Unknown.
	FF2_Flag08 = 1<<3, /// Unused?
	FF2_Flag10 = 1<<4, /// Unused?
	FF2_Flag20 = 1<<5, /// Unused?
	FF2_Flag40 = 1<<6, /// Unused?
	FF2_Flag80 = 1<<7, /// Unused?
} FormatFlags2;

byte sv_read_byte(String_View *sv) {
	assert(sv->count > 0);
	byte b = *sv->data;
	sv->data += sizeof(byte);
	sv->count--;
	return b;
}

typedef enum {
	Str_Char,
	Str_Wide,
	Str_JIS,
} StringType;

typedef struct {
	StringType type;
	union {
		const char *str_char;
		utf16_t *str_wide;
		const char *str_jis;
	} as;
} String;

typedef struct {
	int32_t ID;
	int32_t garbage;
	int64_t DataOffset;
	int64_t nameOffset;
	String Name;
} Row;

DA(Row, Rows);

typedef packed_struct {
	byte BigEndian;
	byte Format2D; //enum FormatFlags1
	byte Format2E; //enum FormatFlags1
	byte ParamdefFormatVersion;
} PARAM_Format;

typedef packed_struct {
	V3f position;
	V3f rotation;
	byte region_type, Unk19, Unk1A, Unk1B;
	V3f scale;
	uint16_t Unk28, Unk2A;
	uint32_t Unk2C;
} EventLocPARAMDEF;

typedef packed_struct {
	int32_t event_id;
	int32_t flag_id;
	uint8_t  Unk08, Unk09, Unk0A, Unk0B, Unk0C, Unk0D, Unk0E, Unk0F;
} EventParamPARAMDEF;

typedef packed_struct {
	int32_t headerID;
	int32_t ParamID;
	int32_t ObjectBullet;
	float Unk02, Unk03;
	int32_t MapObjectInstanceType ;
	uint16_t Unk04;
	uint8_t  Unk05, Unk06, Unk07, Unk08, DefaultState, Unk09;
	uint32_t Unk10;
	int32_t ItemlotID;
} MapObjectInstancePARAMDEF;

typedef packed_struct {
	int32_t unk00;
	float entry_delay;
	int32_t unk01, unk02, key_guide_id_1, key_guide_id_2, unk04, unk05;
} MapObjectWhiteDoorParamPARAMDEF;

typedef packed_struct {
	uint8_t lots_drop_order;
	uint8_t lots_dropped;
	uint8_t unk02;
	uint8_t unk03;
	uint8_t amount[10];
	uint8_t reinforcement_level[10];
	uint8_t infusion[10];
	uint8_t infinite_drop[10];
	uint32_t item_id[10];
	float chance[10];
} ItemLotParam2ChrPARAMDEF;

typedef packed_struct {
	int32_t item_id;
	int32_t unk00;
	int32_t enable_flag;
	int32_t disable_flag;
	int32_t material_id;
	int32_t duplicate_item_id;
	int32_t unk01;
	float price_rate;
	int32_t quantity;
} ShopLineupPARAMDEF;

typedef enum {
	Cell_EventLoc,
	Cell_EventParam,
	Cell_MapObjInstanceParam,
	Cell_MapObjectWhiteDoorParam,
	Cell_ItemLotParam2_Chr,
	Cell_ShopLineupParam,
} CellType;

typedef struct {
	union {
		EventLocPARAMDEF event_loc;
		EventParamPARAMDEF event_param;
		MapObjectInstancePARAMDEF map_obj_instance_param;
		MapObjectWhiteDoorParamPARAMDEF map_obj_white_door_param;
		ItemLotParam2ChrPARAMDEF itemlot_param2_char;
		ShopLineupPARAMDEF shop_lineup_param;
	} as;
} Cell;
typedef struct {
	CellType type;
	Cell *items;
	size_t count;
	size_t capacity;
} Cells;

typedef struct {
	String_View data;
	PARAM_Format format;
	long actualStringsOffset;
	uint32_t stringsOffset;
	int16_t Unk06;
	int16_t ParamdefDataVersion;
	uint16_t rowCount;
	String ParamType;
	Rows rows;
	size_t DetectedSize;
	Cells cells;
} PARAM;


Row new_row(String_View *sv, const char *start, PARAM *param) {
	Row row = {0};
	row.ID = read_item(int32_t, sv);
	if (param->format.Format2D & FF1_LongDataOffset) {
		row.garbage = read_item(int32_t, sv);
		row.DataOffset = read_item(int64_t, sv);
		row.nameOffset = read_item(int64_t, sv);
	} else {
		row.DataOffset = (int64_t)read_item(uint32_t, sv);
		row.nameOffset = (int64_t)read_item(uint32_t, sv);
	}

	if (row.nameOffset != 0) {
		if (param->actualStringsOffset == 0 || row.nameOffset < param->actualStringsOffset) {
			param->actualStringsOffset = row.nameOffset;
		}

		if (param->format.Format2E & FF2_UnicodeRowNames) {
			row.Name.type = Str_Wide;
			row.Name.as.str_wide = (utf16_t *)(start + row.nameOffset);
		} else {
			row.Name.type = Str_JIS;
			row.Name.as.str_jis = (char *)(start + row.nameOffset);
		}
	}

	return row;
}

void PARAM_apply_paramdef(PARAM *param, CellType type) {
	Cells cells = {0};
	cells.type = type;
	da_foreach(Row, row, &param->rows) {
		if (row->DataOffset == 0) continue;
		String_View sv = param->data;
		sv.data += row->DataOffset;
		Cell cell = {0};
		switch (type) {
			case Cell_EventLoc:
				cell.as.event_loc = *(EventLocPARAMDEF *)(sv.data);
			break;
			case Cell_EventParam:
				cell.as.event_param = *(EventParamPARAMDEF *)(sv.data);
			break;
			case Cell_MapObjInstanceParam:
				cell.as.map_obj_instance_param = *(MapObjectInstancePARAMDEF *)(sv.data);
			break;
			case Cell_MapObjectWhiteDoorParam:
				cell.as.map_obj_white_door_param = *(MapObjectWhiteDoorParamPARAMDEF *)(sv.data);
			break;
			case Cell_ItemLotParam2_Chr:
				cell.as.itemlot_param2_char = *(ItemLotParam2ChrPARAMDEF *)(sv.data);
			break;
			case Cell_ShopLineupParam:
				cell.as.shop_lineup_param = *(ShopLineupPARAMDEF *)(sv.data);
			break;
		default: TODO("CellType enum not implemented");
		}
		da_append(&cells, cell);
	}
	param->cells = cells;
}

#define PARAM_FORMAT_POSITION 0x2C
PARAM load_param(String_View *sv, CellType cell_type) {
	PARAM param = {0};
	param.data = *sv;

	if (is_dcx(sv)) {
		TODO("Not implemented!");
	}

	const char *start = sv->data;
	assert(sv->count > PARAM_FORMAT_POSITION);
	sv->data += PARAM_FORMAT_POSITION;

	memcpy(&param.format, sv->data, sizeof(PARAM_Format));
	assert(param.format.BigEndian == 0xFF || param.format.BigEndian == 0);
	sv->data = start;

	// The strings offset in the header is highly unreliable; only use it as a last resort
	param.actualStringsOffset = 0;
	param.stringsOffset = read_item(uint32_t, sv);
	if (((param.format.Format2D & FF1_Flag01) && (param.format.Format2D & FF1_IntDataOffset)) || (param.format.Format2D & FF1_LongDataOffset)) {
		assert_item(int16_t, sv, 0);
	} else { 
		read_item(uint16_t, sv); //data_start
	}
	param.Unk06 = read_item(int16_t, sv);
	param.ParamdefDataVersion = read_item(int16_t, sv);
	param.rowCount = read_item(uint16_t, sv);
	if (param.format.Format2D & FF1_OffsetParamType) {
		assert_item(int32_t, sv, 0);
		int64_t paramTypeOffset = read_item(int64_t, sv);
#define PARAM_PATTERN_SIZE 0x14
		for (size_t i=0; i<PARAM_PATTERN_SIZE; i++) {
			assert_item(byte, sv, 0x00);
		}
		param.ParamType.type = Str_Wide;
		param.ParamType.as.str_wide = (utf16_t *)(start + paramTypeOffset);
		param.actualStringsOffset = paramTypeOffset;
	} else {
#define PARAM_TYPE_NAME_LENGTH 0x20
		param.ParamType.type = Str_Char;
		param.ParamType.as.str_char = sv->data;
		sv->data += PARAM_TYPE_NAME_LENGTH;
	}
	sv->data += sizeof(PARAM_Format); // Skip Format
	if (param.format.Format2D & FF1_Flag01 && param.format.Format2D & FF1_IntDataOffset) {
		read_item(int32_t, sv); // data_start
		assert_item(int32_t, sv, 0);
		assert_item(int32_t, sv, 0);
		assert_item(int32_t, sv, 0);
	} else if (param.format.Format2D & FF1_LongDataOffset) {
		read_item(int64_t, sv); // data_start
		assert_item(int64_t, sv, 0);
	}

	da_reserve(&param.rows, (size_t)param.rowCount);
	for (int i = 0; i < param.rowCount; i++) {
		Row row = new_row(sv, start, &param);
		da_append(&param.rows, row);
	}
	if (param.rows.count > 1) {
		param.DetectedSize = param.rows.items[1].DataOffset - param.rows.items[0].DataOffset;
	} else if (param.rows.count == 1) {
		param.DetectedSize = (param.actualStringsOffset == 0 ? param.stringsOffset : param.actualStringsOffset) - param.rows.items[0].DataOffset;
	} else {
		param.DetectedSize = -1;
	}

	PARAM_apply_paramdef(&param, cell_type);
	return param;
}

void free_PARAM(PARAM *param) {
	da_free(param->rows);
	if (param->cells.items) {
		da_free(param->cells);
	}
}

void write_param(String_Builder *sb, PARAM *param) {
	assert(param->cells.items != NULL);

	Ht(const char *, uint32_t) ht = {
        .hasheq = ht_cstr_hasheq,
    };

	size_t stringsOffset = sb->count;
	sb_reserve(sb, sizeof(int32_t)); // stringsOffset

	size_t data_start;
	if (((param->format.Format2D & FF1_Flag01) && (param->format.Format2D & FF1_IntDataOffset)) || (param->format.Format2D & FF1_LongDataOffset)) {
		sb_write_int16t(sb, 0);
	} else {
		data_start = sb->count;
		sb_reserve(sb, sizeof(uint16_t)); // data_start
	}
	sb_write_int16t(sb, param->Unk06);
	sb_write_int16t(sb, param->ParamdefDataVersion);
	sb_write_uint16t(sb, (uint16_t)param->rows.count);

	size_t param_type_offset = 0;
	if (param->format.Format2D & FF1_OffsetParamType) {
		sb_write_int32t(sb, 0);
		param_type_offset = sb->count;
		sb_reserve(sb, sizeof(int64_t)); //param_type_offset
		for (size_t i=0; i<PARAM_PATTERN_SIZE; i++) {
			sb_append(sb, 0x00);
		}
		TODO("maybe add something here");
	} else {
		assert(param->ParamType.type == Str_Char);
		for (size_t i=0; i<PARAM_TYPE_NAME_LENGTH; ++i) {
			sb_append(sb, param->ParamType.as.str_char[i]);
		}
	}
	size_t length = sizeof(PARAM_Format);
	sb_append_buf(sb, &param->format, length);

	if (param->format.Format2D & FF1_Flag01 && param->format.Format2D & FF1_IntDataOffset) {
		data_start = sb->count;
		sb_reserve(sb, sizeof(uint32_t));
		sb_write_int32t(sb, 0);
		sb_write_int32t(sb, 0);
		sb_write_int32t(sb, 0);
	} else if (param->format.Format2D & FF1_LongDataOffset) {
		data_start = sb->count;
		sb_reserve(sb, sizeof(int64_t));
		sb_write_int64t(sb, 0);
	}

	DArrz row_offsets = {0};
	DArrz name_offsets = {0};
	for (size_t i = 0; i < param->rows.count; i++) {
		Row *row = &param->rows.items[i];
		sb_write_int32t(sb, row->ID);
		if (param->format.Format2D & FF1_LongDataOffset) {
			sb_write_int32t(sb, 0);
			da_append(&row_offsets, sb->count);
			sb_reserve(sb, sizeof(int64_t));
			da_append(&name_offsets, sb->count);
			sb_reserve(sb, sizeof(int64_t));
		} else {
			da_append(&row_offsets, sb->count);
			sb_reserve(sb, sizeof(int32_t));
			da_append(&name_offsets, sb->count);
			sb_reserve(sb, sizeof(int32_t));
		}
	}

	if (param->format.Format2D == FF1_Flag01) {
		for (size_t i=0; i<0x20; i++) {
			sb_append(sb, 0x00);
		}
	}

	if (param->format.Format2D & FF1_Flag01 && param->format.Format2D & FF1_IntDataOffset) {
		sb_patch_uint32t(sb, data_start, (uint32_t)sb->count);
	} else if (param->format.Format2D & FF1_LongDataOffset) {
		sb_patch_int64t(sb, data_start, (int64_t)sb->count);
	} else {
		sb_patch_int16t(sb, data_start, (int16_t)sb->count);
	}

	assert(param->rows.count == param->cells.count);
	for (size_t i = 0; i < param->rows.count; i++) {
		Cell *cell = &param->cells.items[i];

		if (param->format.Format2D & FF1_LongDataOffset)
			sb_patch_int64t(sb, row_offsets.items[i], (int64_t)sb->count);
		else {
			sb_patch_uint32t(sb, row_offsets.items[i], (uint32_t)sb->count);
		}

		switch (param->cells.type) {
		case Cell_EventLoc:
			length = sizeof(EventLocPARAMDEF);
		break;
		case Cell_EventParam:
			length = sizeof(EventParamPARAMDEF);
		break;
		case Cell_MapObjInstanceParam:
			length = sizeof(MapObjectInstancePARAMDEF);
		break;
		case Cell_MapObjectWhiteDoorParam:
			length = sizeof(MapObjectWhiteDoorParamPARAMDEF);
		break;
		case Cell_ItemLotParam2_Chr:
			length = sizeof(ItemLotParam2ChrPARAMDEF);
		break;
		case Cell_ShopLineupParam:
			length = sizeof(ShopLineupPARAMDEF);
		break;
		default: TODO("CellType enum not implemented");
		}
		sb_append_buf(sb, cell, length);
	}

	sb_patch_uint32t(sb, stringsOffset, (uint32_t)sb->count);

	if (param->format.Format2D & FF1_OffsetParamType) {
		sb_patch_int64_t(sb, param_type_offset, (int64_t)sb->count);
		assert(param->ParamType.type == Str_Wide);
		sb_append_utf16t(sb, param->ParamType.as.str_wide);
	}

	assert(param->rows.count == name_offsets.count);
	for (size_t i = 0; i < param->rows.count; i++) {
		Row *row = &param->rows.items[i];
		size_t offset = name_offsets.items[i];

		if (row->Name.as.str_char != 0) {
			if (param->format.Format2E & FF2_UnicodeRowNames) {
				uint32_t *data = ht_find(&ht, row->Name.as.str_char);
				if (data == NULL) {
					*ht_put(&ht, row->Name.as.str_char) = (uint32_t)sb->count;
					sb_patch_int64_t(sb, offset, (int64_t)sb->count);
					sb_append_utf16t(sb, row->Name.as.str_wide);
				} else {
					sb_patch_int64_t(sb, offset, (int64_t)(*data));
				}
			} else {
				TODO("NOT Implemented WriteShiftJIS");
			}
		}
	}
	sb_pad_align(sb, 16);

	da_free(name_offsets);
	da_free(row_offsets);
	ht_free(&ht);
}

int test_loading_all_eventlocations() {
	const char *paths[] = {
		eventlocation_path("m10_02_00_00"),
		eventlocation_path("m10_04_00_00"),
		eventlocation_path("m10_10_00_00"),
		eventlocation_path("m10_14_00_00"),
		eventlocation_path("m10_15_00_00"),
		eventlocation_path("m10_16_00_00"),
		eventlocation_path("m10_17_00_00"),
		eventlocation_path("m10_18_00_00"),
		eventlocation_path("m10_19_00_00"),
		eventlocation_path("m10_23_00_00"),
		eventlocation_path("m10_25_00_00"),
		eventlocation_path("m10_27_00_00"),
		eventlocation_path("m10_29_00_00"),
		eventlocation_path("m10_30_00_00"),
		eventlocation_path("m10_31_00_00"),
		eventlocation_path("m10_32_00_00"),
		eventlocation_path("m10_33_00_00"),
		eventlocation_path("m10_34_00_00"),
		eventlocation_path("m20_10_00_00"),
		eventlocation_path("m20_11_00_00"),
		eventlocation_path("m20_21_00_00"),
		eventlocation_path("m20_24_00_00"),
		eventlocation_path("m20_26_00_00"),
		eventlocation_path("m40_03_00_00"),
		eventlocation_path("m50_35_00_00"),
		eventlocation_path("m50_36_00_00"),
		eventlocation_path("m50_37_00_00"),
		eventlocation_path("m50_38_00_00"),
	};

	String_Builder sb = {0};
	String_Builder wsb = {0};
	for (size_t i=0; i<ARRAY_LEN(paths); i++) {
		sb.count = 0;
		wsb.count = 0;

		const char *path = paths[i];
		if (!nob_read_entire_file(path, &sb)) {
			return 1;
		}
		printf("Read %zu bytes from %s\n", sb.count, path);
		String_View sv = sb_to_sv(sb);

		PARAM param = load_param(&sv, Cell_EventLoc);

		write_param(&wsb, &param);

		for (size_t j=0; j<wsb.count; j++) {
			if(wsb.items[j] != sb.items[j] && j<(size_t)param.actualStringsOffset) asm("int3");
		}
		if (wsb.count != sb.count) asm("int3");

		String_View wsv = sb_to_sv(wsb);
		PARAM param_reloaded = load_param(&wsv, Cell_EventLoc);


		free_PARAM(&param_reloaded);
		free_PARAM(&param);
	}
	da_free(sb);
	da_free(wsb);

	return 0;
}

int test_loading_all_eventparams() {
	const char *paths[] = {
		eventparam_path("m10_02_00_00"),
		eventparam_path("m10_04_00_00"),
		eventparam_path("m10_10_00_00"),
		eventparam_path("m10_14_00_00"),
		eventparam_path("m10_15_00_00"),
		eventparam_path("m10_16_00_00"),
		eventparam_path("m10_17_00_00"),
		eventparam_path("m10_18_00_00"),
		eventparam_path("m10_19_00_00"),
		eventparam_path("m10_23_00_00"),
		eventparam_path("m10_25_00_00"),
		eventparam_path("m10_27_00_00"),
		eventparam_path("m10_29_00_00"),
		eventparam_path("m10_30_00_00"),
		eventparam_path("m10_31_00_00"),
		eventparam_path("m10_32_00_00"),
		eventparam_path("m10_33_00_00"),
		eventparam_path("m10_34_00_00"),
		eventparam_path("m20_10_00_00"),
		eventparam_path("m20_11_00_00"),
		eventparam_path("m20_21_00_00"),
		eventparam_path("m20_24_00_00"),
		eventparam_path("m20_26_00_00"),
		eventparam_path("m40_03_00_00"),
		eventparam_path("m50_35_00_00"),
		eventparam_path("m50_36_00_00"),
		eventparam_path("m50_37_00_00"),
		eventparam_path("m50_38_00_00"),
	};

	String_Builder sb = {0};
	String_Builder wsb = {0};
	for (size_t i=0; i<ARRAY_LEN(paths); i++) {
		sb.count = 0;
		wsb.count = 0;

		const char *path = paths[i];
		if (!nob_read_entire_file(path, &sb)) {
			return 1;
		}
		printf("Read %zu bytes from %s\n", sb.count, path);
		String_View sv = sb_to_sv(sb);

		PARAM param = load_param(&sv, Cell_EventParam);

		write_param(&wsb, &param);

		for (size_t j=0; j<wsb.count; j++) {
			if(wsb.items[j] != sb.items[j] && j<(size_t)param.actualStringsOffset) asm("int3");
		}
		if (wsb.count != sb.count) asm("int3");

		String_View wsv = sb_to_sv(wsb);
		PARAM param_reloaded = load_param(&wsv, Cell_EventParam);


		free_PARAM(&param_reloaded);
		free_PARAM(&param);
	}
	da_free(sb);
	da_free(wsb);

	return 0;
}

int test_loading_all_mapobjinstanceparams() {
	const char *paths[] = {
		map_object_instance_param_path("m10_02_00_00"),
		map_object_instance_param_path("m10_04_00_00"),
		map_object_instance_param_path("m10_10_00_00"),
		map_object_instance_param_path("m10_14_00_00"),
		map_object_instance_param_path("m10_15_00_00"),
		map_object_instance_param_path("m10_16_00_00"),
		map_object_instance_param_path("m10_17_00_00"),
		map_object_instance_param_path("m10_18_00_00"),
		map_object_instance_param_path("m10_19_00_00"),
		map_object_instance_param_path("m10_23_00_00"),
		map_object_instance_param_path("m10_25_00_00"),
		map_object_instance_param_path("m10_27_00_00"),
		map_object_instance_param_path("m10_29_00_00"),
		map_object_instance_param_path("m10_30_00_00"),
		map_object_instance_param_path("m10_31_00_00"),
		map_object_instance_param_path("m10_32_00_00"),
		map_object_instance_param_path("m10_33_00_00"),
		map_object_instance_param_path("m10_34_00_00"),
		map_object_instance_param_path("m20_10_00_00"),
		map_object_instance_param_path("m20_11_00_00"),
		map_object_instance_param_path("m20_21_00_00"),
		map_object_instance_param_path("m20_24_00_00"),
		map_object_instance_param_path("m20_26_00_00"),
		map_object_instance_param_path("m40_03_00_00"),
		map_object_instance_param_path("m50_35_00_00"),
		map_object_instance_param_path("m50_36_00_00"),
		map_object_instance_param_path("m50_37_00_00"),
		map_object_instance_param_path("m50_38_00_00"),
	};

	String_Builder sb = {0};
	String_Builder wsb = {0};
	for (size_t i=0; i<ARRAY_LEN(paths); i++) {
		sb.count = 0;
		wsb.count = 0;

		const char *path = paths[i];
		if (!nob_read_entire_file(path, &sb)) {
			return 1;
		}
		printf("Read %zu bytes from %s\n", sb.count, path);
		String_View sv = sb_to_sv(sb);

		PARAM param = load_param(&sv, Cell_MapObjInstanceParam);

		write_param(&wsb, &param);

		for (size_t j=0; j<wsb.count; j++) {
			if(wsb.items[j] != sb.items[j] && j<(size_t)param.actualStringsOffset) asm("int3");
		}
		if (wsb.count != sb.count) asm("int3");

		String_View wsv = sb_to_sv(wsb);
		PARAM param_reloaded = load_param(&wsv, Cell_MapObjInstanceParam);


		free_PARAM(&param_reloaded);
		free_PARAM(&param);
	}
	da_free(sb);
	da_free(wsb);

	return 0;
}

bool sv_cmp(String_View a, String_View b) {
	if (a.count != b.count) return false;
	for (size_t i=0; i<a.count; i++) {
		if (a.data[i] != b.data[i]) return false;
	}
	return true;
}

bool sv_str_cmp(String_View sv, const char *str) {
	if (sv.count != strlen(str)) return false;
	for (size_t i=0; i<sv.count; i++) {
		if (sv.data[i] != str[i]) return false;
	}
	return true;
}

#include "esd.c"

int test_loading_all_esds() {
	const char *paths[] = {
		esd_path("m10_02_00_00"),
		esd_path("m10_04_00_00"),
		esd_path("m10_10_00_00"),
		esd_path("m10_14_00_00"),
		esd_path("m10_15_00_00"),
		esd_path("m10_16_00_00"),
		esd_path("m10_17_00_00"),
		esd_path("m10_18_00_00"),
		esd_path("m10_19_00_00"),
		esd_path("m10_23_00_00"),
		esd_path("m10_25_00_00"),
		esd_path("m10_27_00_00"),
		esd_path("m10_29_00_00"),
		esd_path("m10_30_00_00"),
		esd_path("m10_31_00_00"),
		esd_path("m10_32_00_00"),
		esd_path("m10_33_00_00"),
		esd_path("m10_34_00_00"),
		esd_path("m20_10_00_00"),
		esd_path("m20_11_00_00"),
		esd_path("m20_21_00_00"),
		esd_path("m20_24_00_00"),
		esd_path("m20_26_00_00"),
		esd_path("m40_03_00_00"),
		esd_path("m50_35_00_00"),
		esd_path("m50_36_00_00"),
		esd_path("m50_37_00_00"),
		esd_path("m50_38_00_00"),
	};

	String_Builder sb = {0};
	String_Builder wsb = {0};
	for (size_t i=0; i<ARRAY_LEN(paths); i++) {
		sb.count = 0;
		wsb.count = 0;

		const char *path = paths[i];
		if (!nob_read_entire_file(path, &sb)) {
			return 1;
		}
		printf("Read %zu bytes from %s\n", sb.count, path);
		String_View sv = sb_to_sv(sb);


		ESD esd = read_esd(sv);
		write_esd(&wsb, &esd);

		for (size_t j=0; j<wsb.count; j++) {
			if(wsb.items[j] != sb.items[j] && j < wsb.count - 0xf - 4) asm("int3");
		}
		if (wsb.count != sb.count) asm("int3");

		// print_state_group(&esd, &esd.state_groups.items[50]);
		// for (size_t i=0; i<esd.state_groups.count; i++) {
		// 	print_state_group(&esd, &esd.state_groups.items[i]);
		// }
		// test_all_expressions(&esd);

		free_esd(&esd);
	}
	da_free(sb);
	da_free(wsb);

	return 0;
}

typedef int32_t s32;
typedef uint8_t u8;

typedef packed_struct {
	char magic[4];
    s32 unk04;
    s32 unk08;
    s32 unk0c;
    s32 unk10;
    s32 unk14;
    char dcs[4];
    s32 uncompressed_size;
    s32 compressed_size;
    char dcp[4];
    char format[4];
    s32 unk2c;
    u8 unk30;
    u8 pad[3];
    s32 unk34;
    u8 unk38;
    u8 pad2[3];
    s32 unk3c;
    s32 unk40;
    char dca[4];
    s32 compressed_header_length;
} DCX_Header;

typedef struct {
	uint8_t *data;
	size_t position;
	size_t length;
} BinaryReader;

BinaryReader sb_to_br(String_Builder sb) {
	return (BinaryReader) {
		.data = (void *)sb.items,
		.length = sb.count,
		.position = 0
	};
}

#define br_read(br, type) *(type *)((br)->data + (br)->position); (br)->position += sizeof(type);
#define br_head(br) (void *)((br)->data + (br)->position)
#define br_rem_bytes(br) (br)->length - (br)->position
#define br_at(br, offset) (void *)((br)->data + offset)
#define br_assert(br, type, value) \
	do {\
		type __i__ = br_read((br), type);\
		assert(__i__ == (value));\
	} while (0)


// initialize the dst before passsing here
bool inflate_zlib(String_View src, String_Builder *dst) {
    z_stream stream = {0};
    stream.next_in = (uint8_t *)src.data;
    stream.avail_in = src.count;
    
    stream.next_out = (uint8_t *)(dst->items + dst->count); 
    stream.avail_out = dst->capacity - dst->count;

    if (inflateInit2(&stream, MZ_DEFAULT_WINDOW_BITS) != Z_OK) {
        printf("ERROR: inflateInit() failed!\n");
        return false;
    }

    int status = inflate(&stream, MZ_FINISH);
    if (status == Z_STREAM_END) {
        dst->count += stream.total_out; 
        inflateEnd(&stream);
        return true;
    } else {
        printf("ERROR: inflate() failed or buffer too small (status %i)!\n", status);
        inflateEnd(&stream);
        return false;
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "miniz.h"

// 1. (Optional) Custom Adler32 implementation.
// Note: miniz natively includes mz_adler32(), but this is the exact 1:1 translation of your C# logic.
uint32_t Adler32(const uint8_t *data, size_t length) {
    uint32_t adlerA = 1;
    uint32_t adlerB = 0;

    for (size_t i = 0; i < length; i++) {
        adlerA = (adlerA + data[i]) % 65521;
        adlerB = (adlerB + adlerA) % 65521;
    }

    return (adlerB << 16) | adlerA;
}

// 2. The WriteZlib implementation
bool write_zlib(String_Builder *dst, String_View *src) {
    if (dst == NULL || src == NULL) return false;

    // Write the custom Zlib header (0x78 and custom formatByte)
	sb_append(dst, 0x78);
	sb_append(dst, 0xda);

    // Compress using raw DEFLATE
    // miniz's tdefl_compress_mem_to_heap creates raw deflate data without zlib headers 
    // when using default flags, which perfectly mimics C#'s DeflateStream.
	long size = tdefl_compress_mem_to_mem((dst->items+dst->count), (dst->capacity-dst->count), src->data, src->count, MZ_BEST_COMPRESSION);
	if (size == 0) {
		printf("ERROR: compression failed\n");
		return false;
	}
	dst->count += size;

    // Compute the Adler32 Checksum
    // You can use your function: Adler32(input, inputLen) 
    // OR miniz's heavily optimized built-in function:
    uint32_t adler = (uint32_t)mz_adler32(1, (unsigned char *)src->data, src->count);

    // Write the Adler32 checksum
    uint8_t adlerBytes[4];
    
    // IMPORTANT ENDIANNESS CHECK:
    // C# BinaryWriter.Write(UInt32) writes in Little-Endian by default.
    // If your goal is to strictly match the C# output byte-for-byte, use Little-Endian:
    // adlerBytes[0] = (uint8_t)(adler & 0xFF);
    // adlerBytes[1] = (uint8_t)((adler >> 8) & 0xFF);
    // adlerBytes[2] = (uint8_t)((adler >> 16) & 0xFF);
    // adlerBytes[3] = (uint8_t)((adler >> 24) & 0xFF);

    // HOWEVER: Standard Zlib format requires the Adler32 checksum to be Big-Endian!
    // If you need standard zlib compatibility, swap to Big-Endian like this:
    adlerBytes[0] = (uint8_t)((adler >> 24) & 0xFF);
    adlerBytes[1] = (uint8_t)((adler >> 16) & 0xFF);
    adlerBytes[2] = (uint8_t)((adler >> 8) & 0xFF);
    adlerBytes[3] = (uint8_t)(adler & 0xFF);

	sb_append_buf(dst, adlerBytes, 4);

    return true;
}

bool deflate_zlib(String_View src, String_Builder *dst) {
    z_stream stream = {0};
    stream.next_in = (uint8_t *)src.data;
    stream.avail_in = src.count;
    
    stream.next_out = (uint8_t *)(dst->items + dst->count); 
    stream.avail_out = dst->capacity - dst->count;

    if (deflateInit(&stream, MZ_BEST_SPEED) != Z_OK) {
        printf("ERROR: deflateInit() failed!\n");
        return false;
    }

    int status = deflate(&stream, MZ_FINISH);
    
    if (status == Z_STREAM_END) {
        dst->count += stream.total_out; 
        deflateEnd(&stream);
        return true;
    } else {
        printf("ERROR: deflate() failed or buffer too small (status %i)!\n", status);
        deflateEnd(&stream);
        return false;
    }
}

int32_t swap_endianness(int32_t val) {
	const uint8_t *b = (uint8_t *)&val;
    return (int32_t)(
        ((uint32_t)b[0] << 24) |
        ((uint32_t)b[1] << 16) |
        ((uint32_t)b[2] << 8)  |
        ((uint32_t)b[3])
    );
}

typedef struct {
	DCX_Header header;
	String_Builder data;
} DCX;

DCX decompress_dcx(BinaryReader br) {
	DCX_Header header = br_read(&br, DCX_Header);
	assert(strcmp(header.magic, "DCX") == 0);
	assert(header.unk04 == swap_endianness(0x10000));
	assert(header.unk08 == swap_endianness(0x18));
	assert(header.unk0c == swap_endianness(0x24));
	assert(header.unk10 == swap_endianness(0x24));
	assert(header.unk14 == swap_endianness(0x2c));
	assert(strcmp(header.dcs, "DCS") == 0);
	assert(strcmp(header.dcp, "DCP") == 0);
	assert(strcmp(header.format, "DFLT") == 0);
	assert(header.unk2c == swap_endianness(0x20));
	assert(header.unk30 == 9);
	assert(header.pad[0] == 0);
	assert(header.pad[1] == 0);
	assert(header.pad[2] == 0);
	assert(header.unk34 == 0);
	assert(header.unk38 == 0);
	assert(header.pad2[0] == 0);
	assert(header.pad2[1] == 0);
	assert(header.unk3c == 0);
	assert(header.unk40 == swap_endianness(0x00010100));
	assert(strcmp(header.dca, "DCA") == 0);
	// assert(header.unk4c == 0x78);
	// assert(header.unk4d == 0x01 || header.unk4d == 0x5e || header.unk4d == 0x9c || header.unk4d == 0xda);
	String_View compressed = {0};
	compressed.data = br_head(&br);
	compressed.count = br_rem_bytes(&br);

	String_Builder sb = {0};
	da_init(&sb, COMPRESSION_ALLOC_SIZE);
	assert(inflate_zlib(compressed, &sb) && "Failed to decompress dcx.");
	return (DCX){
		.data = sb,
		.header = header,
	};
}

void free_dcx(DCX *dcx) {
	da_free(dcx->data);
}

typedef enum {
	BND_Fmt_None = 0,
	BND_Fmt_BigEndian = 1,
	BND_Fmt_IDs = 1<<1,
	BND_Fmt_Names1 = 1<<2,
	BND_Fmt_Names2 = 1<<3,
	BND_Fmt_LongOffsets = 1<<4,
	BND_Fmt_Compression = 1<<5,
	BND_Fmt_Flag6 = 1<<6,
	BND_Fmt_Flag7 = 1<<7,
} BND4Format;

typedef enum {
	None = 0,
	BND4_File_Compressed = 1<<0,
	BND4_File_Flag1 = 1<<1,
	BND4_File_Flag2 = 1<<2,
	BND4_File_Flag3 = 1<<3,
	BND4_File_Flag4 = 1<<4,
	BND4_File_Flag5 = 1<<5,
	BND4_File_Flag6 = 1<<6,
	BND4_File_Flag7 = 1<<7,
} BND4FileFlags;

uint8_t reverse_bits_8(uint8_t b) {
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        result = (result << 1) | (b & 1);
        b >>= 1;
    }
    return result;
}

long get_bnd4_file_header_size(BND4Format format) {
	return 0x10 \
		+ (((format & BND_Fmt_LongOffsets) != 0) ? 8: 4) \
		+ (((format & BND_Fmt_Compression) != 0) ? 8: 0) \
		+ (((format & BND_Fmt_IDs) != 0) ? 4: 0) \
		+ (((format & (BND_Fmt_Names1 | BND_Fmt_Names2)) != 0) ? 4: 0) \
		+ ((format == BND_Fmt_Names1) ? 8: 0);

}

typedef packed_struct {
	char magic[4]; // BND4
	bool unk04;
	bool unk05;
	byte unk06; // 0
	byte unk07; // 0
	byte unk08; // 0
	bool big_endian;
	bool bit_big_endian;
	byte unk11; // 0
	int32_t file_count;
	int64_t header_size; // 0x40
	char version[8];
	int64_t file_header_size;
	int64_t header_end;
	bool unicode;
	byte format;
	byte extended; // 0 || 1 || 4 || 0x80
	uint8_t unk51; // 0
	int32_t unk52; // 0
	int64_t unk56; // 0
} BND4_Header;

BND4_Header read_bnd4_header(BinaryReader *br) {
	BND4_Header header = br_read(br, BND4_Header);
	header.bit_big_endian = !header.bit_big_endian;
	if (!(header.bit_big_endian || (((header.format & 1) != 0) && ((header.format & 1<<7) == 0)))) {
		header.format = reverse_bits_8(header.format);
	}
	return header;
}

typedef packed_struct {
	byte file_flags;
	uint8_t unk01; // 0
	uint8_t unk02; // 0
	uint8_t unk03; // 0
	int32_t unk04; // -1
	int64_t compressed_size;
} BND4_FileHeaderInfo;

typedef struct {
	BND4_FileHeaderInfo info;
	int64_t uncompressed_size;
	bool has_long_offsets;
	int64_t data_offset;
	int32_t id;
	const char *name;
} BinderFileHeader;

DA(BinderFileHeader, BinderFileHeaders);

typedef struct {
	uint8_t *bytes;
	size_t count;

	byte flags;
	int32_t id;
	const char *name;
} BinderFile;
DA(BinderFile, BinderFiles);

typedef struct {
	BND4_Header header;
	BinderFileHeaders file_headers;
	BinderFiles files;
} BND4;

void free_bnd4(BND4 *bnd4) {
	da_free(bnd4->file_headers);
	da_free(bnd4->files);
}

BinderFileHeader read_bnd4_file_header(const BND4_Header *bnd_info, BinaryReader *br) {
	BND4_FileHeaderInfo info = br_read(br, BND4_FileHeaderInfo);
	if (!bnd_info->bit_big_endian) {
		info.file_flags = reverse_bits_8(info.file_flags);
	}
	bool has_compression = ((bnd_info->format & BND_Fmt_Compression) != 0);
	assert(!has_compression);

	BinderFileHeader header = {0};
	header.uncompressed_size = -1;
	header.info = info;
	if ((bnd_info->format & BND_Fmt_LongOffsets) != 0) {
		header.has_long_offsets = true;
		header.data_offset = br_read(br, int64_t);
	} else {
		header.has_long_offsets = false;
		header.data_offset = br_read(br, int32_t);
	}

	if ((bnd_info->format & BND_Fmt_IDs) != 0) {
		header.id = br_read(br, int32_t);
	} else {
		header.id = -1;
	}

	if ((bnd_info->format & (BND_Fmt_Names1 | BND_Fmt_Names2)) != 0) {
		uint32_t name_offset = br_read(br, uint32_t);
		assert(!bnd_info->unicode);
		header.name = br_at(br, name_offset);
	} else {
		header.name = NULL;
	}

	if (bnd_info->format == BND_Fmt_Names1) {
		header.id = br_read(br, int32_t);
		br_assert(br, int32_t, 0);
	}

	return header;
}

BinderFile read_binder_file(BinaryReader *br, BinderFileHeader *header) {
	BinderFile file = {0};
	assert(!((header->info.file_flags & BND4_File_Compressed) != 0));
	file.bytes = br_at(br, header->data_offset);
	file.count = header->info.compressed_size;
	file.flags = header->info.file_flags;
	file.id = header->id;
	file.name = header->name;
	return file;
}

BND4 read_bnd4(DCX *dcx) {
	BinaryReader br = sb_to_br(dcx->data);
	br.length = dcx->data.count;
	String_View sv = sb_to_sv(dcx->data);
	assert(!is_dcx(&sv));
	BND4 bnd4 = {0};
	bnd4.header = read_bnd4_header(&br);
	// printf("offset = %llu\n", offsetof(BND4_Header, unk51));
	assert(bnd4.header.file_header_size == get_bnd4_file_header_size(bnd4.header.format));

	da_init(&bnd4.file_headers, bnd4.header.file_count);
	for (int32_t i=0; i<bnd4.header.file_count; i++) {
		da_append(&bnd4.file_headers, read_bnd4_file_header(&bnd4.header, &br));
	}
	da_init(&bnd4.files, bnd4.header.file_count);
	da_foreach(BinderFileHeader, header, &bnd4.file_headers) {
		da_append(&bnd4.files, read_binder_file(&br, header));
	}

	return bnd4;
}

BinderFileHeader binder_file_to_header(BinderFile *file) {
	BinderFileHeader header = {0};
	header.info = (BND4_FileHeaderInfo){
		.file_flags = file->flags,
		.unk01 = 0,
		.unk02 = 0,
		.unk03 = 0,
		.unk04 = -1,
		.compressed_size = file->count,
	};
	header.uncompressed_size = -1;
	header.has_long_offsets = false;
	header.data_offset = -1;
	header.id = file->id;
	header.name = file->name;
	return header;
}

void write_binder_file_header(String_Builder *sb, const BND4 *bnd4, BinderFileHeader *header, DArrz *offsets) {
	const BND4_Header *bnd_info = &bnd4->header;
	BND4_FileHeaderInfo info = header->info;
	if (!bnd_info->bit_big_endian) {
		info.file_flags = reverse_bits_8(info.file_flags);
	}

	bool has_compression = ((bnd_info->format & BND_Fmt_Compression) != 0);
	assert(!has_compression);

	size_t size = sizeof(info);
	sb_append_buf(sb, &info, size);

	da_append(offsets, sb->count);
	if ((bnd_info->format & BND_Fmt_LongOffsets) != 0) {
		sb_reserve(sb, sizeof(int64_t)); // data_offset
	} else {
		sb_reserve(sb, sizeof(int32_t)); // data_offset
	}

	if ((bnd_info->format & BND_Fmt_IDs) != 0) {
		sb_write_int32t(sb, header->id);
	}

	da_append(offsets, sb->count);
	if ((bnd_info->format & (BND_Fmt_Names1 | BND_Fmt_Names2)) != 0) {
		sb_reserve(sb, sizeof(int32_t)); //name_offset
		assert(!bnd_info->unicode);
	}

	if (bnd_info->format == BND_Fmt_Names1) {
		sb_write_int32t(sb, header->id);
		sb_write_int32t(sb, 0);
	}
}

void write_binder_file_headers(String_Builder *sb, const BND4 *bnd4, BinderFileHeaders *headers, DArrz *offsets) {
	size_t size = sizeof(bnd4->header);
	BND4_Header header = bnd4->header;

	if (!(header.bit_big_endian || (((header.format & 1) != 0) && ((header.format & 1<<7) == 0)))) {
		header.format = reverse_bits_8(header.format);
	}
	header.bit_big_endian = !header.bit_big_endian;
	header.file_count = headers->count;

	sb_append_buf(sb, &header, size);

	for (size_t i=0; i<headers->count; i++) {
		BinderFileHeader *file_header = &headers->items[i];
		write_binder_file_header(sb, bnd4, file_header, offsets);
	}
	for (size_t i=0; i<headers->count; i++) {
		BinderFileHeader *file_header = &headers->items[i];
		sb_patch_int32t(sb, offsets->items[2*i + 1], sb->count);
		sb_append_cstr(sb, file_header->name);
		sb_append(sb, 0);
	}

	void *dst = sb->items + offsetof(BND4_Header, header_end);
	memcpy(dst, &sb->count, sizeof(int64_t));
}

void write_binder_file(String_Builder *sb, BinderFile *file, DArrz *offsets, size_t i) {
	if (file->count > 0) {
		sb_pad_align(sb, 0x10);
	}
	sb_patch_int32t(sb, offsets->items[2*i], sb->count);
	sb_append_buf(sb, file->bytes, file->count);
}

void write_bnd4(String_Builder *sb, const BND4 *bnd4) {
	BinderFileHeaders headers = {0};
	da_foreach(BinderFile, file, &bnd4->files) {
		da_append(&headers, binder_file_to_header(file));
	}
	DArrz offsets = {0}; // even = data_offset, odd = name_offset
	da_init(&offsets, 2*headers.count);
	write_binder_file_headers(sb, bnd4, &headers, &offsets);
	size_t count = 0;
	da_foreach(BinderFile, file, &bnd4->files) {
		write_binder_file(sb, file, &offsets, count++);
	}

	da_free(headers);
	da_free(offsets);
}

void compress_dcx(String_Builder *sb, DCX *dcx, const String_Builder *uncomp_sb) {
	size_t begin = sb->count;
	size_t size = sizeof(dcx->header);
	DCX_Header header = dcx->header;
	// header.unk04 = swap_endianness(header.unk04);
	// header.unk08 = swap_endianness(header.unk08);
	// header.unk0c = swap_endianness(header.unk0c);
	// header.unk10 = swap_endianness(header.unk10);
	// header.unk14 = swap_endianness(header.unk14);
	// header.unk2c = swap_endianness(header.unk2c);
	// header.unk40 = swap_endianness(header.unk40);
	sb_append_buf(sb, &header, size);

	String_View uncompressed = sb_to_sv(*uncomp_sb);
	da_reserve(sb, sb->count + uncompressed.count); // reserve enough to accomodate the uncompressed bytes
	size = sb->count;
	assert(write_zlib(sb, &uncompressed));
	size = swap_endianness(sb->count - size);
	char *dst = sb->items + begin + offsetof(DCX_Header, compressed_size);
	memcpy(dst, &size, sizeof(int32_t));
	// assert(deflate_zlib(uncompressed, sb) && "Failed to decompress dcx.");
}

int test_loading_enc_reg() {
	String_Builder sb = {0};
	const char *path = "./enc_regulation.bnd.dcx";
	if (!nob_read_entire_file(path, &sb)) {
		return 1;
	}
	printf("Read %zu bytes from %s\n", sb.count, path);
	BinaryReader br = sb_to_br(sb);

	DCX dcx = decompress_dcx(br);

	// if (!nob_write_entire_file("out-orig.bnd4", dcx.data.items, dcx.data.count)) {
	// 	return 1;
	// }

	BND4 bnd4 = read_bnd4(&dcx);

	String_Builder bnd4_sb = {0};
	write_bnd4(&bnd4_sb, &bnd4);

	// check if generated data is same
	for (size_t i=0; i<dcx.data.count; i++) {
		if (dcx.data.items[i] != bnd4_sb.items[i]) asm("int3");
	}
	if (dcx.data.count != bnd4_sb.count) asm("int3");

	// if (!nob_write_entire_file("out-gen.bnd4", bnd4_sb.items, bnd4_sb.count)) {
	// 	return 1;
	// }

	String_Builder wsb = {0};
	compress_dcx(&wsb, &dcx, &bnd4_sb);

	br = sb_to_br(wsb);
	DCX dcx_check = decompress_dcx(br);
	for (size_t i=0; i<dcx.data.count; i++) {
		if (dcx.data.items[i] != dcx_check.data.items[i]) asm("int3");
	}
	if (dcx.data.count != dcx_check.data.count) asm("int3");


	if (!nob_write_entire_file("output/enc_regulation.bnd.dcx", wsb.items, wsb.count)) {
		return 1;
	}

	free_dcx(&dcx_check);
	free_bnd4(&bnd4);
	free_dcx(&dcx);
	da_free(sb);
	da_free(bnd4_sb);
	da_free(wsb);
	return 0;
}

#define WHITE_DOOR_PARAM_NAME "MapObjectWhiteDoorParam.param"

bool disable_fog_gate(char *input_path, char *param_file_path, char *output_path) {
	String_Builder sb = {0};
	if (!nob_read_entire_file(input_path, &sb)) {
		return false;
	}
#ifdef LOG_GENERATED_FILES
	printf("[Read] %s: %zu bytes\n", input_path, sb.count);
#endif
	BinaryReader br = sb_to_br(sb);
	DCX dcx = decompress_dcx(br);
	BND4 bnd4 = read_bnd4(&dcx);
	PARAM param = {0};

	bool param_exists = false;
	byte flags = 0;
	for (size_t i=0; i<bnd4.files.count; i++) {
		BinderFile *file = &bnd4.files.items[i];
		if (strcmp(file->name, WHITE_DOOR_PARAM_NAME) == 0) {
			param_exists = true;
			break;
		}

		if (i == 0) {
			flags = file->flags;
		}
	}

	// add white door param to the reg file if it does not exist
	// when merging with other mods this is necessary
	String_Builder out_param_sb = {0};
	if (!param_exists) {
		String_Builder param_sb = {0};
		if (!nob_read_entire_file(param_file_path, &param_sb)) {
			return false;
		}
		String_View sv = sb_to_sv(param_sb);
		param = load_param(&sv, Cell_MapObjectWhiteDoorParam);
		da_foreach(Cell, cell, &param.cells) {
			cell->as.map_obj_white_door_param.key_guide_id_1 = 0; // for disabling the fog gate key guide
			cell->as.map_obj_white_door_param.key_guide_id_2 = 0; // for disabling the fog gate key guide
			cell->as.map_obj_white_door_param.unk04 = 0; // to diable boss gates to disappear
		}
		write_param(&out_param_sb, &param);
		BinderFile file = {0};
		file.bytes = (void *)out_param_sb.items;
		file.count = out_param_sb.count;
		file.flags = flags;
		file.id = bnd4.files.count;
		file.name = WHITE_DOOR_PARAM_NAME;
		da_append(&bnd4.files, file);
		da_free(param_sb);
	}

	// disable fog gates
	String_Builder white_door_param_buf = {0};
	da_foreach(BinderFile, file, &bnd4.files) {
		if (strcmp(file->name, WHITE_DOOR_PARAM_NAME) == 0) {
			String_View sv = {.data = (char *)file->bytes, .count = file->count};
			param = load_param(&sv, Cell_MapObjectWhiteDoorParam);
			da_foreach(Cell, cell, &param.cells) {
				cell->as.map_obj_white_door_param.key_guide_id_1 = 0; // for disabling the fog gate key guide
				cell->as.map_obj_white_door_param.key_guide_id_2 = 0; // for disabling the fog gate key guide
				cell->as.map_obj_white_door_param.unk04 = 0; // to diable boss gates to disappear
			}
			write_param(&white_door_param_buf, &param);
			file->bytes = (uint8_t *)white_door_param_buf.items;
			file->count = white_door_param_buf.count;
			break;
		}
	}

	String_Builder bnd4_sb = {0};
	write_bnd4(&bnd4_sb, &bnd4);
	String_Builder wsb = {0};
	compress_dcx(&wsb, &dcx, &bnd4_sb);

	if (!write_entire_file(output_path, wsb.items, wsb.count)) return false;
#ifdef LOG_GENERATED_FILES
	printf("[Generated] %s\n", output_path);
#endif

	da_free(out_param_sb);
	free_PARAM(&param);
	da_free(white_door_param_buf);
	da_free(sb);
	free_dcx(&dcx);
	free_bnd4(&bnd4);
	da_free(bnd4_sb);
	da_free(wsb);
	return true;
}

typedef struct {
	int id;
	wchar_t *text;
} FMG_Entry;

DA(FMG_Entry, FMG_Entries);

typedef struct {
	FMG_Entries entries;
	bool big_endian;
	uint8_t version;
} FMG;

void free_fmg(FMG *fmg) {
	da_free(fmg->entries);
}

FMG read_fmg(BinaryReader *br) {
	FMG fmg = {0};
	br_assert(br, byte, 0);
	fmg.big_endian = br_read(br, byte);
	assert(!fmg.big_endian);
	fmg.version = br_read(br, byte);
	assert(fmg.version == 1);
	br_assert(br, byte, 0);
	int32_t file_size = br_read(br, int32_t);
	UNUSED(file_size);
	br_assert(br, byte, 1);
	br_assert(br, byte, 0);
	br_assert(br, byte, 0);
	br_assert(br, byte, 0);
	int32_t group_count = br_read(br, int32_t);
	int32_t string_count = br_read(br, int32_t);
	int32_t string_offsets_offset = br_read(br, int32_t);
	br_assert(br, int32_t, 0);
	da_init(&fmg.entries, string_count);
	for (int32_t i=0; i<group_count; i++) {
		int32_t offset_index = br_read(br, int32_t);
		int32_t first_id = br_read(br, int32_t);
		int32_t last_id = br_read(br, int32_t);

		size_t position = br->position; {
			br->position = string_offsets_offset + offset_index*4;
			for (int32_t j=0; j<last_id-first_id+1; j++) {
				int32_t string_offset = br_read(br, int32_t);
				FMG_Entry entry = {
					.id = first_id + j,
					.text = string_offset != 0 ? br_at(br, string_offset) : NULL,
				};
				da_append(&fmg.entries, entry);

			}
		} br->position = position;
	}
	return fmg;
}

void write_fmg(String_Builder *sb, FMG *fmg) {
	sb_append(sb, 0);
	sb_append(sb, fmg->big_endian);
	sb_append(sb, fmg->version);
	sb_append(sb, 0);
	size_t file_size_offset = sb->count;
	sb_reserve(sb, sizeof(int32_t)); // file_size
	sb_append(sb, 1);
	sb_append(sb, 0);
	sb_append(sb, 0);
	sb_append(sb, 0);
	size_t group_count_offset = sb->count;
	sb_reserve(sb, sizeof(int32_t)); // group_count
	sb_write_int32t(sb, fmg->entries.count);
	size_t string_offsets_offset = sb->count;
	sb_reserve(sb, sizeof(int32_t)); // string_offsets_offset
	sb_write_int32t(sb, 0);
	int32_t group_count = 0;
	for (size_t i=0; i<fmg->entries.count; i++) {
		sb_write_int32t(sb, i);
		sb_write_int32t(sb, fmg->entries.items[i].id);
		while (i < fmg->entries.count - 1 && fmg->entries.items[i + 1].id == fmg->entries.items[i].id + 1)
			i++;
		sb_write_int32t(sb, fmg->entries.items[i].id);
		group_count++;
	}
	sb_patch_int32t(sb, group_count_offset, group_count);
	sb_patch_int32t(sb, string_offsets_offset, sb->count);
	DArrz offsets = {0};
	for (size_t i=0; i<fmg->entries.count; i++) {
		da_append(&offsets, sb->count);
		sb_reserve(sb, sizeof(int32_t));
	}
	for (size_t i=0; i<fmg->entries.count; i++) {
		FMG_Entry *entry = &fmg->entries.items[i];
		if (entry->text) {
			sb_patch_int32t(sb, offsets.items[i], sb->count);
			sb_append_utf16t(sb, entry->text);
		} else {
			sb_patch_int32t(sb, offsets.items[i], 0);
		}
	}
	sb_pad_align(sb, 0x10);
	sb_patch_int32t(sb, file_size_offset, sb->count);
	da_free(offsets);
}

#define PIRATE_SHIP_ARRIVAL_MSG L"Ghost ship in No Mans Wharf not docked"

bool add_pirate_ship_msg_to_game_files(const char *input_path, const char *output_path, int *msg_id) {
	String_Builder sb = {0};
	if (!nob_read_entire_file(input_path, &sb)) {
		return false;
	}
#ifdef LOG_GENERATED_FILES
	printf("[Read] %s: %zu bytes\n", input_path, sb.count);
#endif
	BinaryReader br = sb_to_br(sb);
	FMG fmg = read_fmg(&br);
	da_foreach(FMG_Entry, entry, &fmg.entries) {
		if (entry->text == NULL) {
			entry->text = PIRATE_SHIP_ARRIVAL_MSG;
			*msg_id = entry->id;
			break;
		}
	}

	String_Builder wsb = {0};
	write_fmg(&wsb, &fmg);

	if (!mkdir_if_not_exists(OUT_FOLDER_PATH"menu")) return false;
	if (!mkdir_if_not_exists(OUT_FOLDER_PATH"menu/text")) return false;
	if (!mkdir_if_not_exists(OUT_FOLDER_PATH"menu/text/english")) return false;

	if (!write_entire_file(output_path, wsb.items, wsb.count)) return 1;

#ifdef LOG_GENERATED_FILES
	printf("[Generated] %s\n", output_path);
#endif

	free_fmg(&fmg);
	da_free(sb);
	da_free(wsb);
	return true;
}

#define WARP_OBJ_MODEL_NAME "o02_1050"
#define WARP_OBJ_NAME WARP_OBJ_MODEL_NAME"_0000"
#define WARP_OBJ_INST_ID 10021101 

#define MAP_ITEMS \
  X("m10_02_00_00") \
  X("m10_04_00_00") \
  X("m10_10_00_00") \
  X("m10_14_00_00") \
  X("m10_15_00_00") \
  X("m10_16_00_00") \
  X("m10_17_00_00") \
  X("m10_18_00_00") \
  X("m10_19_00_00") \
  X("m10_23_00_00") \
  X("m10_25_00_00") \
  X("m10_27_00_00") \
  X("m10_29_00_00") \
  X("m10_30_00_00") \
  X("m10_31_00_00") \
  X("m10_32_00_00") \
  X("m10_33_00_00") \
  X("m10_34_00_00") \
  X("m20_10_00_00") \
  X("m20_11_00_00") \
  X("m20_21_00_00") \
  X("m20_24_00_00") \
  X("m20_26_00_00") \
  X("m40_03_00_00") \
  X("m50_35_00_00") \
  X("m50_36_00_00") \
  X("m50_37_00_00") \
  X("m50_38_00_00")

// Generate the names array
const char map_names[][13] = {
#define X(name) name,
  MAP_ITEMS
#undef X
};

// Generate the paths array
const char map_paths[][34] = {
#define X(name) "map/" name "/" name ".msb",
  MAP_ITEMS
#undef X
};

// Generate the mapobjectinstanceparam array
const char obj_inst_param_paths[][48] = {
#define X(name) "Param/mapobjectinstanceparam_" name ".param",
  MAP_ITEMS
#undef X
};

// Generate the event_param array
const char event_param_paths[][36] = {
#define X(name) "Param/eventparam_" name ".param",
  MAP_ITEMS
#undef X
};

// Generate the event_location array
const char event_loc_paths[][39] = {
#define X(name) "Param/eventlocation_" name ".param",
  MAP_ITEMS
#undef X
};


// Generate the esd path array
const char esd_paths[][31] = {
#define X(name) "ezstate/event_" name ".esd",
  MAP_ITEMS
#undef X
};

static_assert(ARRAY_LEN(map_names) == __MapNameCount, "Missing map_name");
static_assert(ARRAY_LEN(map_paths) == __MapNameCount, "Missing map_path");
static_assert(ARRAY_LEN(obj_inst_param_paths) == __MapNameCount, "Missing obj_inst_param_path");
static_assert(ARRAY_LEN(event_param_paths) == __MapNameCount, "Missing event_param_path");
static_assert(ARRAY_LEN(event_loc_paths) == __MapNameCount, "Missing event_loc_path");
static_assert(ARRAY_LEN(esd_paths) == __MapNameCount, "Missing esd_path");

size_t map_ids[] = {
	10020000,
	10040000,
	10100000,
	10140000,
	10150000,
	10160000,
	10170000,
	10180000,
	10190000,
	10230000,
	10250000,
	10270000,
	10290000,
	10300000,
	10310000,
	10320000,
	10330000,
	10340000,
	20100000,
	20110000,
	20210000,
	20240000,
	20260000,
	40030000,
	50350000,
	50360000,
	50370000,
	50380000,
};
static_assert(ARRAY_LEN(map_ids) == __MapNameCount, "Missing map id");

wchar_t * write_cstr_to_wchar(const char *src, String_Builder *dst_sb) {
	size_t count = 0;
	assert(dst_sb->capacity - dst_sb->count > 2*strlen(src) + 2);
	char *loc = dst_sb->items + dst_sb->count;
	for(;;) {
		char c = src[count++];
		if (c == 0) {
			sb_append(dst_sb, 0);
			sb_append(dst_sb, 0);
			break;
		}
		sb_append(dst_sb, c);
		sb_append(dst_sb, 0);
	}
	return (wchar_t *)loc;
}

Part warp_part_new(const Part *src, V3f pos, V3f rot, int32_t instance_id, size_t name_id, String_Builder *name_buf, int model_index) {
	size_t mark = temp_save();
	Part part = *src;
	part.part.position = pos;
	part.part.rotation = rot;
	part.type_data.object.map_object_instance_param_id = instance_id;
	part.name = write_cstr_to_wchar(temp_sprintf(WARP_OBJ_MODEL_NAME"_%zu", name_id), name_buf);
	part.part.model_index = model_index;
	temp_rewind(mark);
	return part;
}

#define da_insert(da, item, position) \
	do {\
		assert((position) <= (da)->count);\
		da_reserve((da), (da)->count + 1);\
		memmove(&(da)->items[(position)+1], &(da)->items[(position)], sizeof(*(da)->items)*((da)->count - (position)));\
		(da)->items[(position)] = (item);\
		(da)->count++;\
	} while(0)

void add_map_obj_instance_param_warpobj(PARAM *param, int32_t id, size_t insert_loc) {
	Row row = {
		.ID = id, .garbage = 0, .Name.type = Str_Wide, .Name.as.str_wide = L"",
	};
	Cell cell = {
		.as.map_obj_instance_param = {
			.headerID = 0, .ParamID = 0, .ObjectBullet = 0,
			.Unk02 = 1.0f, .Unk03 = -1.0f, .MapObjectInstanceType = 100,
			.Unk04 = 0, .Unk05 = 255, .Unk06 = 255,
			.Unk07 = 7, .Unk08 = 255, .DefaultState = 255,
			.Unk09 = 50, .Unk10 = 255, .ItemlotID = 0,
		}
	};
	da_insert(&param->rows, row, insert_loc);
	da_insert(&param->cells, cell, insert_loc);
	param->rowCount++;
}

void add_event_param_warpobj(PARAM *param, int32_t event_param_id, int32_t warp_obj_inst_id) {
	Row row = {
		.ID = event_param_id, .garbage = 0, .Name.type = Str_Wide, .Name.as.str_wide = L"",
	};
	Cell cell = {
		.as.event_param = {
			.event_id = event_param_id, .flag_id = warp_obj_inst_id,
			.Unk08 = 0, .Unk09 = 0, .Unk0A = 0, .Unk0B = 2, .Unk0C = 1, 
			.Unk0D = 0, .Unk0E = 0, .Unk0F = 0,
		}
	};
	da_append(&param->rows, row);
	da_append(&param->cells, cell);
	param->rowCount++;
}

void add_event_param_cutscene_trigger(PARAM *param, int32_t event_param_id) {
	Row row = {
		.ID = event_param_id, .garbage = 0, .Name.type = Str_Wide, .Name.as.str_wide = L"",
	};
	Cell cell = {
		.as.event_param = {
			.event_id = event_param_id, .flag_id = 0,
			.Unk08 = 0, .Unk09 = 0, .Unk0A = 0, .Unk0B = 2, .Unk0C = 1, 
			.Unk0D = 0, .Unk0E = 0, .Unk0F = 0,
		}
	};
	da_append(&param->rows, row);
	da_append(&param->cells, cell);
	param->rowCount++;
}


void add_event_loc_warpobj(PARAM *param, int32_t warp_point_begin, V3f pos, V3f rot, size_t insert_loc) {
	Row row = {
		.ID = warp_point_begin, .garbage = 0, .Name.type = Str_Wide, .Name.as.str_wide = L"",
	};
	Cell cell = {
		.as.event_loc = {
			.position = pos, .rotation = rot, .region_type = 1, .Unk19 = 0, .Unk1A = 255,
			.Unk1B = 0, .scale = (V3f){0.5f, 1.0f, 1.0f}, .Unk28 = 0, .Unk2A = 2560, .Unk2C = 0,
		}
	};
	da_insert(&param->rows, row, insert_loc);
	da_insert(&param->cells, cell, insert_loc);
	param->rowCount++;
}

Evaluator evaluator_1(String_Builder *bytecode_buf) {
	assert(bytecode_buf->capacity - bytecode_buf->count > 2);
	Evaluator e = {0};
	e.items =  (uint8_t *)(bytecode_buf->items + bytecode_buf->count);
	e.count = 2;
	da_append(bytecode_buf, 0x41); // 65 => 1
	da_append(bytecode_buf, 0xa1);
	return e;
}

Evaluator evaluator_condition_group(String_Builder *bytecode_buf, size_t group_id) {
	assert(bytecode_buf->capacity - bytecode_buf->count > 9);
	Evaluator e = {0};
	e.items = (uint8_t *)(bytecode_buf->items + bytecode_buf->count);
	e.count = 9;
	sb_append(bytecode_buf, 0x82);
	sb_append_sint32(bytecode_buf, 140001);
	sb_append(bytecode_buf, group_id + 64);
	sb_append(bytecode_buf, 0x85);
	sb_append(bytecode_buf, 0xa6);
	sb_append(bytecode_buf, 0xa1);
	return e;
}

Evaluator evaluator_get_event_flag(String_Builder *bytecode_buf, size_t flag) {
	assert(bytecode_buf->capacity - bytecode_buf->count > 9);
	Evaluator e = {0};
	e.items = (uint8_t *)(bytecode_buf->items + bytecode_buf->count);
	e.count = 15;
	sb_append(bytecode_buf, (char)EzS32);
	sb_append_sint32(bytecode_buf, 6);
	sb_append(bytecode_buf, (char)EzS32);
	sb_append_sint32(bytecode_buf, flag);
	sb_append(bytecode_buf, (char)EzFuncCall2);
	sb_append(bytecode_buf, (char)0+64);
	sb_append(bytecode_buf, (char)EzNeq);
	sb_append(bytecode_buf, (char)EzTry);
	sb_append(bytecode_buf, (char)EzFin);
	return e;
}

#define FunctionEventMessageDisplay 130432
#define FunctionCutsceneWarpEnded 132101
#define FunctionCompareObjStateId 131653
#define FunctionInGame 132202

Evaluator cond_wait_until_msg_is_displayed(String_Builder *bytecode_buf) {
	// binop => lhs rhs op
	assert(bytecode_buf->capacity - bytecode_buf->count > 10);
	Evaluator e = {0};
	e.items = (uint8_t *)(bytecode_buf->items + bytecode_buf->count);
	e.count = 10;
	sb_append(bytecode_buf, (char)EzS32);
	sb_append_sint32(bytecode_buf, FunctionEventMessageDisplay);
	sb_append(bytecode_buf, (char)EzFuncCall1);
	sb_append(bytecode_buf, (char)1+64);
	sb_append(bytecode_buf, (char)EzNeq);
	sb_append(bytecode_buf, (char)EzTry);
	sb_append(bytecode_buf, (char)EzFin);
	return e;
}

Evaluator cond_cutscene_warp_ended(String_Builder *bytecode_buf) {
	assert(bytecode_buf->capacity - bytecode_buf->count > 10);
	Evaluator e = {0};
	e.items = (uint8_t *)(bytecode_buf->items + bytecode_buf->count);
	e.count = 10;
	sb_append(bytecode_buf, (char)EzS32);
	sb_append_sint32(bytecode_buf, FunctionCutsceneWarpEnded);
	sb_append(bytecode_buf, (char)EzFuncCall1);
	sb_append(bytecode_buf, (char)0+64);
	sb_append(bytecode_buf, (char)EzNeq);
	sb_append(bytecode_buf, (char)EzTry);
	sb_append(bytecode_buf, (char)EzFin);
	return e;
}

Evaluator evaluator_compare_obj_state_id(String_Builder *bytecode_buf, size_t obj_id) {
	assert(bytecode_buf->capacity - bytecode_buf->count > 9);
	Evaluator e = {0};
	e.items = (uint8_t *)(bytecode_buf->items + bytecode_buf->count);
	e.count = 18;
	sb_append(bytecode_buf, (char)EzS32);
	sb_append_sint32(bytecode_buf, FunctionCompareObjStateId);
	sb_append(bytecode_buf, (char)EzS32);
	sb_append_sint32(bytecode_buf, obj_id);
	sb_append(bytecode_buf, (char)EzS32);
	sb_append_sint32(bytecode_buf, 100);
	sb_append(bytecode_buf, (char)1+64);
	sb_append(bytecode_buf, (char)EzFuncCall4);
	sb_append(bytecode_buf, (char)EzFin);
	return e;
}

Evaluator cond_in_game(String_Builder *bytecode_buf) {
	assert(bytecode_buf->capacity - bytecode_buf->count > 10);
	Evaluator e = {0};
	e.items = (uint8_t *)(bytecode_buf->items + bytecode_buf->count);
	e.count = 10;
	sb_append(bytecode_buf, (char)EzS32);
	sb_append_sint32(bytecode_buf, FunctionInGame);
	sb_append(bytecode_buf, (char)EzFuncCall1);
	sb_append(bytecode_buf, (char)0+64);
	sb_append(bytecode_buf, (char)EzNeq);
	sb_append(bytecode_buf, (char)EzTry);
	sb_append(bytecode_buf, (char)EzFin);
	return e;
}

size_t state_init(ESD *esd, size_t target_state_id, String_Builder *bytecode_buf) {
	State state = {0};
	state.id = 0;

	Condition cond = {0};
	cond.target_state_id = target_state_id;

	cond.evaluator = evaluator_1(bytecode_buf);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return esd->states.count - 1;
}

size_t create_arg_number(ESD *esd, int number) {
	size_t id = esd->args.count;
	Arg arg = {0};
	da_init(&arg, 2);
	da_append(&arg, number + 64);
	da_append(&arg, 0xa1);
	da_append(&esd->args, arg);
	return id;
}

size_t create_arg_s32(ESD *esd, int32_t val) {
	size_t id = esd->args.count;
	Arg arg = {0};
	da_init(&arg, 6);
	da_append(&arg, 0x82);
	da_append_many(&arg, &val, 4);
	// memcpy(arg.items + arg.count, &val, sizeof(int32_t));
	// arg.count += sizeof(int32_t);
	da_append(&arg, 0xa1);
	da_append(&esd->args, arg);
	return id;
}

Command command_is_obj_searched(ESD *esd, int group_id, int32_t obj_inst_id) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 140205;
	da_append(&comm.args_ids, create_arg_number(esd, group_id));
	da_append(&comm.args_ids, create_arg_s32(esd, obj_inst_id));
	return comm;
}

Command command_display_event_msg(ESD *esd, int32_t obj_inst_id) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 130447;
	da_append(&comm.args_ids, create_arg_s32(esd, obj_inst_id));
	da_append(&comm.args_ids, create_arg_number(esd, 0));
	da_append(&comm.args_ids, create_arg_number(esd, 0));
	da_append(&comm.args_ids, create_arg_number(esd, 0));
	return comm;
}

Command command_restart_machine() {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 130002;
	return comm;
}

Command command_end_machine() {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 130001;
	return comm;
}

Command command_disable_obj_keyguide(ESD *esd, int obj_inst_id) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 131638;
	da_append(&comm.args_ids, create_arg_s32(esd, obj_inst_id));
	da_append(&comm.args_ids, create_arg_number(esd, 1));
	return comm;
}

Command command_is_chr_dead_or_respawn_over(ESD *esd, int group_id, int id) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 140113;
	da_append(&comm.args_ids, create_arg_number(esd, group_id));
	da_append(&comm.args_ids, create_arg_s32(esd, id));
	da_append(&comm.args_ids, create_arg_number(esd, 1));
	return comm;
}

Command command_set_event_flag(ESD *esd, int flag) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 130101;
	da_append(&comm.args_ids, create_arg_s32(esd, flag));
	da_append(&comm.args_ids, create_arg_number(esd, 1));
	return comm;
}

Command command_compare_event_flag(ESD *esd, int group_id, int flag) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 140301;
	da_append(&comm.args_ids, create_arg_number(esd, group_id));
	da_append(&comm.args_ids, create_arg_s32(esd, flag));
	da_append(&comm.args_ids, create_arg_number(esd, 1));
	return comm;
}

Command command_disable_player_movement(ESD *esd) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 132243;
	da_append(&comm.args_ids, create_arg_number(esd, 1));
	return comm;
}

Command command_enable_obj_keyguide(ESD *esd, int obj_inst_id) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 131638;
	da_append(&comm.args_ids, create_arg_s32(esd, obj_inst_id));
	da_append(&comm.args_ids, create_arg_number(esd, 0));
	return comm;
}

Command command_play_cutscene_and_warp(ESD *esd, int map_id, int dst_id) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 132125;
	da_append(&comm.args_ids, create_arg_number(esd, 0));
	da_append(&comm.args_ids, create_arg_number(esd, 0));
	da_append(&comm.args_ids, create_arg_s32(esd, map_id));
	da_append(&comm.args_ids, create_arg_s32(esd, dst_id));
	da_append(&comm.args_ids, create_arg_number(esd, 0));
	return comm;
}

Command command_is_player_inside_point(ESD *esd, int group_id, int32_t trigger_id) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 140132;
	da_append(&comm.args_ids, create_arg_number(esd, group_id));
	da_append(&comm.args_ids, create_arg_s32(esd, trigger_id));
	da_append(&comm.args_ids, create_arg_s32(esd, trigger_id));
	da_append(&comm.args_ids, create_arg_number(esd, 1));
	return comm;
}

Command command_change_obj_state(ESD *esd, int32_t obj_id) {
	Command comm = {0};
	comm.command_bank = 1;
	comm.command_id = 131636;
	da_append(&comm.args_ids, create_arg_s32(esd, obj_id));
	da_append(&comm.args_ids, create_arg_s32(esd, 100));
	return comm;
}


size_t wait_until_interact_with_obj(ESD *esd, String_Builder *bytecode_buf, int id, int32_t target_state_id, int obj_inst_id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	int group_id = 0;
	Command comm = command_is_obj_searched(esd, group_id, obj_inst_id);

	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  evaluator_condition_group(bytecode_buf, group_id);

	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t check_if_wharf_bell_has_been_rang(ESD *esd, String_Builder *bytecode_buf, int id, int32_t target_if_true, int32_t target_if_false, int ship_event_flag) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	int group_id = 0;
	Command comm = command_compare_event_flag(esd, group_id, ship_event_flag);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	Condition cond = {0};
	cond.target_state_id = target_if_true;
	cond.evaluator =  evaluator_condition_group(bytecode_buf, group_id);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	Condition cond2 = {0};
	cond2.target_state_id = target_if_false;
	cond2.evaluator =  evaluator_1(bytecode_buf);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond2);

	da_append(&esd->states, state);
	return state_id;
}

size_t display_ring_bell_msg(ESD *esd, String_Builder *bytecode_buf, int id, int32_t target_state_id, int warp_obj_inst_id, int msg_id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command comm2 = command_disable_obj_keyguide(esd, warp_obj_inst_id);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm2);

	Command comm = command_display_event_msg(esd, msg_id);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  cond_wait_until_msg_is_displayed(bytecode_buf);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t restart_state(ESD *esd, int id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command comm = command_restart_machine();
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	da_append(&esd->states, state);
	return state_id;
}

size_t end_machine(ESD *esd, int id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command comm = command_end_machine();
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	da_append(&esd->states, state);
	return state_id;
}

size_t prepare_to_warp(ESD *esd, String_Builder *bytecode_buf, int id, int target_state_id, int obj_inst_id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command dok = command_disable_obj_keyguide(esd, obj_inst_id);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, dok);
	Command dpm = command_disable_player_movement(esd);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, dpm);

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  evaluator_1(bytecode_buf);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t enable_obj_keyguide(ESD *esd, String_Builder *bytecode_buf, int id, int target_state_id, int obj_inst_id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command comm = command_enable_obj_keyguide(esd, obj_inst_id);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  evaluator_1(bytecode_buf);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t wait_until_boss_is_dead(ESD *esd, String_Builder *bytecode_buf, int id, int target_state_id, int obj_inst_id, int boss_destruction_flag) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command comm = command_disable_obj_keyguide(esd, obj_inst_id);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);
	int group_id = 0;
	Command comm2 = command_compare_event_flag(esd, group_id, boss_destruction_flag);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm2);

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  evaluator_condition_group(bytecode_buf, group_id);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t wait_until_all_enemies_are_dead(ESD *esd, String_Builder *bytecode_buf, int id, int target_state_id, int obj_inst_id, FixedIntArray enemy_ids) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command comm = command_disable_obj_keyguide(esd, obj_inst_id);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);
	int group_id = 8;
	for (size_t i=0; i<enemy_ids.count; i++) {
		Command comm2 = command_is_chr_dead_or_respawn_over(esd, group_id, enemy_ids.items[i]);
		da_append(&state.entry_commands_ids, esd->commands.count);
		da_append(&esd->commands, comm2);
	}

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  evaluator_condition_group(bytecode_buf, group_id);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t check_if_boss_is_aggro(ESD *esd, String_Builder *bytecode_buf, int id, int target_if_true, int target_if_false, int hostility_flag) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	int group_id = 0;
	Command comm = command_compare_event_flag(esd, group_id, hostility_flag);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	Condition cond = {0};
	cond.target_state_id = target_if_true;
	cond.evaluator =  evaluator_condition_group(bytecode_buf, group_id);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	Condition cond2 = {0};
	cond2.target_state_id = target_if_false;
	cond2.evaluator =  evaluator_1(bytecode_buf);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond2);

	da_append(&esd->states, state);
	return state_id;
}

size_t check_if_flag_is_set(ESD *esd, String_Builder *bytecode_buf, int id, int target_if_true, int target_if_false, int flag) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Condition cond = {0};
	cond.target_state_id = target_if_true;
	cond.evaluator =  evaluator_get_event_flag(bytecode_buf, flag);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	Condition cond2 = {0};
	cond2.target_state_id = target_if_false;
	cond2.evaluator =  evaluator_1(bytecode_buf);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond2);

	da_append(&esd->states, state);
	return state_id;
}

size_t wait_until_event_flag(ESD *esd, String_Builder *bytecode_buf, int id, int target_state, int flag) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	int group_id = 0;
	Command comm = command_compare_event_flag(esd, group_id, flag);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	Condition cond = {0};
	cond.target_state_id = target_state;
	cond.evaluator =  evaluator_condition_group(bytecode_buf, group_id);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t set_event_flag(ESD *esd, String_Builder *bytecode_buf, int id, int target_state, int flag_id) {
	assert(flag_id != -1);
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command comm = command_set_event_flag(esd, flag_id);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	int group_id = 0;
	Command comm2 = command_compare_event_flag(esd, group_id, flag_id);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm2);

	Condition cond = {0};
	cond.target_state_id = target_state;
	cond.evaluator =  evaluator_condition_group(bytecode_buf, group_id);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t warp_to_location(ESD *esd, String_Builder *bytecode_buf, int id, int target_state_id, int map_id, int dst_id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command comm = command_play_cutscene_and_warp(esd, map_id, dst_id);
	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  cond_cutscene_warp_ended(bytecode_buf);
	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t is_player_inside_point(ESD *esd, String_Builder *bytecode_buf, int id, int target_state_id, int trigger_id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	int group_id = 0;
	Command comm = command_is_player_inside_point(esd, group_id, trigger_id);

	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  evaluator_condition_group(bytecode_buf, group_id);

	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t is_player_inside_game(ESD *esd, String_Builder *bytecode_buf, int id, int target_state_id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  cond_in_game(bytecode_buf);

	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

size_t create_change_obj_state(ESD *esd, String_Builder *bytecode_buf, int id, int target_state_id, int obj_id) {
	size_t state_id = esd->states.count;
	State state = {0};
	state.id = id;

	Command comm = command_change_obj_state(esd, obj_id);

	da_append(&state.entry_commands_ids, esd->commands.count);
	da_append(&esd->commands, comm);

	Condition cond = {0};
	cond.target_state_id = target_state_id;
	cond.evaluator =  evaluator_compare_obj_state_id(bytecode_buf, obj_id);

	da_append(&state.conditions_id, esd->conditions.count);
	da_append(&esd->conditions, cond);

	da_append(&esd->states, state);
	return state_id;
}

bool load_map_from_name(String_Builder *sb, MapName name, MSB2 *map) {
	const char *map_path = map_paths[name];
	if (!nob_read_entire_file(map_path, sb)) {
		return false;
	}
#ifdef LOG_GENERATED_FILES
	printf("[Read] %s: %zu bytes\n", map_path, sb->count);
#endif

	String_View sv = sb_to_sv(*sb);
	*map = load_map(map_path, sv);
	return true;
}

#define CONCAT_STR(str, b) str #b

bool write_map_from_name(String_Builder *wsb, MapName name, MSB2 *map) {
	if (!write_map(wsb, map) != 0) return false;
	const char *output_path = temp_sprintf(OUT_FOLDER_PATH"%s", map_paths[name]);
	if (!mkdir_if_not_exists(temp_sprintf(OUT_FOLDER_PATH"map/%s", map_names[name]))) return false;
	if (!write_entire_file(output_path, wsb->items, wsb->count)) return false;
#ifdef LOG_GENERATED_FILES
	printf("[Generated] %s\n", output_path);
#endif
	free_map(map);
	return true;
}


bool load_obj_inst_param_from_map_name(String_Builder *sb, MapName name, PARAM *param) {
	const char *param_path = obj_inst_param_paths[name];
	if (!nob_read_entire_file(param_path, sb)) {
		return false;
	}
#ifdef LOG_GENERATED_FILES
	printf("[Read] %s: %zu bytes\n", param_path, sb->count);
#endif
	String_View sv = sb_to_sv(*sb);
	*param = load_param(&sv, Cell_MapObjInstanceParam);
	return true;
}

bool load_event_param_from_map_name(String_Builder *sb, MapName name, PARAM *param) {
	if (!nob_read_entire_file(event_param_paths[name], sb)) {
		return false;
	}
#ifdef LOG_GENERATED_FILES
	printf("[Read] %s: %zu bytes\n", event_param_paths[name], sb->count);
#endif
	String_View sv = sb_to_sv(*sb);
	*param = load_param(&sv, Cell_EventParam);
	return true;
}

bool load_event_loc_from_map_name(String_Builder *sb, MapName name, PARAM *param) {
	if (!nob_read_entire_file(event_loc_paths[name], sb)) {
		return false;
	}
#ifdef LOG_GENERATED_FILES
	printf("[Read] %s: %zu bytes\n", event_loc_paths[name], sb->count);
#endif
	String_View sv = sb_to_sv(*sb);
	*param = load_param(&sv, Cell_EventLoc);
	return true;
}

bool write_param_from_map_name(String_Builder *wsb, const char *output_path, PARAM *param) {
	write_param(wsb, param);
	if (!write_entire_file(output_path, wsb->items, wsb->count)) return false;
#ifdef LOG_GENERATED_FILES
	printf("[Generated] %s\n", output_path);
#endif
	free_PARAM(param);
	return true;
}


bool load_esd_from_map_name(String_Builder *sb, MapName name, ESD *esd) {
	if (!nob_read_entire_file(esd_paths[name], sb)) {
		return false;
	}
#ifdef LOG_GENERATED_FILES
	printf("[Read] %s: %zu bytes\n", esd_paths[name], sb->count);
#endif
	String_View sv = sb_to_sv(*sb);
	*esd = read_esd(sv);
	return true;
}

bool write_esd_from_map_name(String_Builder *wsb, MapName name, ESD *esd) {
	write_esd(wsb, esd);
	const char *output_path = temp_sprintf(OUT_FOLDER_PATH"%s", esd_paths[name]);
	if (!write_entire_file(output_path, wsb->items, wsb->count)) return false;
#ifdef LOG_GENERATED_FILES
	printf("[Generated] %s\n", output_path);
#endif
	free_esd(esd);
	return true;
}

bool str_wchar_eq(wchar_t *ws, const char *s) {
	const char *ws_cstr = (char *)ws;
	int n = strlen(s);
	for (int i=0; i<n; i++) {
		if (ws_cstr[2*i] != s[i]) return false;
		if (ws_cstr[2*i+1] != 0) return false;
	}
	return true;
}

#define VendrickHostilityFlag 224000088
#define GiantLordDefeatFlag 100972
#define ShipGlobalEventFlag 102002
#define ShipArrivalLocalFlag  118000010
#define AlsannaTalkFlag 537000011

void esd_add_ship_check_fog_gate_event(ESD *esd, String_Builder *bytecode_buf, size_t event_param_id) {
	StateGroup sg = {0};
	sg.id = event_param_id;
	size_t state_id = 1;
	da_append(&sg.states_ids, state_init(esd, esd->states.count + 1, bytecode_buf)); 
	da_append(&sg.states_ids, check_if_flag_is_set(esd, bytecode_buf, state_id++, esd->states.count + 1, esd->states.count + 3, ShipArrivalLocalFlag)); 
	da_append(&sg.states_ids, set_event_flag(esd, bytecode_buf, state_id++, esd->states.count + 1, ShipGlobalEventFlag)); 
	da_append(&sg.states_ids, end_machine(esd, state_id++)); 
	da_append(&sg.states_ids, restart_state(esd, state_id++)); 
	da_append(&esd->state_groups, sg);
}

void esd_add_dlc3_unfreeze_event_script(ESD *esd, String_Builder *bytecode_buf, size_t event_param_id) {
	StateGroup sg = {0};
	sg.id = event_param_id;
	size_t state_id = 1;
	da_append(&sg.states_ids, state_init(esd, esd->states.count + 1, bytecode_buf)); 
	da_append(&sg.states_ids, wait_until_event_flag(esd, bytecode_buf, state_id++, esd->states.count + 1, boss_destruction_flags[BurntIvoryKing])); 
	da_append(&sg.states_ids, check_if_flag_is_set(esd, bytecode_buf, state_id++, esd->states.count + 2, esd->states.count + 1, AlsannaTalkFlag)); 
	da_append(&sg.states_ids, set_event_flag(esd, bytecode_buf, state_id++, esd->states.count + 1, AlsannaTalkFlag)); 
	da_append(&sg.states_ids, restart_state(esd, state_id++)); 
	da_append(&esd->state_groups, sg);
}

void add_esd_event(ESD *esd, String_Builder *bytecode_buf, WarpNode current_node, WarpNode dst_node,
		MapName dst_map_name_id, size_t event_param_id, size_t warp_obj_inst_id, size_t dst_loc_id, BossName boss, int ship_arrival_msg_id) 
{
	StateGroup sg = {0};
	sg.id = event_param_id;
	size_t state_id = 1;
	da_append(&sg.states_ids, state_init(esd, esd->states.count + 1, bytecode_buf)); 
	FixedIntArray enemy_ids = get_chasm_door_enemy_ids(current_node);
	if (boss != Boss_None) {
		if (boss == VendrickBoss) {
			da_append(&sg.states_ids, check_if_boss_is_aggro(esd, bytecode_buf, state_id++, esd->states.count + 1, esd->states.count + 3, VendrickHostilityFlag)); 
		}
		da_append(&sg.states_ids, wait_until_boss_is_dead(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id, boss_destruction_flags[boss])); 
		if (boss == ThroneWatcherAndDefender) {
			da_append(&sg.states_ids, check_if_flag_is_set(esd, bytecode_buf, state_id++, esd->states.count + 1, esd->states.count + 2, GiantLordDefeatFlag)); 
			da_append(&sg.states_ids, restart_state(esd, state_id++)); 
		}
		da_append(&sg.states_ids, enable_obj_keyguide(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id)); 
	} else if (enemy_ids.count > 0) {
		da_append(&sg.states_ids, wait_until_all_enemies_are_dead(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id, enemy_ids)); 
		da_append(&sg.states_ids, enable_obj_keyguide(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id)); 
	}
	da_append(&sg.states_ids, wait_until_interact_with_obj(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id)); 
	if (boss == VendrickBoss) {
		da_append(&sg.states_ids, check_if_boss_is_aggro(esd, bytecode_buf, state_id++, esd->states.count + 1, esd->states.count + 4, VendrickHostilityFlag)); 
		da_append(&sg.states_ids, wait_until_boss_is_dead(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id, boss_destruction_flags[boss])); 
		da_append(&sg.states_ids, enable_obj_keyguide(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id)); 
		da_append(&sg.states_ids, wait_until_interact_with_obj(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id)); 
	}
	if (enemy_ids.count > 0) {
		da_append(&sg.states_ids, set_event_flag(esd, bytecode_buf, state_id++, esd->states.count + 1, get_chasm_event_flag(current_node))); 
	}
	if (is_bell_check_required(dst_node)) {
		da_append(&sg.states_ids, check_if_wharf_bell_has_been_rang(esd, bytecode_buf, state_id++, esd->states.count + 4, esd->states.count + 1, ShipGlobalEventFlag)); 
		da_append(&sg.states_ids, display_ring_bell_msg(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id, ship_arrival_msg_id)); 
		da_append(&sg.states_ids, enable_obj_keyguide(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id)); 
		da_append(&sg.states_ids, restart_state(esd, state_id++)); 
	}
	da_append(&sg.states_ids, prepare_to_warp(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id)); 
	da_append(&sg.states_ids, warp_to_location(esd, bytecode_buf, state_id++, esd->states.count + 1, map_ids[dst_map_name_id], dst_loc_id)); 
	da_append(&sg.states_ids, enable_obj_keyguide(esd, bytecode_buf, state_id++, esd->states.count + 1, warp_obj_inst_id)); 
	da_append(&sg.states_ids, restart_state(esd, state_id++)); 
	da_append(&esd->state_groups, sg);
}

void add_esd_cutscene_trigger(ESD *esd, String_Builder *bytecode_buf, size_t ep_id, size_t trigger_id, size_t fog_obj_inst_id) {
	StateGroup sg = {0};
	sg.id = ep_id;
	size_t state_id = 1;
	da_append(&sg.states_ids, state_init(esd, esd->states.count + 1, bytecode_buf)); 
	da_append(&sg.states_ids, is_player_inside_game(esd, bytecode_buf, state_id++, esd->states.count + 1)); 
	da_append(&sg.states_ids, is_player_inside_point(esd, bytecode_buf, state_id++, esd->states.count + 1, trigger_id)); 
	da_append(&sg.states_ids, create_change_obj_state(esd, bytecode_buf, state_id++, esd->states.count + 1, fog_obj_inst_id)); 
	da_append(&sg.states_ids, restart_state(esd, state_id++)); 
	da_append(&esd->state_groups, sg);
}

int compr_moip_id(const void *a, const void *b) {
    return ((Row *)a)->ID - ((Row *)b)->ID;
}

// Returns the starting position where n new unique IDs can fit
// or -1 if no suitable gap exists
Pair find_row_id_gap(Rows rows, int n) {
    if (rows.count == 0) return (Pair){-1, -1};
    
    qsort(rows.items, rows.count, sizeof(Row), compr_moip_id);
    
    for (size_t i = 0; i < rows.count - 1; i++) {
        int gap_size = rows.items[i + 1].ID - rows.items[i].ID - 1;
        if (gap_size >= n) {
            return (Pair){rows.items[i].ID + 1, i + 1};
        }
    }
    
    return (Pair){-1, -1};
}

size_t compute_n_warp_points(MapName name) {
	size_t count = 0;
	for (size_t i=0; i<ARRAY_LEN(warp_points); i++) {
		WarpPoint wp = warp_points[i];
		if (wp.map != name) continue;
		if (!wp.is_foggate) continue;
		count++;
	}
	return count;
}

typedef struct {
	String_Builder sb;
	ESD esd;
} ESD_Data;

void free_esd_data(ESD_Data *data) {
	free_esd(&data->esd);
	da_free(data->sb);
}

DA(ESD_Data, DA_ESD_Data);

typedef Ht(int, bool) HashSet;

typedef struct {
	String_Builder map_sb;
	String_Builder oip_sb;
	String_Builder ep_sb;
	String_Builder el_sb;
	DA_ESD_Data esd_sbs;

	String_Builder write_buf;
	String_Builder bytecode_buf;
} Editor;

void free_editor(Editor *editor) {
	da_free(editor->map_sb);
	da_free(editor->oip_sb);
	da_free(editor->ep_sb);
	da_free(editor->el_sb);
	da_foreach(ESD_Data, esd_sb, &editor->esd_sbs) {
		da_free(esd_sb->sb);
	}
	// ESD is freed when writing to output
	da_free(editor->esd_sbs);
	da_free(editor->write_buf);
	da_free(editor->bytecode_buf);
}

#define MAP_EVENT_TEXT_LOCATION "menu/text/english/mapevent.fmg"

int bell_ring_param_id = -1;
int unfreeze_param_id = -1;
int ship_arrival_msg_id = -1;

bool generate_map_objects(Editor *editor) {
	Ht(size_t, bool) completed_ids = {0};
	Part warp_part = {0};
	Entry warp_model = {0};
	const char warp_part_name_buf[128];
	const char warp_model_name_buf[128];
	String_Builder name_buf = {0};
	da_init(&name_buf, 1024);
	da_init(&editor->bytecode_buf, 10*1024*1024); // 10mb for holding bytecode
	if (!mkdir_if_not_exists(OUT_FOLDER_PATH"map")) return false;
	if (!mkdir_if_not_exists(OUT_FOLDER_PATH"Param/")) return false;
	for (MapName map_name_id=ThingsBetwixt; map_name_id<__MapNameCount; map_name_id++) {
		printf("Loading maps %d/%d\r", map_name_id, __MapNameCount-1);
		ht_reset(&completed_ids);
		name_buf.count = 0;
		editor->map_sb.count = 0;
		editor->oip_sb.count = 0;
		editor->ep_sb.count = 0;
		editor->el_sb.count = 0;
		size_t mark = temp_save();
		// String_Builder memory_buf = {0};
		// da_init(&memory_buf, 2048);
		MSB2 map;
		if (!load_map_from_name(&editor->map_sb, map_name_id, &map)) return false;
		V3f pos_offs_event_loc = map.events.MapOffsets.count > 0 ?\
								 map.events.MapOffsets.items[0].type_data.map_offset.translation :\
								 (V3f){0, 0, 0};
		int model_index = -1;
		if (map_name_id == ThingsBetwixt) {
			for (size_t j=0; j<map.parts.Objects.count; j++) {
				Part *part = &map.parts.Objects.items[j];
				if (wcscmp(part->name, L""WARP_OBJ_NAME) == 0) {
					part_copy(&warp_part, part, warp_part_name_buf, sizeof(warp_part_name_buf));
					break;
				}
			}
			for (size_t j=0; j<map.models.Objects.count; j++) {
				Entry *model = &map.models.Objects.items[j];
				if (wcscmp(model->name, L""WARP_OBJ_MODEL_NAME) == 0) {
					entry_copy(&warp_model, model, warp_model_name_buf, sizeof(warp_model_name_buf));
					model_index = j + map.models.MapPieces.count;
					break;
				}
			}
		} else {
			assert(warp_model.name != NULL);
			model_index = map.models.Objects.count + map.models.MapPieces.count;
			da_append(&map.models.Objects, warp_model);

			// after adding new objects the other references must be updated
			for (size_t k=0; k<map.parts.MapPieces.count; k++) {
				Part *part = &map.parts.MapPieces.items[k];
				if (part->part.model_index >= model_index) part->part.model_index += 1;
			}
			for (size_t k=0; k<map.parts.Objects.count; k++) {
				Part *part = &map.parts.Objects.items[k];
				if (part->part.model_index >= model_index) part->part.model_index += 1;
			}
			for (size_t k=0; k<map.parts.Collisions.count; k++) {
				Part *part = &map.parts.Collisions.items[k];
				if (part->part.model_index >= model_index) part->part.model_index += 1;
			}
			for (size_t k=0; k<map.parts.ConnectCollisions.count; k++) {
				Part *part = &map.parts.ConnectCollisions.items[k];
				if (part->part.model_index >= model_index) part->part.model_index += 1;
			}
			for (size_t k=0; k<map.parts.Navmeshes.count; k++) {
				Part *part = &map.parts.Navmeshes.items[k];
				if (part->part.model_index >= model_index) part->part.model_index += 1;
			}
		}
		assert(model_index != -1);
		assert(warp_model.name != NULL);
		assert(warp_part.name != NULL);
		// disabling the visibility of warp_part
		for (size_t i=0; i<ARRAY_LEN(warp_part.part.draw_groups); i++) {
			warp_part.part.draw_groups[i] = 0;
		}
		size_t name_id = 1001; // hardcoded should not break as far as I have tested

		PARAM obj_inst_param, ev_param, el_param;

		if (!load_obj_inst_param_from_map_name(&editor->oip_sb, map_name_id, &obj_inst_param)) return false;

		size_t n_warp_points = compute_n_warp_points(map_name_id);
		Pair pair = find_row_id_gap(obj_inst_param.rows, n_warp_points);
		size_t warp_obj_inst_id = pair.from;
		size_t oip_insert_loc = pair.to;

		assert(warp_obj_inst_id >= 0);
		assert(oip_insert_loc >= 0);

		size_t total_event_count = n_warp_points + CUTSCENES_COUNT;
		if (map_name_id == NomansWharf || map_name_id == FrozenEleumLoyce) total_event_count++;

		if (!load_event_param_from_map_name(&editor->ep_sb, map_name_id, &ev_param)) return false;
		size_t event_param_id = find_row_id_gap(ev_param.rows, total_event_count).from;

		assert(event_param_id >= 0);

		if (!load_event_loc_from_map_name(&editor->el_sb, map_name_id, &el_param)) return false;
		pair = find_row_id_gap(el_param.rows, n_warp_points);
		size_t warp_point_begin = pair.from;
		size_t el_insert_loc = pair.to;

		assert(warp_point_begin >= 0);
		assert(el_insert_loc >= 0);

		ESD esd_orig = {0};
		String_Builder esd_sb_orig = {0};

		if (!nob_read_entire_file(esd_paths[map_name_id], &esd_sb_orig)) {
			return false;
		}
#ifdef LOG_GENERATED_FILES
		printf("[Read] %s: %zu bytes\n", esd_paths[name], sb->count);
#endif
		da_append(&editor->esd_sbs, ((ESD_Data){esd_sb_orig, esd_orig}));

		// get the instance id from map name
		for (size_t i=0; i<ARRAY_LEN(warp_points); i++) {
			WarpPoint *wp = &warp_points[i];
			if (!wp->is_foggate) continue;
			if (wp->map != map_name_id) continue;
			da_foreach(Part, part, &map.parts.Objects) {
				if (str_wchar_eq(part->name, wp->obj_id)) {
					wp->fog_gate_id = part->type_data.object.map_object_instance_param_id;
				}
			}
		}

		// create and add event for checking ringing of bell
		if (map_name_id == NomansWharf) {
			bell_ring_param_id = event_param_id++;
			Row row = {
				.ID = bell_ring_param_id, .garbage = 0, .Name.type = Str_Wide, .Name.as.str_wide = L"",
			};
			Cell cell = {
				.as.event_param = {
					.event_id = bell_ring_param_id, .flag_id = 0,
					.Unk08 = 0, .Unk09 = 0, .Unk0A = 0, .Unk0B = 2, .Unk0C = 1, 
					.Unk0D = 0, .Unk0E = 0, .Unk0F = 0,
				}
			};
			da_append(&ev_param.rows, row);
			da_append(&ev_param.cells, cell);
			ev_param.rowCount++;
		} else if (map_name_id == FrozenEleumLoyce) { // create and add event for unfreezing the dlc3
			unfreeze_param_id = event_param_id++;
			Row row = {
				.ID = unfreeze_param_id, .garbage = 0, .Name.type = Str_Wide, .Name.as.str_wide = L"",
			};
			Cell cell = {
				.as.event_param = {
					.event_id = unfreeze_param_id, .flag_id = 0,
					.Unk08 = 0, .Unk09 = 0, .Unk0A = 0, .Unk0B = 2, .Unk0C = 1, 
					.Unk0D = 0, .Unk0E = 0, .Unk0F = 0,
				}
			};
			da_append(&ev_param.rows, row);
			da_append(&ev_param.cells, cell);
			ev_param.rowCount++;
		}

		for (size_t i=0; i<ARRAY_LEN(warp_points); i++) {
			WarpPoint *wp = &warp_points[i];
			if (wp->map != map_name_id) continue;
			if (!wp->is_foggate) continue;
			Part *fog_gate = NULL;
			bool *found = NULL;
			for (size_t j=0; j<map.parts.Objects.count; j++) {
				Part *p = &map.parts.Objects.items[j];
				if (str_wchar_eq(p->name, wp->obj_id)) {
					found = ht_find(&completed_ids, j);
					if (found == NULL) {
						*ht_put(&completed_ids, j) = true;
					}
					fog_gate = p;
					break;
				}
			}

			assert(fog_gate != NULL && "Could not find the foggate name in the map");

			// if it is a pvp gate set its param id to 100250 and 0x18 Unk04 to 6;
			if (wp->pvp) {
				for(size_t k=0; k<obj_inst_param.rows.count; k++) {
					Row *row = &obj_inst_param.rows.items[k];
					if (row->ID == wp->fog_gate_id) {
						Cell *cell = &obj_inst_param.cells.items[k];
						cell->as.map_obj_instance_param.headerID = 100450;
						cell->as.map_obj_instance_param.Unk04 = 6;
						break;
					}
				}
			}
			for (size_t i=0; i<ARRAY_LEN(warp_part.part.disp_groups); i++) {
				warp_part.part.disp_groups[i] = fog_gate->part.draw_groups[i];
			}

			V3f pos = v3f_add(wp->offset, fog_gate->part.position);
			V3f rot = wp->reverse ? v3f_flip_y(fog_gate->part.rotation) : fog_gate->part.rotation;

			// adjust the boss spawn event loc to the warp point if the boss should spawn
			int event_loc_id = get_boss_spawn_event_loc(i);
			if (event_loc_id != -1) {
				for (size_t k=0; k<el_param.rows.count; k++) {
					Row *row = &el_param.rows.items[k];
					Cell *cell = &el_param.cells.items[k];
					if (row->ID == event_loc_id) {
						cell->as.event_loc.position.x = fog_gate->part.position.x - pos_offs_event_loc.x;
						cell->as.event_loc.position.z = fog_gate->part.position.z - pos_offs_event_loc.z;
						cell->as.event_loc.scale.x = 1.0f;
						cell->as.event_loc.scale.z = 1.0f;
						cell->as.event_loc.position = v3f_move(cell->as.event_loc.position, rot, i == DarkLurkerExitFront ? 2.0f: -2.0f);
					}
				}
			}

			// if it is front side
			if (!found) {
				size_t woi_id1 = warp_obj_inst_id++;
				da_append(&map.parts.Objects, warp_part_new(&warp_part, pos, rot, woi_id1, name_id++, &name_buf, model_index));

				// add the object instance for the warp part
				add_map_obj_instance_param_warpobj(&obj_inst_param, woi_id1, oip_insert_loc++);

				// create and add new event param for infront and back of fog door
				size_t ep_id1 = event_param_id++;
				add_event_param_warpobj(&ev_param, ep_id1, woi_id1);

				// create and add the new warp points behind and front of the fog door
				size_t wp2 = warp_point_begin++;
				add_event_loc_warpobj(&el_param, wp2, v3f_move(v3f_sub(pos, pos_offs_event_loc), rot, 1.0f), v3f_flip_y(rot), el_insert_loc++);

				wp->warp_pt_id = wp2;
				wp->event_param_id = ep_id1;
				wp->warp_obj_inst_id = woi_id1;
			} else { // if it is back size
				size_t woi_id2 = warp_obj_inst_id++;
				da_append(&map.parts.Objects, warp_part_new(&warp_part, pos, v3f_flip_y(rot), woi_id2, name_id++, &name_buf, model_index));
				add_map_obj_instance_param_warpobj(&obj_inst_param, woi_id2, oip_insert_loc++);
				size_t ep_id2 = event_param_id++;
				add_event_param_warpobj(&ev_param, ep_id2, woi_id2);
				size_t wp1 = warp_point_begin++;
				add_event_loc_warpobj(&el_param, wp1, v3f_move(v3f_sub(pos, pos_offs_event_loc), rot, -1.0f), rot, el_insert_loc++);

				wp->warp_pt_id = wp1;
				wp->event_param_id = ep_id2;
				wp->warp_obj_inst_id = woi_id2;

				if (wp->boss != Boss_None && point_has_cutscene(i)) {
					wp->cutscene_event_id = event_param_id++;
					add_event_param_cutscene_trigger(&ev_param, wp->cutscene_event_id);
				}

			}
		}

		for (size_t i=0; i<ARRAY_LEN(warp_points); i++) {
			WarpPoint *wp = &warp_points[i];
			if (wp->map != map_name_id) continue;
			if (!wp->is_foggate) continue;
			if (wp->boss == Boss_None) continue;
			if (wp->cutscene_gate_id == __WarpNodeCount) continue;
			wp->cutscene_event_id = event_param_id++;
			add_event_param_cutscene_trigger(&ev_param, wp->cutscene_event_id);
		}

		editor->write_buf.count = 0;
		if (!write_map_from_name(&editor->write_buf, map_name_id, &map)) return false;
		const char *output_path = temp_sprintf(OUT_FOLDER_PATH"%s", obj_inst_param_paths[map_name_id]);
		editor->write_buf.count = 0;
		if (!write_param_from_map_name(&editor->write_buf, output_path, &obj_inst_param)) return false;
		output_path = temp_sprintf(OUT_FOLDER_PATH"%s", event_param_paths[map_name_id]);
		editor->write_buf.count = 0;
		if (!write_param_from_map_name(&editor->write_buf, output_path, &ev_param)) return false;
		output_path = temp_sprintf(OUT_FOLDER_PATH"%s", event_loc_paths[map_name_id]);
		editor->write_buf.count = 0;
		if (!write_param_from_map_name(&editor->write_buf, output_path, &el_param)) return false;
		temp_rewind(mark);
	}
	printf("\n");
	ht_free(&completed_ids);

	if (!add_pirate_ship_msg_to_game_files(MAP_EVENT_TEXT_LOCATION, OUT_FOLDER_PATH MAP_EVENT_TEXT_LOCATION, &ship_arrival_msg_id)) return 1;

	// initialize the pool of foggates
	// da_init(&editor->pool, AllFogGatesEnd - AllFogGatesStart + 1);
	// generate all the possible connections
	// generate_connections();

	return true;
}

bool randomize_warps(Editor *editor) {
	int seed = 2496; // TODO: set seed dynamically
	// int seed = time(NULL);
	srand(seed);
	for (MapName map_name_id=ThingsBetwixt; map_name_id<__MapNameCount; map_name_id++) {
		size_t mark = temp_save();
		
		String_View sv = sb_to_sv(editor->esd_sbs.items[map_name_id].sb);
		editor->esd_sbs.items[map_name_id].esd = read_esd(sv);

		// create and add event for checking ringing of bell
		if (map_name_id == NomansWharf) {
			assert(bell_ring_param_id != -1);
			esd_add_ship_check_fog_gate_event(&editor->esd_sbs.items[map_name_id].esd, &editor->bytecode_buf, bell_ring_param_id);
		} else if (map_name_id == FrozenEleumLoyce) { // create and add event for unfreezing the dlc3
			assert(unfreeze_param_id != -1);
			esd_add_dlc3_unfreeze_event_script(&editor->esd_sbs.items[map_name_id].esd, &editor->bytecode_buf, unfreeze_param_id);
		}

		temp_rewind(mark);
	}
	assert(ship_arrival_msg_id != -1);

	// TODO: randomize the connections here

	for (WarpNode i=FogGateStart; i<=FogGateEnd; i++) {
		printf("Adding warps %d/%d\r", i, FogGateEnd);
		WarpNode j = default_connections[i];

		if (i == Tutorial1EntryFront) j = IvoryKingBack;
		else if (i == IvoryKingBack) j = Tutorial1EntryFront;

		WarpPoint *wp1 = &warp_points[i];
		WarpPoint *wp2 = &warp_points[j];
		ESD *esd_src = &editor->esd_sbs.items[wp1->map].esd;
		ESD *esd_dst = &editor->esd_sbs.items[wp2->map].esd;

		if (wp2->boss != Boss_None && point_has_cutscene(j)) {
			assert(wp2->cutscene_event_id >= 0);
			add_esd_cutscene_trigger(esd_dst, &editor->bytecode_buf, wp2->cutscene_event_id, wp2->warp_pt_id, wp2->fog_gate_id);
		} 
		if (wp1->boss != Boss_None && wp1->cutscene_gate_id != __WarpNodeCount) {
			WarpPoint *wn = &warp_points[wp1->cutscene_gate_id];
			assert(wp1->cutscene_event_id >= 0);
			add_esd_cutscene_trigger(esd_dst, &editor->bytecode_buf, wp1->cutscene_event_id, wp1->warp_pt_id, wn->fog_gate_id);
		}

		add_esd_event(esd_src, &editor->bytecode_buf, i, j, wp2->map, wp1->event_param_id, wp1->warp_obj_inst_id, wp2->warp_pt_id, wp1->boss, ship_arrival_msg_id);
		add_esd_event(esd_dst, &editor->bytecode_buf, j, i, wp1->map, wp2->event_param_id, wp2->warp_obj_inst_id, wp1->warp_pt_id, wp2->boss, ship_arrival_msg_id);
	}
	printf("\n");

	if (!mkdir_if_not_exists(OUT_FOLDER_PATH"ezstate/")) return false;
	for (MapName map_name_id=ThingsBetwixt; map_name_id<__MapNameCount; map_name_id++) {
		editor->write_buf.count = 0;
		printf("Saving maps %d/%d\r", map_name_id, __MapNameCount-1);
		if (!write_esd_from_map_name(&editor->write_buf, map_name_id, &editor->esd_sbs.items[map_name_id].esd)) return false;
	}
	printf("\n");

	return true;
}

typedef struct {
	uint64_t found_keys;
	size_t branches_count;
	size_t lockstone_count;
} Inventory;

void inventory_add_key(Inventory *i, ConnectionType key) {
	if (key == Conn_FragrantBranchOfYore) i->branches_count++;
	else if (key == Conn_PharrosLockstone) i->lockstone_count++;
	i->found_keys |= key;
}

bool is_key_consumable(ConnectionType key) {
	if (key == Conn_FragrantBranchOfYore || key == Conn_PharrosLockstone) return true;
	return false;
}

bool inventory_has_key(Inventory *i, ConnectionType key) {
	if (!is_key_consumable(key)) {
		return key & i->found_keys;
	}
	return false;
}

void generate_all_nodes(HashSet *hs) {
	ht_reset(hs);
	for (size_t i=AllConnectionsStart; i<=AllConnectionsEnd; i++) {
		*ht_put(hs, i) = true;
	}
}

void generate_all_foggates(DArrz *arr) {
	arr->count = 0;
	if (arr->capacity != AllFogGatesCount) da_init(arr, AllFogGatesCount);
	for (size_t i=AllFogGatesStart; i<=AllFogGatesEnd; i++) {
		da_append(arr, i);
	}
}

int da_idx_of(DArrz *arr, size_t value) {
	for (size_t i=0; i<arr->count; i++) {
		if (arr->items[i] == value) return i;
	}
	return -1;
}

DA(bool, DBool);

// Helper to run a BFS from a given node and discover all newly reachable fog gates
void explore_from_node(size_t start_node, HashSet *visited, DArrz *queue, size_t *head, DArrz *fog_gates, DBool *gate_reached, Inventory *inv, HashSet *locked_nodes) {
    // Only queue up if we haven't visited it yet
    if (ht_find(visited, (int)start_node) == NULL) {
        ht_put(visited, (int)start_node);
        da_append(queue, start_node);
    }

    while (*head < queue->count) {
        size_t curr = queue->items[(*head)++];

        // If the current node is a fog gate, mark it as reached
        for (size_t i = 0; i < fog_gates->count; i++) {
            if (fog_gates->items[i] == curr && !gate_reached->items[i]) {
                gate_reached->items[i] = true;
            }
        }

        // Traverse all adjacent fixed paths and current warp connections
        ConnectionGroup cg = connections.items[curr];
        for (size_t i = 0; i < cg.count; i++) {
			Connection next = cg.items[i];
            size_t next_node = (size_t)next.to;
			if (next.type != Conn_None) {
				if (!inventory_has_key(inv, next.type)) {
					// TODO: add to a list of nodes to check after getting a key
					if (ht_find(visited, (int)next_node) == NULL) {
						if (ht_find(locked_nodes, (size_t)next.to) == NULL) {
							ht_put(locked_nodes, (size_t)next.to);
						}
					}
					continue;
				}
			}
            if (ht_find(visited, (int)next_node) == NULL) {
                ht_put(visited, (int)next_node);
                da_append(queue, next_node);
            }
        }
    }
}

typedef struct {
	DArrz reached_unpaired;
	DArrz unreached_unpaired;
    HashSet visited;
    DArrz queue;
	DArrz matching;
    DBool gate_reached;
    DArrz fog_gates;
	DArrz leftover;
	Inventory inventory;
	HashSet locked_nodes;
} Randomizer;

void randomizer_free(Randomizer *r) {
	da_free(r->reached_unpaired);
	da_free(r->unreached_unpaired);
	ht_free(&r->visited);
	da_free(r->queue);
	da_free(r->matching);
	da_free(r->gate_reached);
	da_free(r->fog_gates);
	da_free(r->leftover);
	ht_free(&r->locked_nodes);
}

void get_connections(Connections *c) {
    if (connections.count == 0) generate_connections();
	if (c->capacity == 0) {
		da_init(c, connections.count);
		memset(c->items, 0, c->capacity*sizeof(*c->items));
	}
	for (size_t i=0; i<connections.count; i++) {
		ConnectionGroup *cg_orig = &connections.items[i];
		ConnectionGroup *cg = &c->items[i];
		if (cg->capacity < cg_orig->capacity) {
			da_init(cg, cg_orig->capacity + 1);
		}
		cg->count = 0;
		da_append_many(cg, cg_orig->items, cg_orig->count);
		c->count++;
	}
}

void free_connections_by_ref(Connections *c) {
	for (size_t i=0; i<c->capacity; i++) {
		da_free(c->items[i]);
	}
	da_free(*c);
}

// Attempts to create a fully connected random matching of all fog gates
bool attempt_randomization(Randomizer *r, Connections *c) {
    // 1. Reset and generate base fixed connections
	get_connections(c);
	memset(&r->inventory, 0, sizeof(r->inventory));
	r->locked_nodes.count = 0;

    // 2. Fetch all gates that need randomizing
	r->fog_gates.count = 0;
    generate_all_foggates(&r->fog_gates);

    if (r->fog_gates.count == 0) {
		r->fog_gates.count = 0;
        return true; // Nothing to randomize!
    }

    // Tracks which gate index maps to which gate index (-1 means unpaired)
	if (r->matching.capacity != r->fog_gates.count) da_init(&r->matching, r->fog_gates.count);
	if (r->gate_reached.capacity != r->fog_gates.count) da_init(&r->gate_reached, r->fog_gates.count);
	r->matching.count = 0;
	r->gate_reached.count = 0;
	memset(r->gate_reached.items, 0, sizeof(*r->gate_reached.items)*r->gate_reached.capacity);
    for (size_t i = 0; i < r->fog_gates.count; i++) {
        r->matching.items[i] = (size_t)-1;
    }

    // Set up our traversal trackers
	ht_reset(&r->visited);
	r->queue.count = 0;
    size_t queue_head = 0;

    // Discover what is initially reachable from the spawn point via fixed paths
    explore_from_node((size_t)GameStartSpawnSrc, &r->visited, &r->queue, &queue_head, &r->fog_gates, &r->gate_reached, &r->inventory, &r->locked_nodes);

    // 3. Keep bridging reached components to unreached components
    while (true) {
		r->reached_unpaired.count = 0;
		r->unreached_unpaired.count = 0;

        // Classify our unpaired gates
        for (size_t i = 0; i < r->fog_gates.count; i++) {
            if (r->matching.items[i] == (size_t)-1) {
                if (r->gate_reached.items[i]) {
                    da_append(&r->reached_unpaired, i);
                } else {
                    da_append(&r->unreached_unpaired, i);
                }
            }
        }

        // If no unreached gates remain, we have successfully chained all components!
        if (r->unreached_unpaired.count == 0) {
			r->reached_unpaired.count = 0;
			r->unreached_unpaired.count = 0;
            break;
        }

        // If we have unreached gates but NO available reached gates to link them out,
        // we hit a localized deadlock. Return false to retry with a new seed.
        if (r->reached_unpaired.count == 0) {
			r->reached_unpaired.count = 0;
			r->unreached_unpaired.count = 0;
			ht_reset(&r->visited);
			r->queue.count = 0;
			r->matching.count = 0;
			r->gate_reached.count = 0;
			r->fog_gates.count = 0;
            return false;
        }

        // Pick a random reached gate and a random unreached gate to forge a bridge
        size_t r_idx = rand() % r->reached_unpaired.count;
        size_t u_idx = rand() % r->unreached_unpaired.count;

        size_t gate_r = r->reached_unpaired.items[r_idx];
        size_t gate_u = r->unreached_unpaired.items[u_idx];

        // Bind them bidirectionally
        r->matching.items[gate_r] = gate_u;
        r->matching.items[gate_u] = gate_r;

        size_t node_r = r->fog_gates.items[gate_r];
        size_t node_u = r->fog_gates.items[gate_u];

        Connection conn_to_u = { .to = (WarpNode)node_u, .is_warp = true };
        Connection conn_to_r = { .to = (WarpNode)node_r, .is_warp = true };

        da_append(&c->items[node_r], conn_to_u);
        da_append(&c->items[node_u], conn_to_r);

        // Resume exploration from the newly reached node to expand our pool
        explore_from_node(node_u, &r->visited, &r->queue, &queue_head, &r->fog_gates, &r->gate_reached, &r->inventory, &r->locked_nodes);

		r->reached_unpaired.count = 0;
		r->unreached_unpaired.count = 0;
    }

    // 4. Match any leftover gates that are already within our reached network
	r->leftover.count = 0;
    for (size_t i = 0; i < r->fog_gates.count; i++) {
        if (r->matching.items[i] == (size_t)-1) {
            da_append(&r->leftover, i);
        }
    }

    // Fisher-Yates shuffle the leftovers to randomize pairings
    for (size_t i = r->leftover.count; i > 1; i--) {
        size_t j = rand() % i;
        size_t temp = r->leftover.items[i - 1];
        r->leftover.items[i - 1] = r->leftover.items[j];
        r->leftover.items[j] = temp;
    }

    // Pair up the shuffled leftovers
    for (size_t i = 0; i + 1 < r->leftover.count; i += 2) {
        size_t gate_a = r->leftover.items[i];
        size_t gate_b = r->leftover.items[i + 1];

        r->matching.items[gate_a] = gate_b;
        r->matching.items[gate_b] = gate_a;

        size_t node_a = r->fog_gates.items[gate_a];
        size_t node_b = r->fog_gates.items[gate_b];

        Connection conn_to_b = { .to = (WarpNode)node_b, .is_warp = true };
        Connection conn_to_a = { .to = (WarpNode)node_a, .is_warp = true };

        da_append(&c->items[node_a], conn_to_b);
        da_append(&c->items[node_b], conn_to_a);
    }

    // Clean up
	r->leftover.count = 0;
	ht_reset(&r->visited);
	r->queue.count = 0;
	r->matching.count = 0;
	r->gate_reached.count = 0;
	r->fog_gates.count = 0;
    return true;
}

// 1. Recursive visual formatter (traverses the built BFS tree, not the master graph)
void print_bfs_tree_node(size_t node, Connections *bfs_tree, const char *prefix, bool is_last, bool is_warp, bool is_root) {
    // Print the branch lines (skip for the very first root node)
    if (!is_root) {
        printf("%s", prefix);
        if (is_last) {
            printf(is_warp ? "┗━~> " : "┗━━> ");
        } else {
            printf(is_warp ? "┣━~> " : "┣━━> ");
        }
    }
    
    // Print the node name
    printf("%s\n", WarpNodeNames[node]);
    
    // Get the shortest-path children for this node
    ConnectionGroup cg = bfs_tree->items[node];
    
    // Calculate the prefix for the next level down
    char next_prefix[512] = {0};
    if (!is_root) {
        snprintf(next_prefix, sizeof(next_prefix), "%s%s", prefix, is_last ? "     " : "┃    ");
    }
    
    for (size_t i = 0; i < cg.count; i++) {
        Connection conn = cg.items[i];
        bool last_child = (i == cg.count - 1);
        print_bfs_tree_node((size_t)conn.to, bfs_tree, next_prefix, last_child, conn.is_warp, false);
    }
}

// 2. Main builder function
void print_game_progression_bfs_tree(Connections *c) {
    printf("\n==================================================\n");
    printf("PROGRESSION\n");
    printf("==================================================\n");
    printf("Legends:\n");
    printf("  ━━> Fixed Path\n");
    printf("  ━~> Random Warp Fog Gate\n");
    printf("--------------------------------------------------\n\n");

    // Allocate a temporary adjacency list strictly for the BFS spanning tree
    Connections bfs_tree = {0};
	da_init(&bfs_tree, c->count);
	memset(bfs_tree.items, 0, sizeof(*bfs_tree.items)*c->count);
    
    HashSet visited = {0};
    DArrz queue = {0};
    size_t head = 0;

    // Start at the spawn point
    ht_put(&visited, (int)GameStartSpawnSrc);
    da_append(&queue, GameStartSpawnSrc);

    // Build the BFS Tree
    while (head < queue.count) {
        size_t curr = queue.items[head++];
        ConnectionGroup cg = c->items[curr];

        for (size_t i = 0; i < cg.count; i++) {
            Connection conn = cg.items[i];
            size_t next = (size_t)conn.to;

            // If we have NEVER seen this node before, it is a new BFS discovery
            if (ht_find(&visited, (int)next) == NULL) {
                ht_put(&visited, (int)next);
                da_append(&queue, next);

                // Add this connection to our loop-free BFS tree
				da_append(&bfs_tree.items[curr], conn);
            }
            // If it is in visited, we simply do nothing. 
        }
    }

    print_bfs_tree_node(GameStartSpawnSrc, &bfs_tree, "", true, false, true);

    printf("\n==================================================\n");

    // Safely free the temporary BFS tree and trackers
    for (size_t i = 0; i < c->count; i++) {
		da_free(bfs_tree.items[i]);
    }
	da_free(bfs_tree);
    ht_free(&visited);
    da_free(queue);
}

void randomizer() {
	srand(44);
	size_t attempt = 0;

	Randomizer r = {0};
	Connections c = {0};

	while (!attempt_randomization(&r, &c)) {
		attempt++;
		if (attempt % 100 == 0) printf("Attempt: %zu\r", attempt);
	}
	randomizer_free(&r);
	printf("Done in %zu attempts\n", attempt);
	print_game_progression_bfs_tree(&c);
	free_connections_by_ref(&c);
}

int main() {
	// if (!mkdir_if_not_exists(OUT_FOLDER_PATH)) return 1;
	// if (!disable_fog_gate("enc_regulation.bnd.dcx", WHITE_DOOR_PARAM_NAME,  OUT_FOLDER_PATH"enc_regulation.bnd.dcx")) return 1;
	// Editor editor = {0};
	// generate_default_connections();
	// if (!generate_map_objects(&editor)) return 1;
	// if (!randomize_warps(&editor)) return 1;
	// free_editor(&editor);
	// free_connections();

	// build the item param
	// generate_item_infos();
	// ht_foreach(iig, &gate_item_infos) {
	// 	WarpNode node = ht_key(&gate_item_infos, iig);
	// 	printf("%s => %zu\n", WarpNodeNames[node], iig->count);
	// }
	// free_item_infos();
	if (0) {
		const char *path = "./Param/ItemLotParam2_Chr.param"; // 3
		// const char *path = "./Param/ItemLotParam2_Other.param"; // 12
		String_Builder sb = {0};
		if (!nob_read_entire_file(path, &sb)) {
			return false;
		}
#ifdef LOG_GENERATED_FILES
		printf("[Read] %s: %zu bytes\n", param_path, sb->count);
#endif
		String_View sv = sb_to_sv(sb);
		// PARAM param = load_param(&sv, Cell_ItemLotParam2_Chr);
		PARAM param = load_param(&sv, Cell_ShopLineupParam);

		generate_item_infos();
		for (size_t i=0; i<param.cells.count; i++) {
			ItemLotParam2ChrPARAMDEF c = param.cells.items[i].as.itemlot_param2_char;
			for (size_t j=0; j<ARRAY_LEN(c.item_id); j++) {
				if (LotID_Item_FragrantBranchOfYore == c.item_id[j]) {
					int32_t row_id = param.rows.items[i].ID;
					bool found = false;
					ht_foreach(iig, &gate_item_infos) {
						WarpNode wn = ht_key(&gate_item_infos, iig);
						da_foreach(ItemInfo, ii, iig) {
							if ((int32_t)ii->id == row_id) {
								printf("The key (%d) can be found at %s", row_id, WarpNodeNames[wn]);
								if (ii->keys.count > 0) {
									printf(" and requires keys (");
								}
								for (size_t k=0; k<ii->keys.count; k++) {
									if (k!=0) printf(", ");
									print_connection_type(ii->keys.items[k]);
								}
								if (ii->keys.count > 0) {
									printf(")");
								}
								printf("\n");
								found = true;
							}
						}
					}
					if (!found) printf("The key (%d) was not found\n", row_id);
				}
			}
		}
		free_PARAM(&param);
	}

	const char *path = "./Param/ShopLineupParam.param"; // 2

	String_Builder sb = {0};
	if (!nob_read_entire_file(path, &sb)) {
		return false;
	}
#ifdef LOG_GENERATED_FILES
	printf("[Read] %s: %zu bytes\n", param_path, sb->count);
#endif
	String_View sv = sb_to_sv(sb);
	// PARAM param = load_param(&sv, Cell_ItemLotParam2_Chr);
	PARAM param = load_param(&sv, Cell_ShopLineupParam);
	generate_item_infos();

	for (size_t i=0; i<param.cells.count; i++) {
		ShopLineupPARAMDEF c = param.cells.items[i].as.shop_lineup_param;
		if (c.item_id == LotID_Item_FragrantBranchOfYore) {
			bool found = false;
			size_t row_id = param.rows.items[i].ID;
			ht_foreach(iig, &gate_item_infos) {
				WarpNode wn = ht_key(&gate_item_infos, iig);
				da_foreach(ItemInfo, ii, iig) {
					if (ii->id == row_id) {
						printf("The key (%zu) can be found at %s", row_id, WarpNodeNames[wn]);
						if (ii->keys.count > 0) {
							printf(" and requires keys (");
						}
						for (size_t k=0; k<ii->keys.count; k++) {
							if (k!=0) printf(", ");
							print_connection_type(ii->keys.items[k]);
						}
						if (ii->keys.count > 0) {
							printf(")");
						}
						printf("\n");
						found = true;
					}
				}
			}
			if (!found) printf("The key (%zu) was not found\n", row_id);
		}
	}

	free_PARAM(&param);
	free_item_infos();

	return 0;
}
