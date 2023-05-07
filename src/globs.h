//
// Created by Travor Oguna Oneya on 06/05/2023.
//

#pragma once


#include "object.h"
#include <mutex>


namespace bond::Globs {
    extern bond::GcPtr<bond::Bool> BondTrue;
    extern bond::GcPtr<bond::Bool> BondFalse;
    extern bond::GcPtr<bond::Nil> BondNil;
}