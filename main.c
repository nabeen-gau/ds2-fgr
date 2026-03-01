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

typedef struct {
	ModelType type;
	bool has_type_data;
	utf16_t *name;
} Entry;
DA(Entry, Model);

typedef struct {
    Model map_pieces;
    Model objects;
    Model collisions;
    Model navmeshes;
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
} Region;

typedef Region Region0;
typedef Region StartPoint;

#define SoundPatternSize 0x14
typedef struct {
	Region region;
	int32_t UnkT00;
	int32_t sound_id;
	int32_t UnkT08;
	char pattern[SoundPatternSize]; // 0x00
} Sound;

#define SFXPatternSize 0x18
typedef struct {
	Region region;
	int32_t effect_id;
	int32_t UnkT04;
	char pattern[SFXPatternSize]; // 0x00
} SFX;

typedef struct {
	Region region;
	int32_t UnkT00;
	float UnkT04;
	float UnkT08;
	float UnkT0C;
	float UnkT10;
	float UnkT14;
	float UnkT18;
	int32_t val9; // 0
} Wind;

#define EnvLightPatternSize 0x14
typedef struct {
	Region region;
	int32_t UnkT00;
	float   UnkT04;
	float   UnkT08;
	char pattern[EnvLightPatternSize]; // 0x00
} EnvLight;

DA(Region0, Region0s);
DA(StartPoint, StartPoints);
DA(Sound, Sounds);
DA(SFX, SFXs);
DA(Wind, Winds);
DA(EnvLight, EnvLights);

typedef struct {
	Region0s Region0s;
	Lights Lights;
	StartPoints StartPoints;
	Sounds Sounds;
	SFXs SFXs;
	Winds Winds;
	EnvLights EnvLights;
	Fogs Fogs;
} PointParam;

typedef struct {} Parts;
typedef struct {} BoneNames;

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
	Parts parts;
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

ModelParam read_model(String_View *sv) {
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
			da_append(&model.map_pieces, entry);
	    } break;
		case MT_Object: {
			entry.has_type_data = true;
			read_entry(sv, &entry);
			da_append(&model.objects, entry);
		} break;
		case MT_Collision: {
			entry.has_type_data = false;
			read_entry(sv, &entry);
			da_append(&model.collisions, entry);
		} break;
		case MT_Navmesh: {
			entry.has_type_data = false;
			read_entry(sv, &entry);
			da_append(&model.navmeshes, entry);
		} break;
		default: UNREACHABLE("Model_Type");
		}
	}
	nob_log(INFO, "Read %zu map pieces.", model.map_pieces.count);
	nob_log(INFO, "Read %zu objects.", model.objects.count);
	nob_log(INFO, "Read %zu collisions.", model.collisions.count);
	nob_log(INFO, "Read %zu navmeshes.", model.navmeshes.count);
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

EventParam read_events(String_View *sv) {
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

typedef struct {
	int32_t region_index;
	int32_t Unk04;
} Child;

Child new_child(String_View *sv) {
	Child child = *(Child *)sv->data;
	sv->data += sizeof(Child);
	return child;
}

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

void read_type_data(RegionType type) {
	UNUSED(type);
	TODO("read_type_data");
}

Region read_region(String_View *sv, bool has_type_data) {
	const char *start = sv->data;
	Region region = *(Region *)sv->data;
	assert(region.val11 == -1);
	for (size_t i=0; i<RegionPatternSize; i+=1) {
		assert(region.pattern[i] == 0x00);
	}
	assert(region.val15 == 0);
	assert(region.val16 == 0);
	sv->data += sizeof(Region);

	Shape shape = create_shape(region.shape_type);

	assert(region.name_offset != 0);
	assert(*(int32_t *)(start + region.unkOffsetA) == 0);
	assert(*(int32_t *)(start + region.unkOffsetB) == 0);

	// TODO: this assertion is incorrect
	// assert((shape.has_shape_data^region.shape_data_offset) == 0);
	// assert((has_type_data^region.type_data_offset) == 0);

	utf16_t *name = (utf16_t *)(start + region.name_offset);
	UNUSED(name);
	// TODO: store name, shape, type_data somewhere


	if (shape.has_shape_data) {
		sv->data = start + region.shape_data_offset;
		read_shape_data(&shape, sv);
	}

	if (has_type_data) {
		sv->data = start + region.type_data_offset;
		read_type_data(region.type);
	}
	return region;
}

Region0 read_region0(String_View *sv, bool has_type_data) {
	Region0 region0 = read_region(sv, has_type_data);
	return region0;
}

Sound read_sound(String_View *sv, bool has_type_data) {
	Sound sound = {0};
	sound.region =  read_region(sv, has_type_data);
	// TODO: i am reading type data in the read_region function maybe do it here?
	return sound;
}

SFX read_sfx(String_View *sv, bool has_type_data) {
	SFX sfx = {0};
	sfx.region =  read_region(sv, has_type_data);
	// TODO: i am reading type data in the read_region function maybe do it here?
	return sfx;
}

EnvLight read_env_light(String_View *sv, bool has_type_data) {
	EnvLight env_light = {0};
	env_light.region =  read_region(sv, has_type_data);
	// TODO: i am reading type data in the read_region function maybe do it here?
	return env_light;
}



StartPoint read_start_point(String_View *sv) {
	StartPoint start_point = *(StartPoint *)sv->data;
	assert(start_point.val11 == -1);
	for (size_t i=0; i<RegionPatternSize; i+=1) {
		assert(start_point.pattern[i] == 0x00);
	}
	assert(start_point.val15 == 0);
	assert(start_point.val16 == 0);
	sv->data += sizeof(StartPoint);
	return start_point;
}



PointParam read_regions(String_View *sv) {
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
			Region0 region0 = read_region0(sv, false);
			da_append(&regions.Region0s, region0);
		} break; 
		case RT_Light: {
			TODO("Light");						
		} break; 
		case RT_StartPoint: {
			StartPoint start_point = read_start_point(sv);
			da_append(&regions.StartPoints, start_point);
		} break; 
		case RT_Sound: {
			Sound sound = read_sound(sv, false);
			da_append(&regions.Sounds, sound);
		} break; 
		case RT_SFX: {
			SFX sfx = read_sfx(sv, false);
			da_append(&regions.SFXs, sfx);
		} break; 
		case RT_Wind: {
			TODO("Wind");					   
		} break; 
		case RT_EnvLight: {
			EnvLight env_light = read_env_light(sv, false);
			da_append(&regions.EnvLights, env_light);
		} break; 
		case RT_Fog: {
			TODO("Fog");					  
		} break; 
		default: UNREACHABLE("region_type");
		}
	}
	nob_log(INFO, "Read %zu Region0", regions.Region0s.count);
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
	entries.models = read_model(&map_sv);
	entries.events = read_events(&map_sv);
	entries.regions = read_regions(&map_sv);

	TODO("next");
    return 0;
}
