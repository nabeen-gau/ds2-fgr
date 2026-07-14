#include <stdio.h>
#include <windows.h>

#include "fgr.h"

int main() {
	bool load_item_rando = false;
	int seed = 6996;

	init_rando();
	int ret = start_randomization(load_item_rando, seed);
	if (ret != 0) { 
		printf("Randomization Failed!\n");
	}
	deinit_rando();

	return 0;
}
