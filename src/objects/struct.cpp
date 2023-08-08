#include "../object.h"


namespace bond {
    GcPtr<Instance> Struct::create_instance(const t_map & fields)  {
        return INSTANCE_STRUCT->create_instance<Instance>(this, fields);
    }

    void Struct::set_globals(const GcPtr<StringMap> &globals) {
        m_globals = globals;

        for (auto const &[_, value]: m_methods) {
            value->set_globals(globals);
        }
    }


     std::optional<GcPtr<Function>> Struct::get_method(const std::string& name) const {
        if (!m_methods.contains(name))
            return std::nullopt;
        return m_methods.at(name);
    }



    obj_result c_Struct(const t_vector &args) {
        Struct* strct;
        auto res = parse_args(args, strct);
        TRY(res);
        return OK(args[0]);
    }


    GcPtr<NativeStruct> STRUCT_STRUCT = make_immortal<NativeStruct>("Type", "Type(name: String, fields: Map, methods: Map)", c_Struct);
};