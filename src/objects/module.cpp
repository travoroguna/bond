//
// Created by travor on 07/07/2023.
//

#include "../object.h"


namespace bond {

    Module::Module(std::string path, const t_map &objects) {
        m_path = std::move(path);

        m_globals = MAP_STRUCT->create_instance<Map>();
        for (auto const &[name, object]: objects) {
            m_globals->set(name, object);
        }
    }

    obj_result Module::get_attribute(const std::string &name) {
//        auto meth = m_native_struct->get_method(name);

//        if (meth.has_value()) {
//            return OK(meth.value());
//        }
//
        auto res = m_globals->get(name);
        if (res.has_value()) {
            return OK(res.value());
        } else {
            return ERR(fmt::format("AttributeError: module '{}' has no attribute '{}'", get_path(), name));
        }
    }

    obj_result get_attribute(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<Module>();
        String *name;

        auto opt = parse_args(args, name);
        TRY(opt);

        auto res = self->get_attribute(name->get_value());

        if (res.has_value()) {
            return OK(res.value());
        } else {
            return ERR(fmt::format("AttributeError: module '{}' has no attribute '{}'", self->get_path(),
                                   name->get_value()));
        }
    }

    obj_result get_values(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<Module>();
        auto list = LIST_STRUCT->create_instance<List>();

        for (auto const &[_, value]: self->get_globals()->get_value()) {
            list->append(value);
        }

        return OK(list);
    }


    GcPtr<NativeStruct> MODULE_STRUCT = make_immortal<NativeStruct>("Module", "Module(path: String, globals: Map)",
                                                                    c_Default<Module>, method_map{
                    {"__getattr__", {get_attribute, "getattr(name)"}},
                    {"get_exports", {get_values, "get_values()"}}
            });

}
