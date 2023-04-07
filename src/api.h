//
// Created by Travor Oguna Oneya on 03/04/2023.
//

#ifndef BOND_SRC_API_H_
#define BOND_SRC_API_H_

#include "gc.h"
#include "object.h"

namespace bond {

#define ASSERT_ARG_COUNT(X, ARGS) \
  if ((ARGS).size() != (X)) return std::unexpected(bond::FunctionError(fmt::format("expected {} arguments got {}", (X), (ARGS).size()), bond::RuntimeError::InvalidArgument))

#define DEFINE(NAME, TYPE, INDEX, ARGUMENTS) \
  bond::GcPtr<TYPE> NAME;  \
  NAME.set(dynamic_cast<TYPE*>(arguments[INDEX].get())); \
  if (NAME.get() == nullptr) return std::unexpected(bond::FunctionError(fmt::format("expected number as argument {} got {}", (INDEX), arguments[INDEX]->type_name()), bond::RuntimeError::InvalidArgument))

NativeErrorOr native_add(const std::vector<GcPtr<Object>> &arguments);

};
#endif //BOND_SRC_API_H_
