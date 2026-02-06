#define NOB_IMPLEMENTATION
#include "nob.h"

Cmd cmd = {0};
int main() {
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_output(&cmd, "./DS2FGR");
    nob_cc_inputs(&cmd, "./main.c");

    if (!cmd_run(&cmd)) return 1;
    return 0;
}
