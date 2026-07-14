#ifndef FGR_H
#define FGR_H

#include <stdbool.h>

// will be linked during compilation
int start_randomization(bool load_item_rando, int seed); // returns 0 on success 1 on failure
void init_rando();
void deinit_rando();

#endif //FGR_H
