//
// Created by travor on 14/07/2023.
//

#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace bond {
    void debug_break();
}