#include "../gb/gb.h"
//#include "platform.h"

// cb.h needs to be made XXX
//#include "cb.h"
//#include "debug.h"
// XXX display needs to be made?
//#include "display.h"
#include "gpu.h"
//#include "interrupts.h"
//#include "keys.h"
//#include "main.h"
//#include "memory.h"
//#include "registers.h"

//#include "cpu.h"

/*
References:

Opcode disassemblies:
http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
http://imrannazar.com/Gameboy-Z80-Opcode-Map
GBE source

Which instructions modify flags:
http://gameboy.mongenel.com/dmg/opcodes.html

Instruction implementation:
GBE source

Testing:
NO$GMB
*/

const uint8_t ioReset[SIZE_IO] = {
	0x0F, 0x00, 0x7C, 0xFF, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0x01, 0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00,
	0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF, 0xFF, 0x00, 0x00, 0xBF,
	0x77, 0xF3, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0xFF, 0x00, 0xFF, 0x91, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xC1, 0x00,
	0xFE, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCE, 0xED, 0x66, 0x66,
	0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
	0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6,
	0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x45, 0xEC, 0x52, 0xFA,
	0x08, 0xB7, 0x07, 0x5D, 0x01, 0xFD, 0xC0, 0xFF, 0x08, 0xFC, 0x00, 0xE5,
	0x0B, 0xF8, 0xC2, 0xCE, 0xF4, 0xF9, 0x0F, 0x7F, 0x45, 0x6D, 0x3D, 0xFE,
	0x46, 0x97, 0x33, 0x5E, 0x08, 0xEF, 0xF1, 0xFF, 0x86, 0x83, 0x24, 0x74,
	0x12, 0xFC, 0x00, 0x9F, 0xB4, 0xB7, 0x06, 0xD5, 0xD0, 0x7A, 0x00, 0x9E,
	0x04, 0x5F, 0x41, 0x2F, 0x1D, 0x77, 0x36, 0x75, 0x81, 0xAA, 0x70, 0x3A,
	0x98, 0xD1, 0x71, 0x02, 0x4D, 0x01, 0xC1, 0xFF, 0x0D, 0x00, 0xD3, 0x05,
	0xF9, 0x00, 0x0B, 0x00
};

struct instruction instructions[256] = {
    {"NOP", 0, nop},                            // 0x00
    {"LD BC, 0x%04X", 2, ld_bc_nn},             // 0x01
    {"LD (BC), A", 0, ld_bcp_a},                // 0x02
    {"INC BC", 0, inc_bc},                      // 0x03
    {"INC B", 0, inc_b},                        // 0x04
    {"DEC B", 0, dec_b},                        // 0x05
    {"LD B, 0x%02X", 1, ld_b_n},                // 0x06
    {"RLCA", 0, rlca},                          // 0x07
    {"LD (0x%04X), SP", 2, ld_nnp_sp},          // 0x08
    {"ADD HL, BC", 0, add_hl_bc},               // 0x09
    {"LD A, (BC)", 0, ld_a_bcp},                // 0x0a
    {"DEC BC", 0, dec_bc},                      // 0x0b
    {"INC C", 0, inc_c},                        // 0x0c
    {"DEC C", 0, dec_c},                        // 0x0d
    {"LD C, 0x%02X", 1, ld_c_n},                // 0x0e
    {"RRCA", 0, rrca},                          // 0x0f
    {"STOP", 1, stop},                          // 0x10
    {"LD DE, 0x%04X", 2, ld_de_nn},             // 0x11
    {"LD (DE), A", 0, ld_dep_a},                // 0x12
    {"INC DE", 0, inc_de},                      // 0x13
    {"INC D", 0, inc_d},                        // 0x14
    {"DEC D", 0, dec_d},                        // 0x15
    {"LD D, 0x%02X", 1, ld_d_n},                // 0x16
    {"RLA", 0, rla},                            // 0x17
    {"JR 0x%02X", 1, jr_n},                     // 0x18
    {"ADD HL, DE", 0, add_hl_de},               // 0x19
    {"LD A, (DE)", 0, ld_a_dep},                // 0x1a
    {"DEC DE", 0, dec_de},                      // 0x1b
    {"INC E", 0, inc_e},                        // 0x1c
    {"DEC E", 0, dec_e},                        // 0x1d
    {"LD E, 0x%02X", 1, ld_e_n},                // 0x1e
    {"RRA", 0, rra},                            // 0x1f
    {"JR NZ, 0x%02X", 1, jr_nz_n},              // 0x20
    {"LD HL, 0x%04X", 2, ld_hl_nn},             // 0x21
    {"LDI (HL), A", 0, ldi_hlp_a},              // 0x22
    {"INC HL", 0, inc_hl},                      // 0x23
    {"INC H", 0, inc_h},                        // 0x24
    {"DEC H", 0, dec_h},                        // 0x25
    {"LD H, 0x%02X", 1, ld_h_n},                // 0x26
    {"DAA", 0, daa},                            // 0x27
    {"JR Z, 0x%02X", 1, jr_z_n},                // 0x28
    {"ADD HL, HL", 0, add_hl_hl},               // 0x29
    {"LDI A, (HL)", 0, ldi_a_hlp},              // 0x2a
    {"DEC HL", 0, dec_hl},                      // 0x2b
    {"INC L", 0, inc_l},                        // 0x2c
    {"DEC L", 0, dec_l},                        // 0x2d
    {"LD L, 0x%02X", 1, ld_l_n},                // 0x2e
    {"CPL", 0, cpl},                            // 0x2f
    {"JR NC, 0x%02X", 1, jr_nc_n},              // 0x30
    {"LD SP, 0x%04X", 2, ld_sp_nn},             // 0x31
    {"LDD (HL), A", 0, ldd_hlp_a},              // 0x32
    {"INC SP", 0, inc_sp},                      // 0x33
    {"INC (HL)", 0, inc_hlp},                   // 0x34
    {"DEC (HL)", 0, dec_hlp},                   // 0x35
    {"LD (HL), 0x%02X", 1, ld_hlp_n},           // 0x36
    {"SCF", 0, scf},                            // 0x37
    {"JR C, 0x%02X", 1, jr_c_n},                // 0x38
    {"ADD HL, SP", 0, add_hl_sp},               // 0x39
    {"LDD A, (HL)", 0, ldd_a_hlp},              // 0x3a
    {"DEC SP", 0, dec_sp},                      // 0x3b
    {"INC A", 0, inc_a},                        // 0x3c
    {"DEC A", 0, dec_a},                        // 0x3d
    {"LD A, 0x%02X", 1, ld_a_n},                // 0x3e
    {"CCF", 0, ccf},                            // 0x3f
    {"LD B, B", 0, nop},                        // 0x40
    {"LD B, C", 0, ld_b_c},                     // 0x41
    {"LD B, D", 0, ld_b_d},                     // 0x42
    {"LD B, E", 0, ld_b_e},                     // 0x43
    {"LD B, H", 0, ld_b_h},                     // 0x44
    {"LD B, L", 0, ld_b_l},                     // 0x45
    {"LD B, (HL)", 0, ld_b_hlp},                // 0x46
    {"LD B, A", 0, ld_b_a},                     // 0x47
    {"LD C, B", 0, ld_c_b},                     // 0x48
    {"LD C, C", 0, nop},                        // 0x49
    {"LD C, D", 0, ld_c_d},                     // 0x4a
    {"LD C, E", 0, ld_c_e},                     // 0x4b
    {"LD C, H", 0, ld_c_h},                     // 0x4c
    {"LD C, L", 0, ld_c_l},                     // 0x4d
    {"LD C, (HL)", 0, ld_c_hlp},                // 0x4e
    {"LD C, A", 0, ld_c_a},                     // 0x4f
    {"LD D, B", 0, ld_d_b},                     // 0x50
    {"LD D, C", 0, ld_d_c},                     // 0x51
    {"LD D, D", 0, nop},                        // 0x52
    {"LD D, E", 0, ld_d_e},                     // 0x53
    {"LD D, H", 0, ld_d_h},                     // 0x54
    {"LD D, L", 0, ld_d_l},                     // 0x55
    {"LD D, (HL)", 0, ld_d_hlp},                // 0x56
    {"LD D, A", 0, ld_d_a},                     // 0x57
    {"LD E, B", 0, ld_e_b},                     // 0x58
    {"LD E, C", 0, ld_e_c},                     // 0x59
    {"LD E, D", 0, ld_e_d},                     // 0x5a
    {"LD E, E", 0, nop},                        // 0x5b
    {"LD E, H", 0, ld_e_h},                     // 0x5c
    {"LD E, L", 0, ld_e_l},                     // 0x5d
    {"LD E, (HL)", 0, ld_e_hlp},                // 0x5e
    {"LD E, A", 0, ld_e_a},                     // 0x5f
    {"LD H, B", 0, ld_h_b},                     // 0x60
    {"LD H, C", 0, ld_h_c},                     // 0x61
    {"LD H, D", 0, ld_h_d},                     // 0x62
    {"LD H, E", 0, ld_h_e},                     // 0x63
    {"LD H, H", 0, nop},                        // 0x64
    {"LD H, L", 0, ld_h_l},                     // 0x65
    {"LD H, (HL)", 0, ld_h_hlp},                // 0x66
    {"LD H, A", 0, ld_h_a},                     // 0x67
    {"LD L, B", 0, ld_l_b},                     // 0x68
    {"LD L, C", 0, ld_l_c},                     // 0x69
    {"LD L, D", 0, ld_l_d},                     // 0x6a
    {"LD L, E", 0, ld_l_e},                     // 0x6b
    {"LD L, H", 0, ld_l_h},                     // 0x6c
    {"LD L, L", 0, nop},                        // 0x6d
    {"LD L, (HL)", 0, ld_l_hlp},                // 0x6e
    {"LD L, A", 0, ld_l_a},                     // 0x6f
    {"LD (HL), B", 0, ld_hlp_b},                // 0x70
    {"LD (HL), C", 0, ld_hlp_c},                // 0x71
    {"LD (HL), D", 0, ld_hlp_d},                // 0x72
    {"LD (HL), E", 0, ld_hlp_e},                // 0x73
    {"LD (HL), H", 0, ld_hlp_h},                // 0x74
    {"LD (HL), L", 0, ld_hlp_l},                // 0x75
    {"HALT", 0, halt},                          // 0x76
    {"LD (HL), A", 0, ld_hlp_a},                // 0x77
    {"LD A, B", 0, ld_a_b},                     // 0x78
    {"LD A, C", 0, ld_a_c},                     // 0x79
    {"LD A, D", 0, ld_a_d},                     // 0x7a
    {"LD A, E", 0, ld_a_e},                     // 0x7b
    {"LD A, H", 0, ld_a_h},                     // 0x7c
    {"LD A, L", 0, ld_a_l},                     // 0x7d
    {"LD A, (HL)", 0, ld_a_hlp},                // 0x7e
    {"LD A, A", 0, nop},                        // 0x7f
    {"ADD A, B", 0, add_a_b},                   // 0x80
    {"ADD A, C", 0, add_a_c},                   // 0x81
    {"ADD A, D", 0, add_a_d},                   // 0x82
    {"ADD A, E", 0, add_a_e},                   // 0x83
    {"ADD A, H", 0, add_a_h},                   // 0x84
    {"ADD A, L", 0, add_a_l},                   // 0x85
    {"ADD A, (HL)", 0, add_a_hlp},              // 0x86
    {"ADD A", 0, add_a_a},                      // 0x87
    {"ADC B", 0, adc_b},                        // 0x88
    {"ADC C", 0, adc_c},                        // 0x89
    {"ADC D", 0, adc_d},                        // 0x8a
    {"ADC E", 0, adc_e},                        // 0x8b
    {"ADC H", 0, adc_h},                        // 0x8c
    {"ADC L", 0, adc_l},                        // 0x8d
    {"ADC (HL)", 0, adc_hlp},                   // 0x8e
    {"ADC A", 0, adc_a},                        // 0x8f
    {"SUB B", 0, sub_b},                        // 0x90
    {"SUB C", 0, sub_c},                        // 0x91
    {"SUB D", 0, sub_d},                        // 0x92
    {"SUB E", 0, sub_e},                        // 0x93
    {"SUB H", 0, sub_h},                        // 0x94
    {"SUB L", 0, sub_l},                        // 0x95
    {"SUB (HL)", 0, sub_hlp},                   // 0x96
    {"SUB A", 0, sub_a},                        // 0x97
    {"SBC B", 0, sbc_b},                        // 0x98
    {"SBC C", 0, sbc_c},                        // 0x99
    {"SBC D", 0, sbc_d},                        // 0x9a
    {"SBC E", 0, sbc_e},                        // 0x9b
    {"SBC H", 0, sbc_h},                        // 0x9c
    {"SBC L", 0, sbc_l},                        // 0x9d
    {"SBC (HL)", 0, sbc_hlp},                   // 0x9e
    {"SBC A", 0, sbc_a},                        // 0x9f
    {"AND B", 0, and_b},                        // 0xa0
    {"AND C", 0, and_c},                        // 0xa1
    {"AND D", 0, and_d},                        // 0xa2
    {"AND E", 0, and_e},                        // 0xa3
    {"AND H", 0, and_h},                        // 0xa4
    {"AND L", 0, and_l},                        // 0xa5
    {"AND (HL)", 0, and_hlp},                   // 0xa6
    {"AND A", 0, and_a},                        // 0xa7
    {"XOR B", 0, xor_b},                        // 0xa8
    {"XOR C", 0, xor_c},                        // 0xa9
    {"XOR D", 0, xor_d},                        // 0xaa
    {"XOR E", 0, xor_e},                        // 0xab
    {"XOR H", 0, xor_h},                        // 0xac
    {"XOR L", 0, xor_l},                        // 0xad
    {"XOR (HL)", 0, xor_hlp},                   // 0xae
    {"XOR A", 0, xor_a},                        // 0xaf
    {"OR B", 0, or_b},                          // 0xb0
    {"OR C", 0, or_c},                          // 0xb1
    {"OR D", 0, or_d},                          // 0xb2
    {"OR E", 0, or_e},                          // 0xb3
    {"OR H", 0, or_h},                          // 0xb4
    {"OR L", 0, or_l},                          // 0xb5
    {"OR (HL)", 0, or_hlp},                     // 0xb6
    {"OR A", 0, or_a},                          // 0xb7
    {"CP B", 0, cp_b},                          // 0xb8
    {"CP C", 0, cp_c},                          // 0xb9
    {"CP D", 0, cp_d},                          // 0xba
    {"CP E", 0, cp_e},                          // 0xbb
    {"CP H", 0, cp_h},                          // 0xbc
    {"CP L", 0, cp_l},                          // 0xbd
    {"CP (HL)", 0, cp_hlp},                     // 0xbe
    {"CP A", 0, cp_a},                          // 0xbf
    {"RET NZ", 0, ret_nz},                      // 0xc0
    {"POP BC", 0, pop_bc},                      // 0xc1
    {"JP NZ, 0x%04X", 2, jp_nz_nn},             // 0xc2
    {"JP 0x%04X", 2, jp_nn},                    // 0xc3
    {"CALL NZ, 0x%04X", 2, call_nz_nn},         // 0xc4
    {"PUSH BC", 0, push_bc},                    // 0xc5
    {"ADD A, 0x%02X", 1, add_a_n},              // 0xc6
    {"RST 0x00", 0, rst_0},                     // 0xc7
    {"RET Z", 0, ret_z},                        // 0xc8
    {"RET", 0, ret},                            // 0xc9
    {"JP Z, 0x%04X", 2, jp_z_nn},               // 0xca
    {"CB %02X", 1, cb_n},                       // 0xcb
    {"CALL Z, 0x%04X", 2, call_z_nn},           // 0xcc
    {"CALL 0x%04X", 2, call_nn},                // 0xcd
    {"ADC 0x%02X", 1, adc_n},                   // 0xce
    {"RST 0x08", 0, rst_08},                    // 0xcf
    {"RET NC", 0, ret_nc},                      // 0xd0
    {"POP DE", 0, pop_de},                      // 0xd1
    {"JP NC, 0x%04X", 2, jp_nc_nn},             // 0xd2
    {"UNKNOWN", 0, undefined},                  // 0xd3
    {"CALL NC, 0x%04X", 2, call_nc_nn},         // 0xd4
    {"PUSH DE", 0, push_de},                    // 0xd5
    {"SUB 0x%02X", 1, sub_n},                   // 0xd6
    {"RST 0x10", 0, rst_10},                    // 0xd7
    {"RET C", 0, ret_c},                        // 0xd8
    {"RETI", 0, returnFromInterrupt},           // 0xd9
    {"JP C, 0x%04X", 2, jp_c_nn},               // 0xda
    {"UNKNOWN", 0, undefined},                  // 0xdb
    {"CALL C, 0x%04X", 2, call_c_nn},           // 0xdc
    {"UNKNOWN", 0, undefined},                  // 0xdd
    {"SBC 0x%02X", 1, sbc_n},                   // 0xde
    {"RST 0x18", 0, rst_18},                    // 0xdf
    {"LD (0xFF00 + 0x%02X), A", 1, ld_ff_n_ap}, // 0xe0
    {"POP HL", 0, pop_hl},                      // 0xe1
    {"LD (0xFF00 + C), A", 0, ld_ff_c_a},       // 0xe2
    {"UNKNOWN", 0, undefined},                  // 0xe3
    {"UNKNOWN", 0, undefined},                  // 0xe4
    {"PUSH HL", 0, push_hl},                    // 0xe5
    {"AND 0x%02X", 1, and_n},                   // 0xe6
    {"RST 0x20", 0, rst_20},                    // 0xe7
    {"ADD SP,0x%02X", 1, add_sp_n},             // 0xe8
    {"JP HL", 0, jp_hl},                        // 0xe9
    {"LD (0x%04X), A", 2, ld_nnp_a},            // 0xea
    {"UNKNOWN", 0, undefined},                  // 0xeb
    {"UNKNOWN", 0, undefined},                  // 0xec
    {"UNKNOWN", 0, undefined},                  // 0xed
    {"XOR 0x%02X", 1, xor_n},                   // 0xee
    {"RST 0x28", 0, rst_28},                    // 0xef
    {"LD A, (0xFF00 + 0x%02X)", 1, ld_ff_ap_n}, // 0xf0
    {"POP AF", 0, pop_af},                      // 0xf1
    {"LD A, (0xFF00 + C)", 0, ld_a_ff_c},       // 0xf2
    {"DI", 0, di_inst},                         // 0xf3
    {"UNKNOWN", 0, undefined},                  // 0xf4
    {"PUSH AF", 0, push_af},                    // 0xf5
    {"OR 0x%02X", 1, or_n},                     // 0xf6
    {"RST 0x30", 0, rst_30},                    // 0xf7
    {"LD HL, SP+0x%02X", 1, ld_hl_sp_n},        // 0xf8
    {"LD SP, HL", 0, ld_sp_hl},                 // 0xf9
    {"LD A, (0x%04X)", 2, ld_a_nnp},            // 0xfa
    {"EI", 0, ei},                              // 0xfb
    {"UNKNOWN", 0, undefined},                  // 0xfc
    {"UNKNOWN", 0, undefined},                  // 0xfd
    {"CP 0x%02X", 1, cp_n},                     // 0xfe
    {"RST 0x38", 0, rst_38},                    // 0xff
};

const unsigned char instructionTicks[256] = {
    2, 6, 4, 4, 2, 2, 4, 4, 10, 4, 4, 4, 2, 2, 4, 4, // 0x0_
    2, 6, 4, 4, 2, 2, 4, 4, 4,  4, 4, 4, 2, 2, 4, 4, // 0x1_
    0, 6, 4, 4, 2, 2, 4, 2, 0,  4, 4, 4, 2, 2, 4, 2, // 0x2_
    4, 6, 4, 4, 6, 6, 6, 2, 0,  4, 4, 4, 2, 2, 4, 2, // 0x3_
    2, 2, 2, 2, 2, 2, 4, 2, 2,  2, 2, 2, 2, 2, 4, 2, // 0x4_
    2, 2, 2, 2, 2, 2, 4, 2, 2,  2, 2, 2, 2, 2, 4, 2, // 0x5_
    2, 2, 2, 2, 2, 2, 4, 2, 2,  2, 2, 2, 2, 2, 4, 2, // 0x6_
    4, 4, 4, 4, 4, 4, 2, 4, 2,  2, 2, 2, 2, 2, 4, 2, // 0x7_
    2, 2, 2, 2, 2, 2, 4, 2, 2,  2, 2, 2, 2, 2, 4, 2, // 0x8_
    2, 2, 2, 2, 2, 2, 4, 2, 2,  2, 2, 2, 2, 2, 4, 2, // 0x9_
    2, 2, 2, 2, 2, 2, 4, 2, 2,  2, 2, 2, 2, 2, 4, 2, // 0xa_
    2, 2, 2, 2, 2, 2, 4, 2, 2,  2, 2, 2, 2, 2, 4, 2, // 0xb_
    0, 6, 0, 6, 0, 8, 4, 8, 0,  2, 0, 0, 0, 6, 4, 8, // 0xc_
    0, 6, 0, 0, 0, 8, 4, 8, 0,  8, 0, 0, 0, 0, 4, 8, // 0xd_
    6, 6, 4, 0, 0, 8, 4, 8, 8,  2, 8, 0, 0, 0, 4, 8, // 0xe_
    6, 6, 4, 2, 0, 8, 4, 8, 6,  4, 8, 2, 0, 0, 4, 8  // 0xf_
};

void reset(gameboy_t *gb) {
  memset(gb, 0, sizeof(*gb));
  memset(gb->memory.sram, 0, sizeof(gb->memory.sram));
  memcpy(gb->memory.io, ioReset, sizeof(gb->memory.io));
  memset(gb->memory.vram, 0, sizeof(gb->memory.vram));
  memset(gb->memory.oam, 0, sizeof(gb->memory.oam));
  memset(gb->memory.wram, 0, sizeof(gb->memory.wram));
  memset(gb->memory.hram, 0, sizeof(gb->memory.hram));
  memset(gb->gpu.framebuffer, 255, sizeof(gb->gpu.framebuffer));

  gb->cpu.registers.a = 0x01;
  gb->cpu.registers.f = 0xb0;
  gb->cpu.registers.b = 0x00;
  gb->cpu.registers.c = 0x13;
  gb->cpu.registers.d = 0x00;
  gb->cpu.registers.e = 0xd8;
  gb->cpu.registers.h = 0x01;
  gb->cpu.registers.l = 0x4d;
  gb->cpu.registers.SP = 0xfffe;
  gb->cpu.registers.PC = 0x100;
  gb->time_to_redraw = 0;

  gb->cpu.interrupt.master = 1;
  gb->cpu.interrupt.enable = 0;
  gb->cpu.interrupt.flags = 0;
  gb->keys.KEY1.a = 1;
  gb->keys.KEY1.b = 1;
  gb->keys.KEY1.select = 1;
  gb->keys.KEY1.start = 1;
  gb->keys.KEY2.right = 1;
  gb->keys.KEY2.left = 1;
  gb->keys.KEY2.up = 1;
  gb->keys.KEY2.down = 1;

  memset(gb->gpu.tiles, 0, sizeof(gb->gpu.tiles));

  gb->gpu.palette[0].r = 255;
  gb->gpu.palette[0].g = 255;
  gb->gpu.palette[0].b = 255;
  gb->gpu.palette[1].r = 192;
  gb->gpu.palette[1].g = 192;
  gb->gpu.palette[1].b = 192;
  gb->gpu.palette[2].r = 96;
  gb->gpu.palette[2].g = 96;
  gb->gpu.palette[2].b = 96;
  gb->gpu.palette[3].r = 0;
  gb->gpu.palette[3].g = 0;
  gb->gpu.palette[3].b = 0;

  gb->gpu.backgroundPalette[0] = gb->gpu.palette[0];
  gb->gpu.backgroundPalette[1] = gb->gpu.palette[1];
  gb->gpu.backgroundPalette[2] = gb->gpu.palette[2];
  gb->gpu.backgroundPalette[3] = gb->gpu.palette[3];

  gb->gpu.spritePalette[0][0] = gb->gpu.palette[0];
  gb->gpu.spritePalette[0][1] = gb->gpu.palette[1];
  gb->gpu.spritePalette[0][2] = gb->gpu.palette[2];
  gb->gpu.spritePalette[0][3] = gb->gpu.palette[3];
  gb->gpu.spritePalette[1][0] = gb->gpu.palette[0];
  gb->gpu.spritePalette[1][1] = gb->gpu.palette[1];
  gb->gpu.spritePalette[1][2] = gb->gpu.palette[2];
  gb->gpu.spritePalette[1][3] = gb->gpu.palette[3];

  gb->gpu.control = 0;
  gb->gpu.scrollX = 0;
  gb->gpu.scrollY = 0;
  gb->gpu.tick = 0;
  gb->gpu.mode = 0;

  gb->cpu.ticks = 0;
  gb->cpu.stopped = 0;

  write_byte(gb, 0xFF05, 0);
  write_byte(gb, 0xFF06, 0);
  write_byte(gb, 0xFF07, 0);
  write_byte(gb, 0xFF10, 0x80);
  write_byte(gb, 0xFF11, 0xBF);
  write_byte(gb, 0xFF12, 0xF3);
  write_byte(gb, 0xFF14, 0xBF);
  write_byte(gb, 0xFF16, 0x3F);
  write_byte(gb, 0xFF17, 0x00);
  write_byte(gb, 0xFF19, 0xBF);
  write_byte(gb, 0xFF1A, 0x7A);
  write_byte(gb, 0xFF1B, 0xFF);
  write_byte(gb, 0xFF1C, 0x9F);
  write_byte(gb, 0xFF1E, 0xBF);
  write_byte(gb, 0xFF20, 0xFF);
  write_byte(gb, 0xFF21, 0x00);
  write_byte(gb, 0xFF22, 0x00);
  write_byte(gb, 0xFF23, 0xBF);
  write_byte(gb, 0xFF24, 0x77);
  write_byte(gb, 0xFF25, 0xF3);
  write_byte(gb, 0xFF26, 0xF1);
  write_byte(gb, 0xFF40, 0x91);
  write_byte(gb, 0xFF42, 0x00);
  write_byte(gb, 0xFF43, 0x00);
  write_byte(gb, 0xFF45, 0x00);
  write_byte(gb, 0xFF47, 0xFC);
  write_byte(gb, 0xFF48, 0xFF);
  write_byte(gb, 0xFF49, 0xFF);
  write_byte(gb, 0xFF4A, 0x00);
  write_byte(gb, 0xFF4B, 0x00);
  write_byte(gb, 0xFFFF, 0x00);
}

void cpuStep(gameboy_t *gb) {
  unsigned char instruction;
  unsigned short operand = 0;

  if (gb->cpu.stopped)
    return;

  // General breakpoints
  // if(registers.PC == 0x034c) { // incorrect load
  // if(registers.PC == 0x0309) { // start of function which writes to ff80
  // if(registers.PC == 0x2a02) { // closer to function call which writes to
  // ff80 if(registers.PC == 0x034c) { // function which writes to ffa6 timer

  // if(registers.PC == 0x036c) { // loop
  // if(registers.PC == 0x0040) { // vblank
  if (gb->cpu.registers.PC == 0x40) {
	  // in the vblank interrupt
  }

  // if(registers.PC == 0x29fa) { // input
  //	realtimeDebugEnable = 1;
  //}

  // if(realtimeDebugEnable) realtimeDebug();

  instruction = read_byte(gb, gb->cpu.registers.PC++);

  if (instructions[instruction].operandLength == 1)
    operand = (unsigned short)read_byte(gb, gb->cpu.registers.PC);
  if (instructions[instruction].operandLength == 2)
    operand = read_short(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC += instructions[instruction].operandLength;

  int disass = 0;
  if (disass) {
    if (instructions[instruction].operandLength)
      printf(instructions[instruction].disassembly, operand);
    else
      printf(instructions[instruction].disassembly);
    printf("\n");
  }

  switch (instructions[instruction].operandLength) {
  case 0:
    ((void (*)(gameboy_t *))instructions[instruction].execute)(gb);
    break;

  case 1:
    ((void (*)(gameboy_t *, unsigned char))instructions[instruction].execute)(
        gb, (unsigned char)operand);
    break;

  case 2:
    ((void (*)(gameboy_t *, unsigned short))instructions[instruction].execute)(
        gb, operand);
    break;
  }

  gb->cpu.ticks += instructionTicks[instruction];
}

void undefined(gameboy_t *gb) {
  gb->cpu.registers.PC--;

  unsigned char instruction = read_byte(gb, gb->cpu.registers.PC);

  printf("Undefined instruction 0x%02x!\n", instruction);

  // printRegisters();
  abort();
}

static unsigned char inc(gameboy_t *gb, unsigned char value) {
  if ((value & 0x0f) == 0x0f)
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);

  value++;

  if (value)
    FLAGS_CLEAR(FLAGS_ZERO);
  else
    FLAGS_SET(FLAGS_ZERO);

  FLAGS_CLEAR(FLAGS_NEGATIVE);

  return value;
}

static unsigned char dec(gameboy_t *gb, unsigned char value) {
  if (value & 0x0f)
    FLAGS_CLEAR(FLAGS_HALFCARRY);
  else
    FLAGS_SET(FLAGS_HALFCARRY);

  value--;

  if (value)
    FLAGS_CLEAR(FLAGS_ZERO);
  else
    FLAGS_SET(FLAGS_ZERO);

  FLAGS_SET(FLAGS_NEGATIVE);

  return value;
}

static void add(gameboy_t *gb, unsigned char *destination,
                unsigned char value) {
  unsigned int result = *destination + value;

  if (result & 0xff00)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  *destination = (unsigned char)(result & 0xff);

  if (*destination)
    FLAGS_CLEAR(FLAGS_ZERO);
  else
    FLAGS_SET(FLAGS_ZERO);

  if (((*destination & 0x0f) + (value & 0x0f)) > 0x0f)
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);

  FLAGS_CLEAR(FLAGS_NEGATIVE);
}

static void add2(gameboy_t *gb, unsigned short *destination,
                 unsigned short value) {
  unsigned long result = *destination + value;

  if (result & 0xffff0000)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  *destination = (unsigned short)(result & 0xffff);

  if (((*destination & 0x0f) + (value & 0x0f)) > 0x0f)
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);

  // zero flag left alone

  FLAGS_CLEAR(FLAGS_NEGATIVE);
}

static void adc(gameboy_t *gb, unsigned char value) {
  value += FLAGS_ISCARRY ? 1 : 0;

  int result = gb->cpu.registers.a + value;

  if (result & 0xff00)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  if (value == gb->cpu.registers.a)
    FLAGS_SET(FLAGS_ZERO);
  else
    FLAGS_CLEAR(FLAGS_ZERO);

  if (((value & 0x0f) + (gb->cpu.registers.a & 0x0f)) > 0x0f)
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);

  FLAGS_SET(FLAGS_NEGATIVE);

  gb->cpu.registers.a = (unsigned char)(result & 0xff);
}

static void sbc(gameboy_t *gb, unsigned char value) {
  value += FLAGS_ISCARRY ? 1 : 0;

  FLAGS_SET(FLAGS_NEGATIVE);

  if (value > gb->cpu.registers.a)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  if (value == gb->cpu.registers.a)
    FLAGS_SET(FLAGS_ZERO);
  else
    FLAGS_CLEAR(FLAGS_ZERO);

  if ((value & 0x0f) > (gb->cpu.registers.a & 0x0f))
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);

  gb->cpu.registers.a -= value;
}

static void sub(gameboy_t *gb, unsigned char value) {
  FLAGS_SET(FLAGS_NEGATIVE);

  if (value > gb->cpu.registers.a)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  if ((value & 0x0f) > (gb->cpu.registers.a & 0x0f))
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);

  gb->cpu.registers.a -= value;

  if (gb->cpu.registers.a)
    FLAGS_CLEAR(FLAGS_ZERO);
  else
    FLAGS_SET(FLAGS_ZERO);
}

static void and (gameboy_t * gb, unsigned char value) {
  gb->cpu.registers.a &= value;

  if (gb->cpu.registers.a)
    FLAGS_CLEAR(FLAGS_ZERO);
  else
    FLAGS_SET(FLAGS_ZERO);

  FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE);
  FLAGS_SET(FLAGS_HALFCARRY);
}

static void or (gameboy_t * gb, unsigned char value) {
  gb->cpu.registers.a |= value;

  if (gb->cpu.registers.a)
    FLAGS_CLEAR(FLAGS_ZERO);
  else
    FLAGS_SET(FLAGS_ZERO);

  FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

static void xor
    (gameboy_t * gb, unsigned char value) {
      gb->cpu.registers.a ^= value;

      if (gb->cpu.registers.a)
        FLAGS_CLEAR(FLAGS_ZERO);
      else
        FLAGS_SET(FLAGS_ZERO);

      FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
    }

    static void cp(gameboy_t *gb, unsigned char value) {
  if (gb->cpu.registers.a == value)
    FLAGS_SET(FLAGS_ZERO);
  else
    FLAGS_CLEAR(FLAGS_ZERO);

  if (value > gb->cpu.registers.a)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  if ((value & 0x0f) > (gb->cpu.registers.a & 0x0f))
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);

  FLAGS_SET(FLAGS_NEGATIVE);
}

// 0x00
void nop(gameboy_t *gb) {}

// 0x01
void ld_bc_nn(gameboy_t *gb, unsigned short operand) {
  gb->cpu.registers.BC = operand;
}

// 0x02
void ld_bcp_a(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.BC, gb->cpu.registers.a);
}

// 0x03
void inc_bc(gameboy_t *gb) { gb->cpu.registers.BC++; }

// 0x04
void inc_b(gameboy_t *gb) {
  gb->cpu.registers.b = inc(gb, gb->cpu.registers.b);
}

// 0x05
void dec_b(gameboy_t *gb) {
  gb->cpu.registers.b = dec(gb, gb->cpu.registers.b);
}

// 0x06
void ld_b_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.b = operand;
}

// 0x07
void rlca(gameboy_t *gb) {
  unsigned char carry = (gb->cpu.registers.a & 0x80) >> 7;
  if (carry)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  gb->cpu.registers.a <<= 1;
  gb->cpu.registers.a += carry;

  FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

// 0x08
void ld_nnp_sp(gameboy_t *gb, unsigned short operand) {
  write_short(gb, operand, gb->cpu.registers.SP);
}

// 0x09
void add_hl_bc(gameboy_t *gb) {
  add2(gb, &gb->cpu.registers.HL, gb->cpu.registers.BC);
}

// 0x0a
void ld_a_bcp(gameboy_t *gb) {
  gb->cpu.registers.a = read_byte(gb, gb->cpu.registers.BC);
}

// 0x0b
void dec_bc(gameboy_t *gb) { gb->cpu.registers.BC--; }

// 0x0c
void inc_c(gameboy_t *gb) {
  gb->cpu.registers.c = inc(gb, gb->cpu.registers.c);
}

// 0x0d
void dec_c(gameboy_t *gb) {
  gb->cpu.registers.c = dec(gb, gb->cpu.registers.c);
}

// 0x0e
void ld_c_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.c = operand;
}

// 0x0f
void rrca(gameboy_t *gb) {
  unsigned char carry = gb->cpu.registers.a & 0x01;
  if (carry)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  gb->cpu.registers.a >>= 1;
  if (carry)
    gb->cpu.registers.a |= 0x80;

  FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

// 0x10
void stop(gameboy_t *gb, unsigned char operand) { gb->cpu.stopped = 1; }

// 0x11
void ld_de_nn(gameboy_t *gb, unsigned short operand) {
  gb->cpu.registers.DE = operand;
}

// 0x12
void ld_dep_a(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.DE, gb->cpu.registers.a);
}

// 0x13
void inc_de(gameboy_t *gb) { gb->cpu.registers.DE++; }

// 0x14
void inc_d(gameboy_t *gb) {
  gb->cpu.registers.d = inc(gb, gb->cpu.registers.d);
}

// 0x15
void dec_d(gameboy_t *gb) {
  gb->cpu.registers.d = dec(gb, gb->cpu.registers.d);
}

// 0x16
void ld_d_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.d = operand;
}

// 0x17
void rla(gameboy_t *gb) {
  int carry = FLAGS_ISSET(FLAGS_CARRY) ? 1 : 0;

  if (gb->cpu.registers.a & 0x80)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  gb->cpu.registers.a <<= 1;
  gb->cpu.registers.a += carry;

  FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

// 0x18
void jr_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.PC += (signed char)operand;
  // debugJump();
}

// 0x19
void add_hl_de(gameboy_t *gb) {
  add2(gb, &gb->cpu.registers.HL, gb->cpu.registers.DE);
}

// 0x1a
void ld_a_dep(gameboy_t *gb) {
  gb->cpu.registers.a = read_byte(gb, gb->cpu.registers.DE);
}

// 0x1b
void dec_de(gameboy_t *gb) { gb->cpu.registers.DE--; }

// 0x1c
void inc_e(gameboy_t *gb) {
  gb->cpu.registers.e = inc(gb, gb->cpu.registers.e);
}

// 0x1d
void dec_e(gameboy_t *gb) {
  gb->cpu.registers.e = dec(gb, gb->cpu.registers.e);
}

// 0x1e
void ld_e_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.e = operand;
}

// 0x1f
void rra(gameboy_t *gb) {
  int carry = (FLAGS_ISSET(FLAGS_CARRY) ? 1 : 0) << 7;

  if (gb->cpu.registers.a & 0x01)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  gb->cpu.registers.a >>= 1;
  gb->cpu.registers.a += carry;

  FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_ZERO | FLAGS_HALFCARRY);
}

// 0x20
void jr_nz_n(gameboy_t *gb, unsigned char operand) {
  if (FLAGS_ISZERO)
    gb->cpu.ticks += 8;
  else {
    gb->cpu.registers.PC += (signed char)operand;

    // debugJump();
    gb->cpu.ticks += 12;
  }
}

// 0x21
void ld_hl_nn(gameboy_t *gb, unsigned short operand) {
  gb->cpu.registers.HL = operand;
}

// 0x22
void ldi_hlp_a(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL++, gb->cpu.registers.a);
}

// 0x23
void inc_hl(gameboy_t *gb) { gb->cpu.registers.HL++; }

// 0x24
void inc_h(gameboy_t *gb) {
  gb->cpu.registers.h = inc(gb, gb->cpu.registers.h);
}

// 0x25
void dec_h(gameboy_t *gb) {
  gb->cpu.registers.h = dec(gb, gb->cpu.registers.h);
}

// 0x26
void ld_h_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.h = operand;
}

// 0x27
void daa(gameboy_t *gb) {
  /*unsigned int reg_one = gb->cpu.registers.a;

  //Add or subtract correction values based on Subtract Flag
  if(!FLAGS_ISNEGATIVE) {
          if(FLAGS_ISHALFCARRY || ((reg_one & 0xF) > 0x09)) reg_one += 0x06;
          if(FLAGS_ISCARRY || (reg_one > 0x9F)) reg_one += 0x60;
  }
  else  {
          if(FLAGS_ISHALFCARRY) reg_one = (reg_one - 0x06) & 0xFF;
          if(FLAGS_ISCARRY) reg_one -= 0x60;
  }

  //Carry
  if(reg_one & 0x100) FLAGS_SET(FLAGS_CARRY);
  reg_one &= 0xFF;

  //Half-Carry
  FLAGS_CLEAR(FLAGS_HALFCARRY);

  //Zero
  if(reg_one == 0) FLAGS_SET(FLAGS_ZERO);
  else FLAGS_CLEAR(FLAGS_ZERO);

  gb->cpu.registers.a = (unsigned char)reg_one;*/

  /*
  {
          unsigned int a = gb->cpu.registers.a;

          if(FLAGS_ISHALFCARRY || ((gb->cpu.registers.a & 15) > 9))
  gb->cpu.registers.a += 6; FLAGS_CLEAR(FLAGS_CARRY);

          if(FLAGS_ISHALFCARRY || a > 0x99) {
                  gb->cpu.registers.a += 0x60;
                  FLAGS_SET(FLAGS_CARRY);
          }

          if(gb->cpu.registers.a) FLAGS_CLEAR(FLAGS_ZERO);
          else FLAGS_SET(FLAGS_ZERO);
  }*/

  {
    unsigned short s = gb->cpu.registers.a;

    if (FLAGS_ISNEGATIVE) {
      if (FLAGS_ISHALFCARRY)
        s = (s - 0x06) & 0xFF;
      if (FLAGS_ISCARRY)
        s -= 0x60;
    } else {
      if (FLAGS_ISHALFCARRY || (s & 0xF) > 9)
        s += 0x06;
      if (FLAGS_ISCARRY || s > 0x9F)
        s += 0x60;
    }

    gb->cpu.registers.a = s;
    FLAGS_CLEAR(FLAGS_HALFCARRY);

    if (gb->cpu.registers.a)
      FLAGS_CLEAR(FLAGS_ZERO);
    else
      FLAGS_SET(FLAGS_ZERO);

    if (s >= 0x100)
      FLAGS_SET(FLAGS_CARRY);
  }

  /*


  {
          unsigned int a = gb->cpu.registers.a;

          unsigned int correction = FLAGS_ISCARRY ? 0x60 : 0x00;

          if(FLAGS_ISHALFCARRY) correction |= 0x06;

          if(!FLAGS_ISNEGATIVE) {
                  if ((a & 0x0F) > 0x09)
                          correction |= 0x06;
                  if (a > 0x99)
                          correction |= 0x60;

                  a += correction;
          }
          else a -= correction;

          if(correction << 2 & 0x100) FLAGS_SET(FLAGS_CARRY);
          else FLAGS_CLEAR(FLAGS_CARRY);

          if(a == 0) FLAGS_SET(FLAGS_ZERO);
          else FLAGS_CLEAR(FLAGS_ZERO);

          a &= 0xFF;

          gb->cpu.registers.a = a;
  }

  */
}

// 0x28
void jr_z_n(gameboy_t *gb, unsigned char operand) {
  if (FLAGS_ISZERO) {
    gb->cpu.registers.PC += (signed char)operand;
    // debugJump();
    gb->cpu.ticks += 12;
  } else
    gb->cpu.ticks += 8;
}

// 0x29
void add_hl_hl(gameboy_t *gb) {
  add2(gb, &gb->cpu.registers.HL, gb->cpu.registers.HL);
}

// 0x2a
void ldi_a_hlp(gameboy_t *gb) {
  gb->cpu.registers.a = read_byte(gb, gb->cpu.registers.HL++);
}

// 0x2b
void dec_hl(gameboy_t *gb) { gb->cpu.registers.HL--; }

// 0x2c
void inc_l(gameboy_t *gb) {
  gb->cpu.registers.l = inc(gb, gb->cpu.registers.l);
}

// 0x2d
void dec_l(gameboy_t *gb) {
  gb->cpu.registers.l = dec(gb, gb->cpu.registers.l);
}

// 0x2e
void ld_l_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.l = operand;
}

// 0x2f
void cpl(gameboy_t *gb) {
  gb->cpu.registers.a = ~gb->cpu.registers.a;
  FLAGS_SET(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

// 0x30
void jr_nc_n(gameboy_t *gb, char operand) {
  if (FLAGS_ISCARRY)
    gb->cpu.ticks += 8;
  else {
    gb->cpu.registers.PC += operand;
    // debugJump();
    gb->cpu.ticks += 12;
  }
}

// 0x31
void ld_sp_nn(gameboy_t *gb, unsigned short operand) {
  gb->cpu.registers.SP = operand;
}

// 0x32
void ldd_hlp_a(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL, gb->cpu.registers.a);
  gb->cpu.registers.HL--;
}

// 0x33
void inc_sp(gameboy_t *gb) { gb->cpu.registers.SP++; }

// 0x34
void inc_hlp(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL,
             inc(gb, read_byte(gb, gb->cpu.registers.HL)));
}

// 0x35
void dec_hlp(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL,
             dec(gb, read_byte(gb, gb->cpu.registers.HL)));
}

// 0x36
void ld_hlp_n(gameboy_t *gb, unsigned char operand) {
  write_byte(gb, gb->cpu.registers.HL, operand);
}

// 0x37
void scf(gameboy_t *gb) {
  FLAGS_SET(FLAGS_CARRY);
  FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

// 0x38
void jr_c_n(gameboy_t *gb, char operand) {
  if (FLAGS_ISCARRY) {
    gb->cpu.registers.PC += operand;
    gb->cpu.ticks += 12;
  } else
    gb->cpu.ticks += 8;
}

// 0x39
void add_hl_sp(gameboy_t *gb) {
  add2(gb, &gb->cpu.registers.HL, gb->cpu.registers.SP);
}

// 0x3a
void ldd_a_hlp(gameboy_t *gb) {
  gb->cpu.registers.a = read_byte(gb, gb->cpu.registers.HL--);
}

// 0x3b
void dec_sp(gameboy_t *gb) { gb->cpu.registers.SP--; }

// 0x3c
void inc_a(gameboy_t *gb) {
  gb->cpu.registers.a = inc(gb, gb->cpu.registers.a);
}

// 0x3d
void dec_a(gameboy_t *gb) {
  gb->cpu.registers.a = dec(gb, gb->cpu.registers.a);
}

// 0x3e
void ld_a_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.a = operand;
}

// 0x3f
void ccf(gameboy_t *gb) {
  if (FLAGS_ISCARRY)
    FLAGS_CLEAR(FLAGS_CARRY);
  else
    FLAGS_SET(FLAGS_CARRY);

  FLAGS_CLEAR(FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

// 0x41
void ld_b_c(gameboy_t *gb) { gb->cpu.registers.b = gb->cpu.registers.c; }

// 0x42
void ld_b_d(gameboy_t *gb) { gb->cpu.registers.b = gb->cpu.registers.d; }

// 0x43
void ld_b_e(gameboy_t *gb) { gb->cpu.registers.b = gb->cpu.registers.e; }

// 0x44
void ld_b_h(gameboy_t *gb) { gb->cpu.registers.b = gb->cpu.registers.h; }

// 0x45
void ld_b_l(gameboy_t *gb) { gb->cpu.registers.b = gb->cpu.registers.l; }

// 0x46
void ld_b_hlp(gameboy_t *gb) {
  gb->cpu.registers.b = read_byte(gb, gb->cpu.registers.HL);
}

// 0x47
void ld_b_a(gameboy_t *gb) { gb->cpu.registers.b = gb->cpu.registers.a; }

// 0x48
void ld_c_b(gameboy_t *gb) { gb->cpu.registers.c = gb->cpu.registers.b; }

// 0x4a
void ld_c_d(gameboy_t *gb) { gb->cpu.registers.c = gb->cpu.registers.d; }

// 0x4b
void ld_c_e(gameboy_t *gb) { gb->cpu.registers.c = gb->cpu.registers.e; }

// 0x4c
void ld_c_h(gameboy_t *gb) { gb->cpu.registers.c = gb->cpu.registers.h; }

// 0x4d
void ld_c_l(gameboy_t *gb) { gb->cpu.registers.c = gb->cpu.registers.l; }

// 0x4e
void ld_c_hlp(gameboy_t *gb) {
  gb->cpu.registers.c = read_byte(gb, gb->cpu.registers.HL);
}

// 0x4f
void ld_c_a(gameboy_t *gb) { gb->cpu.registers.c = gb->cpu.registers.a; }

// 0x50
void ld_d_b(gameboy_t *gb) { gb->cpu.registers.d = gb->cpu.registers.b; }

// 0x51
void ld_d_c(gameboy_t *gb) { gb->cpu.registers.d = gb->cpu.registers.c; }

// 0x53
void ld_d_e(gameboy_t *gb) { gb->cpu.registers.d = gb->cpu.registers.e; }

// 0x54
void ld_d_h(gameboy_t *gb) { gb->cpu.registers.d = gb->cpu.registers.h; }

// 0x55
void ld_d_l(gameboy_t *gb) { gb->cpu.registers.d = gb->cpu.registers.l; }

// 0x56
void ld_d_hlp(gameboy_t *gb) {
  gb->cpu.registers.d = read_byte(gb, gb->cpu.registers.HL);
}

// 0x57
void ld_d_a(gameboy_t *gb) { gb->cpu.registers.d = gb->cpu.registers.a; }

// 0x58
void ld_e_b(gameboy_t *gb) { gb->cpu.registers.e = gb->cpu.registers.b; }

// 0x59
void ld_e_c(gameboy_t *gb) { gb->cpu.registers.e = gb->cpu.registers.c; }

// 0x5a
void ld_e_d(gameboy_t *gb) { gb->cpu.registers.e = gb->cpu.registers.d; }

// 0x5c
void ld_e_h(gameboy_t *gb) { gb->cpu.registers.e = gb->cpu.registers.h; }

// 0x5d
void ld_e_l(gameboy_t *gb) { gb->cpu.registers.e = gb->cpu.registers.l; }

// 0x5e
void ld_e_hlp(gameboy_t *gb) {
  gb->cpu.registers.e = read_byte(gb, gb->cpu.registers.HL);
}

// 0x5f
void ld_e_a(gameboy_t *gb) { gb->cpu.registers.e = gb->cpu.registers.a; }

// 0x60
void ld_h_b(gameboy_t *gb) { gb->cpu.registers.h = gb->cpu.registers.b; }

// 0x61
void ld_h_c(gameboy_t *gb) { gb->cpu.registers.h = gb->cpu.registers.c; }

// 0x62
void ld_h_d(gameboy_t *gb) { gb->cpu.registers.h = gb->cpu.registers.d; }

// 0x63
void ld_h_e(gameboy_t *gb) { gb->cpu.registers.h = gb->cpu.registers.e; }

// 0x65
void ld_h_l(gameboy_t *gb) { gb->cpu.registers.h = gb->cpu.registers.l; }

// 0x66
void ld_h_hlp(gameboy_t *gb) {
  gb->cpu.registers.h = read_byte(gb, gb->cpu.registers.HL);
}

// 0x67
void ld_h_a(gameboy_t *gb) { gb->cpu.registers.h = gb->cpu.registers.a; }

// 0x68
void ld_l_b(gameboy_t *gb) { gb->cpu.registers.l = gb->cpu.registers.b; }

// 0x69
void ld_l_c(gameboy_t *gb) { gb->cpu.registers.l = gb->cpu.registers.c; }

// 0x6a
void ld_l_d(gameboy_t *gb) { gb->cpu.registers.l = gb->cpu.registers.d; }

// 0x6b
void ld_l_e(gameboy_t *gb) { gb->cpu.registers.l = gb->cpu.registers.e; }

// 0x6c
void ld_l_h(gameboy_t *gb) { gb->cpu.registers.l = gb->cpu.registers.h; }

// 0x6e
void ld_l_hlp(gameboy_t *gb) {
  gb->cpu.registers.l = read_byte(gb, gb->cpu.registers.HL);
}

// 0x6f
void ld_l_a(gameboy_t *gb) { gb->cpu.registers.l = gb->cpu.registers.a; }

// 0x70
void ld_hlp_b(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL, gb->cpu.registers.b);
}

// 0x71
void ld_hlp_c(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL, gb->cpu.registers.c);
}

// 0x72
void ld_hlp_d(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL, gb->cpu.registers.d);
}

// 0x73
void ld_hlp_e(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL, gb->cpu.registers.e);
}

// 0x74
void ld_hlp_h(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL, gb->cpu.registers.h);
}

// 0x75
void ld_hlp_l(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL, gb->cpu.registers.l);
}

// 0x76
void halt(gameboy_t *gb) {
  if (gb->cpu.interrupt.master) {
    // HALT EXECUTION UNTIL AN INTERRUPT OCCURS
  } else
    gb->cpu.registers.PC++;
}

// 0x77
void ld_hlp_a(gameboy_t *gb) {
  write_byte(gb, gb->cpu.registers.HL, gb->cpu.registers.a);
}

// 0x78
void ld_a_b(gameboy_t *gb) { gb->cpu.registers.a = gb->cpu.registers.b; }

// 0x79
void ld_a_c(gameboy_t *gb) { gb->cpu.registers.a = gb->cpu.registers.c; }

// 0x7a
void ld_a_d(gameboy_t *gb) { gb->cpu.registers.a = gb->cpu.registers.d; }

// 0x7b
void ld_a_e(gameboy_t *gb) { gb->cpu.registers.a = gb->cpu.registers.e; }

// 0x7c
void ld_a_h(gameboy_t *gb) { gb->cpu.registers.a = gb->cpu.registers.h; }

// 0x7d
void ld_a_l(gameboy_t *gb) { gb->cpu.registers.a = gb->cpu.registers.l; }

// 0x7e
void ld_a_hlp(gameboy_t *gb) {
  gb->cpu.registers.a = read_byte(gb, gb->cpu.registers.HL);
}

// 0x80
void add_a_b(gameboy_t *gb) {
  add(gb, &gb->cpu.registers.a, gb->cpu.registers.b);
}

// 0x81
void add_a_c(gameboy_t *gb) {
  add(gb, &gb->cpu.registers.a, gb->cpu.registers.c);
}

// 0x82
void add_a_d(gameboy_t *gb) {
  add(gb, &gb->cpu.registers.a, gb->cpu.registers.d);
}

// 0x83
void add_a_e(gameboy_t *gb) {
  add(gb, &gb->cpu.registers.a, gb->cpu.registers.e);
}

// 0x84
void add_a_h(gameboy_t *gb) {
  add(gb, &gb->cpu.registers.a, gb->cpu.registers.h);
}

// 0x85
void add_a_l(gameboy_t *gb) {
  add(gb, &gb->cpu.registers.a, gb->cpu.registers.l);
}

// 0x86
void add_a_hlp(gameboy_t *gb) {
  add(gb, &gb->cpu.registers.a, read_byte(gb, gb->cpu.registers.HL));
}

// 0x87
void add_a_a(gameboy_t *gb) {
  add(gb, &gb->cpu.registers.a, gb->cpu.registers.a);
}

// 0x88
void adc_b(gameboy_t *gb) { adc(gb, gb->cpu.registers.b); }

// 0x89
void adc_c(gameboy_t *gb) { adc(gb, gb->cpu.registers.c); }

// 0x8a
void adc_d(gameboy_t *gb) { adc(gb, gb->cpu.registers.d); }

// 0x8b
void adc_e(gameboy_t *gb) { adc(gb, gb->cpu.registers.e); }

// 0x8c
void adc_h(gameboy_t *gb) { adc(gb, gb->cpu.registers.h); }

// 0x8d
void adc_l(gameboy_t *gb) { adc(gb, gb->cpu.registers.l); }

// 0x8e
void adc_hlp(gameboy_t *gb) { adc(gb, read_byte(gb, gb->cpu.registers.HL)); }

// 0x8f
void adc_a(gameboy_t *gb) { adc(gb, gb->cpu.registers.a); }

// 0x90
void sub_b(gameboy_t *gb) { sub(gb, gb->cpu.registers.b); }

// 0x91
void sub_c(gameboy_t *gb) { sub(gb, gb->cpu.registers.c); }

// 0x92
void sub_d(gameboy_t *gb) { sub(gb, gb->cpu.registers.d); }

// 0x93
void sub_e(gameboy_t *gb) { sub(gb, gb->cpu.registers.e); }

// 0x94
void sub_h(gameboy_t *gb) { sub(gb, gb->cpu.registers.h); }

// 0x95
void sub_l(gameboy_t *gb) { sub(gb, gb->cpu.registers.l); }

// 0x96
void sub_hlp(gameboy_t *gb) { sub(gb, read_byte(gb, gb->cpu.registers.HL)); }

// 0x97
void sub_a(gameboy_t *gb) { sub(gb, gb->cpu.registers.a); }

// 0x98
void sbc_b(gameboy_t *gb) { sbc(gb, gb->cpu.registers.b); }

// 0x99
void sbc_c(gameboy_t *gb) { sbc(gb, gb->cpu.registers.c); }

// 0x9a
void sbc_d(gameboy_t *gb) { sbc(gb, gb->cpu.registers.d); }

// 0x9b
void sbc_e(gameboy_t *gb) { sbc(gb, gb->cpu.registers.e); }

// 0x9c
void sbc_h(gameboy_t *gb) { sbc(gb, gb->cpu.registers.h); }

// 0x9d
void sbc_l(gameboy_t *gb) { sbc(gb, gb->cpu.registers.l); }

// 0x9e
void sbc_hlp(gameboy_t *gb) { sbc(gb, read_byte(gb, gb->cpu.registers.HL)); }

// 0x9f
void sbc_a(gameboy_t *gb) { sbc(gb, gb->cpu.registers.a); }

// 0xa0
void and_b(gameboy_t *gb) { and(gb, gb->cpu.registers.b); }

// 0xa1
void and_c(gameboy_t *gb) { and(gb, gb->cpu.registers.c); }

// 0xa2
void and_d(gameboy_t *gb) { and(gb, gb->cpu.registers.d); }

// 0xa3
void and_e(gameboy_t *gb) { and(gb, gb->cpu.registers.e); }

// 0xa4
void and_h(gameboy_t *gb) { and(gb, gb->cpu.registers.h); }

// 0xa5
void and_l(gameboy_t *gb) { and(gb, gb->cpu.registers.l); }

// 0xa6
void and_hlp(gameboy_t *gb) { and(gb, read_byte(gb, gb->cpu.registers.HL)); }

// 0xa7
void and_a(gameboy_t *gb) { and(gb, gb->cpu.registers.a); }

// 0xa8
void xor_b(gameboy_t *gb) { xor(gb, gb->cpu.registers.b); }

// 0xa9
void xor_c(gameboy_t *gb) { xor(gb, gb->cpu.registers.c); }

// 0xaa
void xor_d(gameboy_t *gb) { xor(gb, gb->cpu.registers.d); }

// 0xab
void xor_e(gameboy_t *gb) { xor(gb, gb->cpu.registers.e); }

// 0xac
void xor_h(gameboy_t *gb) { xor(gb, gb->cpu.registers.h); }

// 0xad
void xor_l(gameboy_t *gb) { xor(gb, gb->cpu.registers.l); }

// 0xae
void xor_hlp(gameboy_t *gb) { xor(gb, read_byte(gb, gb->cpu.registers.HL)); }

// 0xaf
void xor_a(gameboy_t *gb) { xor(gb, gb->cpu.registers.a); }

// 0xb0
void or_b(gameboy_t *gb) { or (gb, gb->cpu.registers.b); }

// 0xb1
void or_c(gameboy_t *gb) { or (gb, gb->cpu.registers.c); }

// 0xb2
void or_d(gameboy_t *gb) { or (gb, gb->cpu.registers.d); }

// 0xb3
void or_e(gameboy_t *gb) { or (gb, gb->cpu.registers.e); }

// 0xb4
void or_h(gameboy_t *gb) { or (gb, gb->cpu.registers.h); }

// 0xb5
void or_l(gameboy_t *gb) { or (gb, gb->cpu.registers.l); }

// 0xb6
void or_hlp(gameboy_t *gb) { or (gb, read_byte(gb, gb->cpu.registers.HL)); }

// 0xb7
void or_a(gameboy_t *gb) { or (gb, gb->cpu.registers.a); }

// 0xb8
void cp_b(gameboy_t *gb) { cp(gb, gb->cpu.registers.b); }

// 0xb9
void cp_c(gameboy_t *gb) { cp(gb, gb->cpu.registers.c); }

// 0xba
void cp_d(gameboy_t *gb) { cp(gb, gb->cpu.registers.d); }

// 0xbb
void cp_e(gameboy_t *gb) { cp(gb, gb->cpu.registers.e); }

// 0xbc
void cp_h(gameboy_t *gb) { cp(gb, gb->cpu.registers.h); }

// 0xbd
void cp_l(gameboy_t *gb) { cp(gb, gb->cpu.registers.l); }

// 0xbe
void cp_hlp(gameboy_t *gb) { cp(gb, read_byte(gb, gb->cpu.registers.HL)); }

// 0xbf
void cp_a(gameboy_t *gb) { cp(gb, gb->cpu.registers.a); }

// 0xc0
void ret_nz(gameboy_t *gb) {
  if (FLAGS_ISZERO)
    gb->cpu.ticks += 8;
  else {
    gb->cpu.registers.PC = read_short_from_stack(gb);
    // debugJump();
    gb->cpu.ticks += 20;
  }
}

// 0xc1
void pop_bc(gameboy_t *gb) { gb->cpu.registers.BC = read_short_from_stack(gb); }

// 0xc2
void jp_nz_nn(gameboy_t *gb, unsigned short operand) {
  if (FLAGS_ISZERO)
    gb->cpu.ticks += 12;
  else {
    gb->cpu.registers.PC = operand;
    // debugJump();
    gb->cpu.ticks += 16;
  }
}

// 0xc3
void jp_nn(gameboy_t *gb, unsigned short operand) {
  gb->cpu.registers.PC = operand;
  // debugJump();
}

// 0xc4
void call_nz_nn(gameboy_t *gb, unsigned short operand) {
  if (FLAGS_ISZERO)
    gb->cpu.ticks += 12;
  else {
    write_short_to_stack(gb, gb->cpu.registers.PC);
    gb->cpu.registers.PC = operand;
    // debugJump();
    gb->cpu.ticks += 24;
  }
}

// 0xc5
void push_bc(gameboy_t *gb) { write_short_to_stack(gb, gb->cpu.registers.BC); }

// 0xc6
void add_a_n(gameboy_t *gb, unsigned char operand) {
  add(gb, &gb->cpu.registers.a, operand);
}

// 0xc7
void rst_0(gameboy_t *gb) {
  write_short_to_stack(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC = 0x0000;
}

// 0xc8
void ret_z(gameboy_t *gb) {
  if (FLAGS_ISZERO) {
    gb->cpu.registers.PC = read_short_from_stack(gb);
    gb->cpu.ticks += 20;
  } else
    gb->cpu.ticks += 8;
}

// 0xc9
void ret(gameboy_t *gb) { gb->cpu.registers.PC = read_short_from_stack(gb); }

// 0xca
void jp_z_nn(gameboy_t *gb, unsigned short operand) {
  if (FLAGS_ISZERO) {
    gb->cpu.registers.PC = operand;
    // debugJump();
    gb->cpu.ticks += 16;
  } else
    gb->cpu.ticks += 12;
}

// 0xcb
// cb.c

// 0xcc
void call_z_nn(gameboy_t *gb, unsigned short operand) {
  if (FLAGS_ISZERO) {
    write_short_to_stack(gb, gb->cpu.registers.PC);
    gb->cpu.registers.PC = operand;
    gb->cpu.ticks += 24;
  } else
    gb->cpu.ticks += 12;
}

// 0xcd
void call_nn(gameboy_t *gb, unsigned short operand) {
  write_short_to_stack(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC = operand;
}

// 0xce
void adc_n(gameboy_t *gb, unsigned char operand) { adc(gb, operand); }

// 0xcf
void rst_08(gameboy_t *gb) {
  write_short_to_stack(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC = 0x0008;
}

// 0xd0
void ret_nc(gameboy_t *gb) {
  if (FLAGS_ISCARRY)
    gb->cpu.ticks += 8;
  else {
    gb->cpu.registers.PC = read_short_from_stack(gb);
    gb->cpu.ticks += 20;
  }
}

// 0xd1
void pop_de(gameboy_t *gb) { gb->cpu.registers.DE = read_short_from_stack(gb); }

// 0xd2
void jp_nc_nn(gameboy_t *gb, unsigned short operand) {
  if (!FLAGS_ISCARRY) {
    gb->cpu.registers.PC = operand;
    gb->cpu.ticks += 16;
  } else
    gb->cpu.ticks += 12;
}

// 0xd4
void call_nc_nn(gameboy_t *gb, unsigned short operand) {
  if (!FLAGS_ISCARRY) {
    write_short_to_stack(gb, gb->cpu.registers.PC);
    gb->cpu.registers.PC = operand;
    gb->cpu.ticks += 24;
  } else
    gb->cpu.ticks += 12;
}

// 0xd5
void push_de(gameboy_t *gb) { write_short_to_stack(gb, gb->cpu.registers.DE); }

// 0xd6
void sub_n(gameboy_t *gb, unsigned char operand) { sub(gb, operand); }

// 0xd7
void rst_10(gameboy_t *gb) {
  write_short_to_stack(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC = 0x0010;
}

// 0xd8
void ret_c(gameboy_t *gb) {
  if (FLAGS_ISCARRY) {
    gb->cpu.registers.PC = read_short_from_stack(gb);
    gb->cpu.ticks += 20;
  } else
    gb->cpu.ticks += 8;
}

// 0xd9
// interrupts.c

// 0xda
void jp_c_nn(gameboy_t *gb, unsigned short operand) {
  if (FLAGS_ISCARRY) {
    gb->cpu.registers.PC = operand;
    // debugJump();
    gb->cpu.ticks += 16;
  } else
    gb->cpu.ticks += 12;
}

// 0xdc
void call_c_nn(gameboy_t *gb, unsigned short operand) {
  if (FLAGS_ISCARRY) {
    write_short_to_stack(gb, gb->cpu.registers.PC);
    gb->cpu.registers.PC = operand;
    gb->cpu.ticks += 24;
  } else
    gb->cpu.ticks += 12;
}

// 0xde
void sbc_n(gameboy_t *gb, unsigned char operand) { sbc(gb, operand); }

// 0xdf
void rst_18(gameboy_t *gb) {
  write_short_to_stack(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC = 0x0018;
}

// 0xe0
void ld_ff_n_ap(gameboy_t *gb, unsigned char operand) {
  write_byte(gb, 0xff00 + operand, gb->cpu.registers.a);
}

// 0xe1
void pop_hl(gameboy_t *gb) { gb->cpu.registers.HL = read_short_from_stack(gb); }

// 0xe2
void ld_ff_c_a(gameboy_t *gb) {
  write_byte(gb, 0xff00 + gb->cpu.registers.c, gb->cpu.registers.a);
}

// 0xe5
void push_hl(gameboy_t *gb) { write_short_to_stack(gb, gb->cpu.registers.HL); }

// 0xe6
void and_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.a &= operand;

  FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE);
  FLAGS_SET(FLAGS_HALFCARRY);
  if (gb->cpu.registers.a)
    FLAGS_CLEAR(FLAGS_ZERO);
  else
    FLAGS_SET(FLAGS_ZERO);
}

// 0xe7
void rst_20(gameboy_t *gb) {
  write_short_to_stack(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC = 0x0020;
}

// 0xe8
void add_sp_n(gameboy_t *gb, char operand) {
  int result = gb->cpu.registers.SP + operand;

  if (result & 0xffff0000)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  gb->cpu.registers.SP = result & 0xffff;

  if (((gb->cpu.registers.SP & 0x0f) + (operand & 0x0f)) > 0x0f)
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);

  // _does_ clear the zero flag
  FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE);
}

// 0xe9
void jp_hl(gameboy_t *gb) {
  gb->cpu.registers.PC = gb->cpu.registers.HL;
  // debugJump();
}

// 0xea
void ld_nnp_a(gameboy_t *gb, unsigned short operand) {
  write_byte(gb, operand, gb->cpu.registers.a);
}

// 0xee
void xor_n(gameboy_t *gb, unsigned char operand) { xor(gb, operand); }

// 0xef
void rst_28(gameboy_t *gb) {
  write_short_to_stack(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC = 0x0028;
}

// 0xf0
void ld_ff_ap_n(gameboy_t *gb, unsigned char operand) {
  gb->cpu.registers.a = read_byte(gb, 0xff00 + operand);
}

// 0xf1
void pop_af(gameboy_t *gb) { gb->cpu.registers.AF = read_short_from_stack(gb); }

// 0xf2
void ld_a_ff_c(gameboy_t *gb) {
  gb->cpu.registers.a = read_byte(gb, 0xff00 + gb->cpu.registers.c);
}

// 0xf3
void di_inst(gameboy_t *gb) { gb->cpu.interrupt.master = 0; }

// 0xf5
void push_af(gameboy_t *gb) { write_short_to_stack(gb, gb->cpu.registers.AF); }

// 0xf6
void or_n(gameboy_t *gb, unsigned char operand) { or (gb, operand); }

// 0xf7
void rst_30(gameboy_t *gb) {
  write_short_to_stack(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC = 0x0030;
}

// 0xf8
void ld_hl_sp_n(gameboy_t *gb, unsigned char operand) {
  int result = gb->cpu.registers.SP + (signed char)operand;

  if (result & 0xffff0000)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  if (((gb->cpu.registers.SP & 0x0f) + (operand & 0x0f)) > 0x0f)
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);

  FLAGS_CLEAR(FLAGS_ZERO | FLAGS_NEGATIVE);

  gb->cpu.registers.HL = (unsigned short)(result & 0xffff);
}

// 0xf9
void ld_sp_hl(gameboy_t *gb) { gb->cpu.registers.SP = gb->cpu.registers.HL; }

// 0xfa
void ld_a_nnp(gameboy_t *gb, unsigned short operand) {
  gb->cpu.registers.a = read_byte(gb, operand);
}

// 0xfb
void ei(gameboy_t *gb) { gb->cpu.interrupt.master = 1; }

// 0xfe
void cp_n(gameboy_t *gb, unsigned char operand) {
  FLAGS_SET(FLAGS_NEGATIVE);

  if (gb->cpu.registers.a == operand)
    FLAGS_SET(FLAGS_ZERO);
  else
    FLAGS_CLEAR(FLAGS_ZERO);

  if (operand > gb->cpu.registers.a)
    FLAGS_SET(FLAGS_CARRY);
  else
    FLAGS_CLEAR(FLAGS_CARRY);

  if ((operand & 0x0f) > (gb->cpu.registers.a & 0x0f))
    FLAGS_SET(FLAGS_HALFCARRY);
  else
    FLAGS_CLEAR(FLAGS_HALFCARRY);
}

// 0xff
void rst_38(gameboy_t *gb) {
  write_short_to_stack(gb, gb->cpu.registers.PC);
  gb->cpu.registers.PC = 0x0038;
}
