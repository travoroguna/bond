//
// Created by Travor Oguna Oneya on 22/04/2023.
//

#pragma once

#include "gc.h"

namespace bond {
    struct FunctionError {
        std::string message;
        RuntimeError error;

        FunctionError(std::string message, RuntimeError error)
                : message(std::move(message)), error(error) {}
    };

    using NativeErrorOr = std::expected<GcPtr<Object>, FunctionError>;
    using NativeFunctionPtr = std::function<NativeErrorOr(const std::vector<GcPtr<Object>> &args)>;

    NativeErrorOr Ok(const GcPtr<Object> &ok);

    NativeErrorOr Err(const GcPtr<Object> &err);

    NativeErrorOr Err(const std::string &err);

#define MAKE_METHOD(name) bond::GarbageCollector::instance().make<bond::NativeFunction>([this](auto && ARGS){ return name (std::forward<decltype(ARGS)>(ARGS)); }, #name)
#define BIND(name) [this](auto && ARGS){ return name (std::forward<decltype(ARGS)>(ARGS)); }
#define MAKE_FORM_BIND(bind) bond::GarbageCollector::instance().make<bond::NativeFunction>((bind))

#define ASSERT_ARG_COUNT(X, ARGS) \
  if ((ARGS).size() != (X)) return std::unexpected(bond::FunctionError(fmt::format("expected {} arguments got {}", (X), (ARGS).size()), bond::RuntimeError::InvalidArgument))

#define DEFINE(NAME, TYPE, INDEX, ARGUMENTS) \
  bond::GcPtr<TYPE> NAME;  \
  (NAME).set(dynamic_cast<TYPE*>( (ARGUMENTS) [INDEX].get())); \
  if ((NAME).get() == nullptr) return std::unexpected(bond::FunctionError(fmt::format("expected {} as argument {} got {}", #TYPE, (INDEX), (ARGUMENTS) [INDEX]->str()), bond::RuntimeError::InvalidArgument))


}
