#pragma once

#include "cpu.h"
#include "mem.h"
#include "cart.h"
#include "lcd.h"
#include "debugger.h"

#define FRAME_CYCLES 70224

#define J_ACTION (1 << 5)
#define J_DIRECT (1 << 4)
#define J_DOWN   (1 << 3)
#define J_UP     (1 << 2)
#define J_LEFT   (1 << 1)
#define J_RIGHT  (1 << 0)
#define J_SELECT (J_ACTION | J_DIRECT)

struct gameboy {
	struct cpu cpu;
	struct mmu mmu;
	struct cart cart;
	struct lcd lcd;

	u8 dpad;
	u8 buttons;
	u8 joyp;
} gb;

int gb_init(char**);
int gb_bootrom_init(char**);
void gb_run();
void gb_update_joypad();