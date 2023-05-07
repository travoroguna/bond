//
// Created by Travor Oguna Oneya on 06/05/2023.
//

#include "globs.h"

namespace bond::Globs {
    auto tr = bond::Bool(true);
    auto fl = bond::Bool(false);
    auto nl = bond::Nil();

    bond::GcPtr<bond::Bool> BondTrue = bond::GcPtr<bond::Bool>(&tr);
    bond::GcPtr<bond::Bool> BondFalse = bond::GcPtr<bond::Bool>(&fl);
    bond::GcPtr<bond::Nil> BondNil = bond::GcPtr<bond::Nil>(&nl);

    std::recursive_mutex m_recursive_mutex;
}