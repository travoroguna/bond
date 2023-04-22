//
// Created by Travor Oguna Oneya on 05/04/2023.
//

#pragma once

#include "api.h"
#include "object.h"

namespace bond {
    NativeErrorOr println(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr range(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr str(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr int_(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr float_(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr Ok_(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr Err_(const std::vector<GcPtr<Object>> &arguments);

    void add_builtins_to_globals(GcPtr<Map> &globals);

    NativeErrorOr is_instance(const std::vector<GcPtr<Object>> &arguments);
}
