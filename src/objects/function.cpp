//
// Created by travor on 06/07/2023.
//



#include "../object.h"
#include "../runtime.h"

namespace bond {
    obj_result c_Function(const t_vector &args) {
        Function *func;
        auto res = parse_args(args, func);
        TRY(res);
        return OK(args[0]);
    }


    obj_result F_get_attribute(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<Function>();
        String *n;
        auto res = parse_args(args, n);
        TRY(res);

        auto name = n->get_value();

        if (name == "__name__") {
            return OK(make_string(self->get_name()));
        }

        return runtime_error(fmt::format("AttributeError: function '{}' has no attribute '{}'", self->get_name(), name)) ;
    }


    void init_function() {
        Runtime::ins()->FUNCTION_STRUCT = make_immortal<NativeStruct>("Function", "Function(value)", c_Function,
                                                                          method_map{
                                                                                  {"__getattr__", {F_get_attribute,
                                                                                                   "__getattr__(name: String)"}},
                                                                          });

        Runtime::ins()->NATIVE_FUNCTION_STRUCT = make_immortal<NativeStruct>("NativeFunction",
                                                                                 "NativeFunction(value)",
                                                                                 c_Default<NativeFunction>);
    }

    GcPtr<Result> make_result(const GcPtr<Object> &value, bool is_error) {
        return Runtime::ins()->make_result(value, is_error);
    }

    GcPtr<Float> make_float(double value) {
        return Runtime::ins()->make_float(value);
    }

    GcPtr<List> make_list(const t_vector &values) {
        return Runtime::ins()->make_list(values);
    }

    GcPtr<Int> make_int(int64_t value) {
        return Runtime::ins()->make_int(value);
    }

    GcPtr<String> make_string(const t_string& value) {
        return Runtime::ins()->make_string(value);
    }
}