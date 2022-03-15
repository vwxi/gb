#include "gameboy.h"

int main(int argc, char* argv[])
{
	switch (argc) {
	case 2: if (!gb_init(argv)) return 0; break; /* ROM file only */
		/*TODO: boot rom option*/
	default: printf("USAGE: %s [ROM file]\n", argv[0]); return 0;
	}

	gb_run();

	return 0;
}