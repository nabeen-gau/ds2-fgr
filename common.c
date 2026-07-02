#ifndef __DS2FGR_COMMON_H
#define __DS2FGR_COMMON_H

#ifdef _MSC_VER
#define packed_struct struct
#define asm(val) (void)val
#else
#define packed_struct struct __attribute__((packed))
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// #define LOG_GENERATED_FILES

#ifndef LOG_GENERATED_FILES
#define NOB_NO_ECHO
#endif

#define NOB_DA_INIT_CAP 32
#define NOB_IMPLEMENTATION
#include "nob.h"

#define HT_IMPLEMENTATION
#include "ht.h"

typedef packed_struct { 
	float x,y; 
} V2f;

typedef packed_struct { 
	float x,y,z; 
} V3f;

#define da_init(da, size) \
	do {\
		if ((size) > 0) {\
			assert((da)->capacity == 0 && "0 init the da first");\
			(da)->capacity = (size);\
			(da)->items = malloc(sizeof(*(da)->items)*(da)->capacity);\
		}\
	} while (0)

#define DA(type, name) typedef struct {type *items; size_t count; size_t capacity;} name
#define DA2(type, name) typedef struct {struct type *items; size_t count; size_t capacity;} name

#define varint int64_t
#define utf16_t wchar_t

typedef Ht(varint, size_t) Dictlldz;
typedef Ht(size_t, varint) Dictzulld;

#define sv_read(sv, type) *(type *)sv_chop_left((sv), sizeof(type)).data;

#define da_alloc(da, n)\
	do {\
		(da).items = realloc((da).items, sizeof(*(da).items)*(n));\
		(da).capacity = (n);\
		(da).count = 0;\
	} while (0)


DA(size_t, DArrz);

typedef Ht(const char *, size_t) Dictstrzu;
typedef Ht(size_t, size_t) Dictzuzu;

typedef struct {
	size_t from;
	size_t to;
} Pair;
DA(Pair, DAPair);
typedef Ht(Pair, size_t) DictPairzu;

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

void sb_reserve(String_Builder *sb, size_t nbytes) {
	for (size_t i=0; i<nbytes; i++) {
		nob_sb_append(sb, 0);
	}
}

int64_t pack(int32_t a, int32_t b) {
    return ((int64_t)a << 32) | ((uint32_t)b);
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

typedef struct {
	uint8_t *data;
	size_t position;
	size_t length;
} BinaryReader;

BinaryReader sb_to_br(String_Builder sb) {
	return (BinaryReader) {
		.data = (uint8_t *)sb.items,
		.position = 0,
		.length = sb.count,
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

#endif
