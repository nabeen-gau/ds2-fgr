#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

const char* FORMAT = "MSB";
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

#define VERSION 5

typedef enum {
	MT_MapPiece = 0,
	MT_Object = 1,
	MT_Collision = 3,
	MT_Navmesh = 4,
} Model_Type;

typedef struct {
	Model_Type type;
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

typedef struct {} Events;
typedef struct {} Regions;
typedef struct {} Parts;
typedef struct {} BoneNames;

void assert_header(String_View* sv) {
    assert_item(int32_t, sv, 1);
    assert_item(int32_t, sv, 0x10);
    assert_item(bool, sv, false);
    assert_item(bool, sv, false);
    assert_item(char, sv, 1);
    assert_item(char, sv, (char)0xFF);
    nob_log(INFO, "MSB Header verified");
}

void print_model_type(Model_Type type) {
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
	int64_t name_offset = read_item(varint, sv);
	assert_item(int8_t, sv, (int8_t)entry->type);
	assert_item(int8_t, sv, 0);
	read_item(int16_t, sv); // id
	assert_item(int32_t, sv, 0);
	int64_t type_data_offset = read_item(varint, sv);
	assert_item(int32_t, sv, 0);
	assert(name_offset != 0 && "invalid");
	sv->data = start + name_offset;
	entry->name = (utf16_t *)sv->data;
	if (entry->has_type_data) {
		sv->data = start + type_data_offset; 
		assert_item(varint, sv, 0);
	}
}

typedef struct {
	ModelParam models;
	Events events;
	Regions regions;
	Parts parts;
	BoneNames boneNames;
} Entries;

ModelParam read_model(String_View *sv) {
    ModelParam model = {0};
    assert_item(int32_t, sv, VERSION);

    int32_t offset_count = read_item(int32_t, sv);
    int64_t name_offset  = read_item(int64_t, sv);

    varint *entry_offsets = read_item_ptr(varint, sv, offset_count-1);
    varint  next_param_offset = read_item(varint, sv);

    utf16_t *name = (utf16_t *)(map_begin + name_offset);
	utf16_t *required = L"MODEL_PARAM_ST";
	assert(wcscmp(name, required) == 0);

	for (int i=0; i<offset_count-1; i++) {
		Entry entry = {0};
		sv->data = (void*)(map_begin + entry_offsets[i]);
		Model_Type model_type = *(int8_t *)(entry_offsets[i] + map_begin + sizeof(varint));
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
	sv->data = map_begin + next_param_offset;

	return model;
}

Events read_events(String_View *sv) {
	Events events = {};
	TODO("next");
	return events;
}

int main() {
    char format[5];

    Nob_String_Builder map_sb = {0};
    const char *map_path = "./map/m10_02_00_00/m10_02_00_00.msb";
    if (!nob_read_entire_file(map_path, &map_sb)) return 1;
    printf("Read %zu bytes from %s\n", map_sb.count, map_path);
    String_View map_sv = sb_to_sv(map_sb);
    map_begin = map_sv.data;
    String_View fmt_sv = sv_chop_left(&map_sv, 4);
    snprintf(format, 4, "%s", fmt_sv.data);
    if (strcmp(format, FORMAT) != 0) {
        nob_log(ERROR, "Invalid file format for %s: Expected `%s` but got `%s`\n", map_path, FORMAT, format);
        return 1;
    }
    assert_header(&map_sv);

	Entries entries = {0};
	entries.models = read_model(&map_sv);
	entries.events = read_events(&map_sv);

	TODO("next");
    return 0;
}
