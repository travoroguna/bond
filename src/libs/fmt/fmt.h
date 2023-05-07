//
// Created by Travor Oguna Oneya on 07/05/2023.
//

#pragma once

#include "../../bond.h"

namespace bond::fmt {
    NativeErrorOr print(const std::vector<GcPtr < Object>>

    &objects);

    NativeErrorOr println(const std::vector<GcPtr < Object>>

    &objects);

    NativeErrorOr format(const std::vector<GcPtr < Object>>

    &objects);
} // bond
