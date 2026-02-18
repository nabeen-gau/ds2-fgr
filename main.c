#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

//
// [4][4][4][1][1][1][1]
//       Header
//

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

typedef struct {} Event;

typedef struct {
	Event Lights;
	Event Shadows;
	Event Fogs;
	Event BGColors;
	Event MapOffsets;
	Event Warps;
	Event CheapModes;
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
	Events events;
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
		Model_Type model_type = *(int8_t *)(sv->data + sizeof(varint));
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
	UNUSED(sv);
	Events events = {};
	TODO("next");
	return events;
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

	TODO("next");
    return 0;
}
