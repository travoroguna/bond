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
        auto res = const_cast<GcPtr<Object> &>(other)
                .use_if<Number, GcPtr<Number>>([&](Number &other_number) {
                    return GarbageCollector::instance().make<Number>(m_value - other_number.get_value());
                });

        if (res.has_value()) {
            return res.value();
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    OBJ_RESULT Number::$mul(const GcPtr<Object> &other) {
        auto res = const_cast<GcPtr<Object> &>(other)
                .use_if<Number, GcPtr<Number>>([&](Number &other_number) {
                    return GarbageCollector::instance().make<Number>(m_value * other_number.get_value());
                });

        if (res.has_value()) {
            return res.value();
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    OBJ_RESULT Number::$div(const GcPtr<Object> &other) {
        auto res = const_cast<GcPtr<Object> &>(other)
                .use_if<Number, std::optional<GcPtr<Number>>>([&](Number &other_number) -> std::optional<GcPtr<Number>> {
                    if (other_number.get_value() == 0) {
                        return std::nullopt;
                    }
                    return GarbageCollector::instance().make<Number>(m_value / other_number.get_value());

                });

        if (res.has_value()) {
            if (res.value().has_value()) {
                return res.value().value();
            }
            return std::unexpected(RuntimeError::DivisionByZero);
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    std::string Number::str() {
        return std::to_string(m_value);
    }


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

    std::string String::str() {
        return fmt::format("\"{}\"", m_value);
    }

    std::string Bool::str() {
        return m_value ? "true" : "false";
    }

    std::string Nil::str() {
        return "nil";
    }
}