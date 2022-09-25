#include "lcd.h"

const u32 colors[4] = {
	0xff4fbc9b, /*white*/
	0xff4fac8b, /*light gray*/
	0xff706230, /*dark gray*/
	0xff4f380f  /*black*/
};

u8 lcd_init(struct lcd* lcd)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("couldn't init sdl, error: %s\n", SDL_GetError());
		return 0;
	}

	lcd->window = SDL_CreateWindow(
		"game boy emulator", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		320, 
		288,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	if (!lcd->window) {
		printf("couldn't init window, error: %s\n", SDL_GetError());
		return 0;
	}

	lcd->renderer = SDL_CreateRenderer(
		lcd->window,
		-1,
		SDL_RENDERER_ACCELERATED |
		SDL_RENDERER_PRESENTVSYNC
	);

	if (!lcd->renderer) {
		printf("couldn't init renderer, error: %s\n", SDL_GetError());
		return 0;
	}

	lcd->texture = SDL_CreateTexture(
		lcd->renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING, 
		160,
		144
	);

	if (!lcd->texture) {
		printf("couldn't create texture, error: %s\n", SDL_GetError());
		return 0;
	}

	lcd->cycles = 0;
	lcd->scan = 0;
	lcd->line_drawn = 0;
	memset(lcd->pixels, 0, 160 * 144);

	return 1;
}

void lcd_destroy(struct lcd* lcd)
{
	if (SDL_WasInit(SDL_INIT_EVERYTHING) != 0) {
		SDL_DestroyTexture(lcd->texture);
		SDL_DestroyRenderer(lcd->renderer);
		SDL_DestroyWindow(lcd->window);
		SDL_Quit();
	}
}

void lcd_check_lyc(struct lcd* lcd)
{
	if (lcd->ly == lcd->lyc) {
		lcd->stat |= S_LYC;
		if (lcd->stat & S_INT_LYC)
			lcd->cpu->IF |= STAT_INTERRUPT;
	}
	else {
		lcd->stat &= ~S_LYC;
	}
}

void lcd_mode(struct lcd* lcd, u8 mode)
{
	u8 int_enabled = 0;

	lcd->stat &= ~S_MODE;
	lcd->stat |= mode;

	switch (mode) {
	case HBLANK:
		int_enabled = lcd->stat & S_INT_HBLANK;
		break;

	case VBLANK:
		int_enabled = lcd->stat & S_INT_VBLANK;
		break;

	case OAMSCAN:
		int_enabled = lcd->stat & S_INT_OAM;
		break;
	}

	if (int_enabled)
		lcd->cpu->IF |= STAT_INTERRUPT;
}

void lcd_tick(struct lcd* lcd, u8 cycles)
{
	lcd->cycles += cycles;
	
	if (lcd->lcdc & L_LCD_ENABLE) {
		switch (lcd->stat & S_MODE) {
		case OAMSCAN:
			if (lcd->cycles >= 80) {
				lcd->cycles -= 80;
				lcd_mode(lcd, LCDTRANSFER);
			}
			
			break;

		case LCDTRANSFER:
			if (lcd->cycles >= 172) {
				lcd->cycles -= 172;
				lcd_mode(lcd, HBLANK);
				lcd_draw(lcd);
			}

			break;

		case HBLANK:
			if (lcd->cycles >= 204) {
				lcd->cycles -= 204;
				if (++lcd->ly >= 144) {
					lcd_mode(lcd, VBLANK);
					lcd->cpu->IF |= VBLANK_INTERRUPT;
				}
				else {
					lcd_mode(lcd, OAMSCAN);
				}

				lcd_check_lyc(lcd);
			}

			break;

		case VBLANK:
			if (lcd->cycles >= 456) {
				lcd->cycles -= 456;
				if (++lcd->ly == 154) {
					lcd->ly = 0;
					lcd->wilc = 0xff;
					lcd_check_lyc(lcd);
					lcd_mode(lcd, OAMSCAN);
					lcd_paint(lcd);
				}
			}

			break;
		}
	}
	else {
		lcd->stat &= ~S_MODE;
		lcd->ly = 0;
		lcd->cycles = 0;
	}
}

/* draw 1 scanline's worth of background/window tiles */
void lcd_draw_bg(struct lcd* lcd)
{
	u16 tilemap = (lcd->lcdc & L_TILEMAP_SELECT) ? 0x9c00 : 0x9800,
		w_tilemap = (lcd->lcdc & L_WIN_TILEMAP_SELECT) ? 0x9c00 : 0x9800,
		tiledata = (lcd->lcdc & L_BGWIN_SELECT) ? 0x8000 : 0x8800;

	u16 tile, tile_addr;
	u8 i = 0, wx, x, y, num, col, row, hi, lo, idx, pcl;

	wx = lcd->wx < 7 ? 7 - lcd->wx : lcd->wx - 7;

	if (lcd->ly >= lcd->wy &&
		lcd->wx >= 0 &&
		lcd->wx <= 166)
		lcd->wilc++;

	for (; i < 160; i++) {
		if ((lcd->lcdc & L_WIN_ENABLE) && 
			lcd->wy <= lcd->ly &&
			wx <= i) {
			/* render window */
			x = i - wx;
			y = (lcd->wilc > 0) ? lcd->wilc : lcd->ly - lcd->wy;
			lcd->wilc = (lcd->wilc > 0) ? y : lcd->wilc;
			tilemap = w_tilemap;
		}
		else {
			/* render bg */
			x = i + lcd->scx;
			y = lcd->ly + lcd->scy;
		}

		row = y % 8 << 1;
		col = x % 8;

		num = lREAD(tilemap + ((y / 8) * 32) + (x / 8));

		if (!(lcd->lcdc & L_BGWIN_SELECT))
			num = (num & 127) - (num & 128) + 128;

		tile_addr = tiledata + (num * 16) + row;
		tile = lREAD(tile_addr);
		tile |= lREAD(tile_addr + 1) << 8;

		hi = ((tile >> 8) >> (7 - col)) & 1;
		lo = ((tile & 0xff) >> (7 - col)) & 1;

		idx = (hi << 1) | lo;
		pcl = (lcd->bgp >> (idx * 2)) & 3;

		PX(i, lcd->ly) = colors[pcl];
	}
}

int spr_cmp_x(const void* a, const void* b)
{
	struct oam_entry s1 = *(struct oam_entry*)a,
		s2 = *(struct oam_entry*)b;

	return s1.x <= s2.x;
}

/* draw 1 scanline's worth of sprites */
/* this is garbage, refactor later */
void lcd_draw_sprites(struct lcd* lcd)
{
	u16 tile, addr;
	u8 i, j = 0, k, height, hi, lo, col, 
		row, pal, idx, no, pcl, x;
	struct oam_entry sprite, sprites[10] = { 0 };

	if (!(lcd->lcdc & L_OBJ_ENABLE))
		return;

	height = (lcd->lcdc & L_OBJ_SIZE) ? 16 : 8;

	for (i = 0; i < 40 && j != 10; i++) {
		sprite.y = lREAD(0xfe00 + (i * 4));
		sprite.x = lREAD(0xfe00 + (i * 4) + 1);
		sprite.tile = lREAD(0xfe00 + (i * 4) + 2);
		sprite.flags = lREAD(0xfe00 + (i * 4) + 3);
		
		if (sprite.x == 0 && sprite.x >= 168)
			continue;

		if (sprite.y - 16 <= lcd->ly &&
			lcd->ly < sprite.y - 16 + height)
			sprites[j++] = sprite;
	}

	qsort(sprites, j, sizeof(struct oam_entry), spr_cmp_x);

	for (i = 0; i < j; i++) {
		pal = (sprites[i].flags & A_PAL_NUM) ? lcd->obp1 : lcd->obp0;
		row = (sprites[i].flags & A_Y_FLIP) ?
			((sprites[i].y - 16 + height) - lcd->ly - 1) * 2 :
			(lcd->ly - ((sprites[i].y - 16))) * 2;
		no = (lcd->lcdc & L_OBJ_SIZE) ? sprites[i].tile & ~1 : sprites[i].tile;

		addr = 0x8000 + (no * 16) + row;
		tile = lREAD(addr);
		tile |= lREAD(addr + 1) << 8;

		for (k = 0; k < 8; k++) {
			x = sprites[i].x - 8 + k;

			col = (sprites[i].flags & A_X_FLIP) ? k : 7 - k;

			hi = ((tile >> 8) >> col) & 1;
			lo = ((tile & 0xff) >> col) & 1;

			idx = (hi << 1) | lo;
			pcl = (pal >> (idx * 2)) & 3;

			if (idx) {
				if (sprites[i].flags & A_BGWIN_PRIORITY) {
					if(pcl && PX(x, lcd->ly) == colors[0])
						PX(x, lcd->ly) = colors[pcl];
				}
				else {
					PX(x, lcd->ly) = colors[pcl];
				}
			}
		}
	}
}

/* hacky, yucky, eww. if bg/win is disabled, draw palette color 0 */
void lcd_draw_nothing(struct lcd* lcd)
{
	u8 i = 0;
	for (; i < 160; i++) {
		PX(i, lcd->ly) = colors[0];
	}
}

/* sets up colors for single line */
void lcd_draw(struct lcd* lcd)
{
	/* BG & win */
	if (lcd->lcdc & L_BGWIN_ENABLE) {
		lcd_draw_bg(lcd);
		lcd_draw_sprites(lcd);
	}
	else lcd_draw_nothing(lcd);
}

/* paint to screen */
void lcd_paint(struct lcd* lcd)
{
	SDL_RenderClear(lcd->renderer);
	SDL_UpdateTexture(lcd->texture, NULL, lcd->pixels, 160 * sizeof(u32));
	SDL_RenderCopy(lcd->renderer, lcd->texture, NULL, NULL);
	SDL_RenderPresent(lcd->renderer);
}
