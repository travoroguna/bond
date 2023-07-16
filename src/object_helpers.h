//
// Created by Travor Oguna Oneya on 22/04/2023.
//

#pragma once

#include "gc.h"

namespace bond {
#define MAKE_METHOD(name) bond::GarbageCollector::instance().make<bond::NativeFunction>([this](auto && ARGS){ return name (std::forward<decltype(ARGS)>(ARGS)); }, #name)
#define BIND(name) [this](auto && ARGS){ return name (std::forward<decltype(ARGS)>(ARGS)); }
#define MAKE_FORM_BIND(bind) bond::GarbageCollector::instance().make<bond::NativeFunction>((bind))

}
