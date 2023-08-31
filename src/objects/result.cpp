//
// Created by travor on 10/07/2023.
//

#include "../object.h"
#include "../runtime.h"

namespace bond {

    obj_result Result_construct(const t_vector &args) {
        Object* value;
        Bool* is_error;
        TRY(parse_args(args, value, is_error));
        return make_result(value, is_error->get_value());
    }

    obj_result Result_is_ok(const GcPtr<Object>& Self, const t_vector &args) {
        auto self = Self->as<Result>();
        TRY(parse_args(args));
        return AS_BOOL(self->has_value());
    }

    obj_result  Result_is_error(const GcPtr<Object>& Self, const t_vector &args) {
        auto self = Self->as<Result>();
        TRY(parse_args(args));
        return AS_BOOL(self->has_error());

    }

    obj_result Result_value(const GcPtr<Object>& Self, const t_vector &args) {
        auto self = Self->as<Result>();
        TRY(parse_args(args));
        if (self->has_value()) {
            return self->get_value();
        } else {
            return ERR("Attempted to get value of error Result");
        }
    }

    obj_result  Result_error(const GcPtr<Object>& Self, const t_vector &args) {
        auto self = Self->as<Result>();
        TRY(parse_args(args));
        if (self->has_error()) {
            return self->get_value();
        } else {
            return ERR("Attempted to get error of value Result");
        }
    }

    obj_result  Result_or_else(const GcPtr<Object>& Self, const t_vector &args) {
        auto self = Self->as<Result>();
        Object* else_value;
        TRY(parse_args(args, else_value));
        if (self->has_value()) {
            return self->get_value();
        } else {
            return else_value;
        }
    }

    void init_result() {
        Runtime::ins()->RESULT_STRUCT = make_immortal<NativeStruct>("Result",
                                                                        "Result(value: Object, error: Object)",
                                                                        Result_construct, method_map{
                        {"is_ok",    {Result_is_ok,    "is_ok() -> Bool"}},
                        {"is_error", {Result_is_error, "is_error() -> Bool"}},
                        {"value",    {Result_value,    "value() -> Any"}},
                        {"error",    {Result_error,    "error() -> Any"}},
                        {"or_else",  {Result_or_else,  "or_else(else_value: Any) -> Any"}}

                });
    }

}