#ifndef __GAMEBOY_H_
#define __GAMEBOY_H_

#include "../cpu/memory.h"
#include "../cpu/cpu.h"
#include "../gpu/gpu.h"
#include "../input/input.h"
#define CPU_SERVER 8080
#define ROMLOADER_SERVER 8081
#define INPUT_SERVER 8082
#define GPU_SERVER 8083
#define SOUND_SERVER 8084
#define LOGS_SERVER 8085

typedef struct gameboy {
	cpu_t cpu;
	memory_t memory;
	gpu_t gpu;
	keys_t keys;
	uint8_t time_to_redraw;
} gameboy_t;

#endif
