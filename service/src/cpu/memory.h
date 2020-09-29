#ifndef _MEMORY_H__
#define _MEMORY_H__

#include <stdint.h>
#include <stdlib.h>
//#include "../gb/gb.h"
//#include "registers.h"
//#include "cpu.h"
//#include "../input/input.h"
//#include "../gpu/gpu.h"

typedef struct gameboy gameboy_t;

#define SIZE_IO 0x100
#define SIZE_CART 0x8000
#define SIZE_SRAM 0x2000
#define SIZE_VRAM 0x2000
#define SIZE_OAM 0x100
#define SIZE_WRAM 0x2000
#define SIZE_HRAM 0x80


typedef struct memory {
	uint8_t cart[SIZE_CART];
	uint8_t vram[SIZE_VRAM];
	uint8_t sram[SIZE_SRAM];
	uint8_t wram[SIZE_WRAM];
	uint8_t oam[SIZE_OAM];
	uint8_t io[SIZE_IO];
	uint8_t hram[SIZE_HRAM];
} memory_t;

#define CART_MAX (SIZE_CART - 1)
#define VRAM_MAX (CART_MAX + SIZE_VRAM - 1)
#define SRAM_MAX (VRAM_MAX + SIZE_SRAM - 1)
#define WRAM_MAX (SRAM_MAX + SIZE_SRAM - 1)

void memory_copy(gameboy_t *gb, uint16_t dst, uint16_t src, size_t len);

uint8_t read_byte(gameboy_t *gb, uint16_t address);
uint16_t read_short(gameboy_t *gb, uint16_t addres);
uint16_t read_short_from_stack(gameboy_t *gb);

void write_byte(gameboy_t *gb, uint16_t address, uint8_t value);
void write_short(gameboy_t *gb, uint16_t address, uint16_t value);
void write_short_to_stack(gameboy_t *gb, uint16_t value);

#endif // _MEMORY_H__
