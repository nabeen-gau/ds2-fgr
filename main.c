#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define packed_struct struct __attribute__((packed))

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

int test_loading_all_eventparam() {
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

int main() {
	int ret = 0;
	// ret += test_loading_all_maps();
	// ret += test_loading_all_eventlocations();
	// ret += test_loading_all_eventparam();
	String_Builder sb = {0};
	String_Builder wsb = {0};
	const char *path = map_object_instance_param_path("m10_02_00_00");
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
	da_free(sb);
	da_free(wsb);

	printf("Done.\n");
	return ret;
}
