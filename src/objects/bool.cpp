#include "../object.h"


namespace bond {
    std::string Bool::str() {
        return m_value ? "true" : "false";
    }

    bool Bool::equal(const GcPtr<Object> &other) {
        if (!is<Bool>(other)) return false;
        return m_value == as<Bool>(other)->get_value();
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Bool::$eq(const GcPtr<Object> &other) {
        return BOOL_(this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Bool::$ne(const GcPtr<Object> &other) {
        return BOOL_(!this->equal(other));
    }

    size_t Bool::hash() {
        return std::hash<bool>{}(m_value);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Bool::$_bool() {
        return BOOL_(m_value);
    }
}