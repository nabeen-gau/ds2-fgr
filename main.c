#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

const char* FORMAT = "MSB";
const char* map_begin = NULL;

#define Event void
#define Region void
#define Part void
#define BoneName void

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

typedef struct {
    int16_t id;
    utf16_t* name;
} MapPiece;

DA(MapPiece, MapPieces);
DA(void, Objects);
DA(void, Collisions);
DA(void, Navmeshes);

typedef struct {
    MapPieces map_pieces;
    Objects objects;
    Collisions collisions;
    Navmeshes navmeshes;
} Model;

DA(Model, Models);
DA(Event, Events);
DA(Region, Regions);
DA(Part, Parts);
DA(BoneName, BoneNames);

typedef struct {
    Models models;
    Events events;
    Regions regions;
    Parts parts;
    BoneNames boneNames;
} Entry;

DA(Entry, Entries);

typedef enum {
    MT_MapPiece,
    MT_Object,
    MT_Enemy,
    MT_Item,
    MT_Player,
    MT_Collision,
    MT_Navmesh,
    MT_Other,
} ModelType;


void print_model_type(ModelType type) {
    switch (type) {
    case MT_MapPiece: printf("MT_MapPiece\n"); break;
    case MT_Object: printf("MT_Object\n"); break;
    case MT_Enemy: printf("MT_Enemy\n"); break;
    case MT_Item: printf("MT_Item\n"); break;
    case MT_Player: printf("MT_Player\n"); break;
    case MT_Collision: printf("MT_Collision\n"); break;
    case MT_Navmesh: printf("MT_Navmesh\n"); break;
    case MT_Other: printf("MT_Other\n"); break;
    default: UNREACHABLE("ModelType");
    }
}

bool model_read(String_View sv, MapPiece *map_piece, ModelType type) {
    const char *start = sv.data;

    varint name_offset = read_item(varint, &sv);
    printf("name_offset = %lld\n", name_offset);

    assert_item(char, &sv, (char)type);
    assert_item(char, &sv, 0);

    map_piece->id = read_item(int16_t, &sv);
    printf("id = %d\n", map_piece->id);

    assert_item(int32_t, &sv, 0);

    varint type_data_offset = read_item(varint, &sv);
    printf("type_data_offset = %lld\n", type_data_offset);

    assert(name_offset != 0);

    sv.data = start + name_offset;

    map_piece->name = read_utf_16(&sv);
    wprintf(L"name = %ls\n", map_piece->name);
    exit(69);

    return true;
}

bool entry_read(Entry* entry, String_View sv) {
    entry->models = (Models){0};
    Model model = {0};
    model.map_pieces = (MapPieces){0};

    ModelType type = (ModelType)*(int32_t*)(sv.data + 8);
    printf("[INFO] Model Type: ");
    print_model_type(type);
    switch (type) {
    case MT_MapPiece: {
        MapPiece mp = {0};
        if (!model_read(sv, &mp, type)) return false;
        da_append(&model.map_pieces, mp);
    }
    break;
    case MT_Object: TODO("MT_Object"); break;
    case MT_Enemy: TODO("MT_Enemy"); break;
    case MT_Item: TODO("MT_Item"); break;
    case MT_Player: TODO("MT_Player"); break;
    case MT_Collision: TODO("MT_Collision"); break;
    case MT_Navmesh: TODO("MT_Navmesh"); break;
    case MT_Other: TODO("MT_Other"); break;
    default: UNREACHABLE("ModelType");
    }
    exit(69);
    return true;
}

bool map_pieces_read(MapPieces* map_pieces, String_View sv) {
    UNUSED(map_pieces);
    MapPiece map_piece = {0};
    if (!model_read(sv, &map_piece, MT_MapPiece)) return false;
    return true;
}

bool models_read(Models* models, String_View sv) {
    assert_item(int32_t, &sv, VERSION);

    int32_t offset_count = read_item(int32_t, &sv);
    int64_t name_offset  = read_item(int64_t, &sv);
    nob_log(INFO, "offset_count: %d",   offset_count);
    nob_log(INFO, "name_offset:  %lld", name_offset);

    varint *entry_offsets = read_item_ptr(varint, &sv, offset_count);
    varint  next_param_offset = read_item(varint, &sv);
    nob_log(INFO, "entry_offsets: %p",   entry_offsets);
    nob_log(INFO, "next_param_offset:  %lld", next_param_offset);

    wchar_t *name = (wchar_t *)(sv.data + name_offset);
    wprintf(L"name = %ls\n", name);

    Entries entries = {0};
    da_reserve(&entries, (size_t)offset_count);
    for (int32_t i=0; i<offset_count; i++) {
        varint offset = entry_offsets[i];
        sv.data = map_begin + offset;
        sv.count -= offset;
        Entry entry = {0};
        if (!entry_read(&entry, sv)) return false;
        da_append(&entries, entry);
    }
    exit(69);


    UNUSED(models);
    MapPieces map_pieces = {0};
    map_pieces_read(&map_pieces, sv);
    return true;
}

bool entries_read(Entries *entries, String_View sv) {
    Entry entry = {0};
    entry.models = (Models){0};
    if (!models_read(&entry.models, sv)) return false;
    da_append(entries, entry);
    return true;
}

void assert_header(String_View* sv) {
    assert_item(int32_t, sv, 1);
    assert_item(int32_t, sv, 0x10);
    assert_item(bool, sv, false);
    assert_item(bool, sv, false);
    assert_item(char, sv, 1);
    assert_item(char, sv, (char)0xFF);
    nob_log(INFO, "MSB Header verified");
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
    else {
        nob_log(INFO, "Format: `"SV_Fmt"`", SV_Arg(fmt_sv));
    }
    assert_header(&map_sv);
    Entries entries = {0};
    if (!entries_read(&entries, map_sv)) return 1;
    return 0;
}
