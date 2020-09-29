#ifndef _REGISTERS_H__
#define _REGISTERS_H__
#define __LITTLE_ENDIAN__ 1

#include <stdint.h>

typedef struct registers {
	/* 8 bit regs */
	union {
		struct {
#ifdef __LITTLE_ENDIAN__
			uint8_t f, a;
#else
			uint8_t a, f;
#endif
		};
		uint16_t AF;
	};
	union {
		struct {
#ifdef __LITTLE_ENDIAN__
			uint8_t c, b;
#else
			uint8_t b, c;
#endif
		};
		uint16_t BC;
	};
	union {
		struct {
#ifdef __LITTLE_ENDIAN__
			uint8_t e, d;
#else
			uint8_t d, e;
#endif
		};
		uint16_t DE;
	};
	union {
		struct {
#ifdef __LITTLE_ENDIAN__
			uint8_t l, h;
#else
			uint8_t h, l;
#endif
		};
		uint16_t HL;
	};
	/* 16 bit regs */
	uint16_t PC, SP;
} registers_t;

#endif // _REGISTERS_H__
