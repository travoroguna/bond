//
// Created by Travor Oguna Oneya on 05/04/2023.
//

#pragma once

#include "object.h"

namespace bond {
    void add_builtins_to_globals(const GcPtr<StringMap> &globals);
    obj_result b_help(const t_vector &args);
}
