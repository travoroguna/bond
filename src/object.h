#pragma once

#include "compiler/ast.h"
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
        UNPACK_SEQ,
        CALL_METHOD,
        MAKE_ERROR,
        MAKE_OK,

        CREATE_CLOSURE
    };

    enum Slot : uint32_t {
        NE = 0,
        EQ,
        LE,
        GT,
        GE,
        LT,

        BIN_ADD,
        BIN_SUB,
        BIN_MUL,
        BIN_DIV,
        BIN_MOD,
        UNARY_SUB,

        GET_ATTR,
        SET_ATTR,
        GET_ITEM,
        SET_ITEM,

        ITER,
        NEXT,
        HAS_NEXT,

        SIZE
    };


    class Object;

    using obj_result = std::expected<GcPtr<Object>, std::string>;


    class Object : public GcObject {
    public:
        Object() = default;

        ~Object() override = default;

        [[nodiscard]] virtual std::string str() const {
            return fmt::format("<Object at {}>", (void *) this);
        }

    };

    using t_vector = std::vector<GcPtr<Object>, gc_allocator<GcPtr<Object>>>;
    using t_map = std::unordered_map<std::string, GcPtr<Object>, std::hash<std::string>, std::equal_to<>,
            gc_allocator<std::pair<const std::string, GcPtr<Object>>>>;

    using NativeMethodPtr = std::function<obj_result(const GcPtr<Object> &self, const t_vector &)>;
    using NativeFunctionPtr = std::function<obj_result(const t_vector &)>;

    class NativeInstance;

    using slot_array = std::array<NativeMethodPtr, Slot::SIZE>;
    using getter = std::function<obj_result(const GcPtr<Object> &)>;
    using setter = std::function<obj_result(const GcPtr<Object> &, const GcPtr<Object> &)>;


    class NativeStruct : public Object {
    public:
        NativeStruct(std::string name, std::string doc, NativeFunctionPtr constructor)
                : m_name(std::move(name)), m_doc(std::move(doc)), m_constructor(std::move(constructor)) { set_slots(); }

        NativeStruct(std::string name, std::string doc, NativeFunctionPtr constructor,
                     const std::unordered_map<std::string, std::pair<NativeMethodPtr, std::string>> &methods)
                : m_name(std::move(name)), m_doc(std::move(doc)), m_methods(methods),
                  m_constructor(std::move(constructor)) { set_slots(); }

        NativeStruct(std::string name, std::string doc, NativeFunctionPtr constructor,
                     const std::unordered_map<std::string, std::pair<NativeMethodPtr, std::string>> &methods,
                     std::unordered_map<std::string, std::pair<getter, setter>> &properties)
                : m_name(std::move(name)), m_doc(std::move(doc)), m_methods(methods),
                  m_constructor(std::move(constructor)), m_attributes(properties) { set_slots(); }

        void add_methods(const std::unordered_map<std::string, std::pair<NativeMethodPtr, std::string>> &methods);

        [[nodiscard]] std::optional<NativeMethodPtr> get_method(const std::string &name) const;

        [[nodiscard]] std::string get_name() const { return m_name; }

        [[nodiscard]] std::string get_doc() const { return m_doc; }

        [[nodiscard]] NativeFunctionPtr get_constructor() const { return m_constructor; }

        void set_constructor(NativeFunctionPtr constructor) { m_constructor = std::move(constructor); }

        [[nodiscard]] obj_result create(const t_vector &args) const;

        template<typename T, typename = std::enable_if<std::is_base_of_v<NativeInstance, T>>, typename ...Args>
        GcPtr<T> create_instance(Args &&...args) const {
            auto ins = make<T>(std::forward<Args>(args)...);
            ins->set_native_struct(const_cast<NativeStruct *>(this));
            return ins;
        }

        template<typename T, typename = std::enable_if<std::is_base_of_v<NativeInstance, T>>, typename ...Args>
        GcPtr<T> create_immortal(Args &&...args) const {
            auto ins = make<T>(std::forward<Args>(args)...);
            ins->set_native_struct(const_cast<NativeStruct *>(this));
            return ins;
        }

        [[nodiscard]] const slot_array &get_slots() const { return m_slots; }

        void set_slots();

        NativeMethodPtr get_slot(Slot slot);

        bool has_method(const std::string &name) const;

        [[nodiscard]]
        std::string str() const override { return m_name; }

        std::unordered_map<std::string, std::pair<NativeMethodPtr, std::string>> &get_methods() { return m_methods; }

        std::optional<getter> get_getter(const std::string &name) const;

        std::optional<setter> get_setter(const std::string &name) const;


    protected:
        std::string m_name;
        std::string m_doc;
        std::unordered_map<std::string, std::pair<NativeMethodPtr, std::string>> m_methods;
        NativeFunctionPtr m_constructor;
        slot_array m_slots;

        //getter and setter
        std::unordered_map<std::string, std::pair<getter, setter>> m_attributes;

    };


    class NativeInstance : public Object {
    public:
        explicit NativeInstance(NativeStruct *native_struct) : m_native_struct(native_struct) {}

        NativeInstance() = default;

        [[nodiscard]] NativeStruct *get_native_struct() const { return m_native_struct; }

        [[nodiscard]] obj_result call_method(const std::string &name, const t_vector &args);

        bool has_method(const std::string &name);

        void set_native_struct(NativeStruct *native_struct) { m_native_struct = native_struct; }

        obj_result call_slot(Slot slot, const t_vector &args);

        virtual bool has_slot(Slot slot);

        void mark() override;

        void unmark() override;

        [[nodiscard]] std::string str() const override {
            return fmt::format("<instance of {} at {}>", m_native_struct->get_name(), (void *) this);
        }

        std::optional<obj_result> get_attr(const std::string &name);

        std::optional<obj_result> set_attr(const std::string &name, const GcPtr<Object> &value);


    protected:
        NativeStruct *m_native_struct = nullptr;
    };

    obj_result OK(const GcPtr<Object> &object);

    obj_result OK();

    obj_result ERR(std::string error);

    using method_map = std::unordered_map<std::string, std::pair<NativeMethodPtr, std::string>>;




// builtin types

    class Float : public NativeInstance {
    public:
        explicit Float(double value) : m_value(value) {}

        [[nodiscard]] double get_value() const { return m_value; }

        [[nodiscard]] std::string str() const override { return fmt::format("{}", m_value); }

    private:
        double m_value;
    };

    class Int : public NativeInstance {
    public:
        explicit Int(int64_t value) : m_value(value) {}

        [[nodiscard]] int64_t get_value() const { return m_value; }

        std::string str() const override { return fmt::format("{}", m_value); }

    private:
        int64_t m_value;
    };

    class Bool : public NativeInstance {
    public:
        explicit Bool(bool value) : m_value(value) {}

        [[nodiscard]] bool get_value() const { return m_value; }

        [[nodiscard]] std::string str() const override { return fmt::format("{}", m_value); }

    private:
        bool m_value;
    };

    class String : public NativeInstance {
    public:
        explicit String(std::string value) : m_value(std::move(value)) {}

        [[nodiscard]] std::string get_value() const { return m_value; }

        [[nodiscard]] std::string str() const override { return m_value; }

    private:
        std::string m_value;
    };

    class StringIterator : public NativeInstance {
    public:
        explicit StringIterator(std::string value) : m_value(std::move(value)) {}

        [[nodiscard]] std::string get_value() const { return m_value; }

        [[nodiscard]] std::string str() const override { return fmt::format("<string iterator at {}>", (void *) this); }

        std::string m_value;
        size_t m_index = 0;


    };

    class None : public NativeInstance {
    public:
        None() = default;

        [[nodiscard]] std::string str() const override { return "Nil"; }
    };

    class Map : public NativeInstance {
    public:
        Map() = default;

        Map(const t_map &map) : m_value(map) {}

        [[nodiscard]] t_map get_value() const { return m_value; }

        void set_value(t_map value) { m_value = std::move(value); }

        void set(const std::string &key, const GcPtr<Object> &obj);

        std::optional<GcPtr<Object>> get(const std::string &key);

        GcPtr<Object> get_unchecked(const std::string &key);

        bool has(const std::string &key) { return m_value.contains(key); }

        void mark() override;

        void unmark() override;

    private:
        t_map m_value;
    };


    class Code : public NativeInstance {
    public:
        Code() = default;

        Code(std::vector<uint32_t> instructions, std::vector<std::shared_ptr<Span>> spans,
             t_vector constants)
                : m_instructions(std::move(instructions)), m_spans(std::move(spans)),
                  m_constants(std::move(constants)) {}

        [[nodiscard]] std::vector<uint32_t> get_instructions() const { return m_instructions; }

        [[nodiscard]] std::vector<std::shared_ptr<Span>> get_spans() const { return m_spans; }

        [[nodiscard]] t_vector get_constants() const { return m_constants; }

        void add_ins(Opcode code, const std::shared_ptr<Span> &span);

        void add_ins(Opcode code, uint32_t oprand, const std::shared_ptr<Span> &span);

        uint32_t add_constant(const GcPtr<Object> &obj);

        void patch_code(uint32_t offset, uint32_t oprand) { m_instructions[offset] = oprand; }

        uint32_t current_index() { return m_instructions.size(); }

        [[nodiscard]] std::string disassemble() const;

        GcPtr<Object> get_constant(size_t index) { return m_constants[index]; }

        uint32_t get_code(size_t index) { return m_instructions[index]; }

        uint32_t get_code_size() { return m_instructions.size(); }

        SharedSpan get_span(size_t index) { return m_spans[index]; }

        SharedSpan last_span() { return m_spans[m_spans.size() - 1]; }

        std::vector<uint32_t> &get_instructions() { return m_instructions; }

        std::vector<uint32_t> &get_opcodes() { return m_instructions; }


        std::vector<SharedSpan> &get_spans() { return m_spans; }


    private:
        std::vector<uint32_t> m_instructions;
        std::vector<std::shared_ptr<Span>> m_spans;
        t_vector m_constants{};

        size_t simple_instruction(std::stringstream &ss, const char *name, size_t offset) const;

        size_t constant_instruction(std::stringstream &ss, const char *name, size_t offset) const;

        size_t oprand_instruction(std::stringstream &ss, const char *name, size_t offset) const;

        std::unordered_map<uint64_t, uint32_t> m_int_map;
        std::unordered_map<double, uint32_t> m_float_map;
        std::unordered_map<std::string, uint32_t> m_string_map;
    };

    class Function : public NativeInstance {
    public:
        Function(std::string name, std::vector<std::pair<std::string, SharedSpan>> arguments, const GcPtr<Code> &code)
                : m_name(
                std::move(name)), m_arguments(std::move(arguments)), m_code(code) {}

        [[nodiscard]] std::string get_name() const { return m_name; }

        [[nodiscard]] std::vector<std::pair<std::string, SharedSpan>> &get_arguments() { return m_arguments; }

        [[nodiscard]] GcPtr<Code> get_code() const { return m_code; }

        void set_globals(const GcPtr<Map> &globals) { m_globals = globals; }

        [[nodiscard]] GcPtr<Map> get_globals() const { return m_globals; }

        void mark() override;

        void unmark() override;


    private:
        std::string m_name;
        std::vector<std::pair<std::string, SharedSpan>> m_arguments;
        GcPtr<Code> m_code;
        GcPtr<Map> m_globals;
    };

    class Instance;

    class Struct : public NativeInstance {
    public:
        Struct(std::string name, const std::vector<std::string> &fields) : m_name(std::move(name)), m_fields(fields) {}

        [[nodiscard]] std::string get_name() const { return m_name; }

        [[nodiscard]] std::vector<std::string> get_fields() const { return m_fields; }

        [[nodiscard]] GcPtr<Instance> create_instance(const t_map &fields);

        void add_method(const std::string &name, const GcPtr<Function> &func) { m_methods[name] = func; }

        [[nodiscard]] std::optional<GcPtr<Function>> get_method(const std::string &name) const;

        bool has_method(const std::string &name) const { return m_methods.contains(name); }

        void set_globals(const GcPtr<Map> &globals);

        [[nodiscard]] GcPtr<Map> get_globals() const { return m_globals; }

        std::unordered_map<std::string, GcPtr<Function>> &get_methods() { return m_methods; }

        std::string str() const override { return fmt::format("<struct {}>", m_name); }


    private:
        std::string m_name;
        std::vector<std::string> m_fields;
        std::unordered_map<std::string, GcPtr<Function>> m_methods;
        GcPtr<Map> m_globals;
    };


    class Instance : public NativeInstance {
    public:
        Instance(Struct *type, t_map fields)
                : m_type(type), m_fields(std::move(fields)) {}

        [[nodiscard]] GcPtr<Struct> get_type() const { return m_type; }

        [[nodiscard]] t_map get_fields() const { return m_fields; }

        void set_fields(t_map fields) { m_fields = std::move(fields); }

        obj_result bind_method(const std::string &name);

        obj_result get_method(const std::string &name);

        void mark() override;

        void unmark() override;

        obj_result get_field(const std::string &name);

        obj_result set_field(const std::string &name, const GcPtr<Object> &value);

        obj_result get_type();

        [[nodiscard]] std::string str() const override;

        Struct *get_struct() const { return m_type; }

        bool has_slot(Slot slot) override;


    private:
        Struct *m_type;
        t_map m_fields;

    };

    class NativeFunction : public NativeInstance {
    public:
        NativeFunction(std::string name, std::string doc, NativeFunctionPtr function)
                : m_name(std::move(name)), m_doc(std::move(doc)), m_function(std::move(function)) {}

        [[nodiscard]] std::string get_name() const { return m_name; }

        [[nodiscard]] std::string get_doc() const { return m_doc; }

        [[nodiscard]] NativeFunctionPtr get_function() const { return m_function; }

    private:
        std::string m_name;
        std::string m_doc;
        NativeFunctionPtr m_function;
    };

    class Module : public NativeInstance {
    public:
        explicit Module(std::string path, const GcPtr<Map> &globals) : m_globals(globals), m_path(std::move(path)) {}

        Module(std::string path, const t_map &objects);

        void mark() override {
            NativeInstance::mark();
            m_globals->mark();
        }

        void unmark() override {
            NativeInstance::unmark();
            m_globals->unmark();
        }

        GcPtr<Map> get_globals() { return m_globals; }

        obj_result get_attr(const std::string &name);

        std::string get_path() { return m_path; }

    private:
        GcPtr<Map> m_globals;
        std::string m_path;
    };


    class BoundMethod : public NativeInstance {
    public:
        BoundMethod(GcPtr<Instance> instance, GcPtr<Function> method) : m_instance(std::move(instance)),
                                                                        m_method(std::move(method)) {}

        [[nodiscard]] GcPtr<Object> get_instance() const { return m_instance; }

        [[nodiscard]] GcPtr<Function> get_method() const { return m_method; }

    private:
        GcPtr<Instance> m_instance;
        GcPtr<Function> m_method;
    };


    class List : public NativeInstance {
    public:
        List() = default;

        List(const t_vector &elements);

        void mark() override;

        void unmark() override;

        obj_result get_item(int64_t index);

        obj_result set_item(int64_t index, const GcPtr<Object> &item);

        obj_result size() const;

        void prepend(const GcPtr<Object> &item);

        void append(const GcPtr<Object> &item);

        void insert(int64_t index, const GcPtr<Object> &item);

        GcPtr<Object> pop();

        std::string str() const override;

        friend class ListIterator;

        size_t get_size() const;

        t_vector &get_elements() { return m_elements; }

    private:
        t_vector m_elements;

    };

    class ListIterator : public NativeInstance {
    public:
        explicit ListIterator(const GcPtr<List> &list) : m_list(list) {}

        GcPtr<List> m_list;
        int64_t m_index;
    };


    class Result : public NativeInstance {
    public:
        Result(GcPtr<Object> value, bool is_error) : m_value(std::move(value)), m_error(is_error) {}

        [[nodiscard]] GcPtr<Object> get_value() const { return m_value; }

        [[nodiscard]] bool has_value() const { return !m_error; }

        [[nodiscard]] bool has_error() const { return m_error; }

        std::string str() const override { return fmt::format("{}({})", m_error ? "Error" : "Ok", m_value->str()); }

    private:
        GcPtr<Object> m_value;
        bool m_error;
    };

    class Closure : public NativeInstance {
    public:
        Closure(GcPtr<Function> function, GcPtr<Map> up_values) : m_function(std::move(function)),
                                                                  m_up_values(std::move(up_values)) {}

        [[nodiscard]] GcPtr<Function> get_function() const { return m_function; }

        [[nodiscard]] GcPtr<Map> get_up_values() const { return m_up_values; }

    private:
        GcPtr<Function> m_function;
        GcPtr<Map> m_up_values;
    };


    extern GcPtr<NativeStruct> CLOSURE_STRUCT;
    extern GcPtr<NativeStruct> RESULT_STRUCT;
    extern GcPtr<NativeStruct> FLOAT_STRUCT;
    extern GcPtr<NativeStruct> INT_STRUCT;
    extern GcPtr<NativeStruct> BOOL_STRUCT;
    extern GcPtr<NativeStruct> STRING_STRUCT;
    extern GcPtr<NativeStruct> NONE_STRUCT;
    extern GcPtr<NativeStruct> MAP_STRUCT;
    extern GcPtr<NativeStruct> STRUCT_STRUCT;
    extern GcPtr<NativeStruct> INSTANCE_STRUCT;
    extern GcPtr<NativeStruct> FUNCTION_STRUCT;
    extern GcPtr<NativeStruct> NATIVE_FUNCTION_STRUCT;
    extern GcPtr<NativeStruct> CODE_STRUCT;
    extern GcPtr<NativeStruct> MODULE_STRUCT;
    extern GcPtr<NativeStruct> LIST_STRUCT;
    extern GcPtr<NativeStruct> BOUND_METHOD_STRUCT;


    extern GcPtr<Bool> C_TRUE;
    extern GcPtr<Bool> C_FALSE;
    extern GcPtr<None> C_NONE;


#define TRY(expr) if (auto result = (expr); !result) return result
#define AS_BOOL(cond) (cond ? C_TRUE : C_FALSE)

    template<typename T>
    obj_result c_Default(const t_vector args) {
        T *value;
        auto res = parse_args(args, value);
        TRY(res);
        return OK(args[0]);
    }

    static t_vector immortals;


    template<typename T, typename... Args>
    inline GcPtr<T> make(Args &&...args) {
        return GcPtr<T>(new(GC) T(std::forward<Args>(args)...));
    }

    template<typename T, typename... Args>
    inline GcPtr<T> make_immortal(Args &&...args) {
        auto imm = GcPtr<T>(new(GC) T(std::forward<Args>(args)...));
        immortals.push_back(imm);
        return imm;
    }

    template<typename T, typename... Args>
    inline obj_result OK(Args &&...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }


    std::string get_type_name(const GcPtr<Object> &obj);


    template<typename... Values>
    obj_result parse_args(const t_vector &args, Values &... values) {
        if (args.size() != sizeof...(Values)) {
            return ERR(fmt::format("Expected {} arguments, but {} were given", sizeof...(Values), args.size()));
        }

        return assign_args<0>(args, values...);  // Assign arguments to values
    }

    template<typename T>
    T *get_value(const GcPtr<Object> &obj) {
        return dynamic_cast<T *>(obj.get());
    }

    template<size_t I, typename... Values>
    obj_result assign_args(const t_vector &args, Values &... values) {
        constexpr size_t N = sizeof...(Values);
        if constexpr (I < N) {
            using ValueType = std::remove_pointer<typename std::tuple_element<I, std::tuple<Values...>>::type>::type;
            auto &value = std::get<I>(std::tie(values...));

            if (auto *val = get_value<ValueType>(args[I])) {
                value = val;
            } else {
                //TODO: find a way to get the type name of ValueType
                return ERR(fmt::format("Expected argument {} to be of type {}, but got {}", I, typeid(ValueType).name(),
                                       get_type_name(args[I])));
            }

            return assign_args < I + 1 >(args, values...);  // Recursive call to assign the next argument
        }

        return OK();
    }


    extern GcPtr<Int> int_cache[256];

    void init_caches();

    GcPtr<Float> make_float(double value);

    GcPtr<Result> make_result(const GcPtr<Object> &value, bool is_error);

    inline GcPtr<Result> make_ok(const GcPtr<Object> &value){
        return make_result(value, false);
    }

    inline GcPtr<Result> make_error(const GcPtr<Object> &value) {
        return make_result(value, true);
    }

    GcPtr<Int> make_int(int64_t value);

    GcPtr<String> make_string(std::string value);

    GcPtr<NativeFunction> make_native_function(std::string name, std::string doc, const NativeFunctionPtr &function);

    GcPtr<Module> make_module(std::string path, const GcPtr<Map> &globals);

    GcPtr<NativeStruct>
    make_native_struct(std::string name, std::string doc, NativeFunctionPtr constructor, const method_map &methods);

} // namespace bond
