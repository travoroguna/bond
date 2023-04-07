//
// Created by Travor Oguna Oneya on 03/04/2023.
//


#include "api.h"

namespace bond {
NativeErrorOr native_add(const std::vector<GcPtr<Object>> &arguments) {
  ASSERT_ARG_COUNT(2, arguments);

  DEFINE(left, bond::Number, 0, arguments);
  DEFINE(right, bond::Number, 1, arguments);

  return GarbageCollector::instance().make<bond::Number>(left->get_value() + right->get_value());
}

};