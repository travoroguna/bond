//
// Created by Travor Oguna Oneya on 03/04/2023.
//

#ifndef BOND_SRC_API_H_
#define BOND_SRC_API_H_

#include "gc.h"
#include "object.h"

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif

namespace bond {

#define ASSERT_ARG_COUNT(X, ARGS) \
  if ((ARGS).size() != (X)) return std::unexpected(bond::FunctionError(fmt::format("expected {} arguments got {}", (X), (ARGS).size()), bond::RuntimeError::InvalidArgument))

#define DEFINE(NAME, TYPE, INDEX, ARGUMENTS) \
  bond::GcPtr<TYPE> NAME;  \
  NAME.set(dynamic_cast<TYPE*>(arguments[INDEX].get())); \
  if (NAME.get() == nullptr) return std::unexpected(bond::FunctionError(fmt::format("expected {} as argument {} got {}", #TYPE, (INDEX), arguments[INDEX]->str()), bond::RuntimeError::InvalidArgument))

    NativeErrorOr native_add(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr Ok(const GcPtr<Object> &ok);

    NativeErrorOr Err(const GcPtr<Object> &err);

    NativeErrorOr Err(const std::string &err);
};
#endif //BOND_SRC_API_H_
