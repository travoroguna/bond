//
// Created by Travor Oguna Oneya on 05/04/2023.
//

#pragma once

#include "api.h"
#include "object.h"

namespace bond {
NativeErrorOr println(const std::vector<GcPtr<Object>> &arguments);
void add_builtins_to_globals(GcPtr<Map> &globals);
}
