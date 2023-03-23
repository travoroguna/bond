//
// Created by travor on 3/18/23.
//

#pragma once


#include <utility>
#include <vector>
#include <expected>
#include <type_traits>
#include <fmt/core.h>

namespace bond {
    enum class ObjectType {
        Number, String, Bool, Nil, Object
    };

    enum class RuntimeError {
        TypeError, Unimplemented, DivisionByZero
    };


    #define UNIMPLEMENTED return std::unexpected(RuntimeError::Unimplemented)
    #define OBJ_RESULT std::expected<Object*, RuntimeError>

    class Object {
    public:
        virtual OBJ_RESULT $add(Object const& other) { UNIMPLEMENTED; }
        virtual OBJ_RESULT $sub(Object const& other) { UNIMPLEMENTED; }
        virtual OBJ_RESULT $mul(Object const& other) { UNIMPLEMENTED; }
        virtual OBJ_RESULT $div(Object const& other) { UNIMPLEMENTED; }
        virtual ~Object() = default;

        bool is_marked() { return m_marked; }
        void mark() { m_marked=true; }
        void unmark() { m_marked=false; }
        ObjectType get_type() const { return m_type; }

        Object* next {nullptr};

    protected:
        ObjectType m_type {ObjectType::Object};
        bool m_marked { false };

    };

    class Number : public Object{
    public:
        explicit Number(float value) { m_value = value; m_type = ObjectType::Number; }
        float get_value() const { return m_value; }
        ~Number() override = default;

        OBJ_RESULT $add(Object const& other) override;
        OBJ_RESULT $sub(Object const& other) override;
        OBJ_RESULT $mul(Object const& other) override;
        OBJ_RESULT $div(Object const& other) override;
    private:
        float m_value {0};
    };

    class Vm{
    public:
        Vm() = default;
        void add_object(Object *obj);

    private:
        Object *head = nullptr;
    };

} // bond

