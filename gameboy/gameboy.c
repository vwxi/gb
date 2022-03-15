#include "gameboy.h"

int gb_init(char** argv)
{
	long sz = 0;
	FILE* fp = NULL;

	gb.cpu.mmu = &gb.mmu;
	gb.lcd.cpu = &gb.cpu;
	gb.mmu.cart = &gb.cart;
	
	fp = fopen(argv[1], "rb");
	if (!fp) {
		printf("could not open file %s\n", argv[1]);
		return 0;
	}

	printf("reading file %s\n", argv[1]);

	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	printf("ROM size %u bytes\n", sz);

	if (!cart_load(&gb.cart, fp, sz)) {
		fclose(fp);
		return 0;
	}

	if (!cart_ram_init(&gb.cart)) {
		printf("RAM allocation failure\n");
		goto end;
	}

	if (!cart_get_mbc_type(&gb.cart)) {
		printf("could not get MBC type\n");
		goto end;
	}

	cpu_reset(&gb.cpu);

	if (!lcd_init(&gb.lcd)) {
		goto end;
	}

	return 1;
end:
	fclose(fp);
	cart_free(&gb.cart);
	lcd_destroy(&gb.lcd);
	return 0;
}

void gb_update_joypad()
{
	u8 temp;
	gb.joyp &= J_SELECT;
	
	temp = !(gb.joyp & J_ACTION) ? ~gb.buttons :
		!(gb.joyp & J_DIRECT) ? ~gb.dpad :
		0xff;

	gb.joyp &= ~J_SELECT;
	gb.joyp |= temp;
}

void gb_kbd(int e, int k)
{
	switch (e) {
	case SDL_KEYDOWN:
		gb.cpu.IF |= JOYP_INTERRUPT;
		
		switch (k) {
		case SDLK_UP:
			gb.dpad |= J_UP;
			break;
		case SDLK_DOWN:
			gb.dpad |= J_DOWN;
			break;
		case SDLK_LEFT:
			gb.dpad |= J_LEFT;
			break;
		case SDLK_RIGHT:
			gb.dpad |= J_RIGHT;
			break;
		case SDLK_z:
			gb.buttons |= J_LEFT;
			break;
		case SDLK_x:
			gb.buttons |= J_RIGHT;
			break;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			gb.buttons |= J_UP;
			break;
		case SDLK_RETURN:
			gb.buttons |= J_DOWN;
			break;
		}

		break;

	case SDL_KEYUP:
		switch (k) {
		case SDLK_UP:
			gb.dpad &= ~J_UP;
			break;
		case SDLK_DOWN:
			gb.dpad &= ~J_DOWN;
			break;
		case SDLK_LEFT:
			gb.dpad &= ~J_LEFT;
			break;
		case SDLK_RIGHT:
			gb.dpad &= ~J_RIGHT;
			break;
		case SDLK_z:
			gb.buttons &= ~J_LEFT;
			break;
		case SDLK_x:
			gb.buttons &= ~J_RIGHT;
			break;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			gb.buttons &= ~J_UP;
			break;
		case SDLK_RETURN:
			gb.buttons &= ~J_DOWN;
			break;
		}

		break;
	}

	gb_update_joypad();
}

void gb_run()
{
	u8 quit = 0;
	while (!quit) {
		while (gb.cpu.cycles < 70224) {
			cpu_tick(&gb.cpu);
			cpu_update_timer(&gb.cpu);
			cpu_handle_interrupts(&gb.cpu);
			lcd_tick(&gb.lcd, gb.cpu.cyc);
		}

		gb.cpu.cycles -= 70224;

		SDL_PollEvent(&gb.lcd.e);

		switch (gb.lcd.e.type) {
		case SDL_QUIT: quit = 1; break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			gb_kbd(gb.lcd.e.type, gb.lcd.e.key.keysym.sym);
			break;
		}
	}
}