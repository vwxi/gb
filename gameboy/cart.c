#include "cart.h"

const u8 mbc_masks[7] = {
	0x00, 0x03, 0x07, 0x0f, 0x1f, 0x1f, 0x1f
};

int cart_load(struct cart* cart, FILE* fp, long sz)
{
	size_t rd = 0;

	cart->rom_size = sz;
	cart->rom_bank = 1;
	cart->zero_bank = 0;
	cart->high_bank = 1;
	cart->ram_bank = 0;
	cart->rtc_time = time(NULL);

	cart->ROM = malloc(sz + 1);
	if (!cart->ROM) {
		printf("cannot allocate ROM\n");
		return 0;
	}

	rd = fread(cart->ROM, 1, sz, fp);
	if (!rd) {
		printf("failed to load cart ROM\n");
		return 0;
	}

	if (!cart->ROM) {
		printf("empty ROM :(\n");
		return 0;
	}

	cart->rom_bank_count = cart->ROM[0x148];
	cart->ram_bank_count = cart->ROM[0x149];

	return 1;
}

void cart_free(struct cart* cart)
{
	if (cart->ROM) free(cart->ROM);
	if (cart->RAM) free(cart->RAM);
}

int cart_ram_init(struct cart* cart)
{
	long ram_sizes[5] = {
		0, 0x800, 0x2000, 0x8000, 0x200000
	};

	cart->ram_size = (cart->MBC != 2) ? ram_sizes[cart->ram_bank_count] : ram_sizes[1];

	printf("ram size %#x\n", cart->ram_size);

	cart->RAM = malloc(cart->ram_size + 1);

	return !!cart->RAM;
}

int cart_get_mbc_type(struct cart* cart)
{
	if (!cart->ROM) return 0;

	switch (cart->ROM[0x147]) {
	case 0x0: case 0x8: case 0x9:
		cart->MBC = 0;
		puts("mapper: none");
		break;
	case 0x1: case 0x2: case 0x3:
		cart->MBC = 1;
		puts("mapper: mbc1");
		break;
	case 0x5: case 0x6:
		cart->MBC = 2;
		puts("mapper: mbc2");
		break;
	case 0xf: case 0x10:
		cart->MBC = 3;
		puts("mapper: mbc3");
		break;
	case 0x11: case 0x12: case 0x13:
		puts("mapper: mbc3+rtc");
		cart->rtc_supported = 1;
		cart->MBC = 3;
		break;
	default:
		printf("unsupported, type: %x\n", cart->ROM[0x147]);
		cart->MBC = 0xff;
		return 0;
	}

	return 1;
}

u8 nombc_read(struct cart* cart, u16 addr)
{
	if (addr <= 0x7fff)
		return cart->ROM[addr];

	if (addr >= 0xa000 && addr <= 0xbfff)
		return cart->RAM[addr - 0xa000];

	return 0xff;
}

void nombc_write(struct cart* cart, u16 addr, u8 val)
{
	if (addr >= 0xa000 && addr <= 0xbfff)
		cart->RAM[addr - 0xa000] = val;
}

u8 mbc1_read(struct cart* cart, u16 addr)
{
	/* rom bank 00 */
	if (addr <= 0x3fff)
		if (cart->mode_select)
			return cart->ROM[addr + (cart->zero_bank * 0x4000)];
		else
			return cart->ROM[addr];

	/* rom bank 01-7f */
	if (addr >= 0x4000 && addr <= 0x7fff)
		return cart->ROM[(addr - 0x4000) + (cart->high_bank * 0x4000)];
	
	/* ram bank 00-03 */
	if (addr >= 0xa000 && addr <= 0xbfff)
		if (cart->ram_enable && cart->RAM)
			if (cart->ram_bank_count <= 2)
				return cart->RAM[(addr - 0xa000) % cart->ram_size];
			else if (cart->ram_bank_count == 3 && cart->mode_select)
				return cart->RAM[(addr - 0xa000) + (cart->ram_bank * 0x2000)];
			else
				return cart->RAM[addr - 0xa000];

	return 0xff;
}

void mbc1_write(struct cart* cart, u16 addr, u8 val)
{
	/* ram enable */
	if (addr <= 0x1fff)
		cart->ram_enable = (val & 0xf) == 0xa;

	/* lower 5 bits of rom bank number */
	if (addr >= 0x2000 && addr <= 0x3fff) {
		cart->rom_bank = val & 0x1f;
		if (!cart->rom_bank) cart->rom_bank = 1;
		cart->rom_bank &= mbc_masks[cart->rom_bank_count];

		if (cart->rom_bank_count <= 4) {
			cart->zero_bank = 0;
			cart->high_bank = cart->rom_bank;
		}
		else {
			if (cart->rom_bank_count == 5) {
				cart->zero_bank = (cart->ram_bank & 1) << 5;
				cart->high_bank = cart->rom_bank | ((cart->ram_bank & 1) << 5);
			}
			else {
				cart->zero_bank = cart->ram_bank << 5;
				cart->high_bank = cart->rom_bank | (cart->ram_bank << 5);
			}
		}
	}

	/* ram bank OR upper rom bank bits */
	if (addr >= 0x4000 && addr <= 0x5fff)
		cart->ram_bank = val & 3;

	/* ROM/RAM banking modes */
	if (addr >= 0x6000 && addr <= 0x7fff)
		cart->mode_select = val & 1;

	if (addr >= 0xa000 && addr <= 0xbfff)
		if (cart->ram_enable && cart->RAM)
			if (cart->ram_bank_count <= 2)
				cart->RAM[(addr - 0xa000) % cart->ram_size] = val;
			else if (cart->ram_bank_count == 3 && cart->mode_select)
				cart->RAM[(addr - 0xa000) + (cart->ram_bank * 0x2000)] = val;
			else
				cart->RAM[addr - 0xa000] = val;

}

u8 mbc2_read(struct cart* cart, u16 addr)
{
	/* ROM bank 00 */
	if (addr <= 0x3fff)
		return cart->ROM[addr];

	/* ROM bank 01-0f */
	if (addr >= 0x4000 && addr <= 0x7fff)
		return cart->ROM[(addr - 0x4000) + (cart->rom_bank * 0x4000)];

	/* internal RAM */
	if (addr >= 0xa000 && addr <= 0xbfff)
		if (cart->ram_enable)
			return 0xf0 | cart->RAM[addr & 0x1ff];

	return 0xff;
}

void mbc2_write(struct cart* cart, u16 addr, u8 val)
{
	if (addr <= 0x3fff) {
		if (addr & 0x100) {
			/* ROM bank */
			cart->rom_bank = val & 0xf;
			if (!cart->rom_bank) cart->rom_bank = 1;
			cart->rom_bank &= mbc_masks[cart->rom_bank_count];
		}
		else {
			/* RAM enable */
			cart->ram_enable = (val & 0xf) == 0xa;
		}
	}

	if (addr >= 0xa000 && addr <= 0xbfff)
		if(cart->ram_enable)
			cart->RAM[addr & 0x1ff] = val & 0xf;
}

void mbc3_update_rtc(struct cart* cart)
{
	/*update RTC registers*/
	time_t difference;

	cart->rtc_old_time = cart->rtc_time;
	cart->rtc_time = time(NULL);

	difference = cart->rtc_time - cart->rtc_old_time;

	if (cart->rtc_ctrl & 0x40)
		return; /* timer halt bit*/

	if (difference < 0 || cart->rtc_old_time == cart->rtc_time) {
		return;
	}

	cart->rtc_seconds += difference % 60;
	cart->rtc_minutes += (difference / 60) % 60;
	cart->rtc_hours += (difference / 3600) % 24;
	cart->rtc_days += difference / 86400;

	if (cart->rtc_seconds > 59) {
		cart->rtc_seconds -= 60;
		cart->rtc_minutes++;
	}

	if (cart->rtc_minutes > 59) {
		cart->rtc_minutes -= 60;
		cart->rtc_hours++;
	}

	if (cart->rtc_hours > 23) {
		cart->rtc_hours -= 24;
		cart->rtc_days++;
	}

	if (cart->rtc_days == 0x100) {
		cart->rtc_days -= 0x100;
		if (cart->rtc_ctrl & 1) {
			cart->rtc_ctrl &= ~1;
			cart->rtc_ctrl |= 0x80;
		}
		else {
			cart->rtc_ctrl |= 1;
		}
	}
}

u8 mbc3_read(struct cart* cart, u16 addr)
{
	if (addr <= 0x3fff)
		return cart->ROM[addr];

	if (addr >= 0x4000 && addr <= 0x7fff)
		return cart->ROM[(addr - 0x4000) + (cart->rom_bank * 0x4000)];

	if (addr >= 0xa000 && addr <= 0xbfff)
		if (cart->ram_enable) {
			if (cart->rtc_supported && cart->rtc_mode)
				switch (cart->rtc_register) {
				case 0x08: return cart->rtc_seconds & 0xff; break;
				case 0x09: return cart->rtc_minutes & 0xff; break;
				case 0x0a: return cart->rtc_hours & 0xff; break;
				case 0x0b: return cart->rtc_days & 0xff; break;
				case 0x0c: return cart->rtc_ctrl & 0xc1; break;
				}
			else if(cart->RAM && !cart->rtc_mode)
				return cart->RAM[(addr - 0xa000) + (cart->ram_bank * 0x2000)];
		}

	return 0xff;
}

void mbc3_write(struct cart* cart, u16 addr, u8 val)
{
	if (addr <= 0x1fff)
		if(cart->RAM)
			cart->ram_enable = (val & 0xf) == 0xa;

	if (addr >= 0x2000 && addr <= 0x3fff) {
		if (!val) val = 1;
		cart->rom_bank = val & 0x7f;
	}

	if (addr >= 0x4000 && addr <= 0x5fff) {
		if (val <= 3) {
			cart->ram_bank = val;
			cart->rtc_mode = 0; /*RAM banking*/
		}
		else if (val >= 8 && val <= 0xc) {
			cart->rtc_register = val;
			cart->rtc_mode = 1; /*RTC registers*/
		}
	}

	if (addr >= 0x6000 && addr <= 0x7fff) {
		if (cart->ram_enable && cart->rtc_latch == 0 && val == 1) {
			mbc3_update_rtc(cart);
		}

		cart->rtc_latch = val;
	}

	if (addr >= 0xa000 && addr <= 0xbfff) {
		if (cart->ram_enable) {
			if (cart->RAM && !cart->rtc_mode)
				cart->RAM[(addr - 0xa000) + (cart->ram_bank * 0x2000)] = val;
			else if (cart->rtc_supported && cart->rtc_mode)
				switch (cart->rtc_register) {
				case 0x08: cart->rtc_seconds = val; break;
				case 0x09: cart->rtc_minutes = val; break;
				case 0x0a: cart->rtc_hours = val; break;
				case 0x0b: cart->rtc_days = val; break;
				case 0x0c: cart->rtc_ctrl = val; break;
				}
		}
	}
}

u8 cart_read(struct cart* cart, u16 addr)
{
	switch (cart->MBC) {
	case 0: return nombc_read(cart, addr);
	case 1: return mbc1_read(cart, addr);
	case 2: return mbc2_read(cart, addr);
	case 3: return mbc3_read(cart, addr);
	}

	return 0xff;
}

void cart_write(struct cart* cart, u16 addr, u8 val)
{
	switch (cart->MBC) {
	case 0: nombc_write(cart, addr, val); break;
	case 1: mbc1_write(cart, addr, val); break;
	case 2: mbc2_write(cart, addr, val); break;
	case 3: mbc3_write(cart, addr, val); break;
	}
}