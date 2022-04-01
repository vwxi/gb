#include "io.h"

u8 io_read(u16 addr)
{
	switch (addr) {
	case 0xff00: /*joypad*/ return gb.joyp;
	case 0xff04: /*div*/ return gb.cpu.div;
	case 0xff05: /*tima*/ return gb.cpu.tima;
	case 0xff06: /*tma*/ return gb.cpu.tma;
	case 0xff07: /*tac*/ return gb.cpu.tac;
	case 0xff0f: /*if*/ return gb.cpu.IF;
	/*TODO: ADD APU REGISTERS*/
	case 0xff24: /*nr50*/ return gb.apu.ch_ctrl;
	case 0xff40: /*lcdc*/ return gb.lcd.lcdc;
	case 0xff41: /*stat*/ return gb.lcd.stat;		
	case 0xff42: /*scy*/ return gb.lcd.scy;
	case 0xff43: /*scx*/ return gb.lcd.scx;
	case 0xff44: /*ly*/ return gb.lcd.ly;
	case 0xff45: /*lyc*/ return gb.lcd.lyc;
	case 0xff47: /*bgp*/ return gb.lcd.bgp;
	case 0xff48: /*obp0*/ return gb.lcd.obp0;
	case 0xff49: /*obp1*/ return gb.lcd.obp1;
	case 0xff4a: /*wy*/ return gb.lcd.wy;
	case 0xff4b: /*wx*/ return gb.lcd.wx;
	case 0xffff: /*ie*/ return gb.cpu.IE;
	default: return 0xff;
	}
}

void io_write(u16 addr, u8 val)
{
	switch (addr) {
	case 0xff00: /*joypad*/
		gb.joyp &= ~J_SELECT;
		gb.joyp |= val & J_SELECT;
		gb_update_joypad();
		break; 
	case 0xff04: /*div*/ gb.cpu.div = 0; break;
	case 0xff05: /*tima*/ gb.cpu.tima = val; break;
	case 0xff06: /*tma*/ gb.cpu.tma = val; break;
	case 0xff07: /*tac*/
		gb.cpu.tac = val;
		gb.cpu.timer_enable = (val & 4) >> 2;
		switch (val & 0x3) {
		case 1: gb.cpu.timer_length = 16; break;
		case 2: gb.cpu.timer_length = 64; break;
		case 3: gb.cpu.timer_length = 256; break;
		case 0: gb.cpu.timer_length = 1024; break;
		}
		break;
	case 0xff0f: /*if*/	gb.cpu.IF = val; break;
	/*TODO: ADD APU REGISTERS*/
	case 0xff24: /*nr50*/ gb.apu.ch_ctrl = val;	break;
	case 0xff40: /*lcdc*/ gb.lcd.lcdc = val; break;
	case 0xff41: /*stat*/ gb.lcd.stat = val; break;
	case 0xff42: /*scy*/ gb.lcd.scy = val; break;
	case 0xff43: /*scx*/ gb.lcd.scx = val; break;
	case 0xff45: /*lyc*/ gb.lcd.lyc = val; break;
	case 0xff46: /*dma*/ mmu_dma(&gb.mmu, val << 8); break;
	case 0xff47: /*bgp*/ gb.lcd.bgp = val; break;
	case 0xff48: /*obp0*/ gb.lcd.obp0 = val; break;
	case 0xff49: /*obp1*/ gb.lcd.obp1 = val; break;
	case 0xff4a: /*wy*/ gb.lcd.wy = val; break;
	case 0xff4b: /*wx*/ gb.lcd.wx = val; break;
	case 0xffff: /*ie*/ gb.cpu.IE = val; break;
	}
}