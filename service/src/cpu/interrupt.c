#include "interrupt.h"

#include "../gb/gb.h"

struct interrupt interrupt;

void interruptStep(gameboy_t *gb) {
	if(interrupt.master && interrupt.enable && interrupt.flags) {
		unsigned char fire = interrupt.enable & interrupt.flags;

		if(fire & INTERRUPTS_VBLANK) {
			gb->cpu.interrupt.flags &= ~INTERRUPTS_VBLANK;
			vblank(gb);
		}

		if(fire & INTERRUPTS_LCDSTAT) {
			gb->cpu.interrupt.flags &= ~INTERRUPTS_LCDSTAT;
			lcdStat(gb);
		}

		if(fire & INTERRUPTS_TIMER) {
			gb->cpu.interrupt.flags &= ~INTERRUPTS_TIMER;
			timer(gb);
		}

		if(fire & INTERRUPTS_SERIAL) {
			gb->cpu.interrupt.flags &= ~INTERRUPTS_SERIAL;
			serial(gb);
		}

		if(fire & INTERRUPTS_JOYPAD) {
			gb->cpu.interrupt.flags &= ~INTERRUPTS_JOYPAD;
			joypad(gb);
		}
	}
}

void vblank(gameboy_t *gb) {
	// XXX this is kijnd of fucky?
	gb->time_to_redraw = 1;

	interrupt.master = 0;
	write_short_to_stack(gb, gb->cpu.registers.PC);
	gb->cpu.registers.PC = 0x40;

	gb->cpu.ticks += 12;
}

void lcdStat(gameboy_t *gb) {
	gb->cpu.interrupt.master = 0;
	write_short_to_stack(gb, gb->cpu.registers.PC);
	gb->cpu.registers.PC = 0x48;

	gb->cpu.ticks += 12;
}

void timer(gameboy_t *gb) {
	gb->cpu.interrupt.master = 0;
	write_short_to_stack(gb, gb->cpu.registers.PC);
	gb->cpu.registers.PC = 0x50;

	gb->cpu.ticks += 12;
}

void serial(gameboy_t *gb) {
	gb->cpu.interrupt.master = 0;
	write_short_to_stack(gb, gb->cpu.registers.PC);
	gb->cpu.registers.PC = 0x58;

	gb->cpu.ticks += 12;
}

void joypad(gameboy_t *gb) {
	gb->cpu.interrupt.master = 0;
	write_short_to_stack(gb, gb->cpu.registers.PC);
	gb->cpu.registers.PC = 0x60;

	gb->cpu.ticks += 12;
}

void returnFromInterrupt(gameboy_t *gb) {
	gb->cpu.interrupt.master = 1;
	gb->cpu.registers.PC = read_short_from_stack(gb);
}

