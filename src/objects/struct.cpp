#include "../object.h"


namespace bond {
    Struct::Struct(const GcPtr<String> &name, const std::vector<GcPtr<String>> &instance_variables) {
        m_name = name;
        m_instance_variables = instance_variables;
        m_methods = GarbageCollector::instance().make_immortal<Map>();
    }


    OBJ_RESULT Struct::$get_attribute(const GcPtr<Object> &index) {
        auto attr = get_method(index);
        if (attr.has_value()) return attr.value();
        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    void Struct::mark() {
        Object::mark();

        for (auto &name: m_instance_variables) name.mark();
        m_methods.mark();
    }

    void Struct::unmark() {
        Object::unmark();

        for (auto &name: m_instance_variables) name.unmark();
        m_methods.unmark();
    }

    void Struct::set_globals(const GcPtr<Map> &globals) {
        m_globals = globals;

        for (auto &[_, meth]: m_methods->get_map()) {
            auto func = meth->as<Function>();
            func->set_globals(m_globals);
        }
    }

    std::expected<bool, std::string> Struct::is_instance(const GcPtr<Object> &other) {
        if (is<StructInstance>(other)) {
            auto instance = as<StructInstance>(other);
            return instance->get_struct_type() == this;
        }
        return false;
    }

};