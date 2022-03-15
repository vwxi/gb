#pragma once

#include "def.h"
#include "mem.h"

#define FREQ 4194304

#define VBLANK_INTERRUPT 0x01
#define STAT_INTERRUPT   0x02
#define TIMER_INTERRUPT  0x04
#define SERIAL_INTERRUPT 0x08
#define JOYP_INTERRUPT   0x10 /*UNDEF*/

struct cpu {
	u8 op;

	u16 AF;
	u16 BC;
	u16 DE;
	u16 HL;

	u16 PC;
	u16 SP;

	u64 cycles;

	u8 stopped;
	u8 halted;
	u8 ime;
	
	u8 IE;
	u8 IF;

	u8 cyc;

	u8 timer_enable;
	u16 timer_length;

	u8 div;
	u8 tima;
	u8 tma;
	u8 tac;

	u16 div_internal;
	u16 tima_internal;

	u16* r16g1[4];
	u16* r16g2[4];
	u16* r16g3[4];

	struct mmu* mmu;
};

void cpu_reset(struct cpu*);
void cpu_tick(struct cpu*);
void cpu_update_timer(struct cpu*);
u8 cpu_serve_interrupt(struct cpu*, u8, u16);
void cpu_handle_interrupts(struct cpu*);