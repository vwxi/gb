#pragma once

#include "def.h"

struct cart {
	u8 MBC;
	
	u8 rom_bank;
	u8 rom_bank_upper;

	u8 zero_bank;
	u8 high_bank;

	u8 ram_bank;
	u8 ram_enable;
	u8 mode_select;

	u8 rom_bank_count;
	u8 ram_bank_count;

	long rom_size;
	long ram_size;

	u8* ROM;
	u8* RAM;
};

int  cart_load              (struct cart*, FILE*, long);
void cart_free              (struct cart*);
int  cart_ram_init          (struct cart*);
int  cart_get_mbc_type      (struct cart*);
u8   cart_read              (struct cart*, u16);
void cart_write             (struct cart*, u16,   u8);