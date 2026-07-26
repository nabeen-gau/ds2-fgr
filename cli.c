#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define CONFIG_IMPLEMENTATION
#include "fgr.h"

// program_name <seed> <load_item_rando>
int main(int argc, char **argv) {
	Config config = load_config();
	printf("c.Seed: %s\n", config.Seed);
	printf("c.ItemRando: %d\n", config.ItemRando);
	printf("c.ExcludePVP: %d\n", config.ExcludePVP);
	printf("\n");

	if (argc >= 2) {
		int ret = atoi(argv[1]);
		if (ret != 0) {
			memcpy(config.Seed, argv[1], min(strlen(argv[1]), sizeof(config.Seed)));
		} else {
			printf("[ERROR] Invalid seed `%s`.\n", argv[1]);
			return 1;
		}
	} 
	if (argc >= 3) {
		config.ItemRando = atoi(argv[2]) == 1;
	}
	if (argc > 3) {
		printf("[WARNING] Unnecessary additional arguments provided.\n");
	}

	init_rando();
	int ret = start_randomization(config);
	if (ret != 0) { 
		printf("Randomization Failed!\n");
	}
	deinit_rando();

	save_config(config);

	return 0;
}
