#include "cpu.h"
#include "ops.h"
#include "debugger.h"

void cpu_reset(struct cpu* cpu)
{
	CLRALL;
	cpu->AF = 0x01b0;
	cpu->BC = 0x0013;
	cpu->DE = 0x00d8;
	cpu->HL = 0x014d;
	cpu->SP = 0xfffe;
	cpu->PC = 0x0100;
	cpu->op = 0;
	cpu->ime = 0;
	cpu->cycles = 0;
	cpu->halted = 0;
	cpu->div = 0;
	cpu->timer_enable = 0;
	cpu->timer_length = 1024;

	cpu->r16g1[0] = &cpu->BC;
	cpu->r16g1[1] = &cpu->DE;
	cpu->r16g1[2] = &cpu->HL;
	cpu->r16g1[3] = &cpu->SP;

	cpu->r16g2[0] = &cpu->BC;
	cpu->r16g2[1] = &cpu->DE;
	cpu->r16g2[2] = 0;
	cpu->r16g2[3] = 0;

	cpu->r16g3[0] = &cpu->BC;
	cpu->r16g3[1] = &cpu->DE;
	cpu->r16g3[2] = &cpu->HL;
	cpu->r16g3[3] = &cpu->AF;

	io_reset(cpu->mmu);
}

void cpu_tick(struct cpu* cpu)
{
	cpu->cyc = 0;

	if (!cpu->halted) {
		cpu->op = READ8;
		cpu->cyc += cyc_table[cpu->op] * 4;

		/*dbg_print_cpu_data(cpu, cpu->op);*/
		op_list[cpu->op].fp(cpu);
	}
	else {
		cpu->cyc = 4;
		if (cpu->IF) cpu->halted = 0;
	}

	cpu->cycles += cpu->cyc;
}

void cpu_update_timer(struct cpu* cpu)
{
	/* update timers */
	cpu->div_internal += cpu->cyc;
	if (cpu->div_internal >= 0x100) {
		cpu->div_internal -= 0x100;
		cpu->div++;
	}

	if (cpu->timer_enable) {
		cpu->tima_internal += cpu->cyc;
		while (cpu->tima_internal >= cpu->timer_length) {
			cpu->tima_internal -= cpu->timer_length;
			if (!++cpu->tima) {
				cpu->tima = cpu->tma;
				cpu->IF |= TIMER_INTERRUPT;
			}
		}
	}
}

u8 cpu_serve_interrupt(struct cpu* cpu, u8 interrupt, u16 jmp)
{
	if (!(cpu->IF & interrupt) || !(cpu->IE & interrupt))
		return 0;

	push16(cpu, cpu->PC);

	cpu->IF &= ~interrupt;
	cpu->ime = 0;
	cpu->halted = 0;
	cpu->PC = jmp;

	return 1;
}

void cpu_handle_interrupts(struct cpu* cpu)
{
	if (cpu->ime) {
		if (cpu_serve_interrupt(cpu, VBLANK_INTERRUPT, 0x40)) 
			return;
		if (cpu_serve_interrupt(cpu, STAT_INTERRUPT, 0x48)) 
			return;
		if (cpu_serve_interrupt(cpu, TIMER_INTERRUPT, 0x50))
			return;
		if (cpu_serve_interrupt(cpu, SERIAL_INTERRUPT, 0x58)) 
			return;
		if (cpu_serve_interrupt(cpu, JOYP_INTERRUPT, 0x60)) 
			return;
	}
}