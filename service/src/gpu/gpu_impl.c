#include <stdio.h>
#include "../gb/gb.h"
#include "gpu.h"
#include "libbmp.h"

volatile int scanline = 0;
void logit(char *x);

void gpuReset() {
  scanline = 0;
}

void gpuStep(gameboy_t *gb) {
  static int lastTicks = 0;

  gb->gpu.tick += gb->cpu.ticks - lastTicks;

  lastTicks = gb->cpu.ticks;

  switch (gb->gpu.mode) {
  case GPU_MODE_HBLANK:
    if (gb->gpu.tick >= 204) {
      hblank();

      if (scanline == 143) {
        if (gb->cpu.interrupt.enable & INTERRUPTS_VBLANK)
          gb->cpu.interrupt.flags |= INTERRUPTS_VBLANK;

        gb->gpu.mode = GPU_MODE_VBLANK;
      }

      else
        gb->gpu.mode = GPU_MODE_OAM;

      gb->gpu.tick -= 204;
    }

    break;

  case GPU_MODE_VBLANK:
    if (gb->gpu.tick >= 456) {
      scanline++;

      if (scanline > 153) {
        scanline = 0;
        gb->gpu.mode = GPU_MODE_OAM;
      }

      gb->gpu.tick -= 456;
    }

    break;

  case GPU_MODE_OAM:
    if (gb->gpu.tick >= 80) {
      gb->gpu.mode = GPU_MODE_VRAM;

      gb->gpu.tick -= 80;
    }

    break;

  case GPU_MODE_VRAM:
    if (gb->gpu.tick >= 172) {
      gb->gpu.mode = GPU_MODE_HBLANK;

      renderScanline(gb);

      gb->gpu.tick -= 172;
    }

    break;
  }
  gb->gpu.scanline = scanline;
}

void hblank() { scanline++; }

/*void updateTile(gameboy_t *gb, unsigned short address, unsigned char value) {
  address &= 0x1ffe;

  unsigned short tile = (address >> 4) & 511;
  unsigned short y = (address >> 1) & 7;

  unsigned char x, bitIndex;
  for (x = 0; x < 8; x++) {
    bitIndex = 1 << (7 - x);

    //((unsigned char (*)[8][8])tiles)[tile][y][x] = ((vram[address] & bitIndex)
    //? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
    gb->gpu.tiles[tile][y][x] =
        ((gb->memory.vram[address] & bitIndex) ? 1 : 0) +
        ((gb->memory.vram[address + 1] & bitIndex) ? 2 : 0);
  }
}*/

void renderScanline(gameboy_t *gb) {
  int mapOffset = (gb->gpu.control & GPU_CONTROL_TILEMAP) ? 0x1c00 : 0x1800;
  mapOffset += (((scanline + gb->gpu.scrollY) & 255) >> 3) << 5;

  int lineOffset = (gb->gpu.scrollX >> 3);

  int x = gb->gpu.scrollX & 7;
  int y = (scanline + gb->gpu.scrollY) & 7;

  int pixelOffset;

  // invalid state
  if (scanline > 143) {
    return;
  }


  logit("RENDERING");
  logit("RENDERING");
  logit("RENDERING");
  pixelOffset = scanline * 160;

  unsigned short tile = (unsigned short)gb->memory.vram[mapOffset + lineOffset];
  // if((gpu.control & GPU_CONTROL_TILESET) && tile < 128) tile += 256;

  unsigned char scanlineRow[160];

  // if bg enabled
  int i;
  for (i = 0; i < 160; i++) {
    unsigned char colour = gb->gpu.tiles[tile][y][x];

    scanlineRow[i] = colour;

    gb->gpu.framebuffer[pixelOffset].r = gb->gpu.backgroundPalette[colour].r;
    gb->gpu.framebuffer[pixelOffset].g = gb->gpu.backgroundPalette[colour].g;
    gb->gpu.framebuffer[pixelOffset].b = gb->gpu.backgroundPalette[colour].b;

    pixelOffset++;

    x++;
    if (x == 8) {
      x = 0;
      lineOffset = (lineOffset + 1) & 31;
      tile = gb->memory.vram[mapOffset + lineOffset];
      // if((gpu.control & GPU_CONTROL_TILESET) && tile < 128) tile += 256;
    }
  }

  // if sprites enabled
  for (i = 0; i < 40; i++) {
    struct sprite sprite = ((struct sprite *)gb->memory.oam)[i];
	sprite_t *s;

    int sx = sprite.x - 8;
    int sy = sprite.y - 16;

    if (sy <= scanline && (sy + 8) > scanline) {
      COLOR *pal = gb->gpu.spritePalette[sprite.palette];

      int pixelOffset;
      pixelOffset = scanline * 160 + sx;

      unsigned char tileRow;
      if (sprite.vFlip)
        tileRow = 7 - (scanline - sy);
      else
        tileRow = scanline - sy;

      int x;
      for (x = 0; x < 8; x++) {
        if (sx + x >= 0 && sx + x < 160 &&
            (~sprite.priority || !scanlineRow[sx + x])) {
          unsigned char colour;

          if (sprite.hFlip)
            colour = gb->gpu.tiles[sprite.tile][tileRow][7 - x];
          else
            colour = gb->gpu.tiles[sprite.tile][tileRow][x];

          if (colour) {
            gb->gpu.framebuffer[pixelOffset].r = pal[colour].r;
            gb->gpu.framebuffer[pixelOffset].g = pal[colour].g;
            gb->gpu.framebuffer[pixelOffset].b = pal[colour].b;
          }

          pixelOffset++;
        }
      }
    }
  }
}


uint8_t fireInterrupt(gameboy_t *gb) {
  if (gb->cpu.interrupt.master && gb->cpu.interrupt.enable && gb->cpu.interrupt.flags) {
    return gb->cpu.interrupt.enable & gb->cpu.interrupt.flags;
  }
  return 0;
}

void postDraw(gameboy_t *gb) {
	uint8_t fire = fireInterrupt(gb);
	if (fire & INTERRUPTS_VBLANK) {
		gb->cpu.interrupt.flags &= ~INTERRUPTS_VBLANK;
		gb->cpu.interrupt.master = 0;
		write_short_to_stack(gb, gb->cpu.registers.PC);
		gb->cpu.registers.PC = 0x40;
		gb->cpu.ticks += 12;
	}
	if(fire & INTERRUPTS_LCDSTAT) {
		gb->cpu.interrupt.flags &= ~INTERRUPTS_LCDSTAT;
		gb->cpu.interrupt.master = 0;
		write_short_to_stack(gb, gb->cpu.registers.PC);
		gb->cpu.registers.PC = 0x48;
		gb->cpu.ticks += 12;

	}
	if(fire & INTERRUPTS_TIMER) {
		gb->cpu.interrupt.flags &= ~INTERRUPTS_TIMER;
		gb->cpu.interrupt.master = 0;
		write_short_to_stack(gb, gb->cpu.registers.PC);
		gb->cpu.registers.PC = 0x50;
		gb->cpu.ticks += 12;
	}

	if(fire & INTERRUPTS_SERIAL) {
		gb->cpu.interrupt.flags &= ~INTERRUPTS_SERIAL;
		gb->cpu.interrupt.master = 0;
		write_short_to_stack(gb, gb->cpu.registers.PC);
		gb->cpu.registers.PC = 0x58;
		gb->cpu.ticks += 12;
	}

	if(fire & INTERRUPTS_JOYPAD) {
		gb->cpu.interrupt.flags &= ~INTERRUPTS_JOYPAD;
		gb->cpu.interrupt.master = 0;
		write_short_to_stack(gb, gb->cpu.registers.PC);
		gb->cpu.registers.PC = 0x60;
		gb->cpu.ticks += 12;
	}

}

// Length is dynamic here even though screen is constant size to support
// different output formats, BMP, JPEG, etc
int drawFramebuffer(gameboy_t *gb, uint8_t **image, size_t *length) {

  bmp_img img;

  uint8_t fire = fireInterrupt(gb);
  if (fire & INTERRUPTS_VBLANK) {
    //gb->cpu.interrupt.flags &= ~INTERRUPTS_VBLANK;
  } else {
    return 0;
  }

  bmp_img_init_df (&img, 160, 144);

  *image = NULL;
  *length = 0;

  // initialize image
  size_t x, y;
  for (y = 0; y < 144; y++) {

    for (x = 0; x < 160; x++) {
      COLOR *col = &gb->gpu.framebuffer[x + y * 160];

      /*if (col->r || col->g || col->b)
        printf("%d:%d - %x %x %x\n", y, x, col->r, col->g, col->b);
		*/

      bmp_pixel_init(&img.img_pixels[y][x], col->r, col->g, col->b);
    }
  }

  *length = sizeof(short) +\
            sizeof(bmp_header) +\
            img.img_header.biHeight * img.img_header.biWidth * sizeof(bmp_pixel);

  // write image file out to buffer
  *image = (uint8_t *)malloc(*length);
  if (*image == NULL) {
    return 0;
  }

  uint8_t *p = *image;

  // MAGIC and header
  *((uint16_t *)p) = BMP_MAGIC;
  p += 2;

  memcpy(p, &img.img_header, sizeof(bmp_header));
  p += sizeof(bmp_header);

  // actual BMP body
  size_t h = img.img_header.biHeight;
  size_t offset = img.img_header.biHeight > 0 ? h - 1 : 0;

  for(y = 0; y < h; y++) {
    memcpy(p,
           img.img_pixels[offset - y],
           img.img_header.biWidth * sizeof(bmp_pixel));
    p += img.img_header.biWidth * sizeof(bmp_pixel);
  }

  if (*length != p - *image) {
    printf("ERROR: failed to write out entire image!\n");
  }

  bmp_img_free(&img);
  return 1;
}
