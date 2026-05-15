#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOB_DA_INIT_CAP 32
#define NOB_IMPLEMENTATION
#include "nob.h"

#define HT_IMPLEMENTATION
#include "ht.h"


// #define LOG_GENERATED_FILES

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

#define varint int64_t
#define utf16_t wchar_t

#define read_utf_16(sv) (utf16_t *)(sv)->data;

#define DA(type, name) typedef struct {type *items; size_t count; size_t capacity;} name
#define DA2(type, name) typedef struct {struct type *items; size_t count; size_t capacity;} name

#define assert_item(type, sv, val)\
    do {\
        assert(*(type *)(sv)->data == (val));\
        (sv)->data += sizeof(type);\
    } while (0)\

#define _read_item(type, sv)        *(type *)(sv)->data
#define _read_item_ptr(type, sv)    (type *)(sv)->data
#define read_item(type, sv)         _read_item(type, sv);     (sv)->data += sizeof(type)
#define read_item_ptr(type, sv, n)  _read_item_ptr(type, sv); (sv)->data += sizeof(type)*(n)

#define assert_val_ret(item, val) if ((item) != (val)) {nob_log(ERROR, "Invalid value"); return false;}
#define assert_val(item, val) if ((item) != (val)) {nob_log(ERROR, "Invalid value"); assert(false);}

#ifdef _MSC_VER
#define packed_struct struct
#define asm(val) (void)val
#else
#define packed_struct struct __attribute__((packed))
#endif

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

typedef packed_struct { 
	float x,y; 
} V2f;

typedef packed_struct { 
	float x,y,z; 
} V3f;

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

void sb_reserve(String_Builder *sb, size_t nbytes) {
	for (size_t i=0; i<nbytes; i++) {
		nob_sb_append(sb, 0);
	}
}

void __sb_write(String_Builder *sb, void *data, size_t length) {
	nob_sb_append_buf(sb, data, length);
}

#define sb_write(sb, data) __sb_write((sb), &(data), sizeof(data));

void sb_write_varint(String_Builder *sb, varint value) {
	nob_da_reserve(sb, sb->count + sizeof(value));
	memcpy(sb->items + sb->count, &value, sizeof(value));
	sb->count += sizeof(varint);
}

void sb_write_int64t(String_Builder *sb, int64_t value) {
	nob_da_reserve(sb, sb->count + sizeof(value));
	memcpy(sb->items + sb->count, &value, sizeof(value));
	sb->count += sizeof(int64_t);
}

void sb_write_int32t(String_Builder *sb, int32_t value) {
	nob_da_reserve(sb, sb->count + sizeof(value));
	memcpy(sb->items + sb->count, &value, sizeof(value));
	sb->count += sizeof(int32_t);
}

void sb_write_uint32t(String_Builder *sb, uint32_t value) {
	nob_da_reserve(sb, sb->count + sizeof(value));
	memcpy(sb->items + sb->count, &value, sizeof(value));
	sb->count += sizeof(uint32_t);
}


void sb_write_int16t(String_Builder *sb, int16_t value) {
	nob_da_reserve(sb, sb->count + sizeof(value));
	memcpy(sb->items + sb->count, &value, sizeof(value));
	sb->count += sizeof(int16_t);
}

void sb_write_uint16t(String_Builder *sb, uint16_t value) {
	nob_da_reserve(sb, sb->count + sizeof(value));
	memcpy(sb->items + sb->count, &value, sizeof(value));
	sb->count += sizeof(uint16_t);
}



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

void sb_append_utf16t(String_Builder *sb, utf16_t *str) {
	// compute the size of string
	char *c = (char *)str;
	size_t name_size = 0;
	size_t zcount = 0;
	while (true) {
		if (*c == 0) {
			zcount++;
		} else {
			zcount = 0;
		}
		name_size++;
		if (zcount >= sizeof(utf16_t) && name_size%sizeof(utf16_t) == 0) {
			break;
		}
		c++;
	}

	nob_da_reserve(sb, sb->count + name_size);
	memcpy(sb->items + sb->count, str, name_size);
	sb->count += name_size;
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

DA(size_t, DArrz);
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

typedef enum {
	Cell_EventLoc,
	Cell_EventParam,
	Cell_MapObjInstanceParam
} CellType;

typedef struct {
	union {
		EventLocPARAMDEF event_loc;
		EventParamPARAMDEF event_param;
		MapObjectInstancePARAMDEF map_obj_instance_param;
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

typedef packed_struct {
	int32_t a;
	int32_t dark_souls_count; // 1, 2 or 3
	int32_t dark_souls_count_repeat;
	int32_t b; // 0x54
	int32_t data_size;
	int32_t c; // 6
	int32_t d; // 0x48
	int32_t e; // 1;
	int32_t stateGroupSize; // 0x20
	int32_t stateGroupCount;
	int32_t stateSize; // 0x48
	int32_t stateCount;
	int32_t f; // 0x38
	int32_t conditionCount;
	int32_t g; // 0x18
	int32_t commandCallCount;
	int32_t h; // 0x10
	int32_t commandArgCount;
	int32_t conditionOffsetsOffset;
	int32_t conditionOffsetsCount;
	int32_t nameBlockOffset;
	int32_t nameLength;
	int32_t unkOffset1;
	int32_t i; // 0
	int32_t unkOffset2;
	int32_t j; // 0
} ESD_HEADER;

typedef packed_struct {
	int32_t a; // 1
	int32_t Unk70;
	int32_t Unk74;
	int32_t Unk78;
	int32_t Unk7C;
	int32_t b; // 0
	varint stateGroupsOffset;
	varint stateGroupCount; // =header.stateGroupCount
	varint nameOffset;
	varint nameLength; // = header.nameLength
	varint unkNull; // -1
	varint unkNull2; // -1
} StateGroupHeader;

#define sv_read(sv, type) *(type *)sv_chop_left((sv), sizeof(type)).data;

typedef packed_struct {
	varint ID;
	varint conditionOffsetsOffset;
	varint conditionOffsetCount;
	varint entryCommandsOffset;
	varint entryCommandCount;
	varint exitCommandsOffset;
	varint exitCommandCount;
	varint whileCommandsOffset;
	varint whileCommandCount;
} StateHeader;

typedef packed_struct {
	varint argOffset;
	varint argSize;
} ArgumentHeader;

typedef packed_struct {
	int32_t CommandBank; // 1, 5, 6 or 7
	int32_t CommandID;
	varint argsOffset;
	varint argsCount;
} CommandCallHeader;

typedef packed_struct {
	varint stateOffset;
	varint passCommandsOffset;
	varint passCommandCount;
	varint conditionOffsetsOffset;
	varint conditionOffsetCount;
	varint evaluatorOffset;
	varint evaluatorLength;
} ConditionHeader;

DA(uint8_t, Arg);
DA(Arg, DAArg);

typedef struct { 
	int32_t command_bank;
	int32_t command_id;
	DArrz args_ids;
} Command;

DA(Command, DACommand);

DA(void *, DAOpaque);

// reference from https://gist.github.com/JKAnderson/e3fd38d4efd5f3350e13a613319b8bed
typedef enum {
	EzNumber = 0x7F, // from 0x00 to 0x7f | 0
	EzF32 = 0x80, // | 0
	EzF64 = 0x81, // | 0
	EzS32 = 0x82, // | 0
	EzUnk1 = 0x83,
	EzFuncCall1 = 0x84, // from 0x84 to 0x8b | 1-8 | 1
	EzFuncCall2 = 0x85, // from 0x84 to 0x8b | 1-8 | 2
	EzFuncCall3 = 0x86, // from 0x84 to 0x8b | 1-8 | 3
	EzFuncCall4 = 0x87, // from 0x84 to 0x8b | 1-8 | 4
	EzFuncCall5 = 0x88, // from 0x84 to 0x8b | 1-8 | 5
	EzFuncCall6 = 0x89, // from 0x84 to 0x8b | 1-8 | 6
	EzFuncCall7 = 0x8a, // from 0x84 to 0x8b | 1-8 | 7
	EzFuncCall8 = 0x8b, // from 0x84 to 0x8b | 1-8 | 8
	EzAdd = 0x8c, // | 2
	EzNeg = 0x8e, // | 1
	EzMul = 0x8f, // | 2
	EzDiv = 0x90, // | 2
	EzLt = 0x91, // | 2
	EzGt = 0x92, // | 2
	EzLte = 0x93, // | 2
	EzGte = 0x94, // | 2
	EzEq = 0x95, // | 2
	EzNeq = 0x96, // | 2
	EzAbs = 0x97, // | 1
	EzAnd = 0x98, // | 2
	EzOr = 0x99, // | 2
	EzNot = 0x9a, // | 1
	EzLt0 = 0x9b, // | 1
	EzGt0 = 0x9c, // | 1
	EzLte0 = 0x9d, // | 1
	EzGte0 = 0x9e, // | 1
	EzEq0 = 0x9f, // | 1
	EzFin = 0xa1, // | 0
	EzUnk2 = 0xa2,
	EzUnk3 = 0xa3,
	EzUnk4 = 0xa4,
	EzCstr = 0xa5, // | 0
	EzTry = 0xa6, // | 1
	EzSetReg0 = 0xa7, // from 0xa7 - 0xae | 1
	EzSetReg1 = 0xa8, // from 0xa7 - 0xae | 1
	EzSetReg2 = 0xa9, // from 0xa7 - 0xae | 1
	EzSetReg3 = 0xaa, // from 0xa7 - 0xae | 1
	EzSetReg4 = 0xab, // from 0xa7 - 0xae | 1
	EzSetReg5 = 0xac, // from 0xa7 - 0xae | 1
	EzSetReg6 = 0xad, // from 0xa7 - 0xae | 1
	EzSetReg7 = 0xae, // from 0xa7 - 0xae | 1
	EzGetReg0 = 0xaf, // from 0xaf - 0xb6 | 0
	EzGetReg1 = 0xb0, // from 0xaf - 0xb6 | 0
	EzGetReg2 = 0xb1, // from 0xaf - 0xb6 | 0
	EzGetReg3 = 0xb2, // from 0xaf - 0xb6 | 0
	EzGetReg4 = 0xb3, // from 0xaf - 0xb6 | 0
	EzGetReg5 = 0xb4, // from 0xaf - 0xb6 | 0
	EzGetReg6 = 0xb5, // from 0xaf - 0xb6 | 0
	EzGetReg7 = 0xb6, // from 0xaf - 0xb6 | 0
	EzAssert = 0xb7, // | 1
	EzGetArg = 0xb8, // get machine argument | 1
	EzGetRet = 0xb9, // | 0
	EzRetVal = 0xba, // | 0
	EzEnd = 0xff, // from 0xbb - 0xff, represents the constant 0x7fffffff (return value of machine)
} EzType;

bool ez_is_unop(EzType type) {
	return (type == EzNeg || type == EzAbs);
}

bool ez_is_binop(EzType type) {
	if (ez_is_unop(type)) return false;
	return type >= EzAdd && type <= EzOr;
}

bool ez_is_funcall(EzType type) {
	return type >= EzFuncCall1 && type <= EzFuncCall8;
}

typedef struct {
	size_t lhs_idx;
	size_t rhs_idx;
} Binop;

typedef struct {
	DArrz arg_idx;
	int id;
} FunCall;

typedef enum {
	CH_NONE = 0,
	CH_ABORT,
	CH_TRY,
} ConditionHandle;

typedef struct {
	EzType type;
	ConditionHandle cond_handle;
	union {
		int8_t number;
		float f32;
		double f64;
		int32_t s32;
		Binop binop;
		size_t arg_idx;
		FunCall fun_call;
		struct { 
			uint8_t reg_index;
			int arg_idx;
		} regist;
	} as;
} EzExprType;

DA(EzExprType, DAEzExpr);

typedef struct {
	DAEzExpr exprs;
	DArrz stack;
} Expression;

void free_funcall(FunCall *f) {
	da_free(f->arg_idx);
}

void dealloc_funcalls(Expression *e) {
	da_foreach(EzExprType, expr, &e->exprs) {
		if (ez_is_funcall(expr->type)) {
			free_funcall(&expr->as.fun_call);
		}
	}
}

void dealloc_expression(Expression *e) {
	dealloc_funcalls(e);
	da_free(e->exprs);
	da_free(e->stack);
}

typedef struct {
	const unsigned char *items;
	size_t count;
} Evaluator;

size_t expr_pop_arg(Expression *e) {
	assert(e->stack.count > 0 && "stack is empty");
	size_t idx = e->stack.items[e->stack.count-1];
	e->stack.count--;
	return idx;
}

void darrz_reverse(DArrz *arr) {
	if (arr == NULL || arr->count <= 1 || arr->items == NULL) return;
	size_t l = 0;
	size_t r = arr->count - 1;
	while (l < r) {
		size_t value = arr->items[l];
		arr->items[l] = arr->items[r];
		arr->items[r] = value;
		l++;
		r--;
	}
}

void expr_pop_args(Expression *e, FunCall *f, size_t n) {
	for (size_t i=0; i<n; i++) {
		da_append(&f->arg_idx, expr_pop_arg(e));
	}
	darrz_reverse(&f->arg_idx);
}

void *ez_get_val(EzExprType *e) {
	size_t offset = offsetof(EzExprType, as);
	return (void *)e + offset;
}

EzExprType* expr_peek(Expression *e) {
	return &e->exprs.items[e->stack.items[e->stack.count-1]];
}

#define ez_get(type, expr) *(type *)ez_get_val((expr));

void parse_expression(const Evaluator *e, Expression *expression) {
	for (size_t i=0; i<e->count; i++) {
		const unsigned char b = e->items[i];
		EzType type = (EzType)b;
		EzExprType expr = {0};
		if (b <= EzNumber) {
			expr.type = EzNumber;
			expr.as.number = (int8_t)(b - 64);
		} else if (ez_is_unop(type)) {
			expr.type = type;
			expr.as.arg_idx = expr_pop_arg(expression);
		} else if (ez_is_binop(type)) {
			expr.type = type;
			expr.as.binop.rhs_idx = expr_pop_arg(expression);
			expr.as.binop.lhs_idx = expr_pop_arg(expression);
		} else if (b >= EzSetReg0 && b <= EzSetReg7) {
			expr.type = type;
			expr.as.regist.reg_index = (uint8_t)(b - EzSetReg0);
			expr.as.regist.arg_idx = expr_pop_arg(expression);
		} else if (b >= EzGetReg0 && b <= EzGetReg7) {
			expr.type = type;
			expr.as.regist.reg_index = (uint8_t)(b - EzGetReg0);
			expr.as.regist.arg_idx = -1;
		} else {
			expr.type = type;
			size_t n = 0;
			switch (type) {
				case EzF32: {
					assert(i < e->count - sizeof(float));
					float val = *(float *)(e->items+i+1);
					i += sizeof(float);
					expr.as.f32 = val;
				} break;
				case EzF64: {
					assert(i < e->count - sizeof(double));
					double val = *(double *)(e->items+i+1);
					i += sizeof(double);
					expr.as.f64 = val;
				} break;
				case EzS32: {
					assert(i < e->count - sizeof(int32_t));
					int32_t val = *(int32_t *)(e->items+i+1);
					i += sizeof(int32_t);
					expr.as.s32 = val;
			    } break;
				case EzAssert: {
				    EzExprType *expn = expr_peek(expression);
					expn->cond_handle = CH_ABORT;
					continue;
				} break;
				case EzTry: {
				    EzExprType *expn = expr_peek(expression);
					expn->cond_handle = CH_TRY;
					continue;
			    } break;
				case EzFuncCall8: n++;
				case EzFuncCall7: n++;
				case EzFuncCall6: n++;
				case EzFuncCall5: n++;
				case EzFuncCall4: n++;
				case EzFuncCall3: n++;
				case EzFuncCall2: n++;
				case EzFuncCall1: {
					FunCall f = {0};
					expr_pop_args(expression, &f, n);
					EzExprType *expn = &expression->exprs.items[expr_pop_arg(expression)];
					f.id = ez_get(int, expn);
					expr.as.fun_call = f;
				} break;
				case EzGetArg:
					expr.as.arg_idx = expr_pop_arg(expression);
					break;
				case EzRetVal:
				case EzGetRet:
					break;
				case EzFin:
					assert(i == e->count - 1 && "End of expression before the end of data");
					break;
				default:
					TODO("EzType not implemented");
			}
		}
		da_append(&expression->stack, expression->exprs.count);
		da_append(&expression->exprs, expr);
	}
}

void print_expr(const Expression *e, EzExprType *expr);
void print_binop(const Expression *e, EzExprType *expr, const char *sep) {
	size_t id = expr->as.binop.lhs_idx;
	printf("(");
	print_expr(e, &e->exprs.items[id]);
	printf(")");
	printf(" %s ", sep);
	id = expr->as.binop.rhs_idx;
	printf("(");
	print_expr(e, &e->exprs.items[id]);
	printf(")");
}

void print_funcall(const Expression *e, EzExprType *expr) {
	printf("func%zu_%d(", expr->as.fun_call.arg_idx.count, expr->as.fun_call.id);
	for (size_t i=0; i<expr->as.fun_call.arg_idx.count; i++) {
		if (i != 0) printf(" , ");
		EzExprType *arg = &e->exprs.items[expr->as.fun_call.arg_idx.items[i]];
		print_expr(e, arg);
	}
	printf(")");
}

void print_expr(const Expression *e, EzExprType *expr) {
	switch (expr->cond_handle) {
		case CH_NONE: break;
		case CH_TRY: printf("try "); break;
		case CH_ABORT: printf("assert "); break;
		default: UNREACHABLE("expr.cond_handle");
	}
	switch (expr->type) {
		case EzF32: printf("%f", expr->as.f32); break;
		case EzF64: printf("%f", expr->as.f64); break;
		case EzS32: printf("%d", expr->as.s32); break;
		case EzSetReg0:
		case EzSetReg1:
		case EzSetReg2:
		case EzSetReg3:
		case EzSetReg4:
		case EzSetReg5:
		case EzSetReg6:
		case EzSetReg7:
			printf("set_register%d(", expr->as.regist.reg_index);
			print_expr(e, &e->exprs.items[expr->as.regist.arg_idx]);
			printf(")");
			break;
		case EzGetReg0:
		case EzGetReg1:
		case EzGetReg2:
		case EzGetReg3:
		case EzGetReg4:
		case EzGetReg5:
		case EzGetReg6:
		case EzGetReg7:
			printf("get_register%d(", expr->as.regist.reg_index);
			printf(")");
			break;
		case EzLte: print_binop(e, expr, "<="); break;
		case EzGte: print_binop(e, expr, ">="); break;
		case EzLt: print_binop(e, expr, "<"); break;
		case EzGt: print_binop(e, expr, ">"); break;
		case EzAnd: print_binop(e, expr, "&&"); break;
		case EzOr: print_binop(e, expr, "||"); break;
		case EzAdd: print_binop(e, expr, "+"); break;
		case EzMul: 
			print_binop(e, expr, "x"); 
			break;
		case EzDiv: print_binop(e, expr, "x"); break;
		case EzGetArg: 
			printf("GetArg("); 
			print_expr(e, &e->exprs.items[expr->as.arg_idx]);
			printf(")");
			break;
		case EzFuncCall8:
		case EzFuncCall7:
		case EzFuncCall6:
		case EzFuncCall5:
		case EzFuncCall4:
		case EzFuncCall3:
		case EzFuncCall2:
		case EzFuncCall1: print_funcall(e, expr); break;
		case EzEq: print_binop(e, expr, "=="); break;
		case EzNeq: print_binop(e, expr, "!="); break;
		case EzNumber:
			printf("%d", expr->as.number);
			break;
		case EzFin:
			break;
		case EzGetRet:
			printf("CallResult()");
			break;
		case EzRetVal:
			printf("0x7fffffff");
			break;
		default: TODO("print expression (expression type)");
	}
}

void print_expression(const Expression *e) {
	assert(e->stack.count == 2);
	for (size_t i=0; i<e->stack.count; i++) {
		size_t id = e->stack.items[i];
		EzExprType *expr = &e->exprs.items[id];
		print_expr(e, expr);
	}
}

typedef struct {
	varint target_state_offset;
	int target_state_id;
	DArrz pass_commands_ids;
	DArrz sub_conditions_ids;
	Evaluator evaluator;
} Condition;

DA(Condition, DACondition);
DA(Condition *, DAConditionRef);

typedef struct {
	varint id;
	DArrz conditions_id;
	DArrz entry_commands_ids;
	DArrz exit_commands_ids;
	DArrz while_commands_ids;
} State;

DA(State, DAState);
typedef struct {
	varint id;
	DArrz states_ids;
} StateGroup;

DA(StateGroup, DAStateGroup);
typedef Ht(varint, size_t) Dictlldz;
typedef Ht(size_t, varint) Dictzulld;

typedef struct {
	wchar_t *name;
	size_t nameLength;

	int32_t Unk70;
	int32_t Unk74;
	int32_t Unk78;
	int32_t Unk7C;

	DAStateGroup state_groups;
	DAState states;
	DACondition conditions;
	DACommand commands;
	DAArg args;

	Dictlldz dict_cond; // [varint (offset), size_t (index)]
	Dictlldz dict_state; // [varint (offset), size_t (index)]
	size_t duplicate_states_count;
} ESD;

void print_condition(const Condition *c, Expression *e) {
	parse_expression(&c->evaluator, e);
	print_expression(e);
	dealloc_funcalls(e);
	e->stack.count = 0;
	e->exprs.count = 0;
}

void print_command(ESD *esd, const Command *c) {
	printf("        fun_call_%d(", c->command_id);
	size_t i=0;
	Expression e = {0};
	da_foreach(size_t, aid, &c->args_ids) {
		if (i!=0) printf(", ");
		Evaluator *arg = (Evaluator *)&esd->args.items[*aid];
		parse_expression(arg, &e);
		print_expression(&e);
		dealloc_funcalls(&e);
		e.stack.count = 0;
		e.exprs.count = 0;
		i++;
	}
	da_free(e.exprs);
	da_free(e.stack);
	printf(")\n");
}

void print_state(ESD *esd, State *state) {
	printf("    State_%lld:\n", state->id);
	da_foreach(size_t, ci, &state->entry_commands_ids) {
		Command *command = &esd->commands.items[*ci];
		print_command(esd, command);
	}
	Expression e = {0};
	da_foreach(size_t, cid, &state->conditions_id) {
		printf("        if (");
		const Condition *c = &esd->conditions.items[*cid];
		print_condition(c, &e);
		printf(")");
		if (c->target_state_offset > 0) {
			State *ts = &esd->states.items[c->target_state_id];
			printf(" goto State %lld\n", ts->id);
		}
		da_foreach(size_t, pcid, &c->pass_commands_ids) {
			const Command *pc = &esd->commands.items[*pcid];
			assert (pc->command_id == -1);
			printf(" return ");
			Expression e = {0};
			size_t i=0;
			da_foreach(size_t, pcai, &pc->args_ids) {
				if (i!=0) printf(", ");
				Evaluator *arg = (Evaluator *)&esd->args.items[*pcai];
				parse_expression(arg, &e);
				print_expression(&e);
				dealloc_funcalls(&e);
				e.stack.count = 0;
				e.exprs.count = 0;
				i++;
			}
			da_free(e.exprs);
			da_free(e.stack);
			printf("\n");
		}
		da_foreach(size_t, scid, &c->sub_conditions_ids) {
			const Condition *sc = &esd->conditions.items[*scid];
			if (sc->target_state_offset > 0) {
				State *ts = &esd->states.items[sc->target_state_id];
				printf(" goto State %lld\n", ts->id);
			}
		}
	}
	// if (state->conditions_id.count == 0) printf("\n");
	da_free(e.exprs);
	da_free(e.stack);
	// dealloc_expression(&e);
}

void print_state_group(ESD *esd, StateGroup *sg) {
	printf("StateGroup_%lld {\n", sg->id);
	da_foreach(size_t, id, &sg->states_ids) {
		State *state = &esd->states.items[*id];
		print_state(esd, state);
	}
	printf("}\n\n");
}

void free_condition(Condition *condition) {
	da_free(condition->pass_commands_ids);
	da_free(condition->sub_conditions_ids);
}

void free_commands(DACommand *commands) {
	da_foreach(Command, command, commands) {
		da_free(command->args_ids);
	}
	da_free(*commands);
}

void free_esd(ESD *esd) {
	da_foreach(State, state, &esd->states) {
		da_free(state->conditions_id);
		da_free(state->entry_commands_ids);
		da_free(state->exit_commands_ids);
		da_free(state->while_commands_ids);
	}
	da_free(esd->states);
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_free(sg->states_ids);
	}
	da_foreach(Condition, cond, &esd->conditions) {
		free_condition(cond);
	}
	da_free(esd->state_groups);
	ht_free(&esd->dict_cond);
	free_commands(&esd->commands);
	da_foreach(Arg, arg, &esd->args) {
		da_free(*arg);
	}
	da_free(esd->args);
	ht_free(&esd->dict_state);
}

#define da_alloc(da, n)\
	do {\
		(da).items = realloc((da).items, sizeof(*(da).items)*(n));\
		(da).capacity = (n);\
		(da).count = 0;\
	} while (0)

const char *ESD_MAGIC = "fsSL";
#define ESD_MAGIC_LENGTH  4

Arg read_arg(String_View *sv, size_t size) {
	Arg arg = {0};
	for (size_t i=0; i<size; i++) {
		da_append(&arg, *(sv->data+i));
	}
	return arg;
}

#define da_init(da, size) \
	do {\
		if ((size) > 0) {\
			assert((da)->capacity == 0 && "0 init the da first");\
			(da)->capacity = (size);\
			(da)->items = malloc(sizeof(*(da)->items)*(da)->capacity);\
		}\
	} while (0)

Command read_command(String_View *sv, ESD *esd, const char *data_start) {
	Command command = {0};
	CommandCallHeader info = sv_read(sv, CommandCallHeader);
	command.command_bank = info.CommandBank;
	command.command_id = info.CommandID;
	String_View saved = *sv;

	sv->data = data_start + info.argsOffset;
	da_init(&command.args_ids, info.argsCount);
	for (size_t i=0; i<(size_t)info.argsCount; i++) {
		ArgumentHeader arg_info = sv_read(sv, ArgumentHeader);
		String_View bak = *sv;
		sv->data = data_start + arg_info.argOffset;
		da_append(&command.args_ids, esd->args.count);
		da_append(&esd->args, read_arg(sv, arg_info.argSize));
		*sv = bak;
	}
	*sv = saved;
	return command;
}

Condition read_condition(String_View *sv, ESD *esd, const char *data_start) {
	Condition cond = {0};
	ConditionHeader info = sv_read(sv, ConditionHeader);
	varint offset = info.passCommandsOffset;
	cond.target_state_offset = info.stateOffset;
	sv->data = data_start + offset;
	da_init(&cond.pass_commands_ids, info.passCommandCount);
	for (size_t i=0; i<(size_t)info.passCommandCount; i++) {
		da_append(&cond.pass_commands_ids, esd->commands.count);
		da_append(&esd->commands, read_command(sv, esd, data_start));
	}
	offset = info.conditionOffsetsOffset;
	sv->data = data_start + offset;
	da_init(&cond.sub_conditions_ids, info.conditionOffsetCount);
	for (size_t i=0; i<(size_t)info.conditionOffsetCount; i++) {
		offset = sv_read(sv, varint);
		String_View saved = *sv;
		sv->data = data_start + offset;
		size_t *index_ref = ht_find(&esd->dict_cond, offset);
		if (index_ref == NULL) {
			*ht_put(&esd->dict_cond, offset) = esd->conditions.count;
			da_append(&cond.sub_conditions_ids, esd->conditions.count);
			da_append(&esd->conditions, read_condition(sv, esd, data_start));
		} else {
			da_append(&cond.sub_conditions_ids, *index_ref);
		}
		*sv = saved;
	}
	offset = info.evaluatorOffset;
	if (offset > 0) {
		cond.evaluator.items = (unsigned char *)(data_start + offset);
		cond.evaluator.count = info.evaluatorLength;
	}
	return cond;
}

State read_state(String_View *sv, ESD *esd, const char *data_start, StateHeader *info)  {
	State state = {0};
	state.id = info->ID;
	sv->data = data_start + info->conditionOffsetsOffset;
	da_init(&state.conditions_id, info->conditionOffsetCount);
	for (size_t j=0; j<(size_t)info->conditionOffsetCount; j++) {
		varint offset = sv_read(sv, varint);
		if (offset < 0) continue;
		String_View saved = *sv;
		sv->data = data_start + offset;
		size_t *index_ref = ht_find(&esd->dict_cond, offset);
		if (index_ref == NULL) {
			da_append(&state.conditions_id, esd->conditions.count);
			*ht_put(&esd->dict_cond, offset) = esd->conditions.count;
			da_append(&esd->conditions, read_condition(sv, esd, data_start));
		} else {
			da_append(&state.conditions_id, *index_ref);
		}
		*sv = saved;
	}
	varint offset = info->entryCommandsOffset;
	sv->data = data_start + offset;
	da_init(&state.entry_commands_ids, info->entryCommandCount);
	for (size_t j=0; j<(size_t)info->entryCommandCount; j++) {
		da_append(&state.entry_commands_ids, esd->commands.count);
		da_append(&esd->commands, read_command(sv, esd, data_start));
	}
	offset = info->exitCommandsOffset;
	sv->data = data_start + offset;
	da_init(&state.exit_commands_ids, info->exitCommandCount);
	for (size_t j=0; j<(size_t)info->exitCommandCount; j++) {
		da_append(&state.exit_commands_ids, esd->commands.count);
		da_append(&esd->commands, read_command(sv, esd, data_start));
	}
	offset = info->whileCommandsOffset;
	sv->data = data_start + offset;
	da_init(&state.while_commands_ids, info->whileCommandCount);
	for (size_t j=0; j<(size_t)info->whileCommandCount; j++) {
		da_append(&state.while_commands_ids, esd->commands.count);
		da_append(&esd->commands, read_command(sv, esd, data_start));
	}
	return state;
}

StateGroup read_state_group(String_View* sv, ESD *esd, const char *data_start, int32_t state_size, Dictlldz *ht) {
	StateGroup sg = {0};
	sg.id = sv_read(sv, varint);
	varint offset = sv_read(sv, varint);
	varint count = sv_read(sv, varint);
	sv_read(sv, varint);
	String_View saved = *sv;
	da_init(&sg.states_ids, count);
	for (size_t i=0; i<(size_t)count; i++) {
		sv->data = data_start + offset;
		StateHeader header = sv_read(sv, StateHeader);
		da_append(&sg.states_ids, esd->states.count);
		*ht_put(ht, offset) = esd->states.count;
		da_append(&esd->states, read_state(sv, esd, data_start, &header));

		offset = offset + state_size;
	} 
	if (count > 1) {
		esd->duplicate_states_count++;
	}
	*sv = saved;
	return sg;
}

#define STATE_SIZE 0x48

ESD read_esd_old(String_View sv) {
	ESD esd = {0};
	Dictlldz ht = {0}; // offset, state_index

	String_View magic = sv_chop_left(&sv, strlen(ESD_MAGIC));
	assert(sv_str_cmp(magic, ESD_MAGIC) && nob_temp_sprintf("Expected `%s` got `"SV_Fmt"`", ESD_MAGIC, SV_Arg(magic)));

	ESD_HEADER esd_header = sv_read(&sv, ESD_HEADER);
	assert(esd_header.stateSize = STATE_SIZE);
	const char *data_start = sv.data;

	if (esd_header.nameLength > 0) {
		esd.name = (wchar_t *)(data_start + esd_header.nameBlockOffset);
	} else {
		esd.name = NULL;
	}

	StateGroupHeader ed = sv_read(&sv, StateGroupHeader);

	esd.Unk70 = ed.Unk70;
	esd.Unk74 = ed.Unk74;
	esd.Unk78 = ed.Unk78;
	esd.Unk7C = ed.Unk7C;
	da_init(&esd.state_groups, esd_header.stateGroupCount);
	da_init(&esd.states, esd_header.stateCount);
	da_init(&esd.conditions, esd_header.conditionCount);
	da_init(&esd.commands, esd_header.commandCallCount);
	da_init(&esd.args, esd_header.commandArgCount);
	for (size_t i=0; i<(size_t)esd_header.stateGroupCount; i++) {
		da_append(&esd.state_groups, read_state_group(&sv, &esd, data_start, esd_header.stateSize, &ht));
	}
	assert((int)esd.state_groups.count == esd_header.stateGroupCount);
	assert((int)(esd.states.count+esd.duplicate_states_count) == esd_header.stateCount);
	assert((int)esd.conditions.count == esd_header.conditionCount);
	assert((int)esd.commands.count == esd_header.commandCallCount);
	assert((int)esd.args.count == esd_header.commandArgCount);

	da_foreach(Condition, cond, &esd.conditions) {
		if (cond->target_state_offset > 0) {
			size_t *state_id_ref = ht_find(&ht, cond->target_state_offset);
			assert(state_id_ref && "key error");
			assert(esd.states.count > *state_id_ref);
			cond->target_state_id = *state_id_ref;
		} else {
			cond->target_state_id = -1;
		}
	}

	ht_free(&ht);

	return esd;
}

typedef Ht(const char *, size_t) Dictstrzu;
typedef Ht(size_t, size_t) Dictzuzu;

void sb_reserve_ex(String_Builder *sb, size_t bytes) {
	da_reserve(sb, sb->count + bytes);
	sb->count += bytes;
}

// ensures sb has enough memory to add 'bytes' and advances the current position
// returns false if the key already exists;
bool ht_reserve(String_Builder *sb, Dictstrzu *res, const char *name, size_t bytes) {
	size_t *val = ht_find(res, name);
	if (val != NULL) {
		return false;
	}
	*ht_put(res, name) = sb->count;
	sb_reserve_ex(sb, bytes);
	return true;
}

bool ht_reserve_varint(String_Builder *sb, Dictstrzu *res, const char *name) {
	return ht_reserve(sb, res, name, sizeof(varint));
}

bool ht_unreserve(String_Builder *sb, Dictstrzu *res, const char *name, void *buf, size_t bytes) {
	size_t *position = ht_find(res, name);
	if (position == NULL) return false;
	size_t head = sb->count;
	sb->count = *position;
	sb_append_buf(sb, buf, bytes);
	sb->count = head;
	// ht_delete(res, position);
	return true;
}

bool ht_unreserve_varint(String_Builder *sb, Dictstrzu *res, const char *name, varint value) {
	return ht_unreserve(sb, res, name, &value, sizeof(varint));
}

bool ht_unreserve_int32_t(String_Builder *sb, Dictstrzu *res, const char *name, int32_t value) {
	return ht_unreserve(sb, res, name, &value, sizeof(int32_t));
}



#define DARK_SOULS_COUNT 2
void write_esd_header(String_Builder *sb, ESD *esd, Dictstrzu *res) {
	sb_write_int32t(sb, 1);
	sb_write_int32t(sb, DARK_SOULS_COUNT);
	sb_write_int32t(sb, DARK_SOULS_COUNT);
	sb_write_int32t(sb, 0x54);
	ht_reserve(sb, res, "DataSize", sizeof(int32_t));
	sb_write_int32t(sb, 6);
	sb_write_int32t(sb, 0x48);
	sb_write_int32t(sb, 1);
	sb_write_int32t(sb, 0x20);
	sb_write_int32t(sb, esd->state_groups.count);
	sb_write_int32t(sb, 0x48);
	size_t count = 0;
	da_foreach(StateGroup, sg, &esd->state_groups) {
		count += sg->states_ids.count;
	}
	sb_write_int32t(sb, count + esd->duplicate_states_count);
	sb_write_int32t(sb, 0x38);
	sb_write_int32t(sb, esd->conditions.count); // ht_reserve(sb, res, "ConditionCount", sizeof(int32_t));
	sb_write_int32t(sb, 0x18);
	sb_write_int32t(sb, esd->commands.count); // ht_reserve(sb, res, "CommandCallCount", sizeof(int32_t));
	sb_write_int32t(sb, 0x10);
	sb_write_int32t(sb, esd->args.count); // ht_reserve(sb, res, "CommandArgCount", sizeof(int32_t));
	ht_reserve(sb, res, "ConditionOffsetsOffset", sizeof(int32_t));
	ht_reserve(sb, res, "ConditionOffsetsCount", sizeof(int32_t));
	ht_reserve(sb, res, "NameBlockOffset", sizeof(int32_t));
	if (esd->name == NULL) {
		sb_write_int32t(sb, 0);
	} else {
		sb_write_int32t(sb, wcslen(esd->name)+1);
	}
	ht_reserve(sb, res, "UnkOffset1", sizeof(int32_t));
	sb_write_int32t(sb, 0);
	ht_reserve(sb, res, "UnkOffset2", sizeof(int32_t));
	sb_write_int32t(sb, 0);
}

void write_esd_data_header(String_Builder *sb, ESD *esd, Dictstrzu *res) {
	sb_write_int32t(sb, 1);
	sb_write_int32t(sb, esd->Unk70);
	sb_write_int32t(sb, esd->Unk74);
	sb_write_int32t(sb, esd->Unk78);
	sb_write_int32t(sb, esd->Unk7C);
	sb_write_int32t(sb, 0);
	ht_reserve(sb, res, "StateGroupsOffset", sizeof(varint));
	sb_write_varint(sb, esd->state_groups.count);
	ht_reserve(sb, res, "NameOffset", sizeof(varint));
	if (esd->name == NULL) {
		sb_write_varint(sb, 0);
	} else {
		sb_write_varint(sb, wcslen(esd->name)+1);
	}
	sb_write_varint(sb, -1);
	sb_write_varint(sb, -1);
}

void write_state_group_info(String_Builder *sb, StateGroup *sg, Dictstrzu *res) {
	sb_write_varint(sb, sg->id);
	ht_reserve_varint(sb, res, temp_sprintf("StateGroupOffset:%lld:1", sg->id));
	sb_write_varint(sb, sg->states_ids.count);
	ht_reserve_varint(sb, res, temp_sprintf("StateGroupOffset:%lld:2", sg->id));
}

void write_state_info(String_Builder *sb, varint group_id, State *state, Dictstrzu *res) {
	sb_write_varint(sb, state->id);
	ht_reserve_varint(sb, res, temp_sprintf("StateCondOffset:%lld:%lld", group_id, state->id));
	sb_write_varint(sb, state->conditions_id.count);
	ht_reserve_varint(sb, res, temp_sprintf("StateEntryCommOffset:%lld:%lld", group_id, state->id));
	sb_write_varint(sb, state->entry_commands_ids.count);
	ht_reserve_varint(sb, res, temp_sprintf("StateExitCommOffset:%lld:%lld", group_id, state->id));
	sb_write_varint(sb, state->exit_commands_ids.count);
	ht_reserve_varint(sb, res, temp_sprintf("StateWhileCommOffset:%lld:%lld", group_id, state->id));
	sb_write_varint(sb, state->while_commands_ids.count);
}

void write_condition_info(String_Builder *sb, Dictstrzu *res, const Condition *condition, size_t id, const Dictzuzu *state_offsets) {
	if (condition->target_state_id < 0 || condition->target_state_offset < 0) {
		sb_write_varint(sb, -1);
	} else {
		const size_t *offset = ht_find(state_offsets, condition->target_state_id);
		assert(offset && "Invalid state id");
		sb_write_varint(sb, *offset);
	}
	if (ht_reserve_varint(sb, res, temp_sprintf("PassCommandsOffset:%zu", id))) {
		sb_write_varint(sb, condition->pass_commands_ids.count);
	}
	if (ht_reserve_varint(sb, res, temp_sprintf("ConditionOffsetsOffset:%zu", id))) {
		sb_write_varint(sb, condition->sub_conditions_ids.count);
	}
	if (ht_reserve_varint(sb, res, temp_sprintf("EvaluatorOffset:%zu", id))) {
		sb_write_varint(sb, condition->evaluator.count);
	}
}

void write_command(String_Builder *sb, Command *c, Dictstrzu *res, size_t id) {
	sb_write_int32t(sb, c->command_bank);
	sb_write_int32t(sb, c->command_id);
	ht_reserve_varint(sb, res, temp_sprintf("ArgsOffset:%zu", id));
	sb_write_varint(sb, c->args_ids.count);
}

void write_state_commands(String_Builder *sb, ESD *esd, Dictstrzu *res, size_t data_start, const State *state, size_t group_id) {
	if (state->entry_commands_ids.count == 0) {
		assert(ht_unreserve_varint(sb, res, temp_sprintf("StateEntryCommOffset:%lld:%lld", group_id, state->id), -1));
	} else {
		assert(ht_unreserve_varint(sb, res, temp_sprintf("StateEntryCommOffset:%lld:%lld", group_id, state->id), sb->count - data_start));
		da_foreach(size_t, id, &state->entry_commands_ids) {
			Command *c = &esd->commands.items[*id];
			write_command(sb, c, res, *id);
		}
	}
	if (state->exit_commands_ids.count == 0) {
		assert(ht_unreserve_varint(sb, res, temp_sprintf("StateExitCommOffset:%lld:%lld", group_id, state->id), -1));
	} else {
		assert(ht_unreserve_varint(sb, res, temp_sprintf("StateExitCommOffset:%lld:%lld", group_id, state->id), sb->count - data_start));
		da_foreach(size_t, id, &state->exit_commands_ids) {
			Command *c = &esd->commands.items[*id];
			write_command(sb, c, res, *id);
		}
	}
	if (state->while_commands_ids.count == 0) {
		assert(ht_unreserve_varint(sb, res, temp_sprintf("StateWhileCommOffset:%lld:%lld", group_id, state->id), -1));
	} else {
		assert(ht_unreserve_varint(sb, res, temp_sprintf("StateWhileCommOffset:%lld:%lld", group_id, state->id), sb->count - data_start));
		da_foreach(size_t, id, &state->while_commands_ids) {
			Command *c = &esd->commands.items[*id];
			write_command(sb, c, res, *id);
		}
	}
}

void write_condition(String_Builder *sb, ESD *esd, Dictstrzu *res, const Condition *c, size_t id, size_t data_start) {
	if (c->pass_commands_ids.count == 0) {
		assert(ht_unreserve_varint(sb, res, temp_sprintf("PassCommandsOffset:%zu", id), -1));
	} else {
		assert(ht_unreserve_varint(sb, res, temp_sprintf("PassCommandsOffset:%zu", id), sb->count - data_start));
		da_foreach(size_t, id, &c->pass_commands_ids) {
			Command *c = &esd->commands.items[*id];
			write_command(sb, c, res, *id);
		}
	}
}

typedef struct {
	size_t from;
	size_t to;
} Pair;
DA(Pair, DAPair);
typedef Ht(Pair, size_t) DictPairzu;

void write_esd_old(String_Builder *sb, ESD *esd) {
    size_t mark = temp_save();
	Dictstrzu res = {
		.hasheq = ht_cstr_hasheq,
	};
	sb_append_buf(sb, ESD_MAGIC, strlen(ESD_MAGIC));
	write_esd_header(sb, esd, &res);
	size_t data_start = sb->count;
	write_esd_data_header(sb, esd, &res);
	if (esd->state_groups.count == 0) {
		assert(ht_unreserve_varint(sb, &res, "StateGroupsOffset", -1));
	} else {
		assert(ht_unreserve_varint(sb, &res, "StateGroupsOffset", sb->count - data_start));
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		write_state_group_info(sb, sg, &res);
	}
	DAPair weird_pairs = {0};
	Dictzuzu state_offsets = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		assert(ht_unreserve_varint(sb, &res, temp_sprintf("StateGroupOffset:%lld:1", sg->id), sb->count - data_start));
		assert(ht_unreserve_varint(sb, &res, temp_sprintf("StateGroupOffset:%lld:2", sg->id), sb->count - data_start));
		size_t first_state = sb->count;
		da_foreach(size_t, state_id, &sg->states_ids) {
			assert(*state_id < esd->states.count);
			*ht_put(&state_offsets, *state_id) = sb->count - data_start;
			State *state = &esd->states.items[*state_id];
			write_state_info(sb, sg->id, state, &res);
		}
		if (sg->states_ids.count > 1) {
			Pair pair = {first_state, sb->count};
			da_append(&weird_pairs, pair);
			sb_reserve_ex(sb, STATE_SIZE);
		}
	}

	Dictzuzu condition_offsets = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, state_id, &sg->states_ids) {
			State *state = &esd->states.items[*state_id];
			da_foreach(size_t, cond_id, &state->conditions_id) {
				Condition *cond = &esd->conditions.items[*cond_id];
				*ht_put(&condition_offsets, *cond_id) = sb->count - data_start;
				write_condition_info(sb, &res, cond, *cond_id, &state_offsets);
			}
		}
	}

	// NOTE: in c# impl this is written by grouping together each group's state's & condition's
	// commands at the same place. Here, all the commands are written sequentially
	da_foreach(const StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, state_index, &sg->states_ids) {
			const State *state = &esd->states.items[*state_index];
			write_state_commands(sb, esd, &res, data_start, state, sg->id);
		}
	}
	// ht_foreach(value, &res) {
	// 	const char *key = ht_key(&res, value);
	// 	printf("%s = %zu\n", key , *value);
	// }
	da_foreach(const StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, state_id, &sg->states_ids) {
			const State *state = &esd->states.items[*state_id];
			da_foreach(size_t, cond_id, &state->conditions_id) {
				const Condition *c = &esd->conditions.items[*cond_id];
				write_condition(sb, esd, &res, c, *cond_id, data_start);
			}
		}
	}
	for (size_t i=0; i<esd->commands.count; i++) {
		const Command *c = &esd->commands.items[i];
		assert(ht_unreserve_varint(sb, &res, temp_sprintf("ArgsOffset:%zu", i), sb->count - data_start));
		da_foreach(size_t, id, &c->args_ids) {
			Arg *arg = &esd->args.items[*id];
			ht_reserve_varint(sb, &res, temp_sprintf("ByteCodeOffset:%lld:%lld", i, *id));
			sb_write_varint(sb, arg->count);
		}
	}

	assert(ht_unreserve_varint(sb, &res, "ConditionOffsetsOffset", sb->count - data_start));
	int condition_offsets_count = 0;
	// NOTE: in c# impl this is written by grouping together each group's state's & condition's
	// commands at the same place. Here, all the commands are written sequentially
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, state_index, &sg->states_ids) {
			const State *state = &esd->states.items[*state_index];
			assert(ht_unreserve_varint(sb, &res, temp_sprintf("StateCondOffset:%lld:%lld", sg->id, state->id), sb->count - data_start));
			da_foreach(size_t, cond_id, &state->conditions_id) {
				size_t *offset = ht_find(&condition_offsets, *cond_id);
				assert(offset && "Invalid key");
				sb_write_varint(sb, *offset);
			}
			condition_offsets_count += state->conditions_id.count;
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, state_id, &sg->states_ids) {
			const State *state = &esd->states.items[*state_id];
			da_foreach(size_t, cond_id, &state->conditions_id) { // NOTE: maybe this does not give all the conditions from the state [also same another loop is present above]
				const Condition *c = &esd->conditions.items[*cond_id];
				if (c->sub_conditions_ids.count == 0) {
					assert(ht_unreserve_varint(sb, &res, temp_sprintf("ConditionOffsetsOffset:%zu", *cond_id), -1));
				} else {
					assert(ht_unreserve_varint(sb, &res, temp_sprintf("ConditionOffsetsOffset:%zu", *cond_id), sb->count - data_start));
				}
				da_foreach(size_t, sc_id, &c->sub_conditions_ids) {
					size_t *sc_offset = ht_find(&condition_offsets, *sc_id);
					if (sc_offset) {
						sb_write_varint(sb, *sc_offset);
					} else  {
						sb_write_varint(sb, -1);
					}
				}
				condition_offsets_count += c->sub_conditions_ids.count;
			}
		}
	}
	assert(ht_unreserve_int32_t(sb, &res, "ConditionOffsetsCount", condition_offsets_count));

	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, state_id, &sg->states_ids) {
			const State *state = &esd->states.items[*state_id];
			da_foreach(size_t, cond_id, &state->conditions_id) {
				assert(ht_unreserve_varint(sb, &res, temp_sprintf("EvaluatorOffset:%zu", *cond_id), sb->count - data_start));
				const Condition *c = &esd->conditions.items[*cond_id];
				for (size_t i=0; i<c->evaluator.count; i++) {
					sb_append(sb, c->evaluator.items[i]);
				}
			}
		}
	}

	for (size_t i=0; i<esd->commands.count; i++) {
		const Command *c = &esd->commands.items[i];
		da_foreach(size_t, arg_id, &c->args_ids) {
			assert(ht_unreserve_varint(sb, &res, temp_sprintf("ByteCodeOffset:%lld:%lld", i, *arg_id), sb->count - data_start));
			const Arg *arg = &esd->args.items[*arg_id];
			da_foreach(uint8_t, val, arg) {
				sb_append(sb, *val);
			}
		}
	}
	assert(ht_unreserve_int32_t(sb, &res, "NameBlockOffset", sb->count - data_start));
	if (esd->name == NULL) {
		sb_write_varint(sb, -1);
	} else {
		sb_pad_align(sb, 2);
		assert(ht_unreserve_int32_t(sb, &res, "NameOffset", sb->count - data_start));
		sb_append_utf16t(sb, esd->name);
	}
	assert(ht_unreserve_int32_t(sb, &res, "UnkOffset1", sb->count - data_start));
	assert(ht_unreserve_int32_t(sb, &res, "UnkOffset2", sb->count - data_start));
	assert(ht_unreserve_int32_t(sb, &res, "DataSize", sb->count - data_start));
	sb_pad_align(sb, 0x10);

	da_foreach(Pair, p, &weird_pairs) {
		for (size_t i=0; i<STATE_SIZE; i++) {
			sb->items[p->to] = sb->items[p->from];
		}
	}

	ht_free(&condition_offsets);
	ht_free(&state_offsets);
	da_free(weird_pairs);
	// if (res.count != 0) asm("int3");
	ht_free(&res);
	temp_rewind(mark);
}

void test_all_expressions(ESD *esd) {
	Expression e = {0};
	da_foreach(Condition, c, &esd->conditions) {
		parse_expression(&c->evaluator, &e);
		print_expression(&e);
		dealloc_funcalls(&e);
		e.stack.count = 0;
		e.exprs.count = 0;
	}
	da_free(e.exprs);
	da_free(e.stack);
	// dealloc_expression(&e);
}

typedef packed_struct {
    varint id;
    varint offset;
    varint count;
    varint offset_dup;
} StateGroupFile;

typedef packed_struct {
	char magic[4];
	ESD_HEADER header;
	StateGroupHeader data;
	StateGroupFile *sg;
	StateHeader *sh;
	ConditionHeader *ch;
	CommandCallHeader *cch;
	ArgumentHeader *arg;
	varint *condition_offsets;
	uint8_t *binary;
} ESDFile;

ESDFile read_esd_file(String_View sv) {
	ESDFile esd = {0};
	size_t length = offsetof(ESDFile, sg);
	memcpy(&esd, sv.data, length);
	sv.data += length;
	esd.sg = (void *)sv.data;
	sv.data += sizeof(StateGroupFile)*esd.header.stateGroupCount;
	esd.sh = (void *)sv.data;
	sv.data += sizeof(StateHeader)*esd.header.stateCount;
	esd.ch = (void *)sv.data;
	sv.data += sizeof(ConditionHeader)*esd.header.conditionCount;
	esd.cch = (void *)sv.data;
	sv.data += sizeof(CommandCallHeader)*esd.header.commandCallCount;
	esd.arg = (void *)sv.data;
	sv.data += sizeof(ArgumentHeader)*esd.header.commandArgCount;
	esd.condition_offsets = (void *)sv.data;
	sv.data += sizeof(varint)*esd.header.conditionOffsetsCount;
	esd.binary = (void *)sv.data;
	return esd;
}

static inline void *get_from_offset(ESDFile *esd, size_t offset) {
	size_t header_size = sizeof(StateGroupHeader);
	return (void *)((char *)esd->sg - header_size + offset);
}

#define esd_file_get(esd, type, offset_at) (type *)get_from_offset((esd), (offset_at))

Arg rarg(ESDFile *esd_file, size_t offset) {
	ArgumentHeader *header = esd_file_get(esd_file, ArgumentHeader, offset);
	Arg arg = {0};
	da_init(&arg, header->argSize);
	for (int i=0; i<header->argSize; i++) {
		da_append(&arg, *esd_file_get(esd_file, uint8_t, header->argOffset+i));
	}
	return arg;
}

Command rcomm(ESD *esd, ESDFile *esd_file, size_t offset) {
	CommandCallHeader *header = esd_file_get(esd_file, CommandCallHeader, offset);
	Command comm = {0};
	comm.command_bank = header->CommandBank;
	comm.command_id = header->CommandID;
	da_init(&comm.args_ids, header->argsCount);
	for (int i=0; i<header->argsCount; i++) {
		assert(header->argsOffset > 0);
		size_t arg_offset = header->argsOffset + i*sizeof(ArgumentHeader);
		Arg arg = rarg(esd_file, arg_offset);
		da_append(&comm.args_ids, esd->args.count);
		da_append(&esd->args, arg);
	}
	return comm;
}

// returns the condition id of the saved condition if it already exists
// creates a new condition if it does not exist
size_t check_and_read_cond(ESD *esd, ESDFile *esd_file, size_t cond_offset);

Condition rcond(ESD *esd, ESDFile *esd_file, size_t offset) {
	ConditionHeader *header = esd_file_get(esd_file, ConditionHeader, offset);
	Condition cond = {0};
	cond.target_state_offset = header->stateOffset;
	cond.target_state_id = -1;
	da_init(&cond.pass_commands_ids, header->passCommandCount);
	for (int i=0; i<header->passCommandCount; i++) {
		size_t comm_offset = header->passCommandsOffset + i*sizeof(CommandCallHeader);
		Command comm = rcomm(esd, esd_file, comm_offset);
		da_append(&cond.pass_commands_ids, esd->commands.count);
		da_append(&esd->commands, comm);
	}
	da_init(&cond.sub_conditions_ids, header->conditionOffsetCount);
	for (int i=0; i<header->conditionOffsetCount; i++) {
		varint cond_offset_offset = *esd_file_get(esd_file, varint, header->conditionOffsetsOffset);
		assert(cond_offset_offset > 0);
		size_t cond_offset = (size_t)(cond_offset_offset + i*sizeof(ConditionHeader));
		size_t sub_cond_id = check_and_read_cond(esd, esd_file, cond_offset);
		da_append(&cond.sub_conditions_ids, sub_cond_id);
	}
	if (header->evaluatorOffset > 0) {
		cond.evaluator.items = get_from_offset(esd_file, header->evaluatorOffset);
		cond.evaluator.count = header->evaluatorLength;
	} else {
		cond.evaluator.items = NULL;
		cond.evaluator.count = 0;
	}
	return cond;
}

size_t check_and_read_cond(ESD *esd, ESDFile *esd_file, size_t cond_offset) {
	size_t *idx = ht_find(&esd->dict_cond, cond_offset);
	if (idx != NULL) return *idx;
	Condition cond = rcond(esd, esd_file, cond_offset);
	size_t index = esd->conditions.count;
	*ht_put(&esd->dict_cond, cond_offset) = index;
	da_append(&esd->conditions, cond);
	return index;
}

State rst(ESD *esd, ESDFile *esd_file, size_t offset) {
	StateHeader *header = esd_file_get(esd_file, StateHeader, offset);
	State state = {0};
	state.id = header->ID;
	da_init(&state.conditions_id, header->conditionOffsetCount);
	for (int i=0; i<header->conditionOffsetCount; i++) {
		varint cond_offset = *esd_file_get(esd_file, varint, header->conditionOffsetsOffset + i*sizeof(varint));
		assert(cond_offset > 0);
		size_t cond_id = check_and_read_cond(esd, esd_file, (size_t)cond_offset);
		da_append(&state.conditions_id, cond_id);
	}
	da_init(&state.entry_commands_ids, header->entryCommandCount);
	for (int i=0; i<header->entryCommandCount; i++) {
		size_t comm_offset = header->entryCommandsOffset + i*sizeof(CommandCallHeader);
		Command comm = rcomm(esd, esd_file, comm_offset);
		da_append(&state.entry_commands_ids, esd->commands.count);
		da_append(&esd->commands, comm);
	}
	da_init(&state.exit_commands_ids, header->exitCommandCount);
	for (int i=0; i<header->exitCommandCount; i++) {
		size_t comm_offset = header->exitCommandsOffset + i*sizeof(CommandCallHeader);
		Command comm = rcomm(esd, esd_file, comm_offset);
		da_append(&state.exit_commands_ids, esd->commands.count);
		da_append(&esd->commands, comm);
	}
	da_init(&state.while_commands_ids, header->whileCommandCount);
	for (int i=0; i<header->whileCommandCount; i++) {
		size_t comm_offset = header->whileCommandsOffset + i*sizeof(CommandCallHeader);
		Command comm = rcomm(esd, esd_file, comm_offset);
		da_append(&state.while_commands_ids, esd->commands.count);
		da_append(&esd->commands, comm);
	}
	*ht_put(&esd->dict_state, offset) = esd->states.count;
	return state;
}

StateGroup rsg(ESD *esd, ESDFile *esd_file, size_t i) {
	StateGroupFile *sgf = &esd_file->sg[i];
	StateGroup sg = {0};
	sg.id = sgf->id;
	assert(sgf->offset == sgf->offset_dup);
	da_init(&sg.states_ids, sgf->count);
	for (int i=0; i<sgf->count; i++) {
		size_t state_offset = sgf->offset + esd_file->header.stateSize*i;
		State s = rst(esd, esd_file, state_offset);
		da_append(&sg.states_ids, esd->states.count);
		da_append(&esd->states, s);
	}
	return sg;
}

void read_esd_info(const ESDFile *esd_file, ESD *esd) {
	size_t size = sizeof(StateGroupHeader);
	if (esd_file->data.nameOffset > 0) {
		esd->name = (void *)((char *)esd_file->sg - size + esd_file->data.nameOffset);
		esd->nameLength = esd_file->data.nameLength;
	}
	esd->Unk70 = esd_file->data.Unk70;
	esd->Unk74 = esd_file->data.Unk74;
	esd->Unk78 = esd_file->data.Unk78;
	esd->Unk7C = esd_file->data.Unk7C;
}

ESD read_esd(String_View sv) {
	ESD esd = {0};
	ESDFile esd_file = read_esd_file(sv);
	read_esd_info(&esd_file, &esd);
	da_init(&esd.state_groups, esd_file.header.stateGroupCount);
	da_init(&esd.states, esd_file.header.stateCount);
	da_init(&esd.conditions, esd_file.header.conditionCount);
	da_init(&esd.commands, esd_file.header.commandCallCount);
	da_init(&esd.args, esd_file.header.commandArgCount);
	for (int i=0; i<esd_file.header.stateGroupCount; i++) {
		StateGroup sg = rsg(&esd, &esd_file, i);
		da_append(&esd.state_groups, sg);
	}
	// populate the target states that is not initialized when creating the conditions
	for (size_t i=0; i<esd.conditions.count; i++) {
		Condition *cond = &esd.conditions.items[i];
		if (cond->target_state_offset <= 0) continue;
		size_t *state_id = ht_find(&esd.dict_state, cond->target_state_offset);
		assert(state_id);
		cond->target_state_id = *state_id;
	}
	return esd;
}

void weh(String_Builder *sb, const ESD *esd) {
	sb_append_cstr(sb, ESD_MAGIC);
	sb_write_int32t(sb, 1);
	sb_write_int32t(sb, 2);
	sb_write_int32t(sb, 2);
	sb_write_int32t(sb, 0x54);
	sb_reserve(sb, sizeof(int32_t)); // data_size
	sb_write_int32t(sb, 6);
	sb_write_int32t(sb, 0x48);
	sb_write_int32t(sb, 1);
	sb_write_int32t(sb, 0x20);
	sb_write_int32t(sb, esd->state_groups.count);
	sb_write_int32t(sb, STATE_SIZE);
	sb_reserve(sb, sizeof(int32_t)); // state_count-x
	sb_write_int32t(sb, 0x38);
	sb_write_int32t(sb, esd->conditions.count);
	sb_write_int32t(sb, 0x18);
	sb_write_int32t(sb, esd->commands.count);
	sb_write_int32t(sb, 0x10);
	sb_write_int32t(sb, esd->args.count);
	sb_reserve(sb, sizeof(int32_t)); // conditionOffsetsOffset
	sb_reserve(sb, sizeof(int32_t)); // conditionOffsetCount
	sb_reserve(sb, sizeof(int32_t)); // nameBlockOffset
	sb_reserve(sb, sizeof(int32_t)); // nameLength
	sb_reserve(sb, sizeof(int32_t)); // unkOffset1
	sb_write_int32t(sb, 0);
	sb_reserve(sb, sizeof(int32_t)); // unkOffset2
	sb_write_int32t(sb, 0);
}

void wsgh(String_Builder *sb, const ESD *esd) {
	sb_write_int32t(sb, 1);
	sb_write_int32t(sb, esd->Unk70);
	sb_write_int32t(sb, esd->Unk74);
	sb_write_int32t(sb, esd->Unk78);
	sb_write_int32t(sb, esd->Unk7C);
	sb_write_int32t(sb, 0);
	sb_reserve(sb, sizeof(varint)); // stateGroupsOffset-x
	sb_write_varint(sb, esd->state_groups.count);
	sb_reserve(sb, sizeof(varint)); // nameOffset
	sb_reserve(sb, sizeof(varint)); // nameLength
	sb_write_varint(sb, -1);
	sb_write_varint(sb, -1);
}

#define sb_offset(sb) (sb)->count - sizeof(ESD_HEADER) - ESD_MAGIC_LENGTH
#define sb_base(sb) (sb)->items + ESD_MAGIC_LENGTH + sizeof(ESD_HEADER)

void wst(String_Builder *sb, const State *s) {
	sb_write_varint(sb, s->id);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // conditionOffsetsOffset
	sb_write_varint(sb, s->conditions_id.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // entryCommandsOffset
	sb_write_varint(sb, s->entry_commands_ids.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // exitCommandsOffset
	sb_write_varint(sb, s->exit_commands_ids.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // whileCommandsOffset
	sb_write_varint(sb, s->while_commands_ids.count);
}

void wcomm(String_Builder *sb, const Command *comm) {
	sb_write_int32t(sb, comm->command_bank);
	sb_write_int32t(sb, comm->command_id);
	sb_reserve(sb, sizeof(varint)); // argsOffset
	sb_write_varint(sb, comm->args_ids.count);
}

void wcond(String_Builder *sb, const Condition *cond, varint state_offset) {
	// const Condition *cond = &esd->conditions.items[i];
	// sb_reserve(sb, sizeof(varint)); // stateOffset (target)
	sb_write_varint(sb, state_offset);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // passCommandsOffset
	// sb_reserve(sb, sizeof(varint)); // passCommandsCount
	sb_write_varint(sb, cond->pass_commands_ids.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // conditionOffsetsOffset
	// sb_reserve(sb, sizeof(varint)); // conditionOffsetsCount
	sb_write_varint(sb, cond->sub_conditions_ids.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // evaluatorOffset
	// sb_reserve(sb, sizeof(varint)); // evaluatorLength
	sb_write_varint(sb, cond->evaluator.count);
}

int64_t pack(int32_t a, int32_t b) {
    return ((int64_t)a << 32) | ((uint32_t)b);
}

typedef packed_struct {
	int64_t first;
	int64_t second;
	int64_t third;
} int3x64_t;

typedef Ht(int3x64_t, size_t) Dicthuge;

bool cond_is_same(Condition *c1, Condition *c2) {
	if (c1->target_state_id != c2->target_state_id) return false;
	if (c1->sub_conditions_ids.count != c2->sub_conditions_ids.count) return false;
	if (c1->pass_commands_ids.count != c2->pass_commands_ids.count) return false;
	if (c1->evaluator.count != c2->evaluator.count) return false;
	for (size_t i=0; i<c1->sub_conditions_ids.count; i++) {
		if (c1->sub_conditions_ids.items[i] != c2->sub_conditions_ids.items[i]) return false;
	}
	for (size_t i=0; i<c1->pass_commands_ids.count; i++) {
		if (c1->pass_commands_ids.items[i] != c2->pass_commands_ids.items[i]) return false;
	}
	for (size_t i=0; i<c1->evaluator.count; i++) {
		if (c1->evaluator.items[i] != c2->evaluator.items[i]) return false;
	}
	return true;
}

bool da_containsz(const DArrz *arr, size_t item) {
	da_foreach(size_t, i, arr) {
		if (*i == item) return true;
	}
	return false;
}

// if there is assert (condition) 0x41 and 0xa1 is added to the beginning of the state
// except if the target state has already been added
void write_41_a1(String_Builder *sb, ESD *esd, State *s, DArrz *added_states) {
	da_foreach(size_t, cid, &s->conditions_id) {
		Condition *assert_cond = &esd->conditions.items[*cid];
		if (assert_cond->evaluator.count == 4) {
			if (assert_cond->evaluator.items[0] == EzGetRet
					&& (assert_cond->evaluator.items[1] == EzRetVal || assert_cond->evaluator.items[1] <= EzNumber)
					&& (assert_cond->evaluator.items[2] == EzNeq || assert_cond->evaluator.items[2] == EzEq)
					&& assert_cond->evaluator.items[3] == EzFin) {
				assert(assert_cond->sub_conditions_ids.count == 1);
				Condition *scond = &esd->conditions.items[assert_cond->sub_conditions_ids.items[0]];
				if (da_containsz(added_states, scond->target_state_id)) continue;
				sb_append(sb, 0x41);
				sb_append(sb, (char)EzFin);
				da_append(added_states, scond->target_state_id);
			}
		}
	}
}

void write_args(String_Builder *sb, ESD *esd, Dictzuzu *end_args_offsets, Command *comm) {
	for (size_t i=0; i<comm->args_ids.count; i++) {
		size_t aid = comm->args_ids.items[i];
		Arg *arg = &esd->args.items[aid];
		if (ht_find(end_args_offsets, aid) != NULL) continue;
		*ht_put(end_args_offsets, aid) = sb_offset(sb);
		sb_append_buf(sb, arg->items, arg->count);
	}
}

void write_entry_commands_args(String_Builder *sb, ESD *esd, Dictzuzu *end_args_offsets, State *s, DArrz *added_states) {
	write_41_a1(sb, esd, s, added_states);
	da_foreach(size_t, ecid, &s->entry_commands_ids) {
		Command *comm = &esd->commands.items[*ecid];
		write_args(sb, esd, end_args_offsets, comm);
	}
}

void write_pass_commands_args(String_Builder *sb, ESD *esd, Dictzuzu *end_args_offsets, State *s) {
	da_foreach(size_t, cid, &s->conditions_id) {
		Condition *cond = &esd->conditions.items[*cid];
		da_foreach(size_t, comid, &cond->pass_commands_ids) {
			Command *comm = &esd->commands.items[*comid];
			write_args(sb, esd, end_args_offsets, comm);
		}
	}
}

void write_esd(String_Builder *sb, ESD *esd) {
	weh(sb, esd);
	wsgh(sb, esd);
	void *dst =  sb_base(sb) + offsetof(StateGroupHeader, stateGroupsOffset);
	size_t size = sizeof(varint);
	size_t src = sb_offset(sb);
	memcpy(dst, &src, size);

	// write state groups
	for (size_t i=0; i<esd->state_groups.count; i++) {
		const StateGroup *sg = &esd->state_groups.items[i];
		sb_write_varint(sb, sg->id);
		sb_reserve(sb, sizeof(varint)); // stateOffset
		sb_write_varint(sb, sg->states_ids.count);
		sb_reserve(sb, sizeof(varint)); // stateOffsetDup
	}

	// write states
	size_t state_count = 0;
	size_t state_zero_id = 0;
	DArrz state_offsets = {0};
	DAPair dup_states = {0};
	for (size_t i=0; i<esd->states.count; i++) {
		const State *s = &esd->states.items[i];
		if (s->id == 0) {
			state_zero_id = i;
		}
		da_append(&state_offsets, sb_offset(sb));
		wst(sb, s);
		state_count++;
		if (i == 0 || s->id == 0) continue;
		if (i < esd->states.count - 1) {
			if (esd->states.items[i+1].id != 0) continue;
		}
		s = &esd->states.items[state_zero_id];
		Pair p = {.from = state_offsets.items[state_zero_id], .to = sb_offset(sb)};
		da_append(&dup_states, p);
		wst(sb, s);
		state_count++;
	}

	// write state count to header
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, stateCount);
	size = sizeof(int32_t);
	src = state_count;
	memcpy(dst, &src, size);

	// back patch the state offsets
	for (size_t i=0; i<esd->state_groups.count; i++) {
		const StateGroup *sg = &esd->state_groups.items[i];
		if (sg->states_ids.count == 0) continue;
		size_t idx = sg->states_ids.items[0];
		assert(idx < state_offsets.count);
		size_t offset = state_offsets.items[idx];

		dst =  sb_base(sb) + sizeof(StateGroupHeader) + offsetof(StateGroupFile, offset) + i*sizeof(StateGroupFile);
		size = sizeof(varint);
		src = offset;
		memcpy(dst, &src, size);
		dst =  sb_base(sb) + sizeof(StateGroupHeader) + offsetof(StateGroupFile, offset_dup) + i*sizeof(StateGroupFile);
		memcpy(dst, &src, size);
	}

	// write conditions
	size_t condition_count = 0;
	ht_reset(&esd->dict_cond);
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				size_t *found = ht_find(&esd->dict_cond, *cid);
				if (found == NULL) {
					*ht_put(&esd->dict_cond, *cid) = sb_offset(sb);
					if (cond->target_state_id >= 0) {
						wcond(sb, cond, state_offsets.items[cond->target_state_id]);
					} else {
						wcond(sb, cond, -1);
					}
					condition_count++;
				}
			}
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, scid, &cond->sub_conditions_ids) {
					Condition *scond = &esd->conditions.items[*scid];
					size_t *found = ht_find(&esd->dict_cond, *scid);
					if (found == NULL) {
						*ht_put(&esd->dict_cond, *scid) = sb_offset(sb);
						if (scond->target_state_id >= 0) {
							wcond(sb, scond, state_offsets.items[scond->target_state_id]);
						} else {
							wcond(sb, scond, -1);
						}
						condition_count++;
					}
				}
			}
		}
	}
	assert(condition_count == esd->conditions.count);

	// write commands
	size_t commands_count = 0;
	Dictzuzu entry_commands_offsets = {0};
	Dictzuzu exit_commands_offsets = {0};
	Dictzuzu while_commands_offsets = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, ecid, &s->entry_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				*ht_put(&entry_commands_offsets, *ecid) = sb_offset(sb);
				wcomm(sb, comm);
				commands_count++;
			}
			da_foreach(size_t, ecid, &s->exit_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				*ht_put(&exit_commands_offsets, *ecid) = sb_offset(sb);
				wcomm(sb, comm);
				commands_count++;
			}
			da_foreach(size_t, ecid, &s->while_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				*ht_put(&while_commands_offsets, *ecid) = sb_offset(sb);
				wcomm(sb, comm);
				commands_count++;
			}
		}
	}
	Dictzuzu pass_commands_offsets = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, pcid, &cond->pass_commands_ids) {
					Command *comm = &esd->commands.items[*pcid];
					*ht_put(&pass_commands_offsets, *pcid) = sb_offset(sb);
					wcomm(sb, comm);
					commands_count++;
				}
			}
		}
	}
	assert(commands_count == esd->commands.count);

	// back patch commands offsets
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			size_t state_offset = state_offsets.items[*sid];
			dst =  sb_base(sb) + state_offset + offsetof(StateHeader, entryCommandsOffset);
			size = sizeof(varint);
			size_t count = 0;
			varint first_offset = -1;
			da_foreach(size_t, ecid, &s->entry_commands_ids) {
				if (count == 0) {
					first_offset = *ht_find(&entry_commands_offsets, *ecid);
				}
				count++;
				if (count == s->entry_commands_ids.count && s->entry_commands_ids.count > 0) {
					// if there are multiple entry commands for the same state id of a state group,
					// the last entry command offset is set to the first entry command offset
					assert(first_offset > 0);
					memcpy(dst, &first_offset, size);
				} else {
					memcpy(dst, ht_find(&entry_commands_offsets, *ecid), size);
				}
			}
			dst =  sb_base(sb) + state_offset + offsetof(StateHeader, exitCommandsOffset);
			da_foreach(size_t, ecid, &s->exit_commands_ids) {
				TODO("Not Implemented");
				memcpy(dst, ht_find(&exit_commands_offsets, *ecid), size);
			}
			dst =  sb_base(sb) + state_offset + offsetof(StateHeader, whileCommandsOffset);
			da_foreach(size_t, ecid, &s->while_commands_ids) {
				TODO("Not Implemented");
				memcpy(dst, ht_find(&while_commands_offsets, *ecid), size);
			}
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				size_t *cond_offset = ht_find(&esd->dict_cond, *cid);
				assert(cond_offset);
				dst =  sb_base(sb) + (*cond_offset) + offsetof(ConditionHeader, passCommandsOffset);
				size_t count = 0;
				varint first_offset = -1;
				da_foreach(size_t, pcid, &cond->pass_commands_ids) {
					if (count == 0) {
						first_offset = *ht_find(&pass_commands_offsets, *pcid);
					}
					count++;
					if (count == cond->pass_commands_ids.count && cond->pass_commands_ids.count > 0) {
						// if there are multiple entry commands for the same state id of a state group,
						// the last entry command offset is set to the first entry command offset
						assert(first_offset > 0);
						memcpy(dst, &first_offset, size);
					} else {
						memcpy(dst, ht_find(&pass_commands_offsets, *pcid), size);
					}
				}
			}
		}
	}

	// write args
	size_t args_count = 0;
	Dictzuzu args_offsets = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, ecid, &s->entry_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				da_foreach(size_t, aid, &comm->args_ids) {
					Arg *arg = &esd->args.items[*aid];
					*ht_put(&args_offsets, *aid) = sb_offset(sb);
					sb_reserve(sb, sizeof(varint)); // argOffset
					sb_write_varint(sb, arg->count);
					args_count++;
				}
			}
			da_foreach(size_t, ecid, &s->exit_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				da_foreach(size_t, aid, &comm->args_ids) {
					Arg *arg = &esd->args.items[*aid];
					*ht_put(&args_offsets, *aid) = sb_offset(sb);
					sb_reserve(sb, sizeof(varint)); // argOffset
					sb_write_varint(sb, arg->count);
					args_count++;
				}
			}
			da_foreach(size_t, ecid, &s->while_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				da_foreach(size_t, aid, &comm->args_ids) {
					Arg *arg = &esd->args.items[*aid];
					*ht_put(&args_offsets, *aid) = sb_offset(sb);
					sb_reserve(sb, sizeof(varint)); // argOffset
					sb_write_varint(sb, arg->count);
					args_count++;
				}
			}
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, pcid, &cond->pass_commands_ids) {
					Command *comm = &esd->commands.items[*pcid];
					da_foreach(size_t, aid, &comm->args_ids) {
						Arg *arg = &esd->args.items[*aid];
						*ht_put(&args_offsets, *aid) = sb_offset(sb);
						sb_reserve(sb, sizeof(varint)); // argOffset
						sb_write_varint(sb, arg->count);
						args_count++;
					}
				}
			}
		}
	}
	assert(args_count == esd->args.count);

	// back patch args
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, ecid, &s->entry_commands_ids) {
				size_t *offset = ht_find(&entry_commands_offsets, *ecid);
				assert(offset);
				dst =  sb_base(sb) + (*offset) + offsetof(CommandCallHeader, argsOffset);
				size = sizeof(varint);
				Command *comm = &esd->commands.items[*ecid];
				if (comm->args_ids.count == 0) {
					varint value = -1;
					memcpy(dst, &value, size);
				} else {
					size_t *arg_offset = ht_find(&args_offsets, comm->args_ids.items[0]);
					assert(arg_offset);
					memcpy(dst, arg_offset, size);
				}
			}
			da_foreach(size_t, ecid, &s->exit_commands_ids) {
				size_t *offset = ht_find(&exit_commands_offsets, *ecid);
				assert(offset);
				dst =  sb_base(sb) + (*offset) + offsetof(CommandCallHeader, argsOffset);
				size = sizeof(varint);
				Command *comm = &esd->commands.items[*ecid];
				if (comm->args_ids.count == 0) {
					varint value = -1;
					memcpy(dst, &value, size);
				} else {
					size_t *arg_offset = ht_find(&args_offsets, comm->args_ids.items[0]);
					assert(arg_offset);
					memcpy(dst, arg_offset, size);
				}
			}
			da_foreach(size_t, ecid, &s->while_commands_ids) {
				size_t *offset = ht_find(&while_commands_offsets, *ecid);
				assert(offset);
				dst =  sb_base(sb) + (*offset) + offsetof(CommandCallHeader, argsOffset);
				size = sizeof(varint);
				Command *comm = &esd->commands.items[*ecid];
				if (comm->args_ids.count == 0) {
					varint value = -1;
					memcpy(dst, &value, size);
				} else {
					size_t *arg_offset = ht_find(&args_offsets, comm->args_ids.items[0]);
					assert(arg_offset);
					memcpy(dst, arg_offset, size);
				}
			}
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, pcid, &cond->pass_commands_ids) {
					size_t *offset = ht_find(&pass_commands_offsets, *pcid);
					assert(offset);
					dst =  sb_base(sb) + (*offset) + offsetof(CommandCallHeader, argsOffset);
					size = sizeof(varint);
					Command *comm = &esd->commands.items[*pcid];
					if (comm->args_ids.count == 0) {
						varint value = -1;
						memcpy(dst, &value, size);
					} else {
						size_t *arg_offset = ht_find(&args_offsets, comm->args_ids.items[0]);
						assert(arg_offset);
						memcpy(dst, arg_offset, size);
					}
				}
			}
		}
	}

	// write condition_offsets_offsets to header
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, conditionOffsetsOffset);
	size = sizeof(int32_t);
	src = sb_offset(sb);
	memcpy(dst, &src, size);

	// write conditions offsets
	DictPairzu condition_offset_offsets = {0};
	size_t condition_offsets_offsets_count = 0;
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				size_t *offset = ht_find(&esd->dict_cond, *cid);
				assert(offset);
				Pair key = {*cid, (size_t)s->id};
				size_t *coo = ht_find(&condition_offset_offsets, key);
				if (coo == NULL) {
					*ht_put(&condition_offset_offsets, key) = sb_offset(sb);
				} else {
					*ht_put(&condition_offset_offsets, key) = *coo;
				}
				sb_write_varint(sb, *offset);
				condition_offsets_offsets_count++;
			}
		}
		if (sg->states_ids.count > 1) {
			State *fs = &esd->states.items[sg->states_ids.items[0]];
			if (fs->conditions_id.count > 0) {
				size_t *fco = ht_find(&esd->dict_cond, fs->conditions_id.items[0]);
				assert(fco);
				condition_offsets_offsets_count++;
				sb_write_varint(sb, *fco);
			}
		}
	}
	Dictzuzu same_cid_scid = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, scid, &cond->sub_conditions_ids) {
					size_t *offset = ht_find(&esd->dict_cond, *scid);
					assert(offset);
					size_t *found = ht_find(&same_cid_scid, (*cid)*(*scid));
					// if the cid and scid are same we skip those
					if (found) continue;
					*ht_put(&same_cid_scid, (*scid)*(*cid)) = sb_offset(sb);

					Pair key = {*scid, s->id};
					// TODO: maybe check if it already exists here as well
					*ht_put(&condition_offset_offsets, key) = sb_offset(sb);
					condition_offsets_offsets_count++;
					sb_write_varint(sb, *offset);
				}
			}
		}
	}

	// write condition_offsets_offsets_count to header
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, conditionOffsetsCount);
	size = sizeof(int32_t);
	src = condition_offsets_offsets_count;
	memcpy(dst, &src, size);

	// back patch condition offsets offset in state header
	for (size_t i=0; i<esd->state_groups.count; i++) {
		StateGroup *sg = &esd->state_groups.items[i];
		for (size_t j=0; j<sg->states_ids.count; j++) {
			size_t state_index = sg->states_ids.items[j];
			State *s = &esd->states.items[state_index];
			size_t offset = state_offsets.items[state_index];
			dst =  sb_base(sb) + offset + offsetof(StateHeader, conditionOffsetsOffset);
			if (s->conditions_id.count > 0) {
				size_t cid = s->conditions_id.items[0];
				Pair key = {cid, s->id};
				size_t *condition_offset = ht_find(&condition_offset_offsets, key);
				assert(condition_offset);
				memcpy(dst, condition_offset, sizeof(varint));
			} else {
				varint value = -1;
				memcpy(dst, &value, sizeof(varint));
			}
		}
	}

	// back patch condition offsets offset in condition header
	for (size_t i=0; i<esd->state_groups.count; i++) {
		StateGroup *sg = &esd->state_groups.items[i];
		for (size_t j=0; j<sg->states_ids.count; j++) {
			size_t state_index = sg->states_ids.items[j];
			State *s = &esd->states.items[state_index];
			for (size_t k=0; k<s->conditions_id.count; k++) {
				size_t cond_index = s->conditions_id.items[k];
				Condition *cond = &esd->conditions.items[cond_index];
				if (cond->sub_conditions_ids.count == 0) continue;
				size_t scid = cond->sub_conditions_ids.items[0];

				size_t *condition_offset = ht_find(&esd->dict_cond, cond_index);
				assert(condition_offset);

				Pair key = {scid, s->id};
				size_t *condition_offset_offset = ht_find(&condition_offset_offsets, key);
				if (condition_offset_offset == NULL) continue;

				dst = sb_base(sb) + (*condition_offset) + offsetof(ConditionHeader, conditionOffsetsOffset);
				memcpy(dst, condition_offset_offset, sizeof(varint));
			}
		}
	}

	// write evalulator from conditions
	Dictzuzu evalulator_offsets = {0};
	Evaluator *first_eval = NULL;
	for (size_t i=0; i<esd->state_groups.count; i++) {
		StateGroup *sg = &esd->state_groups.items[i];
		for (size_t j=0; j<sg->states_ids.count; j++) {
			size_t state_index = sg->states_ids.items[j];
			State *s = &esd->states.items[state_index];
			for (size_t k=0; k<s->conditions_id.count; k++) {
				size_t cond_index = s->conditions_id.items[k];
				Condition *cond = &esd->conditions.items[cond_index];
				if (ht_find(&evalulator_offsets, cond_index) != NULL) continue;
				// printf("offset: %zu, sg: %zu, sid: %zu, c: %zu\n", sb_offset(sb), sg->id, s->id, cond_index);
				*ht_put(&evalulator_offsets, cond_index) = sb_offset(sb);
				sb_append_buf(sb, cond->evaluator.items, cond->evaluator.count);
				if (!first_eval) {
					first_eval = &cond->evaluator;
				}
			}
		}
	}
	// for (size_t i=0; i<esd->state_groups.count; i++) {
	// 	StateGroup *sg = &esd->state_groups.items[i];
	// 	for (size_t j=0; j<sg->states_ids.count; j++) {
	// 		size_t state_index = sg->states_ids.items[j];
	// 		State *s = &esd->states.items[state_index];
	// 		for (size_t k=0; k<s->conditions_id.count; k++) {
	// 			size_t cond_index = s->conditions_id.items[k];
	// 			Condition *cond = &esd->conditions.items[cond_index];
	// 			for (size_t l=0; l<cond->sub_conditions_ids.count; l++) {
	// 				size_t scond_index = cond->sub_conditions_ids.items[l];
	// 				Condition *scond = &esd->conditions.items[scond_index];
	// 				if (scond->evaluator.count == 0) continue;
	// 				if (ht_find(&evalulator_offsets, scond_index) != NULL) continue;
	// 				// printf("offset: %zu, sg: %zu, sid: %zu, c: %zu\n", sb_offset(sb), sg->id, s->id, scond_index);
	// 				*ht_put(&evalulator_offsets, scond_index) = sb_offset(sb);
	// 				sb_append_buf(sb, scond->evaluator.items, scond->evaluator.count);
	// 			}
	// 		}
	// 	}
	// }

	// back patch evaluator offsets
	for (size_t i=0; i<esd->state_groups.count; i++) {
		StateGroup *sg = &esd->state_groups.items[i];
		for (size_t j=0; j<sg->states_ids.count; j++) {
			size_t state_index = sg->states_ids.items[j];
			State *s = &esd->states.items[state_index];
			for (size_t k=0; k<s->conditions_id.count; k++) {
				size_t cond_index = s->conditions_id.items[k];
				Condition *cond = &esd->conditions.items[cond_index];
				if (cond->evaluator.count == 0) continue;
				size_t *condition_offset = ht_find(&esd->dict_cond, cond_index);
				assert(condition_offset);
				dst = sb_base(sb) + (*condition_offset) + offsetof(ConditionHeader, evaluatorOffset);
				size_t *eval_offset = ht_find(&evalulator_offsets, cond_index);
				assert(eval_offset);
				memcpy(dst, eval_offset, sizeof(varint));
				// for (size_t l=0; l<cond->sub_conditions_ids.count; l++) {
				// 	size_t scond_index = cond->sub_conditions_ids.items[l];
				// 	Condition *scond = &esd->conditions.items[scond_index];
				// 	if (scond->evaluator.count == 0) continue;
				// 	size_t *scond_offset = ht_find(&esd->dict_cond, scond_index);
				// 	assert(scond_offset);
				// 	dst = sb_base(sb) + (*scond_offset) + offsetof(ConditionHeader, evaluatorOffset);
				// 	size_t *eval_offset = ht_find(&evalulator_offsets, scond_index);
				// 	assert(eval_offset);
				// 	memcpy(dst, eval_offset, sizeof(varint));
				// }
			}
		}
	}


	// write args
	Dictzuzu end_args_offsets = {0};
	DArrz added_states = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		added_states.count = 0;
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			write_entry_commands_args(sb, esd, &end_args_offsets, s, &added_states);
		}
	}
	da_free(added_states);
	da_foreach(StateGroup, sg, &esd->state_groups) {
		added_states.count = 0;
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			write_pass_commands_args(sb, esd, &end_args_offsets, s);
		}
	}


	// copy all backpatching to duplicate state
	for (size_t i=0; i<dup_states.count; i++) {
		Pair *p = &dup_states.items[i];
		char *from = sb_base(sb) + p->from;
		char *to = sb_base(sb) + p->to;
		memcpy(to, from, STATE_SIZE);
	}

	da_free(dup_states);
	da_free(state_offsets);
	ht_free(&entry_commands_offsets);
	ht_free(&exit_commands_offsets);
	ht_free(&while_commands_offsets);
	ht_free(&pass_commands_offsets);
	ht_free(&args_offsets);
	ht_free(&same_cid_scid);
	ht_free(&condition_offset_offsets);
	ht_free(&evalulator_offsets);
	ht_free(&end_args_offsets);
}

int main() {
	int ret = 0;
	// ret += test_loading_all_maps();
	// ret += test_loading_all_eventlocations();
	// ret += test_loading_all_eventparams();
	// ret += test_loading_all_mapobjinstanceparams();
	String_Builder sb = {0};
	String_Builder wsb = {0};
	const char *path = esd_path("m10_18_00_00");
	if (!nob_read_entire_file(path, &sb)) {
		return 1;
	}
	printf("Read %zu bytes from %s\n", sb.count, path);
	String_View sv = sb_to_sv(sb);

	ESD esd = read_esd(sv);

	write_esd(&wsb, &esd);
	if (!write_entire_file("out.esd", wsb.items, wsb.count)) return 1;

	// print_state_group(&esd, &esd.state_groups.items[50]);
	// for (size_t i=0; i<esd.state_groups.count; i++) {
	// 	print_state_group(&esd, &esd.state_groups.items[i]);
	// }

	// test_all_expressions(&esd);
	// write_esd_old(&wsb, &esd);
	// if (!write_entire_file("out.esd", wsb.items, wsb.count)) return 1;

	// String_View wsv = sb_to_sv(sb);
	// ESD wesd = read_esd(wsv);
	// free_esd(&wesd);
	//
	// for (size_t i=0; i<wsb.count; i++) {
	// 	if (sb.items[i] != wsb.items[i]) asm("int3");
	// }

	free_esd(&esd);
	da_free(sb);
	da_free(wsb);
	// asm("int3");
	return ret;
}

// incorrect
// data_size -> will be correct if others will be correct
// conditionOffsetsOffset - ^
