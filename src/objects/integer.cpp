#include "../object.h"

namespace bond {

    OBJ_RESULT Integer::$add(const GcPtr<Object> &other) {
        if (!other->is<Integer>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        return GarbageCollector::instance().make<Integer>(m_value + other->as<Integer>()->get_value());
    }

    OBJ_RESULT Integer::$sub(const GcPtr<Object> &other) {
        if (!other->is<Integer>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        return GarbageCollector::instance().make<Integer>(m_value - other->as<Integer>()->get_value());
    }

    OBJ_RESULT Integer::$mul(const GcPtr<Object> &other) {
        if (!other->is<Integer>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        return GarbageCollector::instance().make<Integer>(m_value * other->as<Integer>()->get_value());
    }

    OBJ_RESULT Integer::$div(const GcPtr<Object> &other) {
        auto res = const_cast<GcPtr<Object> &>(other)
                .use_if<Integer, std::optional<GcPtr<Integer>>>(
                        [&](Integer &other_number) -> std::optional<GcPtr<Integer>> {
                            if (other_number.get_value() == 0) {
                                return std::nullopt;
                            }
                            return GarbageCollector::instance().make<Integer>(m_value / other_number.get_value());

                        });

        if (res.has_value()) {
            if (res.value().has_value()) {
                return res.value().value();
            }
            return std::unexpected(RuntimeError::DivisionByZero);
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    std::string Integer::str() {
        return fmt::format("{}", m_value);
    }

    bool Integer::equal(const GcPtr<Object> &other) {
        if (!is<Integer>(other)) return false;
        return m_value == as<Integer>(other)->get_value();
    }

    size_t Integer::hash() {
        return std::hash<intmax_t>{}(m_value);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Integer::$eq(const GcPtr<Object> &other) {
        return BOOL_(this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Integer::$ne(const GcPtr<Object> &other) {
        return BOOL_(!this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Integer::$lt(const GcPtr<Object> &other) {
        if (!is<Integer>(other)) return std::unexpected(RuntimeError::TypeError);
        return BOOL_(m_value < as<Integer>(other)->get_value());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Integer::$le(const GcPtr<Object> &other) {
        if (!is<Integer>(other)) return std::unexpected(RuntimeError::TypeError);
        return BOOL_(m_value <= as<Integer>(other)->get_value());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Integer::$gt(const GcPtr<Object> &other) {
        if (!is<Integer>(other)) return std::unexpected(RuntimeError::TypeError);
        return BOOL_(m_value > as<Integer>(other)->get_value());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Integer::$ge(const GcPtr<Object> &other) {
        if (!is<Integer>(other)) return std::unexpected(RuntimeError::TypeError);
        return BOOL_(m_value >= as<Integer>(other)->get_value());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Integer::$_bool() {
        return BOOL_(m_value != 0);
    }

    std::expected<GcPtr<Object>, RuntimeError> Integer::$mod(const GcPtr<Object> &other) {
        if (!is<Integer>(other)) return std::unexpected(RuntimeError::TypeError);
        return GarbageCollector::instance().make<Integer>(m_value % as<Integer>(other)->get_value());
    }
}