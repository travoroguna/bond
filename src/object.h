#pragma once

#include "ast.h"
#include "gc.h"
#include <optional>
#include <utility>
#include <cassert>
#include "object_helpers.h"
#include <thread>
#include <mutex>

namespace bond {

    enum class Opcode : uint32_t {
        LOAD_CONST,
        BIN_ADD,
        BIN_SUB,
        BIN_MUL,
        BIN_DIV,
        RETURN,
        PUSH_TRUE,
        PUSH_FALSE,
        PUSH_NIL,
        LOAD_GLOBAL,
        STORE_GLOBAL,
        LOAD_FAST,
        STORE_FAST,

        NE,
        EQ,
        LE,
        GT,
        GE,
        LT,
        OR,
        AND,

        POP_TOP,

        CREATE_GLOBAL,
        CREATE_LOCAL,
        CREATE_FUNCTION,
        CREATE_STRUCT,
        BUILD_LIST,
        GET_ITEM,
        SET_ITEM,
        JUMP_IF_FALSE,
        JUMP,
        CALL,

        ITER,
        ITER_NEXT,
        ITER_END,
        GET_ATTRIBUTE,
        SET_ATTRIBUTE,
        IMPORT,
        TRY,
        BREAK,
        CONTINUE,
        BIN_MOD,
        NOT,
        UNARY_SUB,

        BIT_OR,
        BIT_AND,
        BIT_XOR,
        MAKE_ASYNC,
        AWAIT,
        UNPACK_SEQ
    };


    class Map : public Object {
    public:
        Map() = default;

        // TODO: implement equal and hash correctly
        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

        std::optional<GcPtr<Object>> get(const GcPtr<Object> &key);

        GcPtr<Object> get_unchecked(const GcPtr<Object> &key);

        void set(const GcPtr<Object> &key, const GcPtr<Object> &value);

        bool has(const GcPtr<Object> &key);

        OBJ_RESULT $_bool() override;

        void mark() override;

        void unmark() override;

        static const char *type_name() { return "map"; }

        std::unordered_map<GcPtr<Object>, GcPtr<Object>, Object::HashMe> get_map() { return m_internal_map; }

        std::expected<GcPtr<Object>, RuntimeError> $get_item(const GcPtr<bond::Object> &index) override;

        std::expected<GcPtr<Object>, RuntimeError>
        $set_item(const GcPtr<bond::Object> &index, const GcPtr<bond::Object> &value) override;

        std::string str() override {
            std::string result = "{";
            for (auto &[key, value]: m_internal_map) {
                result += fmt::format("{}: {}, ", key->str(), value->str());
            }
            result += "}";
            return result;
        }

    private:
        std::unordered_map<GcPtr<Object>, GcPtr<Object>, Object::HashMe>
                m_internal_map;
    };

    class NativeFunction : public Object {
    public:
        explicit NativeFunction(NativeFunctionPtr fn) : m_fn{std::move(fn)} {}

        explicit NativeFunction(NativeFunctionPtr fn, [[maybe_unused]]std::string name) : m_fn{std::move(fn)} {}

        [[nodiscard]] NativeFunctionPtr get_fn() const { return m_fn; }

        std::string str() override {
            return fmt::format("<native function at {}>", (void *) this);
        }

        [[nodiscard]] std::string get_name() const { return m_name; }

        bool equal(const GcPtr<Object> &other) override {
            return this == other.get();
        }

        size_t hash() override { return 0; }

        static const char *type_name() { return "native_function"; }

    private:
        NativeFunctionPtr m_fn;
        std::string m_name = "native_function";
    };

    class ListObj : public Object {
    public:
        ListObj() = default;

        explicit ListObj(std::vector<GcPtr<Object>> list) : m_internal_list(std::move(list)) {}

        OBJ_RESULT $set_item(const GcPtr<Object> &index,
                             const GcPtr<Object> &value) override;

        OBJ_RESULT $get_item(const GcPtr<Object> &index) override;

        GcPtr<Object> get_unchecked(size_t index) { return m_internal_list[index]; }

        size_t length() { return m_internal_list.size(); }

        OBJ_RESULT $_bool() override;

        void prepend(const GcPtr<Object> &value);

        void append(const GcPtr<Object> &value);

        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        OBJ_RESULT $iter() override;

        OBJ_RESULT $get_attribute(const GcPtr<Object> &index) override;

        size_t hash() override { return 0; }

        void mark() override;

        void unmark() override;

        std::string str() override;

        static const char *type_name() { return "list"; }

        std::vector<GcPtr<Object>> get_list() { return m_internal_list; }

    private:
        std::vector<GcPtr<Object>> m_internal_list;
    };

    class Code : public Object {

    public:
        Code() = default;

        Code(std::vector<uint32_t> code, std::vector<GcPtr<Object>> constants)
                : m_code{std::move(code)}, m_constants{std::move(constants)} {}

        [[nodiscard]] std::vector<uint32_t> get_opcodes() const { return m_code; }

        [[nodiscard]] std::vector<GcPtr<Object>> get_constants() const {
            return m_constants;
        }

        template<typename T, typename... Args>
        uint32_t add_constant(Args &&...args) {
            GarbageCollector::instance().stop_gc();
            auto tmp = GarbageCollector::instance().make<T>(args...);

            if (m_const_map.contains(tmp)) {
                GarbageCollector::instance().resume_gc();
                return m_const_map[tmp];
            }

            auto t = GarbageCollector::instance().make_immortal<T>(
                    std::forward<Args>(args)...);
            m_constants.push_back(t);
            m_const_map[tmp] = m_constants.size() - 1;

            GarbageCollector::instance().resume_gc();

            return m_constants.size() - 1;
        }

        uint32_t add_constant(const GcPtr<Object> &obj) {
            m_constants.push_back(obj);
            return m_constants.size() - 1;
        }

        void add_code(Opcode code, const SharedSpan &span);

        void patch_code(uint32_t offset, uint32_t oprand) { m_code[offset] = oprand; }

        uint32_t current_index() { return m_code.size(); }

        void add_code(Opcode code, uint32_t oprand, const SharedSpan &span);

        GcPtr<Object> get_constant(size_t index) { return m_constants[index]; }

        uint32_t get_code(size_t index) { return m_code[index]; }

        uint32_t get_code_size() { return m_code.size(); }

        SharedSpan get_span(size_t index) { return m_spans[index]; }

        SharedSpan last_span() { return m_spans[m_spans.size() - 1]; }

        std::string dissasemble();

        static const char *type_name() { return "code"; }

        // TODO: implement equal and hash correctly
        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

        OBJ_RESULT $_bool() override;

        void mark() override;

        void unmark() override;

    private:
        std::vector<uint32_t> m_code{};
        std::vector<GcPtr<Object>> m_constants{};
        std::unordered_map<GcPtr<Object>, uint32_t, Object::HashMe> m_const_map{};
        std::vector<SharedSpan> m_spans{};

        static size_t simple_instruction(std::stringstream &ss, const char *name,
                                         size_t offset);

        size_t constant_instruction(std::stringstream &ss, const char *name,
                                    size_t offset);

        size_t oprand_instruction(std::stringstream &ss, const char *name,
                                  size_t offset);
    };

    class Integer : public Object {
    public:
        explicit Integer(intmax_t value) { m_value = value; }

        [[nodiscard]] intmax_t get_value() const { return m_value; }

        OBJ_RESULT $add(const GcPtr<Object> &other) override;

        OBJ_RESULT $sub(const GcPtr<Object> &other) override;

        OBJ_RESULT $mul(const GcPtr<Object> &other) override;

        OBJ_RESULT $div(const GcPtr<Object> &other) override;

        OBJ_RESULT $eq(const GcPtr<Object> &other) override;

        OBJ_RESULT $ne(const GcPtr<Object> &other) override;

        OBJ_RESULT $lt(const GcPtr<Object> &other) override;

        OBJ_RESULT $le(const GcPtr<Object> &other) override;

        OBJ_RESULT $gt(const GcPtr<Object> &other) override;

        OBJ_RESULT $ge(const GcPtr<Object> &other) override;

        OBJ_RESULT $mod(const GcPtr<Object> &other) override;

        OBJ_RESULT $_bool() override;

        std::string str() override;

        bool equal(const GcPtr<Object> &other) override;

        size_t hash() override;

        static const char *type_name() { return "integer"; }

    private:
        intmax_t m_value{0};
    };


    class Float : public Object {
    public:
        explicit Float(float value) { m_value = value; }

        [[nodiscard]] float get_value() const { return m_value; }

        OBJ_RESULT $add(const GcPtr<Object> &other) override;

        OBJ_RESULT $sub(const GcPtr<Object> &other) override;

        OBJ_RESULT $mul(const GcPtr<Object> &other) override;

        OBJ_RESULT $div(const GcPtr<Object> &other) override;

        OBJ_RESULT $eq(const GcPtr<Object> &other) override;

        OBJ_RESULT $ne(const GcPtr<Object> &other) override;

        OBJ_RESULT $mod(const GcPtr<Object> &other) override;

        OBJ_RESULT $lt(const GcPtr<Object> &other) override;

        OBJ_RESULT $le(const GcPtr<Object> &other) override;

        OBJ_RESULT $gt(const GcPtr<Object> &other) override;

        OBJ_RESULT $ge(const GcPtr<Object> &other) override;

        OBJ_RESULT $_bool() override;

        std::string str() override;

        bool equal(const GcPtr<Object> &other) override;

        size_t hash() override;

        static const char *type_name() { return "float"; }

    private:
        float m_value{0};
    };

    class String : public Object {
    public:
        explicit String(std::string value) { m_value = std::move(value); }

        [[nodiscard]] std::string get_value() const { return m_value; }

        OBJ_RESULT $add(const GcPtr<Object> &other) override;

        OBJ_RESULT $eq(const GcPtr<Object> &other) override;

        std::expected<GcPtr<Object>, RuntimeError> $iter() override;

        OBJ_RESULT $_bool() override;

        OBJ_RESULT $ne(const GcPtr<Object> &other) override;

        std::string str() override;

        size_t hash() override;

        static const char *type_name() { return "string"; }

        bool equal(const GcPtr<Object> &other) override;

        NativeErrorOr join(const std::vector<GcPtr<Object>> &args);

        NativeErrorOr split(const std::vector<GcPtr<Object>> &args);

        NativeErrorOr find(const std::vector<GcPtr<Object>> &args);

        NativeErrorOr replace(const std::vector<GcPtr<Object>> &args);

        NativeErrorOr starts_with(const std::vector<GcPtr<Object>> &args);

        NativeErrorOr ends_with(const std::vector<GcPtr<Object>> &args);

        NativeErrorOr to_upper(const std::vector<GcPtr<Object>> &args);

        NativeErrorOr to_lower(const std::vector<GcPtr<Object>> &args);

        NativeErrorOr strip(const std::vector<GcPtr<Object>> &args);


        std::expected<GcPtr<Object>, RuntimeError> $get_attribute(const GcPtr<bond::Object> &index) override;

    private:
        std::string m_value;
        std::unordered_map<std::string, NativeFunctionPtr> m_attributes = {
                {"join",        BIND(join)},
                {"split",       BIND(split)},
                {"find",        BIND(find)},
                {"replace",     BIND(replace)},
                {"starts_with", BIND(starts_with)},
                {"ends_with",   BIND(ends_with)},
                {"to_upper",    BIND(to_upper)},
                {"to_lower",    BIND(to_lower)},
                {"strip",       BIND(strip)},
        };
    };

    class Bool : public Object {
    public:
        explicit Bool(bool value) { m_value = value; }

        [[nodiscard]] bool get_value() const { return m_value; }

        OBJ_RESULT $eq(const GcPtr<Object> &other) override;

        OBJ_RESULT $ne(const GcPtr<Object> &other) override;

        std::string str() override;

        bool equal(const GcPtr<Object> &other) override;

        size_t hash() override;

        OBJ_RESULT $_bool() override;

        static const char *type_name() { return "bool"; }

    private:
        bool m_value;
    };

    class Nil : public Object {
    public:
        Nil() = default;

        std::string str() override;

        OBJ_RESULT $eq(const GcPtr<Object> &other) override;

        OBJ_RESULT $ne(const GcPtr<Object> &other) override;

        bool equal(const GcPtr<Object> &other) override;

        OBJ_RESULT $_bool() override;

        static const char *type_name() { return "nil"; }

        size_t hash() override;
    };


    template<typename T>
    class NativeStruct : public Object {
    public:
        explicit NativeStruct(std::string name, const NativeFunctionPtr &constructor) {
            m_name = std::move(name);
            m_constructor = constructor;
        }

        [[nodiscard]] NativeFunctionPtr get_constructor() const { return m_constructor; }

        bool equal(const GcPtr<Object> &other) override {
            return this == other.get();
        }

        size_t hash() override { return 0; }

        std::string str() override {
            return fmt::format("<native struct {} at {}>", m_name, (void *) this);
        }

        static const char *type_name() { return "native_struct"; }

        [[nodiscard]] std::string get_name() const { return m_name; }

        NativeErrorOr call(const std::vector<GcPtr<Object>> &args) {
            return m_constructor(args);
        }

        std::expected<bool, std::string> is_instance(const GcPtr<Object> &obj) override {
            return instanceof<T>(obj.get());
        }

        std::expected<GcPtr<Object>, std::string> call__(std::vector<GcPtr<Object>> &arguments) override {
            auto result = m_constructor(arguments);

            if (result.has_value()) {
                return result.value();
            } else {
                return std::unexpected(result.error().message);
            }
        }

    private:
        std::string m_name;
        NativeFunctionPtr m_constructor;
    };

    class Function : public Object {
    public:
        Function(GcPtr<Code> code, std::vector<std::pair<std::string, SharedSpan>> params, std::string name)
                : m_code{std::move(code)}, m_params{std::move(params)}, m_name(std::move(name)) {}

        [[nodiscard]] GcPtr<Code> get_code() const { return m_code; }

        [[nodiscard]] std::vector<std::pair<std::string, SharedSpan>> get_params() const { return m_params; }

        [[nodiscard]] std::string get_name() const { return m_name; }

        void set_globals(const GcPtr<Map> &globals) { m_globals = globals; }

        GcPtr<Map> get_globals() { return m_globals; }

        std::string str() override {
            return fmt::format("<function {} at {}>", m_name, (void *) this);
        }

        bool equal(const GcPtr<Object> &other) override {
            return this == other.get();
        }

        size_t hash() override { return 0; }

        OBJ_RESULT $_bool() override {
            return GarbageCollector::instance().make<Bool>(true);
        }

        static const char *type_name() { return "function"; }

        void mark() override {
            if (m_marked) return;
            m_marked = true;
            m_code->mark();
            m_globals->mark();
        }

        void unmark() override {
            if (!m_marked) return;

            m_marked = false;
            m_code->unmark();
            m_globals->unmark();
        }

    private:
        GcPtr<Code> m_code;
        std::string m_name;
        GcPtr<Map> m_globals;
        std::vector<std::pair<std::string, SharedSpan>> m_params;
    };

    class ListIterator : public Object {
    public:
        explicit ListIterator(const GcPtr<Object> &list) : m_list{list} {}

        OBJ_RESULT $next() override { return m_list->as<ListObj>()->get_unchecked(m_index++); }

        OBJ_RESULT $has_next() override;

        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

        static const char *type_name() { return "list_iterator"; }

        void mark() override {
            m_marked = true;
            m_list->mark();
        }

        void unmark() override {
            m_marked = false;
            m_list->unmark();
        }

    private:
        GcPtr<Object> m_list;
        size_t m_index = 0;
    };

    class Closure : public Object {
    public:
        explicit Closure(GcPtr<Function> function)
                : m_function{std::move(function)} {}

        [[nodiscard]] GcPtr<Function> get_function() const { return m_function; }

        std::string str() override {
            return fmt::format("<closure {} at {}>", m_function->get_name(), (void *) this);
        }

        bool equal(const GcPtr<Object> &other) override {
            return this == other.get();
        }

        size_t hash() override { return 0; }

        OBJ_RESULT $_bool() override {
            return GarbageCollector::instance().make<Bool>(true);
        }

        static const char *type_name() { return "closure"; }

    private:
        GcPtr<Function> m_function;
    };


    class Struct : public Object {
    public:
        Struct(const GcPtr<String> &name, const std::vector<GcPtr<String>> &instance_variables);

        // TODO: implement hash correctly
        bool equal(const GcPtr<Object> &other) override { return this == other.get(); }

        size_t hash() override { return 0; }


        OBJ_RESULT $_bool() override { return GarbageCollector::instance().make<Bool>(true); }

        OBJ_RESULT $get_attribute(const GcPtr<Object> &index) override;

        void add_method(const GcPtr<Object> &name, const GcPtr<Object> &function) { m_methods->set(name, function); }

        std::optional<GcPtr<Object>> get_method(const GcPtr<Object> &name) {
            return m_methods->get(name);
        }

        void mark() override;

        void unmark() override;

        static const char *type_name() { return "struct"; }

        std::vector<GcPtr<String>> get_instance_variables() { return m_instance_variables; }

        std::expected<bool, std::string> is_instance(GcPtr<Object> const &other) override;

        GcPtr<Map> get_globals() { return m_globals; }

        void set_globals(const GcPtr<Map> &globals);

        std::string get_name() { return m_name->get_value(); }

        std::string str() override {
            return fmt::format("<structure {} at {}>", get_name(), (void *) this);
        }

        GcPtr<Map> get_methods() { return m_methods; }

    private:
        GcPtr<Map> m_methods;
        GcPtr<Map> m_globals;
        GcPtr<String> m_name;
        std::vector<GcPtr<String>> m_instance_variables;
    };

    class StructInstance : public Object {
    public:
        explicit StructInstance(const GcPtr<Struct> &struct_type);

        OBJ_RESULT $set_attribute(const GcPtr<Object> &index, const GcPtr<Object> &value) override;

        OBJ_RESULT $get_attribute(const GcPtr<Object> &index) override;

        void set_attr(const GcPtr<Object> &index, const GcPtr<Object> &value) { m_attributes->set(index, value); }

        std::optional<GcPtr<Object>> get_attr(const GcPtr<Object> &index) { return m_attributes->get(index); }

        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

        OBJ_RESULT $_bool() override { return GarbageCollector::instance().make<Bool>(true); }

        OBJ_RESULT $eq(const GcPtr<Object> &other) override;

        OBJ_RESULT $ne(const GcPtr<Object> &other) override;


        void mark() override;

        void unmark() override;

        static const char *type_name() { return "struct_instance"; }

        OBJ_RESULT $set_item(const GcPtr<Object> &index, const GcPtr<Object> &value) override;

        OBJ_RESULT $get_item(const GcPtr<Object> &index) override;

        OBJ_RESULT user_method(const std::string &name);

        OBJ_RESULT $add(const GcPtr<Object> &other) override;

        OBJ_RESULT $sub(const GcPtr<Object> &other) override;

        OBJ_RESULT $mul(const GcPtr<Object> &other) override;

        OBJ_RESULT $div(const GcPtr<Object> &other) override;

        OBJ_RESULT $lt(const GcPtr<Object> &other) override;

        OBJ_RESULT $le(const GcPtr<Object> &other) override;

        OBJ_RESULT $gt(const GcPtr<Object> &other) override;

        OBJ_RESULT $ge(const GcPtr<Object> &other) override;

        std::string format_args();

        std::string str() override {
//            return fmt::format("<instance {} at {}>", m_struct_type->get_name(), (void *) this);
            return fmt::format("{}({})", m_struct_type->get_name(), format_args());
        }

        GcPtr<Struct> get_struct_type() { return m_struct_type; }

    private:
        GcPtr<Struct> m_struct_type;
        GcPtr<Map> m_attributes;

    };

    class BoundMethod : public Object {
    public:
        BoundMethod(const GcPtr<Object> &receiver, const GcPtr<Object> &method)
                : m_receiver{receiver}, m_method{method} {}

        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

        OBJ_RESULT $_bool() override { return GarbageCollector::instance().make<Bool>(true); }

        void mark() override;

        void unmark() override;

        static const char *type_name() { return "bound_method"; }

        std::string str() override {
            return fmt::format("<bound method {} at {}>", m_receiver->str(), (void *) this);
        }

        GcPtr<Object> get_receiver() { return m_receiver; }

        GcPtr<Object> get_method() { return m_method; }

    private:
        GcPtr<Object> m_receiver;
        GcPtr<Object> m_method;
    };


    namespace Globs {
        extern GcPtr<Bool> BondTrue;
        extern GcPtr<Bool> BondFalse;
        extern GcPtr<Nil> BondNil;
    }


#define BOOL_(x) (x) ? Globs::BondTrue : Globs::BondFalse


    class Future : public Object {
    public:
        Future() = default;

        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

        OBJ_RESULT $_bool() override { return GarbageCollector::instance().make<Bool>(true); }

        void mark() override;

        void unmark() override;

        static const char *type_name() { return "future"; }

        std::string str() override {
            return fmt::format("<future instance at {}>", (void *) this);
        }

        NativeErrorOr get_result(const std::vector<GcPtr<Object>> &args) {
            ASSERT_ARG_COUNT(0, args);

            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_value.get() == nullptr) return Err("Future not resolved");
            return m_value;
        }

        NativeErrorOr has_result(const std::vector<GcPtr<Object>> &args) {
            ASSERT_ARG_COUNT(0, args);

            std::lock_guard<std::mutex> lock(m_mutex);
            return BOOL_(m_value.get() != nullptr);
        }

        OBJ_RESULT $get_attribute([[maybe_unused]] const GcPtr<Object> &index) override {
            auto idx = index->as<String>()->get_value();
            if (!m_methods.contains(idx)) return std::unexpected(RuntimeError::AttributeNotFound);
            return GarbageCollector::instance().make<NativeFunction>(m_methods[idx]);
        }

        bool has_value() {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_value.get() != nullptr;
        }

        GcPtr<Object> get_value() {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_value;
        }

        void set_value(const GcPtr<Object> &value) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_value = value;
        }

    private:
        GcPtr<Object> m_value = nullptr;
        std::unordered_map<std::string, NativeFunctionPtr> m_methods = {
                {"has_result", BIND(has_result)},
                {"get_result", BIND(get_result)},
        };
        std::mutex m_mutex;
    };

    class Coroutine : public Object {
    public:
        explicit Coroutine(const GcPtr<Function> &function);

        GcPtr<Function> get_function() { return m_function; }

        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

    private:
        GcPtr<Function> m_function;
    };

} // namespace bond
