#include "debugger.h"

void dbg_print_cpu_data(struct cpu* cpu, u8 op)
{
#ifdef _DEBUG
	printf("AF=%04X BC=%04X DE=%04X "
		"HL=%04X PC=%04X SP=%04X FL=%c%c%c%c iTIMA=%04X DIV=%02X TIMA=%02X (%02X %02X %02X %02X) : ",
		cpu->AF, cpu->BC, cpu->DE,
		cpu->HL, cpu->PC - 1, cpu->SP,
		ISF(ZF) ? 'Z' : '-',
		ISF(NF) ? 'N' : '-',
		ISF(HF) ? 'H' : '-',
		ISF(CF) ? 'C' : '-',
		cpu->tima_internal,
		cpu->div, cpu->tima,
		READ(cpu->PC - 1),
		READ(cpu->PC),
		READ(cpu->PC + 1),
		READ(cpu->PC + 2)
	);

	if (op != 0xcb) {
		switch (op_list[op].type) {
		case 2: printf(op_list[op].name, READ(cpu->PC)); break;
		case 3: printf(op_list[op].name, READ(cpu->PC) | READ(cpu->PC + 1) << 8); break;
		case 5: printf(op_list[op].name, cpu->PC + (s8)READ(cpu->PC) + 1); break;
		default: case 1:
			printf(op_list[op].name);
			break;
		}
	}
	else {
		printf(op_cb_list[READ(cpu->PC)].name);
	}

	printf("\n");
	/*printf("A: %02X F: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X SP: %04X PC: 00:%04X (%02X %02X %02X %02X)\n",
		HI(AF), LO(AF),
		HI(BC), LO(BC),
		HI(DE), LO(DE),
		HI(HL), LO(HL),
		cpu->SP, cpu->PC-1,
		READ(cpu->PC - 1), READ(cpu->PC),
		READ(cpu->PC + 1), READ(cpu->PC + 2)
		);*/
#endif
}