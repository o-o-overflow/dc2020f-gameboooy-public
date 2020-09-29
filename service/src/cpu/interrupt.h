#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_

typedef struct gameboy gameboy_t;

#define INTERRUPTS_VBLANK (1 << 0)
#define INTERRUPTS_LCDSTAT (1 << 1)
#define INTERRUPTS_TIMER (1 << 2)
#define INTERRUPTS_SERIAL (1 << 3)
#define INTERRUPTS_JOYPAD (1 << 4)

typedef struct interrupt {
  unsigned char master;
  unsigned char enable;
  unsigned char flags;
} interrupt_t;

void interruptStep(gameboy_t *gb);

void vblank(gameboy_t *gb);
void lcdStat(gameboy_t *gb);
void timer(gameboy_t *gb);
void serial(gameboy_t *gb);
void joypad(gameboy_t *gb);

void returnFromInterrupt(gameboy_t *gb);
#endif
