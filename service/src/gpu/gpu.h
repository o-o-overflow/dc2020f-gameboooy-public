#ifndef __GPU_H_
#define __GPU_H_
#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>
#define __LITTLE_ENDIAN__ 1
#define GPU_CONTROL_BGENABLE (1 << 0)
#define GPU_CONTROL_SPRITEENABLE (1 << 1)
#define GPU_CONTROL_SPRITEVDOUBLE (1 << 2)
#define GPU_CONTROL_TILEMAP (1 << 3)
#define GPU_CONTROL_TILESET (1 << 4)
#define GPU_CONTROL_WINDOWENABLE (1 << 5)
#define GPU_CONTROL_WINDOWTILEMAP (1 << 6)
#define GPU_CONTROL_DISPLAYENABLE (1 << 7)

typedef struct rgb {
	uint8_t r, g, b;
} COLOR;

enum gpuMode {
	GPU_MODE_HBLANK = 0,
	GPU_MODE_VBLANK = 1,
	GPU_MODE_OAM = 2,
	GPU_MODE_VRAM = 3,
};

/*typedef struct options {
  unsigned char priority : 1;
  unsigned char vFlip : 1;
  unsigned char hFlip : 1;
  unsigned char palette : 1;
} options_t;*/
typedef struct sprite {
#ifdef __LITTLE_ENDIAN__
  unsigned char y;
  unsigned char x;
  unsigned char tile;
  unsigned char priority : 1;
  unsigned char vFlip : 1;
  unsigned char hFlip : 1;
  unsigned char palette : 1;
  //options_t options;
#else
  unsigned char y;
  unsigned char x;
  unsigned char tile;
  options_t options;
#endif
} sprite_t;


typedef struct gpu {
  unsigned char control;
  unsigned char scrollX;
  unsigned char scrollY;
  unsigned char scanline;
  unsigned long tick;
  unsigned int lastTicks;
  int mode;

  unsigned char tiles[384][8][8];

  COLOR backgroundPalette[4];
  COLOR spritePalette[2][4];
  COLOR framebuffer[160 * 144];
  COLOR palette[4];
} gpu_t;

void gpuReset();
void gpuStep(gameboy_t *gb);

void hblank();

void renderScanline(gameboy_t *gb);
void postDraw(gameboy_t *gb);

void updateTile(gameboy_t *gb, unsigned short address, unsigned char value);
int drawFramebuffer(gameboy_t *gb, uint8_t **image, size_t *length);
#ifdef __cplusplus
}
#endif
#endif
