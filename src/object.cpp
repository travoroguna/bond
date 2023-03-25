#include "object.h"

namespace bond {
    OBJ_RESULT Number::$add(const GcPtr<Object> &other) {
        auto res = const_cast<GcPtr<Object> &>(other)
                .use_if<Number, GcPtr<Number>>([&](Number &other_number) {
                    return GarbageCollector::instance().make<Number>(m_value + other_number.get_value());
                });

        if (res.has_value()) {
            return res.value();
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    OBJ_RESULT Number::$sub(const GcPtr<Object> &other) {
    }

    OBJ_RESULT Number::$mul(const GcPtr<Object> &other) {
    }

    OBJ_RESULT Number::$div(const GcPtr<Object> &other) {
    }

//    std::expected<GcPtr<Object>, RuntimeError> Bool::$add(const GcPtr<Object> &other) {
//        return Object::$add(other);
//    }
//
//    std::expected<GcPtr<Object>, RuntimeError> String::$add(const GcPtr<Object> &other) {
//        return Object::$add(other);
//    }
    OBJ_RESULT String::$add(const GcPtr<Object> &other) {
        auto res = const_cast<GcPtr<Object> &>(other)
                .use_if<String, GcPtr<String>>([&](String &other_string) {
                    return GarbageCollector::instance().make<String>(m_value + other_string.get_value());
                });

        if (res.has_value()) {
            return res.value();
        }
        return std::unexpected(RuntimeError::TypeError);
    }
}