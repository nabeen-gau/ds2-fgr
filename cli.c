#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "fgr.h"

// program_name <seed> <load_item_rando>
int main(int argc, char **argv) {
	int seed = 848693776;
	bool load_item_rando = false;
	if (argc >= 2) {
		int ret = atoi(argv[1]);
		if (ret != 0) seed = ret;
		else {
			printf("[ERROR] Invalid seed `%s`.\n", argv[1]);
			return 1;
		}
	} 
	if (argc >= 3) {
		load_item_rando = atoi(argv[2]) == 1;
	}
	if (argc > 3) {
		printf("[WARNING] Unnecessary additional arguments provided.\n");
	}

	init_rando();
	int ret = start_randomization(load_item_rando, seed);
	if (ret != 0) { 
		printf("Randomization Failed!\n");
	}
	deinit_rando();

	return 0;
}
