#include "../object.h"


namespace bond {
    OBJ_RESULT String::$add(const GcPtr<Object> &other) {
        auto
                res = const_cast<GcPtr<Object> &>(other)
                .use_if<String, GcPtr<String >>([&](String &other_string) {
                    return GarbageCollector::instance().make<String>(m_value + other_string.get_value());
                });

        if (res.has_value()) {
            return res.value();
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    bool String::equal(const GcPtr<Object> &other) {
        if (!is<String>(other)) return false;
        return m_value == as<String>(other)->get_value();
    }

    std::expected<GcPtr<Object>, RuntimeError>

    String::$eq(const GcPtr<Object> &other) {
        return BOOL_(this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    String::$ne(const GcPtr<Object> &other) {
        return BOOL_(!this->equal(other));
    }

    size_t String::hash() {
        return std::hash<std::string>{}(m_value);
    }

    std::string String::str() {
        return fmt::format("\"{}\"", m_value);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    String::$_bool() {
        return BOOL_(!m_value.empty());
    }
};