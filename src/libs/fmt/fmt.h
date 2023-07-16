//
// Created by Travor Oguna Oneya on 07/05/2023.
//

#pragma once

#include "../../bond.h"

namespace bond::fmt {
    obj_result print(const t_vector &objects);
    obj_result println(const t_vector &objects);
    obj_result format(const t_vector &objects);
} // bond
