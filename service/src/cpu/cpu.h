#ifndef _CPU_H__
#define _CPU_H__
#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include "cb.h"
#include "registers.h"
#include "interrupt.h"

#define FLAGS_ZERO (1 << 7)
#define FLAGS_NEGATIVE (1 << 6)
#define FLAGS_HALFCARRY (1 << 5)
#define FLAGS_CARRY (1 << 4)

typedef struct gameboy gameboy_t;

typedef struct cpu {
	registers_t registers;
	interrupt_t interrupt;
	uint8_t stopped;
	uint32_t ticks;
} cpu_t;

#pragma once

#define FLAGS_ZERO (1 << 7)
#define FLAGS_NEGATIVE (1 << 6)
#define FLAGS_HALFCARRY (1 << 5)
#define FLAGS_CARRY (1 << 4)

#define FLAGS_ISZERO (gb->cpu.registers.f & FLAGS_ZERO)
#define FLAGS_ISNEGATIVE (gb->cpu.registers.f & FLAGS_NEGATIVE)
#define FLAGS_ISCARRY (gb->cpu.registers.f & FLAGS_CARRY)
#define FLAGS_ISHALFCARRY (gb->cpu.registers.f & FLAGS_HALFCARRY)

#define FLAGS_ISSET(x) (gb->cpu.registers.f & (x))
#define FLAGS_SET(x) (gb->cpu.registers.f |= (x))
#define FLAGS_CLEAR(x) (gb->cpu.registers.f &= ~(x))

typedef struct instruction {
	char *disassembly;
	unsigned char operandLength;
	void *execute;
	//unsigned char ticks;
} instruction_t; // instructions[256];

void reset(gameboy_t *gb);
void cpuStep(gameboy_t *gb);


void undefined(gameboy_t *gb);

void nop(gameboy_t *gb);
void ld_bc_nn(gameboy_t *gb, unsigned short operand);
void ld_bcp_a(gameboy_t *gb);
void inc_bc(gameboy_t *gb);
void inc_b(gameboy_t *gb);
void dec_b(gameboy_t *gb);
void ld_b_n(gameboy_t *gb, unsigned char operand);
void rlca(gameboy_t *gb);
void ld_nnp_sp(gameboy_t *gb, unsigned short operand);
void add_hl_bc(gameboy_t *gb);
void ld_a_bcp(gameboy_t *gb);
void dec_bc(gameboy_t *gb);
void inc_c(gameboy_t *gb);
void dec_c(gameboy_t *gb);
void ld_c_n(gameboy_t *gb, unsigned char operand);
void rrca(gameboy_t *gb);
void stop(gameboy_t *gb, unsigned char operand);
void ld_de_nn(gameboy_t *gb, unsigned short operand);
void ld_dep_a(gameboy_t *gb);
void inc_de(gameboy_t *gb);
void inc_d(gameboy_t *gb);
void dec_d(gameboy_t *gb);
void ld_d_n(gameboy_t *gb, unsigned char operand);
void rla(gameboy_t *gb);
void jr_n(gameboy_t *gb, unsigned char operand);
void add_hl_de(gameboy_t *gb);
void ld_a_dep(gameboy_t *gb);
void dec_de(gameboy_t *gb);
void inc_e(gameboy_t *gb);
void dec_e(gameboy_t *gb);
void ld_e_n(gameboy_t *gb, unsigned char operand);
void rra(gameboy_t *gb);
void jr_nz_n(gameboy_t *gb, unsigned char operand);
void ld_hl_nn(gameboy_t *gb, unsigned short operand);
void ldi_hlp_a(gameboy_t *gb);
void inc_hl(gameboy_t *gb);
void inc_h(gameboy_t *gb);
void dec_h(gameboy_t *gb);
void ld_h_n(gameboy_t *gb, unsigned char operand);
void daa(gameboy_t *gb);
void jr_z_n(gameboy_t *gb, unsigned char operand);
void add_hl_hl(gameboy_t *gb);
void ldi_a_hlp(gameboy_t *gb);
void dec_hl(gameboy_t *gb);
void inc_l(gameboy_t *gb);
void dec_l(gameboy_t *gb);
void ld_l_n(gameboy_t *gb, unsigned char operand);
void cpl(gameboy_t *gb);
void jr_nc_n(gameboy_t *gb, char operand);
void ld_sp_nn(gameboy_t *gb, unsigned short operand);
void ldd_hlp_a(gameboy_t *gb);
void inc_sp(gameboy_t *gb);
void inc_hlp(gameboy_t *gb);
void dec_hlp(gameboy_t *gb);
void ld_hlp_n(gameboy_t *gb, unsigned char operand);
void scf(gameboy_t *gb);
void jr_c_n(gameboy_t *gb, char operand);
void add_hl_sp(gameboy_t *gb);
void ldd_a_hlp(gameboy_t *gb);
void dec_sp(gameboy_t *gb);
void inc_a(gameboy_t *gb);
void dec_a(gameboy_t *gb);
void ld_a_n(gameboy_t *gb, unsigned char operand);
void ccf(gameboy_t *gb);
void ld_b_c(gameboy_t *gb);
void ld_b_d(gameboy_t *gb);
void ld_b_e(gameboy_t *gb);
void ld_b_h(gameboy_t *gb);
void ld_b_l(gameboy_t *gb);
void ld_b_hlp(gameboy_t *gb);
void ld_b_a(gameboy_t *gb);
void ld_c_b(gameboy_t *gb);
void ld_c_d(gameboy_t *gb);
void ld_c_e(gameboy_t *gb);
void ld_c_h(gameboy_t *gb);
void ld_c_l(gameboy_t *gb);
void ld_c_hlp(gameboy_t *gb);
void ld_c_a(gameboy_t *gb);
void ld_d_b(gameboy_t *gb);
void ld_d_c(gameboy_t *gb);
void ld_d_e(gameboy_t *gb);
void ld_d_h(gameboy_t *gb);
void ld_d_l(gameboy_t *gb);
void ld_d_hlp(gameboy_t *gb);
void ld_d_a(gameboy_t *gb);
void ld_e_b(gameboy_t *gb);
void ld_e_c(gameboy_t *gb);
void ld_e_d(gameboy_t *gb);
void ld_e_h(gameboy_t *gb);
void ld_e_l(gameboy_t *gb);
void ld_e_hlp(gameboy_t *gb);
void ld_e_a(gameboy_t *gb);
void ld_h_b(gameboy_t *gb);
void ld_h_c(gameboy_t *gb);
void ld_h_d(gameboy_t *gb);
void ld_h_e(gameboy_t *gb);
void ld_h_l(gameboy_t *gb);
void ld_h_hlp(gameboy_t *gb);
void ld_h_a(gameboy_t *gb);
void ld_l_b(gameboy_t *gb);
void ld_l_c(gameboy_t *gb);
void ld_l_d(gameboy_t *gb);
void ld_l_e(gameboy_t *gb);
void ld_l_h(gameboy_t *gb);
void ld_l_hlp(gameboy_t *gb);
void ld_l_a(gameboy_t *gb);
void ld_hlp_b(gameboy_t *gb);
void ld_hlp_c(gameboy_t *gb);
void ld_hlp_d(gameboy_t *gb);
void ld_hlp_e(gameboy_t *gb);
void ld_hlp_h(gameboy_t *gb);
void ld_hlp_l(gameboy_t *gb);
void halt(gameboy_t *gb);
void ld_hlp_a(gameboy_t *gb);
void ld_a_b(gameboy_t *gb);
void ld_a_c(gameboy_t *gb);
void ld_a_d(gameboy_t *gb);
void ld_a_e(gameboy_t *gb);
void ld_a_h(gameboy_t *gb);
void ld_a_l(gameboy_t *gb);
void ld_a_hlp(gameboy_t *gb);
void add_a_b(gameboy_t *gb);
void add_a_c(gameboy_t *gb);
void add_a_d(gameboy_t *gb);
void add_a_e(gameboy_t *gb);
void add_a_h(gameboy_t *gb);
void add_a_l(gameboy_t *gb);
void add_a_hlp(gameboy_t *gb);
void add_a_a(gameboy_t *gb);
void adc_b(gameboy_t *gb);
void adc_c(gameboy_t *gb);
void adc_d(gameboy_t *gb);
void adc_e(gameboy_t *gb);
void adc_h(gameboy_t *gb);
void adc_l(gameboy_t *gb);
void adc_hlp(gameboy_t *gb);
void adc_a(gameboy_t *gb);
void sub_b(gameboy_t *gb);
void sub_c(gameboy_t *gb);
void sub_d(gameboy_t *gb);
void sub_e(gameboy_t *gb);
void sub_h(gameboy_t *gb);
void sub_l(gameboy_t *gb);
void sub_hlp(gameboy_t *gb);
void sub_a(gameboy_t *gb);
void sbc_b(gameboy_t *gb);
void sbc_c(gameboy_t *gb);
void sbc_d(gameboy_t *gb);
void sbc_e(gameboy_t *gb);
void sbc_h(gameboy_t *gb);
void sbc_l(gameboy_t *gb);
void sbc_hlp(gameboy_t *gb);
void sbc_a(gameboy_t *gb);
void and_b(gameboy_t *gb);
void and_c(gameboy_t *gb);
void and_d(gameboy_t *gb);
void and_e(gameboy_t *gb);
void and_h(gameboy_t *gb);
void and_l(gameboy_t *gb);
void and_hlp(gameboy_t *gb);
void and_a(gameboy_t *gb);
void xor_b(gameboy_t *gb);
void xor_c(gameboy_t *gb);
void xor_d(gameboy_t *gb);
void xor_e(gameboy_t *gb);
void xor_h(gameboy_t *gb);
void xor_l(gameboy_t *gb);
void xor_hlp(gameboy_t *gb);
void xor_a(gameboy_t *gb);
void or_b(gameboy_t *gb);
void or_c(gameboy_t *gb);
void or_d(gameboy_t *gb);
void or_e(gameboy_t *gb);
void or_h(gameboy_t *gb);
void or_l(gameboy_t *gb);
void or_hlp(gameboy_t *gb);
void or_a(gameboy_t *gb);
void cp_b(gameboy_t *gb);
void cp_c(gameboy_t *gb);
void cp_d(gameboy_t *gb);
void cp_e(gameboy_t *gb);
void cp_h(gameboy_t *gb);
void cp_l(gameboy_t *gb);
void cp_hlp(gameboy_t *gb);
void cp_a(gameboy_t *gb);
void ret_nz(gameboy_t *gb);
void pop_bc(gameboy_t *gb);
void jp_nz_nn(gameboy_t *gb, unsigned short operand);
void jp_nn(gameboy_t *gb, unsigned short operand);
void call_nz_nn(gameboy_t *gb, unsigned short operand);
void push_bc(gameboy_t *gb);
void add_a_n(gameboy_t *gb, unsigned char operand);
void rst_0(gameboy_t *gb);
void ret_z(gameboy_t *gb);
void ret(gameboy_t *gb);
void jp_z_nn(gameboy_t *gb, unsigned short operand);
void call_z_nn(gameboy_t *gb, unsigned short operand);
void call_nn(gameboy_t *gb, unsigned short operand);
void adc_n(gameboy_t *gb, unsigned char operand);
void rst_08(gameboy_t *gb);
void ret_nc(gameboy_t *gb);
void pop_de(gameboy_t *gb);
void jp_nc_nn(gameboy_t *gb, unsigned short operand);
void call_nc_nn(gameboy_t *gb, unsigned short operand);
void push_de(gameboy_t *gb);
void sub_n(gameboy_t *gb, unsigned char operand);
void rst_10(gameboy_t *gb);
void ret_c(gameboy_t *gb);
void jp_c_nn(gameboy_t *gb, unsigned short operand);
void call_c_nn(gameboy_t *gb, unsigned short operand);
void sbc_n(gameboy_t *gb, unsigned char operand);
void rst_18(gameboy_t *gb);
void ld_ff_n_ap(gameboy_t *gb, unsigned char operand);
void pop_hl(gameboy_t *gb);
void ld_ff_c_a(gameboy_t *gb);
void push_hl(gameboy_t *gb);
void and_n(gameboy_t *gb, unsigned char operand);
void rst_20(gameboy_t *gb);
void add_sp_n(gameboy_t *gb, char operand);
void jp_hl(gameboy_t *gb);
void ld_nnp_a(gameboy_t *gb, unsigned short operand);
void xor_n(gameboy_t *gb, unsigned char operand);
void rst_28(gameboy_t *gb);
void ld_ff_ap_n(gameboy_t *gb, unsigned char operand);
void pop_af(gameboy_t *gb);
void ld_a_ff_c(gameboy_t *gb);
void di_inst(gameboy_t *gb);
void push_af(gameboy_t *gb);
void or_n(gameboy_t *gb, unsigned char operand);
void rst_30(gameboy_t *gb);
void ld_hl_sp_n(gameboy_t *gb, unsigned char operand);
void ld_sp_hl(gameboy_t *gb);
void ld_a_nnp(gameboy_t *gb, unsigned short operand);
void ei(gameboy_t *gb);
void cp_n(gameboy_t *gb, unsigned char operand);
void rst_38(gameboy_t *gb);
#ifdef __cplusplus
}
#endif
#endif // _CPU_H__
