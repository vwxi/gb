#pragma once

#include "cpu.h"
#include <SDL.h>

#define S_SIZE (160 * 144)

#define HBLANK 0
#define VBLANK 1
#define OAMSCAN 2
#define LCDTRANSFER 3

#define L_BGWIN_ENABLE       (1 << 0)
#define L_OBJ_ENABLE         (1 << 1)
#define L_OBJ_SIZE           (1 << 2)
#define L_TILEMAP_SELECT     (1 << 3)
#define L_BGWIN_SELECT       (1 << 4)
#define L_WIN_ENABLE         (1 << 5)
#define L_WIN_TILEMAP_SELECT (1 << 6)
#define L_LCD_ENABLE         (1 << 7)

#define A_CGB_PAL_NUM        3
#define A_VRAM_BANK          (1 << 3)
#define A_PAL_NUM            (1 << 4)
#define A_X_FLIP             (1 << 5)
#define A_Y_FLIP             (1 << 6)
#define A_BGWIN_PRIORITY     (1 << 7)

#define S_MODE 3
#define S_LYC         (1 << 2)
#define S_INT_LYC     (1 << 6)

#define MODE(n, i) \
	lcd->o_il = lcd->il; \
	lcd->stat &= ~S_MODE; \
	lcd->stat |= n; \
	if((lcd->il && !lcd->o_il) && lcd->stat & (1 << i)) { \
		lcd->il = 1; \
		lcd->cpu->IF |= STAT_INTERRUPT; \
	}

#define lREAD(a) mmu_read(lcd->cpu->mmu, a)
#define lWRITE(a,w) mmu_write(lcd->cpu->mmu, a, w)

#define PX(x,y) lcd->pixels[((x)+(y * 160)) % S_SIZE]

struct oam_entry {
	u8 i;
	u8 y;
	u8 x;
	u8 tile;
	u8 flags;
};

struct lcd {
	u16 cycles;
	u8 scan;
	u8 line_drawn;

	u8 lcdc;
	u8 stat;
	u8 scy;
	u8 scx;
	u8 ly;
	u8 lyc;
	u8 bgp;
	u8 obp0;
	u8 obp1;
	u8 wy;
	u8 wx;
	u8 wilc; /* internal line counter */

	u8 il; /* interrupt on line */
	u8 o_il;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event e;

	u32 pixels[S_SIZE];

	struct cpu* cpu;
};

u8 lcd_init(struct lcd*);
void lcd_destroy(struct lcd*);
void lcd_tick(struct lcd*, u8);
void lcd_draw(struct lcd*);
void lcd_paint(struct lcd*);