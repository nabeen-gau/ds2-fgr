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
	char pattern[0x18]; // 0x00
} Shadow;

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
	char pattern[0x11]; // 0x00
} Fog;

typedef struct {
	Color color;
	char pattern[0x24]; // 0x00
} BGColor;

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

typedef struct {
	int16_t UnkT00;
	char pattern[0xE]; // 0x00
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

typedef struct {} Events;
typedef struct {} Regions;
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
	int32_t val7;
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
	Regions regions;
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
		} break;
	    case ET_Fog: {
		} break;
	    case ET_BGColor: {
		} break;
	    case ET_MapOffset: {
		} break;
	    case ET_Warp: {
		} break;
	    case ET_CheapMode: {
		} break;
		default: UNREACHABLE("model_type");
		}
	}

	TODO("next");
	return events;
}

typedef struct {
	Header header;
	ParamSt param_st;
	utf16_t* name;
} MSB2;

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

	TODO("next");
    return 0;
}
