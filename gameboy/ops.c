#include "ops.h"

u8 flag(struct cpu* cpu)
{
	switch ((cpu->op >> 3) & 3) {
	case 0: return !ISF(ZF);
	case 1: return  ISF(ZF);
	case 2: return !ISF(CF);
	case 3: return  ISF(CF);
	default: return 0;
	}
}

u8 high_get(struct cpu* cpu)
{
	switch ((cpu->op & 0x38) >> 3) {
	case 0: return HI(BC);
	case 1: return LO(BC);
	case 2: return HI(DE);
	case 3: return LO(DE);
	case 4: return HI(HL);
	case 5: return LO(HL);
	case 6: return READ(cpu->HL);
	case 7: return HI(AF);
	default: return 0;
	}
}

void high_set(struct cpu* cpu, u8 dat)
{
	switch ((cpu->op & 0x38) >> 3) {
	case 0: HI(BC) = dat; break;
	case 1: LO(BC) = dat; break;
	case 2: HI(DE) = dat; break;
	case 3: LO(DE) = dat; break;
	case 4: HI(HL) = dat; break;
	case 5: LO(HL) = dat; break;
	case 6: WRITE(cpu->HL, dat); break;
	case 7: HI(AF) = dat; break;
	}
}

u8 low_get(struct cpu* cpu)
{
	switch (cpu->op & 7) {
	case 0: return HI(BC);
	case 1: return LO(BC);
	case 2: return HI(DE);
	case 3: return LO(DE);
	case 4: return HI(HL);
	case 5: return LO(HL);
	case 6: return READ(cpu->HL);
	case 7: return HI(AF);
	default: return 0;
	}
}

void low_set(struct cpu* cpu, u8 dat)
{
	switch (cpu->op & 7) {
	case 0: HI(BC) = dat; break;
	case 1: LO(BC) = dat; break;
	case 2: HI(DE) = dat; break;
	case 3: LO(DE) = dat; break;
	case 4: HI(HL) = dat; break;
	case 5: LO(HL) = dat; break;
	case 6: WRITE(cpu->HL, dat); break;
	case 7: HI(AF) = dat; break;
	}
}

void push(struct cpu* cpu, u8 dat)
{
	WRITE(--cpu->SP, dat);
}

void push16(struct cpu* cpu, u16 dat)
{
	WRITE(--cpu->SP, dat >> 8);
	WRITE(--cpu->SP, dat & 0xff);
}

u8 pop(struct cpu* cpu)
{
	return READ(cpu->SP++);
}

u16 pop16(struct cpu* cpu)
{
	u16 res = READ(cpu->SP++);
	res |= (READ(cpu->SP++) << 8);
	return res;
}

void rlca(struct cpu* cpu)
{
	CLRALL;
	TOGIF(CF, HI(AF) >> 7);
	HI(AF) = (HI(AF) << 1) | (HI(AF) >> 7);
}

void rrca(struct cpu* cpu)
{
	CLRALL;
	TOGIF(CF, HI(AF) & 1);
	HI(AF) = (HI(AF) >> 1) | (HI(AF) << 7);
}

void rla(struct cpu* cpu)
{
	u8 oc = ISF(CF) >> 4;
	CLRALL;
	TOGIF(CF, HI(AF) >> 7);
	HI(AF) = (HI(AF) << 1) | oc;
}

void rra(struct cpu* cpu)
{
	u8 ob1 = HI(AF) & 1, oc = ISF(CF) >> 4;
	CLRALL;
	HI(AF) >>= 1;
	if (oc) HI(AF) |= 0x80;
	TOGIF(CF, ob1);
}

void rlc(struct cpu* cpu)
{
	u8 dat = low_get(cpu), nd;
	u8 c = (dat & 0x80) >> 7;

	nd = (dat << 1) | c;

	CLRALL;
	TOGIF(CF, c);
	TOGIF(ZF, (!(dat << 1)));
	low_set(cpu, nd);
}

void rrc(struct cpu* cpu)
{
	u8 dat = low_get(cpu);
	u8 c = dat & 1;

	dat = (dat >> 1) | (c << 7);

	CLRALL;
	TOGIF(CF, c);
	TOGIF(ZF, (!dat));
	low_set(cpu, dat);
}

void rl(struct cpu* cpu)
{
	u8 dat = low_get(cpu), c = ISF(CF) >> 4;
	u8 ob7 = (dat & 0x80);

	dat = (dat << 1) | c;
	CLRALL;
	TOGIF(CF, ob7);
	TOGIF(ZF, (!dat));
	low_set(cpu, dat);
}

void rr(struct cpu* cpu)
{
	u8 dat = low_get(cpu), c = ISF(CF) >> 4;
	u8 ob1 = dat & 1;
	
	dat = (dat >> 1) | (c << 7);
	CLRALL;
	TOGIF(CF, ob1);
	TOGIF(ZF, (!dat));
	CLRF(NF | HF);
	low_set(cpu, dat);
}

void daa(struct cpu* cpu)
{
	if (ISF(NF)) {
		if (ISF(CF)) HI(AF) -= 0x60;
		if (ISF(HF)) HI(AF) -= 0x06;
	}
	else {
		if (ISF(CF) || (HI(AF) & 0xff) > 0x99) {
			HI(AF) += 0x60;
			SETF(CF);
		}

		if (ISF(HF) || (HI(AF) & 0x0f) > 0x09) {
			HI(AF) += 0x06;
		}
	}

	TOGIF(ZF, (!HI(AF)));
	CLRF(HF);
}

void cpl(struct cpu* cpu)
{
	HI(AF) ^= 0xff;
	SETF(NF | HF);
}

void scf(struct cpu* cpu)
{
	CLRF(NF);
	CLRF(HF);
	SETF(CF);
}

void ccf(struct cpu* cpu)
{
	CLRF(NF);
	CLRF(HF);
	TOGF(CF);
}

void sla(struct cpu* cpu)
{
	u8 dat = low_get(cpu);
	
	CLRALL;
	TOGIF(CF, (dat & 0x80));
	TOGIF(ZF, (!(dat & 0x7f)));
	dat = (u8)(dat << 1);

	low_set(cpu, dat);
}

void sra(struct cpu* cpu)
{
	u8 dat = low_get(cpu);
	u8 ob7 = dat & 0x80;

	CLRALL;
	TOGIF(CF, (dat & 1));
	dat = (dat >> 1) | ob7;
	if (dat & 0x80) dat |= 0x80;
	TOGIF(ZF, (!dat));

	low_set(cpu, dat);
}

void swap(struct cpu* cpu)
{
	u8 dat = low_get(cpu), nd;
	nd = (dat >> 4) | (dat << 4);
	TOGIF(ZF, (!dat));
	CLRF(NF | HF | CF);
	low_set(cpu, nd);
}

void srl(struct cpu* cpu)
{
	u8 dat = low_get(cpu);
	
	CLRALL;
	TOGIF(CF, (dat & 1));
	dat >>= 1;
	TOGIF(ZF, (!dat));
	
	low_set(cpu, dat);
}

void add(struct cpu* cpu, u8 reg)
{
	int res = HI(AF) + reg;
	CLRF(NF);
	TOGIF(ZF, (!((u8)res)));
	TOGIF(HF, ((HI(AF) & 0xf) + (reg & 0xf) > 0xf));
	TOGIF(CF, ((((unsigned)HI(AF)) + ((unsigned)reg) > 0xff)));
	HI(AF) = (u8)res;
}

void adc(struct cpu* cpu, u8 reg)
{
	u8 oc = ISF(CF) >> 4, oa = HI(AF);
	int res = HI(AF) + reg + oc;
	CLRF(NF);
	TOGIF(ZF, (!((u8)res)));
	TOGIF(HF, ((oa & 0xf) + (reg & 0xf) + oc > 0xf));
	TOGIF(CF, ((unsigned)oa + (unsigned)reg + oc > 0xff));
	HI(AF) = (u8)res;
}

void sub(struct cpu* cpu, u8 reg)
{
	u8 oa = HI(AF);
	HI(AF) -= reg;
	SETF(NF);
	TOGIF(ZF, (oa == reg));
	TOGIF(CF, (reg > oa));
	TOGIF(HF, ((reg & 0xf) > (oa & 0xf)));
}

void sbc(struct cpu* cpu, u8 reg)
{
	u8 c = ISF(CF) >> 4;
	u8 res = HI(AF) - reg - c;
	SETF(NF);
	TOGIF(ZF, (!res));
	TOGIF(HF, ((HI(AF) & 0xf) < (reg & 0xf) + c));
	TOGIF(CF, (((unsigned)HI(AF) - (unsigned)reg) - c > 0xff));
	HI(AF) = res;
}

void and(struct cpu* cpu, u8 reg)
{
	HI(AF) &= reg;
	SETF(HF);
	CLRF(NF | CF);
	TOGIF(ZF, (!HI(AF)));
}

void or(struct cpu* cpu, u8 reg)
{
	HI(AF) |= reg;
	TOGIF(ZF, (!HI(AF)));
	CLRF(NF | HF | CF);
}

void xor(struct cpu* cpu, u8 reg)
{
	HI(AF) ^= reg;
	TOGIF(ZF, (!HI(AF)));
	CLRF(NF | HF | CF);
}

void cp(struct cpu* cpu, u8 reg)
{
	CLRALL;
	SETF(NF);
	TOGIF(ZF, (HI(AF) == reg));
	TOGIF(HF, ((HI(AF) & 0xf) < (reg & 0xf)));
	TOGIF(CF, (HI(AF) < reg));
}

void illegal(struct cpu* cpu) 
{
	printf("ILLEGAL INSTRUCTION: %02X at $%04X\n", cpu->op, cpu->PC);
}

void nop(struct cpu* cpu)
{
	(void)(cpu); /* do nothing */
}

void ld_u16_sp(struct cpu* cpu)
{
	u16 a16 = READ16;
	WRITE16(a16, cpu->SP);
}

void stop(struct cpu* cpu)
{
	/* https://gbdev.io/pandocs/imgs/gb_stop.png */
	printf("STOPPED\n");
	(void)(cpu);
}

void jr_uncond(struct cpu* cpu)
{
	s8 i = (s8)READ8;
	cpu->PC += i;
}

void jr_cond(struct cpu* cpu)
{
	s8 i = (s8)READ8;
	if (flag(cpu)) {
		cpu->cyc += 4;
		cpu->PC += i;
	}
}

void ld_r16_u16(struct cpu* cpu)
{
	u16* r16 = cpu->r16g1[(cpu->op & 0x30) >> 4];
	*r16 = READ16;
}

void add_hl_r16(struct cpu* cpu)
{
	u16* r16 = cpu->r16g1[(cpu->op & 0x30) >> 4];
	int res = cpu->HL + *r16;
	TOGIF(CF, (res & 0x10000));
	TOGIF(HF, ((cpu->HL ^ *r16 ^ (res & 0xffff)) & 0x1000));
	CLRF(NF);
	cpu->HL = (u16)res;
}

void ld_r16_a(struct cpu* cpu)
{
	u16* r16 = cpu->r16g2[(cpu->op & 0x30) >> 4];
	if (!r16) {
		if ((cpu->op & 0x30) >> 4 == 2) {
			ldi_hl_a(cpu);
		}
		else if ((cpu->op & 0x30) >> 4 == 3) {
			ldd_hl_a(cpu);
		}
	}
	else {
		WRITE(*r16, HI(AF));
	}
}

void ld_a_r16(struct cpu* cpu)
{
	u16* r16 = cpu->r16g2[(cpu->op & 0x30) >> 4];
	if (!r16) {
		if ((cpu->op & 0x30) >> 4 == 2) {
			ldi_a_hl(cpu);
		}
		else {
			ldd_a_hl(cpu);
		}
	}
	else {
		HI(AF) = READ(*r16);
	}
}

void inc_r16(struct cpu* cpu)
{
	u16* r16 = cpu->r16g1[(cpu->op & 0x30) >> 4];
	*r16 += 1;
}

void dec_r16(struct cpu* cpu)
{
	u16* r16 = cpu->r16g1[(cpu->op & 0x30) >> 4];
	*r16 -= 1;
}

void inc_r8(struct cpu* cpu)
{
	u8 r8 = high_get(cpu);
	r8++;
	high_set(cpu, r8);
	TOGIF(ZF, (!r8));
	TOGIF(HF, (!(r8 & 0xf)));
	CLRF(NF);
}

void dec_r8(struct cpu* cpu)
{
	u8 r8 = high_get(cpu);
	r8--;
	high_set(cpu, r8);
	TOGIF(ZF, (!r8));
	TOGIF(HF, ((r8 & 0xf) == 0xf));
	SETF(NF);
}

void ld_r8_u8(struct cpu* cpu)
{
	high_set(cpu, READ8);
}

void do_op1(struct cpu* cpu)
{
	op1[(cpu->op & 0x38) >> 3](cpu);
}

void halt(struct cpu* cpu)
{
	/* TODO: do halt */
	cpu->halted = 1;
}

void ld_r8_r8(struct cpu* cpu)
{
	u8 dat = low_get(cpu);
	high_set(cpu, dat);
}

void alu_a_r8(struct cpu* cpu)
{
	u8 reg = low_get(cpu);
	u8 loc = (cpu->op & 0x38) >> 3;
	op2[loc](cpu, reg);
}

void ret_cond(struct cpu* cpu)
{
	if (flag(cpu)) {
		cpu->cyc += 12;
		cpu->PC = pop16(cpu);
	}
}

void ld_ff00_u8_a(struct cpu* cpu)
{
	WRITE(0xff00 | READ8, HI(AF));
}

void add_sp_i8(struct cpu* cpu)
{
	s8 n = (s8)READ8;

	CLRF(ZF | NF);
	TOGIF(HF, ((cpu->SP & 0xf) + (n & 0xf) > 0xf));
	TOGIF(CF, ((cpu->SP & 0xff) + (n & 0xff) > 0xff));

	cpu->SP += n;
}

void ld_a_ff00_u8(struct cpu* cpu)
{
	u8 a8 = READ8;
	HI(AF) = READ(0xff00 + a8);
}

void ld_hl_sp_i8(struct cpu* cpu)
{
	s8 n = (s8)READ8;

	CLRF(ZF | NF);
	TOGIF(HF, ((cpu->SP & 0xf) + (n & 0xf) > 0xf));
	TOGIF(CF, ((cpu->SP & 0xff) + (n & 0xff) > 0xff));

	cpu->HL = cpu->SP + n;
}

void pop_r16(struct cpu* cpu)
{
	u16* r16 = cpu->r16g3[(cpu->op & 0x30) >> 4];
	*r16 = pop16(cpu);
	cpu->AF &= 0xfff0;
}

void ret(struct cpu* cpu)
{
	cpu->PC = pop16(cpu);
}

void reti(struct cpu* cpu)
{
	ret(cpu);
	cpu->ime = 1;
}

void jp_hl(struct cpu* cpu)
{
	cpu->PC = cpu->HL;
}

void ld_sp_hl(struct cpu* cpu)
{
	cpu->SP = cpu->HL;
}

void jp_cond(struct cpu* cpu)
{
	u16 a16 = READ16;

	if (flag(cpu)) {
		cpu->cyc += 4;
		cpu->PC = a16;
	}
}

void jp_uncond(struct cpu* cpu)
{
	cpu->PC = READ16;
}

void cb(struct cpu* cpu)
{
	cpu->op = READ8;
	cpu->cyc += cb_cyc_table[cpu->op] * 4;
	op_cb_list[cpu->op].fp(cpu);
}

void ld_ff00_c_a(struct cpu* cpu)
{
	WRITE(0xff00 + LO(BC), HI(AF));
}

void ld_u16_a(struct cpu* cpu)
{
	WRITE(READ16, HI(AF));
}

void ld_a_ff00_c(struct cpu* cpu)
{
	HI(AF) = READ(0xff00 + LO(BC));
}

void ld_a_u16(struct cpu* cpu)
{
	HI(AF) = READ(READ16);
}

void ei(struct cpu* cpu)
{
	cpu->ime = 1;
}

void di(struct cpu* cpu)
{
	cpu->ime = 0;
}

void call_cond(struct cpu* cpu)
{
	u16 addr = READ16;
	if (flag(cpu)) {
		cpu->cyc += 12;
		push16(cpu, cpu->PC);
		cpu->PC = addr;
	}
}

void push_r16(struct cpu* cpu)
{
	u16* r16 = cpu->r16g3[(cpu->op & 0x30) >> 4];
	push16(cpu, *r16);
}

void call_u16(struct cpu* cpu)
{
	u16 addr = READ16;
	push16(cpu, cpu->PC);
	cpu->PC = addr;
}

void alu_a_u8(struct cpu* cpu)
{
	u8 loc = (cpu->op & 0x38) >> 3;
	op2[loc](cpu, READ8);
}

void rst(struct cpu* cpu)
{
	push16(cpu, cpu->PC);
	cpu->PC = cpu->op & 0x38;
}

void ldi_hl_a(struct cpu* cpu)
{
	WRITE(cpu->HL++, HI(AF));
}

void ldi_a_hl(struct cpu* cpu)
{
	HI(AF) = READ(cpu->HL++);
}

void ldd_hl_a(struct cpu* cpu)
{
	WRITE(cpu->HL--, HI(AF));
}

void ldd_a_hl(struct cpu* cpu)
{
	HI(AF) = READ(cpu->HL--);
}

void shift_rot(struct cpu* cpu)
{
	op3[(cpu->op & 0x38) >> 3](cpu);
}

void bit_res_set(struct cpu* cpu)
{
	u8 dat = low_get(cpu),
		bit = 1 << ((cpu->op >> 3) & 7);

	switch ((cpu->op & 0xc0) >> 6) {
	case 1: /*BIT b,r*/
		CLRF(NF);
		SETF(HF);
		TOGIF(ZF, (!(dat & bit)));
		return;
	case 2: /*RES b,r*/
		dat &= ~bit;
		break;
	case 3: /*SET b,r*/
		dat |= bit;
		break;
	}

	low_set(cpu, dat);
}