#include "mem.h"
#include "io.h"

void io_reset()
{
	io_write(0xFF05, 0x00);
	io_write(0xFF06, 0x00);
	io_write(0xFF07, 0x00);
	io_write(0xFF10, 0x80);
	io_write(0xFF11, 0xBF);
	io_write(0xFF12, 0xF3);
	io_write(0xFF14, 0xBF);
	io_write(0xFF16, 0x3F);
	io_write(0xFF17, 0x00);
	io_write(0xFF19, 0xBF);
	io_write(0xFF1A, 0x7F);
	io_write(0xFF1B, 0xFF);
	io_write(0xFF1C, 0x9F);
	io_write(0xFF1E, 0xBF);
	io_write(0xFF20, 0xFF);
	io_write(0xFF21, 0x00);
	io_write(0xFF22, 0x00);
	io_write(0xFF23, 0xBF);
	io_write(0xFF24, 0x77);
	io_write(0xFF25, 0xF3);
	io_write(0xFF26, 0xF1);
	io_write(0xFF40, 0x91);
	io_write(0xFF42, 0x00);
	io_write(0xFF43, 0x00);
	io_write(0xFF45, 0x00);
	io_write(0xFF47, 0xFC);
	io_write(0xFF48, 0xFF);
	io_write(0xFF49, 0xFF);
	io_write(0xFF4A, 0x00);
	io_write(0xFF4B, 0x00);
	io_write(0xFFFF, 0x00);
}

u8 mmu_read(struct mmu* mmu, u16 addr)
{
	if (addr <= 0x7fff) return cart_read(mmu->cart, addr);
	if (addr >= 0x8000 && addr <= 0x9fff) return mmu->VRAM[addr - 0x8000];
	if (addr >= 0xa000 && addr <= 0xbfff) return cart_read(mmu->cart, addr);
	if (addr >= 0xc000 && addr <= 0xdfff) return mmu->WRAM[addr - 0xc000];
	if (addr >= 0xe000 && addr <= 0xfdff) return mmu->WRAM[addr - 0xe000];
	if (addr >= 0xfe00 && addr <= 0xfe9f) return mmu->OAM[addr - 0xfe00];
	if (addr >= 0xff00 && addr <= 0xff7f) return io_read(addr);
	if (addr >= 0xff80 && addr <= 0xfffe) return mmu->HRAM[addr - 0xff80];
	if (addr == 0xffff) return io_read(addr);

	return 0xff;
}

void mmu_write(struct mmu* mmu, u16 addr, u8 val)
{
	if (addr <= 0x7fff) cart_write(mmu->cart, addr, val);
	if (addr >= 0x8000 && addr <= 0x9fff) mmu->VRAM[addr - 0x8000] = val;
	if (addr >= 0xa000 && addr <= 0xbfff) cart_write(mmu->cart, addr, val);
	if (addr >= 0xc000 && addr <= 0xdfff) mmu->WRAM[addr - 0xc000] = val;
	if (addr >= 0xe000 && addr <= 0xfdff) mmu->WRAM[addr - 0xe000] = val;
	if (addr >= 0xfe00 && addr <= 0xfe9f) mmu->OAM[addr - 0xfe00] = val;
	if (addr >= 0xff00 && addr <= 0xff7f) io_write(addr, val);
	if (addr >= 0xff80 && addr <= 0xfffe) mmu->HRAM[addr - 0xff80] = val;
	if (addr == 0xffff) io_write(addr, val);
}

void mmu_dma(struct mmu* mmu, u16 addr)
{
	u8 d, a = 0;
	
	for (; a < 0xa0; a++) {
		d = mmu_read(mmu, addr+a);
		mmu_write(mmu, 0xfe00 + a, d);
	}
}