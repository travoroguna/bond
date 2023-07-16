//
// Created by travor on 06/07/2023.
//


#include "../object.h"

namespace bond {
    obj_result c_Function(const t_vector &args) {
    Function* func;
    auto res = parse_args(args, func);
    TRY(res);
    return OK(args[0]);
}

    void Function::mark() {
        if (m_marked)
            return;

        NativeInstance::mark();
        m_globals->mark();
        m_code->mark();
    }

    void Function::unmark() {
        if (!m_marked)
            return;

        NativeInstance::unmark();
        m_globals->unmark();
        m_code->unmark();
    }

    obj_result F_get_attribute(const GcPtr<Object>& Self, const t_vector &args) {
        auto self = Self->as<Function>();
        String* n;
        auto res = parse_args(args,n);
        TRY(res);

        auto name = n->get_value();

        if (name == "__name__") {
            return OK(make_string(self->get_name()));
        }

        return ERR("Attribute " + name + " not found");
    }


GcPtr<NativeStruct> FUNCTION_STRUCT = make_immortal<NativeStruct>("Function", "Function(value)", c_Function, method_map {
        {"__getattr__", {F_get_attribute, "__getattr__(name: String)"}},
});

GcPtr<NativeStruct> NATIVE_FUNCTION_STRUCT = make_immortal<NativeStruct>("NativeFunction", "NativeFunction(value)",
                                                                         c_Default<NativeFunction>);
}