#ifndef _INSTRUCTION_H__
#define _INSTRUCTION_H__
#include <stdint.h>

typedef struct instruction {
	const char *opcode; // human readable instruction
	uint8_t arglen; // either 1 or 2 bytes
	void *callback; // function that gets called
	uint8_t ticks; // ticks that this instruction takes
} instruction_t;

#endif // _INSTRUCTION_H__
