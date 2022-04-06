#pragma once

#include "cpu.h"

#define UNUSED(r) (void)(r)

#define PHI(r) (((u8*)(&cpu->r)+1))
#define PLO(r) (((u8*)(&cpu->r)))
#define HI(r) (*PHI(r))
#define LO(r) (*PLO(r))

#define ZF 0x80
#define NF 0x40
#define HF 0x20
#define CF 0x10

#define ISF(f) (LO(AF) & f)
#define SETF(f) LO(AF) |= f
#define TOGF(f) LO(AF) ^= f
#define CLRF(f) LO(AF) &= ~(f)
#define CLRALL CLRF(ZF|HF|NF|CF)

#define TOGIF(f,p) if(p) SETF(f); else CLRF(f);

#define READ(a) mmu_read(cpu->mmu, a)
#define WRITE(a,w) mmu_write(cpu->mmu, a, w)

#define READ8 READ(cpu->PC++)
#define READ16 (READ8 | READ8 << 8)
#define WRITE16(a,w) \
	WRITE(a, w & 0xff); \
	WRITE(a+1, w >> 8);

static const u8 cyc_table[256] = {
	1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1, 
	1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1, 
	2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1, 
	2, 3, 2, 2, 3, 3, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1, 
	1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 
	1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 
	1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 
	2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 
	1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 
	1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 
	1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 
	1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 
	2, 3, 3, 4, 3, 4, 2, 4, 2, 4, 3, 0, 3, 6, 2, 4, 
	2, 3, 3, 1, 3, 4, 2, 4, 2, 4, 3, 1, 3, 1, 2, 4, 
	3, 3, 2, 1, 1, 4, 2, 4, 4, 1, 4, 1, 1, 1, 2, 4, 
	3, 3, 2, 1, 1, 4, 2, 4, 3, 2, 4, 1, 0, 1, 2, 4  
};

static const u8 cb_cyc_table[256] = {
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
	2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
	2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
	2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2 
};

struct cpu_op {
	const char* name;
	u8 type;
	void (*fp)(struct cpu*);
} static op_list[256], op_cb_list[256];

/*op1*/
void rlca(struct cpu*);
void rrca(struct cpu*);
void rla(struct cpu*);
void rra(struct cpu*);
void daa(struct cpu*);
void cpl(struct cpu*);
void scf(struct cpu*);
void ccf(struct cpu*);

/*op2*/
void add(struct cpu*, u8);
void adc(struct cpu*, u8);
void sub(struct cpu*, u8);
void sbc(struct cpu*, u8);
void and(struct cpu*, u8);
void xor(struct cpu*, u8);
void or(struct cpu*, u8);
void cp(struct cpu*, u8);

/*op3*/
void rlc(struct cpu*);
void rrc(struct cpu*);
void rl(struct cpu*);
void rr(struct cpu*);
void sla(struct cpu*);
void sra(struct cpu*);
void swap(struct cpu*);
void srl(struct cpu*);

static void (*op1[8])(struct cpu*) = {
	rlca, rrca, rla, rra, daa, cpl, scf, ccf
};

static void (*op2[8])(struct cpu*, u8) = {
	add, adc, sub, sbc, and, xor, or, cp
};

static void (*op3[8])(struct cpu*) = {
	rlc, rrc, rl, rr, sla, sra, swap, srl
};

u8 flag(struct cpu*);
u8 high_get(struct cpu*);
void high_set(struct cpu*, u8);
u8 low_get(struct cpu*);
void low_set(struct cpu*, u8);
void push(struct cpu*, u8);
void push16(struct cpu*, u16);
u8 pop(struct cpu*);
u16 pop16(struct cpu*);

void illegal(struct cpu*);
void nop(struct cpu*);
void ld_u16_sp(struct cpu*);
void stop(struct cpu*);
void jr_uncond(struct cpu*);
void jr_cond(struct cpu*);
void ld_r16_u16(struct cpu*);
void add_hl_r16(struct cpu*);
void ld_r16_a(struct cpu*);
void ld_a_r16(struct cpu*);
void inc_r16(struct cpu*);
void dec_r16(struct cpu*);
void inc_r8(struct cpu*);
void dec_r8(struct cpu*);
void ld_r8_u8(struct cpu*);
void do_op1(struct cpu*);
void halt(struct cpu*);
void ld_r8_r8(struct cpu*);
void alu_a_r8(struct cpu*);
void ret_cond(struct cpu*);
void ld_ff00_u8_a(struct cpu*);
void add_sp_i8(struct cpu*);
void ld_a_ff00_u8(struct cpu*);
void ld_hl_sp_i8(struct cpu*);
void pop_r16(struct cpu*);
void ret(struct cpu*);
void reti(struct cpu*);
void jp_hl(struct cpu*);
void ld_sp_hl(struct cpu*);
void jp_cond(struct cpu*);
void jp_uncond(struct cpu*);
void cb(struct cpu*);
void ld_ff00_c_a(struct cpu*);
void ld_u16_a(struct cpu*);
void ld_a_ff00_c(struct cpu*);
void ld_a_u16(struct cpu*);
void ei(struct cpu*);
void di(struct cpu*);
void call_cond(struct cpu*);
void push_r16(struct cpu*);
void call_u16(struct cpu*);
void alu_a_u8(struct cpu*);
void rst(struct cpu*);
void ldi_hl_a(struct cpu*);
void ldi_a_hl(struct cpu*);
void ldd_hl_a(struct cpu*);
void ldd_a_hl(struct cpu*);
void shift_rot(struct cpu*);
void bit_res_set(struct cpu*);

static struct cpu_op op_list[256] = {
   {"NOP", 1, nop},
   {"LD BC, $%04X", 3, ld_r16_u16},
   {"LD (BC), A", 1, ld_r16_a},
   {"INC BC", 1, inc_r16},
   {"INC B", 1, inc_r8},
   {"DEC B", 1, dec_r8},
   {"LD B, $%02X", 2, ld_r8_u8},
   {"RLCA", 1, do_op1},
   {"LD ($%04X), SP", 3, ld_u16_sp},
   {"ADD HL, BC", 1, add_hl_r16},
   {"LD A, (BC)", 1, ld_a_r16},
   {"DEC BC", 1, dec_r16},
   {"INC C", 1, inc_r8},
   {"DEC C", 1, dec_r8},
   {"LD C, $%02X", 2, ld_r8_u8},
   {"RRCA", 1, do_op1},
   {"STOP", 1, stop},
   {"LD DE, $%04X", 3, ld_r16_u16},
   {"LD (DE), A", 1, ld_r16_a},
   {"INC DE", 1, inc_r16},
   {"INC D", 1, inc_r8},
   {"DEC D", 1, dec_r8},
   {"LD D, $%02X", 2, ld_r8_u8},
   {"RLA", 1, do_op1},
   {"JR $%04X", 5, jr_uncond},
   {"ADD HL, DE", 1, add_hl_r16},
   {"LD A, (DE)", 1, ld_a_r16},
   {"DEC DE", 1, dec_r16},
   {"INC E", 1, inc_r8},
   {"DEC E", 1, dec_r8},
   {"LD E, $%02X", 2, ld_r8_u8},
   {"RRA", 1, do_op1},
   {"JR NZ, $%04X", 5, jr_cond},
   {"LD HL, $%04X", 3, ld_r16_u16},
   {"LDI (HL), A", 1, ldi_hl_a},
   {"INC HL", 1, inc_r16},
   {"INC H", 1, inc_r8},
   {"DEC H", 1, dec_r8},
   {"LD H, $%02X", 2, ld_r8_u8},
   {"DAA", 1, do_op1},
   {"JR Z, $%04X", 5, jr_cond},
   {"ADD HL, HL", 1, add_hl_r16},
   {"LDI A, (HL)", 1, ldi_a_hl},
   {"DEC HL", 1, dec_r16},
   {"INC L", 1, inc_r8},
   {"DEC L", 1, dec_r8},
   {"LD L, $%02X", 2, ld_r8_u8},
   {"CPL", 1, do_op1},
   {"JR NC, $%04X", 5, jr_cond},
   {"LD SP, $%04X", 3, ld_r16_u16},
   {"LDD (HL), A", 1, ldd_hl_a},
   {"INC SP", 1, inc_r16},
   {"INC (HL)", 1, inc_r8},
   {"DEC (HL)", 1, dec_r8},
   {"LD (HL), $%02X", 2, ld_r8_u8},
   {"SCF", 1, do_op1},
   {"JR C, $%04X", 5, jr_cond},
   {"ADD HL, SP", 1, add_hl_r16},
   {"LDD A, (HL)", 1, ldd_a_hl},
   {"DEC SP", 1, dec_r16},
   {"INC A", 1, inc_r8},
   {"DEC A", 1, dec_r8},
   {"LD A, $%02X", 2, ld_r8_u8},
   {"CCF", 1, do_op1},
   {"LD B, B", 1, ld_r8_r8},
   {"LD B, C", 1, ld_r8_r8},
   {"LD B, D", 1, ld_r8_r8},
   {"LD B, E", 1, ld_r8_r8},
   {"LD B, H", 1, ld_r8_r8},
   {"LD B, L", 1, ld_r8_r8},
   {"LD B, (HL)", 1, ld_r8_r8},
   {"LD B, A", 1, ld_r8_r8},
   {"LD C, B", 1, ld_r8_r8},
   {"LD C, C", 1, ld_r8_r8},
   {"LD C, D", 1, ld_r8_r8},
   {"LD C, E", 1, ld_r8_r8},
   {"LD C, H", 1, ld_r8_r8},
   {"LD C, L", 1, ld_r8_r8},
   {"LD C, (HL)", 1, ld_r8_r8},
   {"LD C, A", 1, ld_r8_r8},
   {"LD D, B", 1, ld_r8_r8},
   {"LD D, C", 1, ld_r8_r8},
   {"LD D, D", 1, ld_r8_r8},
   {"LD D, E", 1, ld_r8_r8},
   {"LD D, H", 1, ld_r8_r8},
   {"LD D, L", 1, ld_r8_r8},
   {"LD D, (HL)", 1, ld_r8_r8},
   {"LD D, A", 1, ld_r8_r8},
   {"LD E, B", 1, ld_r8_r8},
   {"LD E, C", 1, ld_r8_r8},
   {"LD E, D", 1, ld_r8_r8},
   {"LD E, E", 1, ld_r8_r8},
   {"LD E, H", 1, ld_r8_r8},
   {"LD E, L", 1, ld_r8_r8},
   {"LD E, (HL)", 1, ld_r8_r8},
   {"LD E, A", 1, ld_r8_r8},
   {"LD H, B", 1, ld_r8_r8},
   {"LD H, C", 1, ld_r8_r8},
   {"LD H, D", 1, ld_r8_r8},
   {"LD H, E", 1, ld_r8_r8},
   {"LD H, H", 1, ld_r8_r8},
   {"LD H, L", 1, ld_r8_r8},
   {"LD H, (HL)", 1, ld_r8_r8},
   {"LD H, A", 1, ld_r8_r8},
   {"LD L, B", 1, ld_r8_r8},
   {"LD L, C", 1, ld_r8_r8},
   {"LD L, D", 1, ld_r8_r8},
   {"LD L, E", 1, ld_r8_r8},
   {"LD L, H", 1, ld_r8_r8},
   {"LD L, L", 1, ld_r8_r8},
   {"LD L, (HL)", 1, ld_r8_r8},
   {"LD L, A", 1, ld_r8_r8},
   {"LD (HL), B", 1, ld_r8_r8},
   {"LD (HL), C", 1, ld_r8_r8},
   {"LD (HL), D", 1, ld_r8_r8},
   {"LD (HL), E", 1, ld_r8_r8},
   {"LD (HL), H", 1, ld_r8_r8},
   {"LD (HL), L", 1, ld_r8_r8},
   {"HALT", 1, halt},
   {"LD (HL), A", 1, ld_r8_r8},
   {"LD A, B", 1, ld_r8_r8},
   {"LD A, C", 1, ld_r8_r8},
   {"LD A, D", 1, ld_r8_r8},
   {"LD A, E", 1, ld_r8_r8},
   {"LD A, H", 1, ld_r8_r8},
   {"LD A, L", 1, ld_r8_r8},
   {"LD A, (HL)", 1, ld_r8_r8},
   {"LD A, A", 1, ld_r8_r8},
   {"ADD A, B", 1, alu_a_r8},
   {"ADD A, C", 1, alu_a_r8},
   {"ADD A, D", 1, alu_a_r8},
   {"ADD A, E", 1, alu_a_r8},
   {"ADD A, H", 1, alu_a_r8},
   {"ADD A, L", 1, alu_a_r8},
   {"ADD A, (HL)", 1, alu_a_r8},
   {"ADD A", 1, alu_a_r8},
   {"ADC B", 1, alu_a_r8},
   {"ADC C", 1, alu_a_r8},
   {"ADC D", 1, alu_a_r8},
   {"ADC E", 1, alu_a_r8},
   {"ADC H", 1, alu_a_r8},
   {"ADC L", 1, alu_a_r8},
   {"ADC (HL)", 1, alu_a_r8},
   {"ADC A", 1, alu_a_r8},
   {"SUB B", 1, alu_a_r8},
   {"SUB C", 1, alu_a_r8},
   {"SUB D", 1, alu_a_r8},
   {"SUB E", 1, alu_a_r8},
   {"SUB H", 1, alu_a_r8},
   {"SUB L", 1, alu_a_r8},
   {"SUB (HL)", 1, alu_a_r8},
   {"SUB A", 1, alu_a_r8},
   {"SBC B", 1, alu_a_r8},
   {"SBC C", 1, alu_a_r8},
   {"SBC D", 1, alu_a_r8},
   {"SBC E", 1, alu_a_r8},
   {"SBC H", 1, alu_a_r8},
   {"SBC L", 1, alu_a_r8},
   {"SBC (HL)", 1, alu_a_r8},
   {"SBC A", 1, alu_a_r8},
   {"AND B", 1, alu_a_r8},
   {"AND C", 1, alu_a_r8},
   {"AND D", 1, alu_a_r8},
   {"AND E", 1, alu_a_r8},
   {"AND H", 1, alu_a_r8},
   {"AND L", 1, alu_a_r8},
   {"AND (HL)", 1, alu_a_r8},
   {"AND A", 1, alu_a_r8},
   {"XOR B", 1, alu_a_r8},
   {"XOR C", 1, alu_a_r8},
   {"XOR D", 1, alu_a_r8},
   {"XOR E", 1, alu_a_r8},
   {"XOR H", 1, alu_a_r8},
   {"XOR L", 1, alu_a_r8},
   {"XOR (HL)", 1, alu_a_r8},
   {"XOR A", 1, alu_a_r8},
   {"OR B", 1, alu_a_r8},
   {"OR C", 1, alu_a_r8},
   {"OR D", 1, alu_a_r8},
   {"OR E", 1, alu_a_r8},
   {"OR H", 1, alu_a_r8},
   {"OR L", 1, alu_a_r8},
   {"OR (HL)", 1, alu_a_r8},
   {"OR A", 1, alu_a_r8},
   {"CP B", 1, alu_a_r8},
   {"CP C", 1, alu_a_r8},
   {"CP D", 1, alu_a_r8},
   {"CP E", 1, alu_a_r8},
   {"CP H", 1, alu_a_r8},
   {"CP L", 1, alu_a_r8},
   {"CP (HL)", 1, alu_a_r8},
   {"CP A", 1, alu_a_r8},
   {"RET NZ", 1, ret_cond},
   {"POP BC", 1, pop_r16},
   {"JP NZ, $%04X", 3, jp_cond},
   {"JP $%04X", 3, jp_uncond},
   {"CALL NZ, $%04X", 3, call_cond},
   {"PUSH BC", 1, push_r16},
   {"ADD A, $%02X", 2, alu_a_u8},
   {"RST $00", 1, rst},
   {"RET Z", 1, ret_cond},
   {"RET", 1, ret},
   {"JP Z, $%04X", 3, jp_cond},
   {"CB %02X", 2, cb},
   {"CALL Z, $%04X", 3, call_cond},
   {"CALL $%04X", 3, call_u16},
   {"ADC $%02X", 2, alu_a_u8},
   {"RST $08", 1, rst},
   {"RET NC", 1, ret_cond},
   {"POP DE", 1, pop_r16},
   {"JP NC, $%04X", 3, jp_cond},
   {"UNDEF", 0, illegal},
   {"CALL NC, $%04X", 3, call_cond},
   {"PUSH DE", 1, push_r16},
   {"SUB $%02X", 2, alu_a_u8},
   {"RST $10", 1, rst},
   {"RET C", 1, ret_cond},
   {"RETI", 1, reti},
   {"JP C, $%04X", 3, jp_cond},
   {"UNDEF", 0, illegal},
   {"CALL C, $%04X", 3, call_cond},
   {"UNDEF", 0, illegal},
   {"SBC $%02X", 2, alu_a_u8},
   {"RST $18", 1, rst},
   {"LD ($FF00 + $%02X), A", 2, ld_ff00_u8_a},
   {"POP HL", 1, pop_r16},
   {"LD ($FF00 + C), A", 1, ld_ff00_c_a},
   {"UNDEF", 0, illegal},
   {"UNDEF", 0, illegal},
   {"PUSH HL", 1, push_r16},
   {"AND $%02X", 2, alu_a_u8},
   {"RST $20", 1, rst},
   {"ADD SP,$%02X", 2, add_sp_i8},
   {"JP HL", 1, jp_hl},
   {"LD ($%04X), A", 3, ld_u16_a},
   {"UNDEF", 0, illegal},
   {"UNDEF", 0, illegal},
   {"UNDEF", 0, illegal},
   {"XOR $%02X", 2, alu_a_u8},
   {"RST $28", 1, rst},
   {"LD A, ($FF00 + $%02X)", 2, ld_a_ff00_u8},
   {"POP AF", 1, pop_r16},
   {"LD A, ($FF00 + C)", 1, ld_a_ff00_c},
   {"DI", 1, di},
   {"UNDEF", 0, illegal},
   {"PUSH AF", 1, push_r16},
   {"OR $%02X", 2, alu_a_u8},
   {"RST $30", 1, rst},
   {"LD HL, SP+$%02X", 2, ld_hl_sp_i8},
   {"LD SP, HL", 1, ld_sp_hl},
   {"LD A, ($%04X)", 3, ld_a_u16},
   {"EI", 1, ei},
   {"UNDEF", 0, illegal},
   {"UNDEF", 0, illegal},
   {"CP $%02X", 2, alu_a_u8},
   {"RST $38", 1, rst}
};

static struct cpu_op op_cb_list[256] = {
	{"RLC B", 2, rlc},
	{"RLC C", 2, rlc},
	{"RLC D", 2, rlc},
	{"RLC E", 2, rlc},
	{"RLC H", 2, rlc},
	{"RLC L", 2, rlc},
	{"RLC (HL)", 2, rlc},
	{"RLC A", 2, rlc},
	{"RRC B", 2, rrc},
	{"RRC C", 2, rrc},
	{"RRC D", 2, rrc},
	{"RRC E", 2, rrc},
	{"RRC H", 2, rrc},
	{"RRC L", 2, rrc},
	{"RRC (HL)", 2, rrc},
	{"RRC A", 2, rrc},
	{"RL B", 2, rl},
	{"RL C", 2, rl},
	{"RL D", 2, rl},
	{"RL E", 2, rl},
	{"RL H", 2, rl},
	{"RL L", 2, rl},
	{"RL (HL)", 2, rl},
	{"RL A", 2, rl},
	{"RR B", 2, rr},
	{"RR C", 2, rr},
	{"RR D", 2, rr},
	{"RR E", 2, rr},
	{"RR H", 2, rr},
	{"RR L", 2, rr},
	{"RR (HL)", 2, rr},
	{"RR A", 2, rr},
	{"SLA B", 2, sla},
	{"SLA C", 2, sla},
	{"SLA D", 2, sla},
	{"SLA E", 2, sla},
	{"SLA H", 2, sla},
	{"SLA L", 2, sla},
	{"SLA (HL)", 2, sla},
	{"SLA A", 2, sla},
	{"SRA B", 2, sra},
	{"SRA C", 2, sra},
	{"SRA D", 2, sra},
	{"SRA E", 2, sra},
	{"SRA H", 2, sra},
	{"SRA L", 2, sra},
	{"SRA (HL)", 2, sra},
	{"SRA A", 2, sra},
	{"SWAP B", 2, swap},
	{"SWAP C", 2, swap},
	{"SWAP D", 2, swap},
	{"SWAP E", 2, swap},
	{"SWAP H", 2, swap},
	{"SWAP L", 2, swap},
	{"SWAP (HL)", 2, swap},
	{"SWAP A", 2, swap},
	{"SRL B", 2, srl},
	{"SRL C", 2, srl},
	{"SRL D", 2, srl},
	{"SRL E", 2, srl},
	{"SRL H", 2, srl},
	{"SRL L", 2, srl},
	{"SRL (HL)", 2, srl},
	{"SRL A", 2, srl},
	{"BIT 0, B", 2, bit_res_set},
	{"BIT 0, C", 2, bit_res_set},
	{"BIT 0, D", 2, bit_res_set},
	{"BIT 0, E", 2, bit_res_set},
	{"BIT 0, H", 2, bit_res_set},
	{"BIT 0, L", 2, bit_res_set},
	{"BIT 0, (HL)", 2, bit_res_set},
	{"BIT 0, A", 2, bit_res_set},
	{"BIT 1, B", 2, bit_res_set},
	{"BIT 1, C", 2, bit_res_set},
	{"BIT 1, D", 2, bit_res_set},
	{"BIT 1, E", 2, bit_res_set},
	{"BIT 1, H", 2, bit_res_set},
	{"BIT 1, L", 2, bit_res_set},
	{"BIT 1, (HL)", 2, bit_res_set},
	{"BIT 1, A", 2, bit_res_set},
	{"BIT 2, B", 2, bit_res_set},
	{"BIT 2, C", 2, bit_res_set},
	{"BIT 2, D", 2, bit_res_set},
	{"BIT 2, E", 2, bit_res_set},
	{"BIT 2, H", 2, bit_res_set},
	{"BIT 2, L", 2, bit_res_set},
	{"BIT 2, (HL)", 2, bit_res_set},
	{"BIT 2, A", 2, bit_res_set},
	{"BIT 3, B", 2, bit_res_set},
	{"BIT 3, C", 2, bit_res_set},
	{"BIT 3, D", 2, bit_res_set},
	{"BIT 3, E", 2, bit_res_set},
	{"BIT 3, H", 2, bit_res_set},
	{"BIT 3, L", 2, bit_res_set},
	{"BIT 3, (HL)", 2, bit_res_set},
	{"BIT 3, A", 2, bit_res_set},
	{"BIT 4, B", 2, bit_res_set},
	{"BIT 4, C", 2, bit_res_set},
	{"BIT 4, D", 2, bit_res_set},
	{"BIT 4, E", 2, bit_res_set},
	{"BIT 4, H", 2, bit_res_set},
	{"BIT 4, L", 2, bit_res_set},
	{"BIT 4, (HL)", 2, bit_res_set},
	{"BIT 4, A", 2, bit_res_set},
	{"BIT 5, B", 2, bit_res_set},
	{"BIT 5, C", 2, bit_res_set},
	{"BIT 5, D", 2, bit_res_set},
	{"BIT 5, E", 2, bit_res_set},
	{"BIT 6, H", 2, bit_res_set},
	{"BIT 6, L", 2, bit_res_set},
	{"BIT 5, (HL)", 2, bit_res_set},
	{"BIT 5, A", 2, bit_res_set},
	{"BIT 6, B", 2, bit_res_set},
	{"BIT 6, C", 2, bit_res_set},
	{"BIT 6, D", 2, bit_res_set},
	{"BIT 6, E", 2, bit_res_set},
	{"BIT 6, H", 2, bit_res_set},
	{"BIT 6, L", 2, bit_res_set},
	{"BIT 6, (HL)", 2, bit_res_set},
	{"BIT 6, A", 2, bit_res_set},
	{"BIT 7, B", 2, bit_res_set},
	{"BIT 7, C", 2, bit_res_set},
	{"BIT 7, D", 2, bit_res_set},
	{"BIT 7, E", 2, bit_res_set},
	{"BIT 7, H", 2, bit_res_set},
	{"BIT 7, L", 2, bit_res_set},
	{"BIT 7, (HL)", 2, bit_res_set},
	{"BIT 7, A", 2, bit_res_set},
	{"RES 0, B", 2, bit_res_set},
	{"RES 0, C", 2, bit_res_set},
	{"RES 0, D", 2, bit_res_set},
	{"RES 0, E", 2, bit_res_set},
	{"RES 0, H", 2, bit_res_set},
	{"RES 0, L", 2, bit_res_set},
	{"RES 0, (HL)", 2, bit_res_set},
	{"RES 0, A", 2, bit_res_set},
	{"RES 1, B", 2, bit_res_set},
	{"RES 1, C", 2, bit_res_set},
	{"RES 1, D", 2, bit_res_set},
	{"RES 1, E", 2, bit_res_set},
	{"RES 1, H", 2, bit_res_set},
	{"RES 1, L", 2, bit_res_set},
	{"RES 1, (HL)", 2, bit_res_set},
	{"RES 1, A", 2, bit_res_set},
	{"RES 2, B", 2, bit_res_set},
	{"RES 2, C", 2, bit_res_set},
	{"RES 2, D", 2, bit_res_set},
	{"RES 2, E", 2, bit_res_set},
	{"RES 2, H", 2, bit_res_set},
	{"RES 2, L", 2, bit_res_set},
	{"RES 2, (HL)", 2, bit_res_set},
	{"RES 2, A", 2, bit_res_set},
	{"RES 3, B", 2, bit_res_set},
	{"RES 3, C", 2, bit_res_set},
	{"RES 3, D", 2, bit_res_set},
	{"RES 3, E", 2, bit_res_set},
	{"RES 3, H", 2, bit_res_set},
	{"RES 3, L", 2, bit_res_set},
	{"RES 3, (HL)", 2, bit_res_set},
	{"RES 3, A", 2, bit_res_set},
	{"RES 4, B", 2, bit_res_set},
	{"RES 4, C", 2, bit_res_set},
	{"RES 4, D", 2, bit_res_set},
	{"RES 4, E", 2, bit_res_set},
	{"RES 4, H", 2, bit_res_set},
	{"RES 4, L", 2, bit_res_set},
	{"RES 4, (HL)", 2, bit_res_set},
	{"RES 4, A", 2, bit_res_set},
	{"RES 5, B", 2, bit_res_set},
	{"RES 5, C", 2, bit_res_set},
	{"RES 5, D", 2, bit_res_set},
	{"RES 5, E", 2, bit_res_set},
	{"RES 5, H", 2, bit_res_set},
	{"RES 5, L", 2, bit_res_set},
	{"RES 5, (HL)", 2, bit_res_set},
	{"RES 5, A", 2, bit_res_set},
	{"RES 6, B", 2, bit_res_set},
	{"RES 6, C", 2, bit_res_set},
	{"RES 6, D", 2, bit_res_set},
	{"RES 6, E", 2, bit_res_set},
	{"RES 6, H", 2, bit_res_set},
	{"RES 6, L", 2, bit_res_set},
	{"RES 6, (HL)", 2, bit_res_set},
	{"RES 6, A", 2, bit_res_set},
	{"RES 7, B", 2, bit_res_set},
	{"RES 7, C", 2, bit_res_set},
	{"RES 7, D", 2, bit_res_set},
	{"RES 7, E", 2, bit_res_set},
	{"RES 7, H", 2, bit_res_set},
	{"RES 7, L", 2, bit_res_set},
	{"RES 7, (HL)", 2, bit_res_set},
	{"RES 7, A", 2, bit_res_set},
	{"SET 0, B", 2, bit_res_set},
	{"SET 0, C", 2, bit_res_set},
	{"SET 0, D", 2, bit_res_set},
	{"SET 0, E", 2, bit_res_set},
	{"SET 0, H", 2, bit_res_set},
	{"SET 0, L", 2, bit_res_set},
	{"SET 0, (HL)", 2, bit_res_set},
	{"SET 0, A", 2, bit_res_set},
	{"SET 1, B", 2, bit_res_set},
	{"SET 1, C", 2, bit_res_set},
	{"SET 1, D", 2, bit_res_set},
	{"SET 1, E", 2, bit_res_set},
	{"SET 1, H", 2, bit_res_set},
	{"SET 1, L", 2, bit_res_set},
	{"SET 1, (HL)", 2, bit_res_set},
	{"SET 1, A", 2, bit_res_set},
	{"SET 2, B", 2, bit_res_set},
	{"SET 2, C", 2, bit_res_set},
	{"SET 2, D", 2, bit_res_set},
	{"SET 2, E", 2, bit_res_set},
	{"SET 2, H", 2, bit_res_set},
	{"SET 2, L", 2, bit_res_set},
	{"SET 2, (HL)", 2, bit_res_set},
	{"SET 2, A", 2, bit_res_set},
	{"SET 3, B", 2, bit_res_set},
	{"SET 3, C", 2, bit_res_set},
	{"SET 3, D", 2, bit_res_set},
	{"SET 3, E", 2, bit_res_set},
	{"SET 3, H", 2, bit_res_set},
	{"SET 3, L", 2, bit_res_set},
	{"SET 3, (HL)", 2, bit_res_set},
	{"SET 3, A", 2, bit_res_set},
	{"SET 4, B", 2, bit_res_set},
	{"SET 4, C", 2, bit_res_set},
	{"SET 4, D", 2, bit_res_set},
	{"SET 4, E", 2, bit_res_set},
	{"SET 4, H", 2, bit_res_set},
	{"SET 4, L", 2, bit_res_set},
	{"SET 4, (HL)", 2, bit_res_set},
	{"SET 4, A", 2, bit_res_set},
	{"SET 5, B", 2, bit_res_set},
	{"SET 5, C", 2, bit_res_set},
	{"SET 5, D", 2, bit_res_set},
	{"SET 5, E", 2, bit_res_set},
	{"SET 5, H", 2, bit_res_set},
	{"SET 5, L", 2, bit_res_set},
	{"SET 5, (HL)", 2, bit_res_set},
	{"SET 5, A", 2, bit_res_set},
	{"SET 6, B", 2, bit_res_set},
	{"SET 6, C", 2, bit_res_set},
	{"SET 6, D", 2, bit_res_set},
	{"SET 6, E", 2, bit_res_set},
	{"SET 6, H", 2, bit_res_set},
	{"SET 6, L", 2, bit_res_set},
	{"SET 6, (HL)", 2, bit_res_set},
	{"SET 6, A", 2, bit_res_set},
	{"SET 7, B", 2, bit_res_set},
	{"SET 7, C", 2, bit_res_set},
	{"SET 7, D", 2, bit_res_set},
	{"SET 7, E", 2, bit_res_set},
	{"SET 7, H", 2, bit_res_set},
	{"SET 7, L", 2, bit_res_set},
	{"SET 7, (HL)", 2, bit_res_set},
	{"SET 7, A", 2, bit_res_set }
};