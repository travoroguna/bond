//
// Created by travor on 14/07/2023.
//


#include "debug.h"

namespace bond {
    void debug_break() {
#ifdef _WIN32
        DebugBreak();
#else
        raise(SIGTRAP);
#endif
    }
}