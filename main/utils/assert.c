#ifndef PC_SIMULATOR
#include "hardwareprofile.h"

void assert(int expr) {
    if (!expr) {
        __builtin_software_breakpoint();
        for (;;) {
            if (expr) {
                break;
            }
            Nop();
        }
    }
}

#endif
