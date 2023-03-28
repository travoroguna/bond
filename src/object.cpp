#include "object.h"
#include <functional>


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
                .use_if<Number, std::optional<GcPtr<Number>>>(
                        [&](Number &other_number) -> std::optional<GcPtr<Number>> {
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

    bool Number::equal(const GcPtr<Object> &other) {
        if (!is<Number>(other)) return false;
        return m_value == as<Number>(other)->get_value();
    }

    size_t Number::hash() {
        return std::hash<float>{}(m_value);
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

    bool String::equal(const GcPtr<Object> &other) {
        if (!is<String>(other)) return false;
        return m_value == as<String>(other)->get_value();
    }

    size_t String::hash() {
        return std::hash<std::string>{}(m_value);
    }

    std::string String::str() {
        return fmt::format("\"{}\"", m_value);
    }

    std::string Bool::str() {
        return m_value ? "true" : "false";
    }

    bool Bool::equal(const GcPtr<Object> &other) {
        if (!is<Bool>(other)) return false;
        return m_value == as<Bool>(other)->get_value();
    }

    size_t Bool::hash() {
        return std::hash<bool>{}(m_value);
    }

    std::string Nil::str() {
        return "nil";
    }

    bool Nil::equal(const GcPtr<Object> &other) {
        return is<Nil>(other);
    }

    size_t Nil::hash() {
        //Todo: find a good way to hash as all nill
        //      values are the same
        return 7654345678900987654;
    }

    void Map::set(const GcPtr<Object> &key, const GcPtr<Object> &value) {
        m_internal_map[key] = value;
    }

    bool Map::has(const GcPtr<Object> &key) {
        return m_internal_map.contains(key);
    }

    std::optional<GcPtr<Object>> Map::get(const GcPtr<Object> &key) {
        if (has(key)) {
            return m_internal_map[key];
        }

        return std::nullopt;
    }

    GcPtr<Object> Map::get_unchecked(const GcPtr<Object> &key) {
        return m_internal_map[key];

    }


}