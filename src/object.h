#pragma once

#include <utility>

#include "gc.h"
#include "ast.h"


namespace bond {

    enum class Opcode : uint32_t{
        LOAD_CONST, BIN_ADD, BIN_SUB, BIN_MUL, BIN_DIV, RETURN,
        PUSH_TRUE, PUSH_FALSE, PUSH_NIL
    };

    class Code: public Object {

    public:
        Code() = default;
        Code(std::vector<uint32_t> code, std::vector<GcPtr<Object>> constants)
                : m_code{std::move(code)}, m_constants{std::move(constants)} {}
        [[nodiscard]] std::vector<uint32_t> get_code() const { return m_code; }
        [[nodiscard]] std::vector<GcPtr<Object>> get_constants() const { return m_constants; }

        uint32_t add_constant(const GcPtr<Object> &object)  { m_constants.push_back(object);  return m_constants.size() - 1; }
        void add_code(Opcode code, const SharedSpan& span);
        void add_code(Opcode code, uint32_t oprand, const SharedSpan& span);

        GcPtr<Object> get_object(size_t index) { return m_constants[index]; }
        uint32_t get_code(size_t index) { return m_code[index]; }
        SharedSpan get_span(size_t index) { return m_spans[index]; }

    private:
        std::vector<uint32_t> m_code{};
        std::vector<GcPtr<Object>> m_constants{};
        std::vector<SharedSpan> m_spans{};
    };

    class Number : public Object {
    public:
        explicit Number(float value) { m_value = value; }
        [[nodiscard]] float get_value() const { return m_value; }

        OBJ_RESULT $add(const GcPtr<Object> &other) override;
        OBJ_RESULT $sub(const GcPtr<Object> &other) override;
        OBJ_RESULT $mul(const GcPtr<Object> &other) override;
        OBJ_RESULT $div(const GcPtr<Object> &other) override;

    private:
        float m_value{0};
    };


    class String : public Object {
    public:
        explicit String(std::string value) { m_value = std::move(value); }
        [[nodiscard]] std::string get_value() const { return m_value; }

        OBJ_RESULT $add(const GcPtr<Object> &other) override;

    private:
        std::string m_value;
    };


    class Bool : public Object {
    public:
        explicit Bool(bool value) { m_value = value; }
        [[nodiscard]] bool get_value() const { return m_value; }
        OBJ_RESULT $add(const GcPtr<Object> &other) override;

    private:
        bool m_value;
    };


    class Nil : public Object {
    public:
        Nil() = default;
    };



};