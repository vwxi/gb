#pragma once

#include "def.h"

struct cart {
	u8 MBC;
	
	u8 rom_bank;
	
	u8 zero_bank;
	u8 high_bank;

	u8 ram_bank;
	u8 ram_enable;
	u8 mode_select;

	u8 rom_bank_count;
	u8 ram_bank_count;

	/* MBC3 */
	u8 rtc_supported;

	u8 rtc_mode;
	u8 rtc_register;
	u8 rtc_latch;

	time_t rtc_time;
	time_t rtc_old_time;

	u64 rtc_seconds;
	u64 rtc_minutes;
	u64 rtc_hours;
	u64 rtc_days;
	u8 rtc_ctrl;

	long rom_size;
	long ram_size;

	u8* ROM;
	u8* RAM;
};

int  cart_load(struct cart*, FILE*, long);
void cart_free(struct cart*);
int  cart_ram_init(struct cart*);
int  cart_get_mbc_type(struct cart*);
u8   cart_read(struct cart*, u16);
void cart_write(struct cart*, u16,   u8);