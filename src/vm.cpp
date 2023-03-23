//
// Created by travor on 3/18/23.
//

#include "vm.h"

namespace bond {
    std::expected<Object*, RuntimeError> Number::$add(Object const& other) {
        if (other.get_type() == ObjectType::Number) {
            auto other_number = static_cast<Number const&>(other);
            return new Number(m_value + other_number.m_value);
        }
        return std::unexpected(RuntimeError::TypeError);      
    }

    std::expected<Object *, RuntimeError> Number::$sub(Object const &other) {
        if (other.get_type() == ObjectType::Number) {
            auto other_number = static_cast<Number const&>(other);
            return new Number(m_value - other_number.m_value);
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    std::expected<Object *, RuntimeError> Number::$mul(Object const &other) {
        if (other.get_type() == ObjectType::Number) {
            auto other_number = static_cast<Number const&>(other);
            return new Number(m_value * other_number.m_value);
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    std::expected<Object*, RuntimeError> Number::$div(Object const& other) {
        if (other.get_type() == ObjectType::Number) {
            auto other_number = static_cast<Number const&>(other);
            if (other_number.m_value == 0) {
                return std::unexpected(RuntimeError::DivisionByZero);
            }
            return new Number(m_value / other_number.m_value);
        }
        return std::unexpected(RuntimeError::TypeError);
    }

}; // bond