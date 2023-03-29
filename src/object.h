#pragma once

#include <utility>
#include <optional>
#include "gc.h"
#include "ast.h"


namespace bond {

    enum class Opcode : uint32_t {

        LOAD_CONST, BIN_ADD, BIN_SUB, BIN_MUL, BIN_DIV, RETURN,
        PUSH_TRUE, PUSH_FALSE, PUSH_NIL, LOAD_GLOBAL, SET_GLOBAL,
        LOAD_FAST, STORE_FAST,

        PRINT
    };


    class Map : public Object {
    public:
        Map() = default;

        //TODO: implement equal and hash correctly
        bool equal(const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

        std::optional<GcPtr<Object>> get(const GcPtr<Object> &key);

        GcPtr<Object> get_unchecked(const GcPtr<Object> &key);

        void set(const GcPtr<Object> &key, const GcPtr<Object> &value);

        bool has(const GcPtr<Object> &key);


    private:
        std::unordered_map<GcPtr<Object>, GcPtr<Object>, Object::HashMe> m_internal_map;
    };

    class Code : public Object {

    public:
        Code() = default;

        Code(std::vector<uint32_t> code, std::vector<GcPtr<Object>> constants)
                : m_code{std::move(code)}, m_constants{std::move(constants)} {}

        [[nodiscard]] std::vector<uint32_t> get_opcodes() const { return m_code; }

        [[nodiscard]] std::vector<GcPtr<Object>> get_constants() const { return m_constants; }

        template<typename T, typename... Args>
        uint32_t add_constant(Args &&...args) {
            GarbageCollector::instance().stop_gc();
            auto tmp = GarbageCollector::instance().make<T>(args...);

            if (m_const_map.contains(tmp)) {
                GarbageCollector::instance().resume_gc();
                return m_const_map[tmp];
            }

            auto t = GarbageCollector::instance().make_immortal<T>(std::forward<Args>(args)...);
            m_constants.push_back(t);
            m_const_map[tmp] = m_constants.size() - 1;

            GarbageCollector::instance().resume_gc();

            return m_constants.size() - 1;
        }

        void add_code(Opcode code, const SharedSpan &span);

        void add_code(Opcode code, uint32_t oprand, const SharedSpan &span);

        GcPtr<Object> get_constant(size_t index) { return m_constants[index]; }

        uint32_t get_code(size_t index) { return m_code[index]; }

        SharedSpan get_span(size_t index) { return m_spans[index]; }

        SharedSpan last_span() { return m_spans[m_spans.size() - 1]; }

        //TODO: implement equal and hash correctly
        bool equal(const GcPtr<Object> &other) override { return false; }
        size_t hash() override { return 0; }

    private:
        std::vector<uint32_t> m_code{};
        std::vector<GcPtr<Object>> m_constants{};
        std::unordered_map<GcPtr<Object>, uint32_t, Object::HashMe> m_const_map{};
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

        std::string str() override;

        bool equal(const GcPtr<Object> &other) override;
        size_t hash() override;

    private:
        float m_value{0};
    };


    class String : public Object {
    public:
        explicit String(std::string value) { m_value = std::move(value); }
        [[nodiscard]] std::string get_value() const { return m_value; }

        OBJ_RESULT $add(const GcPtr<Object> &other) override;

        std::string str() override;

        size_t hash() override;
        bool equal(const GcPtr <Object> &other) override;

    private:
        std::string m_value;
    };


    class Bool : public Object {
    public:
        explicit Bool(bool value) { m_value = value; }
        [[nodiscard]] bool get_value() const { return m_value; }

        std::string str() override;

        bool equal(const GcPtr<Object> &other) override;
        size_t hash() override;

    private:
        bool m_value;
    };


    class Nil : public Object {
    public:
        Nil() = default;
        std::string str() override;

        bool equal(const GcPtr<Object> &other) override;
        size_t hash() override;

    };





};