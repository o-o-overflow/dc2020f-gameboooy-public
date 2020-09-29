#include "memory.h"
#include "../gpu/gpu.h"
#include "../gb/gb.h"
#include "../input/input.h"
#include "cpu.h"
#include <stdint.h>
#include <stdlib.h>

void memory_copy(gameboy_t *gb, uint16_t dst, uint16_t src, size_t len) {
  size_t i;
  for (i = 0; i < len; i++) {
    write_byte(gb, dst + i, read_byte(gb, src + i));
  }
}

unsigned char read_byte(gameboy_t *gb, unsigned short address) {
    // Set read breakpoints here
    //if(address == 0x0300) {
    //  realtimeDebugEnable = 1;
    //}

    if(address <= 0x7fff)
        return gb->memory.cart[address];

    else if(address >= 0xa000 && address <= 0xbfff)
        return gb->memory.sram[address - 0xa000];

    else if(address >= 0x8000 && address <= 0x9fff)
        return gb->memory.vram[address - 0x8000];

    else if(address >= 0xc000 && address <= 0xdfff)
        return gb->memory.wram[address - 0xc000];

    else if(address >= 0xe000 && address <= 0xfdff)
        return gb->memory.wram[address - 0xe000];

    else if(address >= 0xfe00 && address <= 0xfeff)
        return gb->memory.oam[address - 0xfe00];

    // Should return a div timer, but a random number works just as well for Tetris
    else if(address == 0xff04) return (unsigned char)rand();

    else if(address == 0xff40) return gb->gpu.control;
    else if(address == 0xff42) return gb->gpu.scrollY;
    else if(address == 0xff43) return gb->gpu.scrollX;
    else if(address == 0xff44) return gb->gpu.scanline; // read only

    else if(address == 0xff00) {
        if(!(gb->memory.io[0x00] & 0x20)) {
            return (unsigned char)(0xc0 |gb->keys.keys1| 0x10);
        }

        else if(!(gb->memory.io[0x00] & 0x10)) {
            return (unsigned char)(0xc0 | gb->keys.keys2 | 0x20);
        }

        else if(!(gb->memory.io[0x00] & 0x30)) return 0xff;
        else return 0;
    }

    else if(address == 0xff0f) return gb->cpu.interrupt.flags;
    else if(address == 0xffff) return gb->cpu.interrupt.enable;

    else if(address >= 0xff80 && address <= 0xfffe)
        return gb->memory.hram[address - 0xff80];

    else if(address >= 0xff00 && address <= 0xff7f)
        return gb->memory.io[address - 0xff00];

    return 0;
}
uint16_t read_short(gameboy_t *gb, uint16_t address) {
  return read_byte(gb, address) | (read_byte(gb, address + 1) << 8);
}

uint16_t read_short_from_stack(gameboy_t *gb) {
  uint16_t value = read_short(gb, gb->cpu.registers.SP);
  gb->cpu.registers.SP += 2;

#ifdef DEBUG_STACK
  printf("Stack read 0x%04x\n", value);
#endif

  return value;
}

void updateTile(gameboy_t *gb, uint16_t address, uint8_t value) {
  address &= 0x1ffe;

  unsigned short tile = (address >> 4) & 511;
  unsigned short y = (address >> 1) & 7;

  unsigned char x, bitIndex;
  for (x = 0; x < 8; x++) {
    bitIndex = 1 << (7 - x);

    //((unsigned char (*)[8][8])tiles)[tile][y][x] = ((vram[address] & bitIndex)
    //? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
    gb->gpu.tiles[tile][y][x] = ((gb->memory.vram[address] & bitIndex) ? 1 : 0) +
                        ((gb->memory.vram[address + 1] & bitIndex) ? 2 : 0);
  }

}

void write_byte(gameboy_t *gb, uint16_t address, uint8_t value) {
  // Set write breakpoints here
  // if(address == 0xff00) {
  //  realtimeDebugEnable = 1;
  //}

  if (address >= 0xa000 && address <= 0xbfff)
    gb->memory.sram[address - 0xa000] = value;

  else if (address >= 0x8000 && address <= 0x9fff) {
    gb->memory.vram[address - 0x8000] = value;
    if (address <= 0x97ff)
      updateTile(gb, address, value);
  }

  if (address >= 0xc000 && address <= 0xdfff)
    gb->memory.wram[address - 0xc000] = value;

  else if (address >= 0xe000 && address <= 0xfdff)
    gb->memory.wram[address - 0xe000] = value;

  else if (address >= 0xfe00 && address <= 0xfeff) {
	 //printf("%x %x %x\n", gb->cpu.registers.PC, value, address);
	 //abort();
    gb->memory.oam[address - 0xfe00] = value;
  }

  else if (address >= 0xff80 && address <= 0xfffe)
    gb->memory.hram[address - 0xff80] = value;

  else if (address == 0xff40)
    gb->gpu.control = value;
  else if (address == 0xff42)
    gb->gpu.scrollY = value;
  else if (address == 0xff43)
    gb->gpu.scrollX = value;
  else if (address == 0xff46) {
	  printf("memcpy(%x, %x)\n", gb->cpu.registers.PC, value);
    memory_copy(gb, 0xfe00, value << 8, 160); // OAM DMA
  }

  else if (address == 0xff47) { // write only
    int i;
    for (i = 0; i < 4; i++)
      gb->gpu.backgroundPalette[i] = gb->gpu.palette[(value >> (i * 2)) & 3];
  }

  else if (address == 0xff48) { // write only
    int i;
    for (i = 0; i < 4; i++)
      gb->gpu.spritePalette[0][i] = gb->gpu.palette[(value >> (i * 2)) & 3];
  }

  else if (address == 0xff49) { // write only
    int i;
    for (i = 0; i < 4; i++)
      gb->gpu.spritePalette[1][i] = gb->gpu.palette[(value >> (i * 2)) & 3];
  }

  else if (address >= 0xff00 && address <= 0xff7f)
    gb->memory.io[address - 0xff00] = value;

  else if (address == 0xff0f)
    gb->cpu.interrupt.flags = value;
  else if (address == 0xffff)
    gb->cpu.interrupt.enable = value;
}

void write_short(gameboy_t *gb, unsigned short address, unsigned short value) {
  write_byte(gb, address, (unsigned char)(value & 0x00ff));
  write_byte(gb, address + 1, (unsigned char)((value & 0xff00) >> 8));
}

void write_short_to_stack(gameboy_t *gb, unsigned short value) {
  gb->cpu.registers.SP -= 2;
  write_short(gb, gb->cpu.registers.SP, value);

#ifdef DEBUG_STACK
  printf("Stack write 0x%04x\n", value);
#endif
}
