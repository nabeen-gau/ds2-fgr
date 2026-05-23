#ifndef __DS2FGR_COMMON_H
#define __DS2FGR_COMMON_H

#ifdef _MSC_VER
#define packed_struct struct
#define asm(val) (void)val
#else
#define packed_struct struct __attribute__((packed))
#endif


typedef packed_struct { 
	float x,y; 
} V2f;

typedef packed_struct { 
	float x,y,z; 
} V3f;
#endif

#define da_init(da, size) \
	do {\
		if ((size) > 0) {\
			assert((da)->capacity == 0 && "0 init the da first");\
			(da)->capacity = (size);\
			(da)->items = malloc(sizeof(*(da)->items)*(da)->capacity);\
		}\
	} while (0)

