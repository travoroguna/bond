//
// Created by Travor Oguna Oneya on 05/04/2023.
//

#pragma once

#include "api.h"
#include "object.h"

namespace bond {

    void add_builtins_to_globals(const GcPtr<Map> &globals);
}
