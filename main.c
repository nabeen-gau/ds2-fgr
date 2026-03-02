#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

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
#define assert_val(item, val) if ((item) != (val)) {nob_log(ERROR, "Invalid value"); exit(1);}


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


typedef struct {
	ModelType type;
	bool has_type_data;
	utf16_t *name;
} Entry;
DA(Entry, Model);

typedef struct {
    Model MapPieces;
    Model Objects;
    Model Collisions;
    Model Navmeshes;
} ModelParam;

typedef struct {
	int32_t version;
	int32_t offset_count;
	int64_t name_offset;
	varint *entry_offsets;
	varint next_param_offset;
} ParamSt;

typedef struct {
	char r, g, b, a;
} Color;

typedef uint8_t byte;

#define LightPatternSize 0x3B
typedef struct {
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
typedef struct {
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
typedef struct {
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
typedef struct {
	Color color;
	char pattern[BGColorPatternSize]; // 0x00
} BGColor;

typedef struct { 
	float x,y; 
} V2f;

typedef struct { 
	float x,y,z; 
} V3f;

typedef struct {
	V3f translation;
	int32_t degree; // 0
} MapOffset;

typedef struct {
	byte UnkT00;
	byte val2; // 0
	byte val3; // 0
	byte val4; // 0
	V3f position;
} Warp;

#define CheapModePatternSize 0xE
typedef struct {
	int16_t UnkT00;
	char pattern[CheapModePatternSize]; // 0x00
} CheapMode;

DA(Light, Lights);
DA(Shadow, Shadows);
DA(Fog, Fogs);
DA(BGColor, BGColors);
DA(MapOffset, MapOffsets);
DA(Warp, Warps);
DA(CheapMode, CheapModes);

typedef struct {
	Lights Lights;
	Shadows Shadows;
	Fogs Fogs;
	BGColors BGColors;
	MapOffsets MapOffsets;
	Warps Warps;
	CheapModes CheapModes;
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
typedef struct { 
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
	struct {
		int32_t UnkT00;
		Color ColorT04;
		Color ColorT08;
		float UnkT0C;
		char pattern[RegionLightPatternSize]; // 0x00
		int32_t val4; // 0
	} light;
	struct {
		int32_t UnkT00;
		int32_t sound_id;
		int32_t UnkT08;
		char pattern[RegionSoundPatternSize]; // 0x00
	} sound;
	struct {
		int32_t effect_id;
		int32_t UnkT04;
		char pattern[RegionSFXPatternSize]; // 0x00
	} sfx;
	struct {
		int32_t UnkT00;
		float UnkT04;
		float UnkT08;
		float UnkT0C;
		float UnkT10;
		float UnkT14;
		float UnkT18;
		int32_t val9; // 0
	} wind;
	struct {
		int32_t UnkT00;
		float   UnkT04;
		float   UnkT08;
		char pattern[RegionEnvLightPatternSize]; // 0x00
	} env_light;
	struct {
		int32_t UnkT00;
		int32_t UnkT04;
		char pattern[RegionFogPatternSize]; // 0x00
		int32_t val4; // 0
	} fog;
} RegionTypeData;

typedef struct {
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
	RegionTypeData type_data;
} Region;

DA(Region, Regions);

typedef struct {
	Regions Regions;
	Regions Lights;
	Regions StartPoints;
	Regions Sounds;
	Regions SFXs;
	Regions Winds;
	Regions EnvLights;
	Regions Fogs;
} PointParam;

#define PartCollisionPatternSize 0x10
#define PartNavmeshPatternSize 0x10
typedef struct {
	varint name_offset;
	byte type;
	byte val3; //0
	int16_t id;
	int16_t model_index;
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
	struct {
		int16_t UnkT00;
		byte UnkT02;
		byte val3; // 0
		int32_t val4; //0
	} map_piece;
	struct {
		int32_t map_object_instance_param_id;
		int16_t UnkT04;
		int16_t val3; // 0
		int32_t val4; // 0
		int32_t val5; // 0
	} object;
	struct {
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
	struct {
		int32_t UnkT00;
		int32_t UnkT04;
		int32_t UnkT08;
		int32_t UnkT0C;
		char pattern[PartNavmeshPatternSize]; // 0x00
	} navmesh;
	struct {
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
	PartTypeData type_data;
} Part;

DA(Part, Parts);

typedef struct {
	Parts MapPieces;
	Parts Objects;
	Parts Collisions;
	Parts Navmeshes;
	Parts ConnectCollisions;
} PartParam;

typedef struct {} BoneNames;
typedef struct {} PartPoses;

typedef struct {
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
    nob_log(INFO, "MSB Header verified");
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

typedef struct {
	varint name_offset;
	int8_t type;
	int8_t val3;
	int16_t id;
	int32_t val5;
	int64_t type_data_offset;
	varint val7;
} EntryFmt;

void read_entry(String_View* sv, Entry *entry) {
	const char *start = sv->data;
	EntryFmt ef = *(EntryFmt *)sv->data;
	assert_val(ef.type, (int8_t)entry->type);
	assert_val(ef.val3, 0);
	assert_val(ef.val5, 0);
	assert_val(ef.val7, 0);
	sv->data += ef.name_offset;
	entry->name = (utf16_t *)sv->data;
	if (entry->has_type_data) {
		sv->data = start + ef.type_data_offset;
		assert_item(varint, sv, 0);
	}
}

typedef struct {
	ModelParam models;
	EventParam events;
	PointParam regions;
	PartParam parts;
	PartPoses partPoses;
	BoneNames boneNames;
} Entries;

typedef struct {
	int32_t version; // 5
	int32_t offset_count;
	int64_t name_offset;
	varint *entry_offsets; // n = offset_count - 1
} ModalParamSTBegin;

typedef struct {

} ModalParamST;

ParamSt read_param(String_View *sv) {
	ParamSt param = {0};
	param.version = read_item(int32_t, sv);
	assert(param.version == VERSION);

    param.offset_count = read_item(int32_t, sv);
    param.name_offset  = read_item(int64_t, sv);

    param.entry_offsets = read_item_ptr(varint, sv, param.offset_count-1);
    param.next_param_offset = read_item(varint, sv);
	return param;
}

utf16_t* get_param_name(const ParamSt *param) {
    return (utf16_t *)(map_begin + param->name_offset);
}

void* get_param_entry(const ParamSt *param, size_t i) {
	return (void*)(map_begin + param->entry_offsets[i]);
}

ModelParam read_model_param(String_View *sv) {
    ModelParam model = {0};
	ParamSt param = read_param(sv);

    utf16_t *name = get_param_name(&param);
	utf16_t *required = L"MODEL_PARAM_ST";
	assert(wcscmp(name, required) == 0);

	for (int i=0; i<param.offset_count-1; i++) {
		Entry entry = {0};
		sv->data = get_param_entry(&param, i);
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
	nob_log(INFO, "Read %zu map pieces.", model.MapPieces.count);
	nob_log(INFO, "Read %zu objects.", model.Objects.count);
	nob_log(INFO, "Read %zu collisions.", model.Collisions.count);
	nob_log(INFO, "Read %zu navmeshes.", model.Navmeshes.count);
	sv->data = map_begin + param.next_param_offset;

	return model;
}

typedef struct {
	varint name_offset;
	int32_t event_id;
	byte type;
	byte val4; // 0
	int16_t id;
	varint type_data_offset;
} Event;

Event read_event(String_View *sv) {
	Event event = *(Event *)sv->data;
	assert(event.val4 == (byte)0);
	sv->data += sizeof(Event);
	return event;
}

Light read_light(String_View *sv) {
	const char* start = sv->data;
	Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	UNUSED(name);
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
	return light;
}

Shadow read_shadow(String_View *sv) {
	const char* start = sv->data;
	Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	UNUSED(name);
	sv->data = start + event.type_data_offset;
	Shadow shadow = *(Shadow *)sv->data;
	assert(shadow.val1 == 0);
	assert(shadow.val5 == 0);
	assert(shadow.val8 == 0);
	for (size_t i=0; i<ShadowPatternSize; i+=1) {
		assert(shadow.pattern[i] == 0x00);
	}
	sv->data += sizeof(Shadow);
	return shadow;
}

Fog read_fog(String_View *sv) {
	const char* start = sv->data;
	Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	UNUSED(name);
	sv->data = start + event.type_data_offset;
	Fog fog = *(Fog *)sv->data;
	assert(fog.val2 == 0);
	assert(fog.val3 == 0);
	assert(fog.val4 == 0);
	for (size_t i=0; i<FogPatternSize; i+=1) {
		assert(fog.pattern[i] == 0x00);
	}
	sv->data += sizeof(Fog);
	return fog;
}

BGColor read_bgcolor(String_View *sv) {
	const char* start = sv->data;
	Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	UNUSED(name);
	sv->data = start + event.type_data_offset;
	BGColor bgcolor = *(BGColor *)sv->data;
	for (size_t i=0; i<BGColorPatternSize; i+=1) {
		assert(bgcolor.pattern[i] == 0x00);
	}
	sv->data += sizeof(BGColor);
	return bgcolor;
}

MapOffset read_map_offset(String_View *sv) {
	const char* start = sv->data;
	Event event = read_event(sv);
	assert(event.type_data_offset != 0);
	assert(event.name_offset != 0);
	sv->data = start + event.name_offset;
	utf16_t *name = (utf16_t *)sv->data;
	UNUSED(name);
	sv->data = start + event.type_data_offset;
	MapOffset map_offset = *(MapOffset *)sv->data;
	assert(map_offset.degree == 0);
	sv->data += sizeof(MapOffset);
	return map_offset;
}

EventParam read_event_param(String_View *sv) {
	EventParam events = {0};
	ParamSt param = read_param(sv);
    utf16_t *name = get_param_name(&param);
	utf16_t *required = L"EVENT_PARAM_ST";
	assert(wcscmp(name, required) == 0);

	for (int i=0; i<param.offset_count-1; i++) {
		sv->data = get_param_entry(&param, i);
		EventType model_type = *(int8_t *)(sv->data + sizeof(varint) + 4);
		switch (model_type) {
	    case ET_Light: {
			Light light = read_light(sv);
			da_append(&events.Lights, light);
		} break;
	    case ET_Shadow: {
			Shadow shadow = read_shadow(sv);
			da_append(&events.Shadows, shadow);
		} break;
	    case ET_Fog: {
			Fog fog = read_fog(sv);
			da_append(&events.Fogs, fog);
		} break;
	    case ET_BGColor: {
			BGColor bgcolor = read_bgcolor(sv);
			da_append(&events.BGColors, bgcolor);
		} break;
	    case ET_MapOffset: {
			MapOffset map_offset = read_map_offset(sv);
			da_append(&events.MapOffsets, map_offset);
		} break;
	    case ET_Warp: {
			TODO("ET_Warp");						  
		} break;
	    case ET_CheapMode: {
			TODO("ET_CheapMode");							   
		} break;
		default: UNREACHABLE("model_type");
		}
	}
	nob_log(INFO, "Read %zu Lights", events.Lights.count);
	nob_log(INFO, "Read %zu Shadows", events.Shadows.count);
	nob_log(INFO, "Read %zu Fogs", events.Fogs.count);
	nob_log(INFO, "Read %zu BGColors", events.BGColors.count);
	nob_log(INFO, "Read %zu MapOffsets", events.MapOffsets.count);
	nob_log(INFO, "Read %zu Warps", events.Warps.count);
	nob_log(INFO, "Read %zu CheapModes", events.CheapModes.count);

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
	region.region = read_region_(sv);

	// TODO: this assertion is incorrect
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
    utf16_t *name = get_param_name(&param);
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
	nob_log(INFO, "Read %zu Region", regions.Regions.count);
	nob_log(INFO, "Read %zu Light", regions.Lights.count);
	nob_log(INFO, "Read %zu StartPoint", regions.StartPoints.count);
	nob_log(INFO, "Read %zu Sound", regions.Sounds.count);
	nob_log(INFO, "Read %zu SFX", regions.SFXs.count);
	nob_log(INFO, "Read %zu Wind", regions.Winds.count);
	nob_log(INFO, "Read %zu EnvLight", regions.EnvLights.count);
	nob_log(INFO, "Read %zu Fog", regions.Fogs.count);
	sv->data = map_begin + param.next_param_offset;
	return regions;
}

void read_route_param(String_View *sv) {
	ParamSt param = read_param(sv);
    utf16_t *name = get_param_name(&param);
	utf16_t *required = L"ROUTE_PARAM_ST";
	assert(wcscmp(name, required) == 0);
	for (int i=0; i<param.offset_count-1; i++) {
		UNREACHABLE("route params are empty in ds2");
	}
	sv->data = map_begin + param.next_param_offset;
}

void read_layer_param(String_View *sv) {
	ParamSt param = read_param(sv);
    utf16_t *name = get_param_name(&param);
	utf16_t *required = L"LAYER_PARAM_ST";
	assert(wcscmp(name, required) == 0);
	for (int i=0; i<param.offset_count-1; i++) {
		UNREACHABLE("layer params are empty in ds2");
	}
	sv->data = map_begin + param.next_param_offset;
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
    utf16_t *name = get_param_name(&param);
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

	nob_log(INFO, "Read %zu MapPiece", parts.MapPieces.count);
	nob_log(INFO, "Read %zu Object", parts.Objects.count);
	nob_log(INFO, "Read %zu Collision", parts.Collisions.count);
	nob_log(INFO, "Read %zu Navmesh", parts.Navmeshes.count);
	nob_log(INFO, "Read %zu ConnectCollision", parts.ConnectCollisions.count);
	sv->data = map_begin + param.next_param_offset;
	return parts;
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
	UNUSED(param);
	// TODO("free part poses");
}

void free_bone_names(BoneNames *param) {
	UNUSED(param);
	// TODO("free bone names");
}

void free_entries(Entries *entries) {
	free_models(&entries->models);
	free_events(&entries->events);
	free_regions(&entries->regions);
	free_parts(&entries->parts);
	free_part_poses(&entries->partPoses);
	free_bone_names(&entries->boneNames);
}


int main() {
    Nob_String_Builder map_sb = {0};
    const char *map_path = "./map/m10_02_00_00/m10_02_00_00.msb";
    if (!nob_read_entire_file(map_path, &map_sb)) return 1;
    printf("Read %zu bytes from %s\n", map_sb.count, map_path);
    String_View map_sv = sb_to_sv(map_sb);

	Header h = *(Header *)map_sv.data;
    map_begin = map_sv.data;
    if (!assert_header(map_path, &h)) return 1;
	map_sv.data += sizeof(Header);

	Entries entries = {0};
	entries.models = read_model_param(&map_sv);
	entries.events = read_event_param(&map_sv);
	entries.regions = read_point_param(&map_sv);
	read_route_param(&map_sv);
	read_layer_param(&map_sv);
	entries.parts = read_parts_param(&map_sv);

	free_entries(&entries);

	printf("Done.");
    return 0;
}
