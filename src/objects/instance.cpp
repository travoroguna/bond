#include "../object.h"

namespace bond {
    StructInstance::StructInstance(const GcPtr<Struct> &struct_type) {
        m_struct_type = struct_type;
        m_attributes = GarbageCollector::instance().make<Map>();
    }

    void StructInstance::mark() {
        Object::mark();
        m_struct_type.mark();
        m_attributes.mark();
    }

    void StructInstance::unmark() {
        Object::unmark();
        m_struct_type.unmark();
        m_attributes.unmark();
    }


    OBJ_RESULT StructInstance::$set_attribute(const GcPtr<Object> &index, const GcPtr<Object> &value) {
        if (!is<String>(index)) return std::unexpected(RuntimeError::ExpectedStringIndex);
        auto name = as<String>(index)->get_value();

        if (m_attributes->has(index)) {
            m_attributes->set(index, value);
            return value;
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    OBJ_RESULT StructInstance::$get_attribute(const GcPtr<Object> &index) {
        if (!is<String>(index)) return std::unexpected(RuntimeError::ExpectedStringIndex);
        auto name = as<String>(index)->get_value();

        if (m_attributes->has(index)) {
            return m_attributes->get_unchecked(index);
        }

        if (auto method = m_struct_type->get_method(index)) {
            return GarbageCollector::instance().make<BoundMethod>(this, method.value());
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    OBJ_RESULT StructInstance::$set_item(const GcPtr<Object> &index, const GcPtr<Object> &value) {
        auto name = String("__set_item__");
        GcPtr<String> name_ptr = &name;

        if (auto method = m_struct_type->get_method(name_ptr)) {
            return GarbageCollector::instance().make<BoundMethod>(this, method.value());
        }

        return std::unexpected(RuntimeError::AttributeNotFound);;
    }

    OBJ_RESULT StructInstance::$get_item(const GcPtr<Object> &index) {
        auto name = String("__get_item__");
        GcPtr<String> name_ptr = &name;

        if (auto method = m_struct_type->get_method(name_ptr)) {
            return GarbageCollector::instance().make<BoundMethod>(this, method.value());
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    OBJ_RESULT StructInstance::user_method(const std::string &name) {
        auto name_ptr = GarbageCollector::instance().make<String>(name);
        if (auto method = m_struct_type->get_method(name_ptr)) {
            return GarbageCollector::instance().make<BoundMethod>(this, method.value());
        }
        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    OBJ_RESULT StructInstance::$add(const GcPtr<Object> &other) { return user_method("__add__"); }

    OBJ_RESULT StructInstance::$sub(const GcPtr<Object> &other) { return user_method("__sub__"); }

    OBJ_RESULT StructInstance::$mul(const GcPtr<Object> &other) { return user_method("__mul__"); }

    OBJ_RESULT StructInstance::$div(const GcPtr<Object> &other) { return user_method("__div__"); }

    OBJ_RESULT StructInstance::$lt(const GcPtr<Object> &other) { return user_method("__lt__"); }

    OBJ_RESULT StructInstance::$le(const GcPtr<Object> &other) { return user_method("__le__"); }

    OBJ_RESULT StructInstance::$gt(const GcPtr<Object> &other) { return user_method("__gt__"); }

    OBJ_RESULT StructInstance::$ge(const GcPtr<Object> &other) { return user_method("__ge__"); }

    std::string StructInstance::format_args() {
        std::string result;

        for (auto &[name, value]: m_attributes->get_map()) {
            result += fmt::format("{}={}, ", name->as<String>()->get_value(), value->str());
        }
        result = result.substr(0, result.size() - 2);
        return result;
    }


    OBJ_RESULT StructInstance::$eq(const GcPtr<Object> &other) {
        if (!other->is<StructInstance>()) return BOOL_(false);

        auto other_instance = other->as<StructInstance>();
        if (other_instance->get_struct_type() != m_struct_type) return BOOL_(false);

        for (auto &[name, value]: m_attributes->get_map()) {
            if (auto v = other_instance->get_attr(name)) {

                if (v == value) continue;
                return BOOL_(false);
            }
        }

        return BOOL_(true);
    }

    OBJ_RESULT StructInstance::$ne(const GcPtr<Object> &other) {
        return BOOL_(!$eq(other).value()->as<Bool>()->get_value());
    }
};