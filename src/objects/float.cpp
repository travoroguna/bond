#include "../object.h"

namespace bond {

    OBJ_RESULT Float::$add(const GcPtr<Object> &other) {
        if (!other->is<Float>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        return GarbageCollector::instance().make<Float>(m_value + other->as<Float>()->get_value());
    }

    OBJ_RESULT Float::$sub(const GcPtr<Object> &other) {
        if (!other->is<Float>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        return GarbageCollector::instance().make<Float>(m_value - other->as<Float>()->get_value());
    }

    OBJ_RESULT Float::$mul(const GcPtr<Object> &other) {
        if (!other->is<Float>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        return GarbageCollector::instance().make<Float>(m_value * other->as<Float>()->get_value());
    }

    OBJ_RESULT Float::$div(const GcPtr<Object> &other) {
        if (!other->is<Float>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        auto other_float = other->as<Float>();

        if (other_float->get_value() == 0) {
            return std::unexpected(RuntimeError::DivisionByZero);
        }

        return GarbageCollector::instance().make<Float>(m_value / other_float->get_value());
    }

    std::string Float::str() {
        return fmt::format("{:.30g}", m_value);
    }

    bool Float::equal(const GcPtr<Object> &other) {
        if (!is<Float>(other)) return false;
        return m_value == as<Float>(other)->get_value();
    }

    size_t Float::hash() {
        return std::hash<float>{}(m_value);
    }

    OBJ_RESULT Float::$eq(const GcPtr<Object> &other) {
        return BOOL_(this->equal(other));
    }

    OBJ_RESULT Float::$ne(const GcPtr<Object> &other) {
        return BOOL_(!this->equal(other));
    }

    OBJ_RESULT Float::$lt(const GcPtr<Object> &other) {
        if (!is<Float>(other)) return std::unexpected(RuntimeError::TypeError);
        return BOOL_(m_value < as<Float>(other)->get_value());
    }

    OBJ_RESULT Float::$le(const GcPtr<Object> &other) {
        if (!is<Float>(other)) return std::unexpected(RuntimeError::TypeError);
        return BOOL_(m_value <= as<Float>(other)->get_value());
    }

    OBJ_RESULT Float::$gt(const GcPtr<Object> &other) {
        if (!is<Float>(other)) return std::unexpected(RuntimeError::TypeError);
        return BOOL_(m_value > as<Float>(other)->get_value());
    }

    OBJ_RESULT Float::$ge(const GcPtr<Object> &other) {
        if (!is<Float>(other)) return std::unexpected(RuntimeError::TypeError);
        return BOOL_(m_value >= as<Float>(other)->get_value());
    }

    OBJ_RESULT Float::$_bool() {
        return BOOL_(m_value != 0);
    }

    std::expected<GcPtr<Object>, RuntimeError> Float::$mod(const GcPtr<Object> &other) {
        if (!other->is<Float>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        auto other_number = other->as<Float>();
        if (other_number->get_value() == 0) {
            return std::unexpected(RuntimeError::DivisionByZero);
        }
        return GarbageCollector::instance().make<Float>(std::fmod(m_value, other_number->get_value()));
    }
};