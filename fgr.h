#ifndef FGR_H
#define FGR_H

#include <stdbool.h>
#include <windows.h>

#define MAX_SEED_SIZE 16

typedef struct {
	char Seed[MAX_SEED_SIZE];
	int ItemRando; // Use data from item-rando or own data
	int ExcludePVP; // Remove PVP gates from the randomized gates pool
} Config;

Config load_config();
void save_config(Config c);

// will be linked during compilation
int start_randomization(Config config); // returns 0 on success 1 on failure
void init_rando();
void deinit_rando();

#ifdef CONFIG_IMPLEMENTATION
const char *config_path = ".\\settings.ini";

Config load_config() {
	Config c = {0};
	GetPrivateProfileString("Config", "Seed", "666666", c.Seed, sizeof(c.Seed), config_path);
	c.ItemRando = GetPrivateProfileInt("Config", "ItemRando", 0, config_path);
	c.ExcludePVP = GetPrivateProfileInt("Config", "ExcludePVP", 0, config_path);
	return c;
}

void save_config(Config c) {
	WritePrivateProfileString("Config", "ItemRando", c.ItemRando == 1 ? "1" : "0", config_path);
	WritePrivateProfileString("Config", "Seed", c.Seed, config_path);
	WritePrivateProfileString("Config", "ExcludePVP", c.ExcludePVP == 1 ? "1" : "0", config_path);
}

#endif // CONFIG_IMPLEMENTATION

#endif //FGR_H
