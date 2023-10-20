#include "../object.h"
#include "../runtime.h"


namespace bond {
    GcPtr<Instance> Struct::create_instance(const t_map &fields) {
        return Runtime::ins()->make_instance(this, fields);
    }

    void Struct::set_globals(const GcPtr<StringMap> &globals) {
        m_globals = globals;

        for (auto const &[_, value]: m_methods) {
            value->set_globals(globals);
        }
    }


    std::optional<obj_result> Struct::get_attr(const t_string &name) {
        auto attr = NativeInstance::get_attr(name);
        if (attr.has_value()) {
            return attr;
        }

        if (!m_methods.contains(name))
            return std::nullopt;

        return m_methods.at(name);
    }

    std::optional<GcPtr<Function>> Struct::get_method(const t_string &name) const {
        if (!m_methods.contains(name))
            return std::nullopt;
        return m_methods.at(name);
    }


    obj_result c_Struct(const t_vector &args) {
        Struct *strct;
        auto res = parse_args(args, strct);
        TRY(res);
        return OK(args[0]);
    }


    void init_struct() {
        Runtime::ins()->STRUCT_STRUCT = make_immortal<NativeStruct>("Type",
                                                                        "Type(name: String, fields: Map, methods: Map)",
                                                                        c_Struct);
    }
};