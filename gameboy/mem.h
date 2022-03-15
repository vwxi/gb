#pragma once

#include "def.h"
#include "cart.h"

struct mmu {
	u8 VRAM[0x2000];
	u8 WRAM[0x2000];
	u8 OAM[0xa0];
	u8 HRAM[0x80];

	struct cart* cart;
};

void io_reset();
u8 mmu_read(struct mmu*, u16);
void mmu_write(struct mmu*, u16, u8);
void mmu_dma(struct mmu*, u16);