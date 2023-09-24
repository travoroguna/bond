#pragma once

#include "compiler/ast.h"
#include "gc.h"
//#include "vm.h"
#include <optional>
#include <utility>
#include <cassert>
#include "object_helpers.h"
#include <thread>
#include <mutex>
#include <fmt/ranges.h>


using custom_str = std::basic_string<char, std::char_traits<char>, gc_allocator<char>>;
class t_string: public custom_str {
public:
    t_string(const std::string& str) : custom_str(str) {}
    t_string(const char* str) : custom_str(str) {}
    t_string(char *string, size_t i) {
        this->assign(string, i);
    }
    t_string() = default;
    t_string(const t_string& str) : custom_str(str) {}
    t_string(t_string&& str) noexcept : custom_str(str) {}
    t_string(const custom_str &str) : custom_str(str) {}

    t_string &operator=(const t_string &str) {
        custom_str::operator=(str);
        return *this;
    }

    t_string &operator=(const std::string &str) {
        custom_str::operator=(str);
        return *this;
    }
};

template <>
struct ::std::hash<t_string> {
    std::size_t operator()(const t_string& k) const {
        return std::hash<std::string>()(k.c_str());
    }
};

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

        CREATE_CLOSURE,
        CREATE_CLOSURE_EX,
        IMPORT_PRE_COMPILED,
        BUILD_DICT, CHECK_RESULT, REPL_TOP
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

        HASH,

        SIZE
    };


    class Object;

    using obj_result = std::expected<GcPtr<Object>, t_string>;

    t_string Slot_to_string(Slot slot);

    class Object : public GcObject {
    public:
        static const char *name() { return "Object"; }

        Object() = default;

        [[nodiscard]] virtual t_string str() const {
            return fmt::format("<Object at {}>", (void *) this);
        }

    };

    template<typename T, typename... Args>
    inline GcPtr<T> make(Args &&...args) {
        GC_collect_a_little();
        return GcPtr<T>(new(GC) T(std::forward<Args>(args)...));
    }

    using t_vector = std::vector<GcPtr<Object>, gc_allocator<GcPtr<Object>>>;
    using t_map = std::unordered_map<t_string, GcPtr<Object>, std::hash<t_string>, std::equal_to<>,
            gc_allocator<std::pair<const t_string, GcPtr<Object>>>>;


    using NativeMethodPtr = std::function<obj_result(const GcPtr<Object> &self, const t_vector &)>;
    using NativeFunctionPtr = std::function<obj_result(const t_vector &)>;

//    using NativeFunctionPtr = obj_result (*)(const t_vector &);
    class NativeInstance;

    using slot_array = std::array<NativeMethodPtr, Slot::SIZE>;
    using getter = std::function<obj_result(const GcPtr<Object> &)>;
    using setter = std::function<obj_result(const GcPtr<Object> &, const GcPtr<Object> &)>;
    using method_map = std::unordered_map<t_string, std::pair<NativeMethodPtr, t_string>>;



    class NativeStruct : public Object {
    public:
        NativeStruct(t_string name, t_string doc, NativeFunctionPtr constructor)
                : m_name(std::move(name)), m_doc(std::move(doc)), m_constructor(std::move(constructor)) { set_slots(); }

        NativeStruct(t_string name, t_string doc, NativeFunctionPtr constructor,
                     const std::unordered_map<t_string, std::pair<NativeMethodPtr, t_string>> &methods)
                : m_name(std::move(name)), m_doc(std::move(doc)), m_methods(methods),
                  m_constructor(std::move(constructor)) { set_slots(); }

        NativeStruct(t_string name, t_string doc, NativeFunctionPtr constructor,
                     const std::unordered_map<t_string, std::pair<NativeMethodPtr, t_string>> &methods,
                     std::unordered_map<t_string, std::pair<getter, setter>> &properties)
                : m_name(std::move(name)), m_doc(std::move(doc)), m_methods(methods),
                  m_constructor(std::move(constructor)), m_attributes(properties) { set_slots(); }

        void add_methods(const std::unordered_map<t_string, std::pair<NativeMethodPtr, t_string>> &methods);

        [[nodiscard]] std::optional<NativeMethodPtr> get_method(const t_string &name) const;

        [[nodiscard]] t_string get_name() const { return m_name; }

        [[nodiscard]] t_string get_doc() const { return m_doc; }

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

        bool has_method(const t_string &name) const;

        [[nodiscard]]
        t_string str() const override { return m_name; }

        std::unordered_map<t_string, std::pair<NativeMethodPtr, t_string>> &get_methods() { return m_methods; }

        void set_methods(const method_map &methods) { m_methods = methods; }

        std::optional<getter> get_getter(const t_string &name) const;

        std::optional<setter> get_setter(const t_string &name) const;

        std::unordered_map<t_string, std::pair<getter, setter>> &get_attributes() { return m_attributes; }

        std::expected<NativeFunctionPtr, t_string> get_static_method(const t_string& name);
        void add_static_method(const t_string &name, const NativeFunctionPtr &s_meth, const t_string &doc);
    protected:
        t_string m_name;
        t_string m_doc;
        std::unordered_map<t_string, std::pair<NativeMethodPtr, t_string>> m_methods;
        NativeFunctionPtr m_constructor;
        slot_array m_slots;

        //getter and setter
        std::unordered_map<t_string, std::pair<getter, setter>> m_attributes;

        //static methods
        std::unordered_map<t_string, std::pair<NativeFunctionPtr, t_string>> m_static_methods;

    };


    class NativeInstance : public Object {
    public:
        explicit NativeInstance(NativeStruct *native_struct) : m_native_struct(native_struct) {}

        NativeInstance() = default;

        [[nodiscard]] NativeStruct *get_native_struct() const { return m_native_struct; }

        [[nodiscard]] obj_result call_method(const t_string &name, const t_vector &args);

        bool has_method(const t_string &name);

        void set_native_struct(NativeStruct *native_struct) { m_native_struct = native_struct; }

        obj_result call_slot(Slot slot, const t_vector &args);

        virtual bool has_slot(Slot slot);

        [[nodiscard]] t_string str() const override {
            return fmt::format("<instance of {} at {}>", m_native_struct->get_name(), (void *) this);
        }

        std::optional<obj_result> get_attr(const t_string &name);

        std::optional<obj_result> set_attr(const t_string &name, const GcPtr<Object> &value);


    protected:
        NativeStruct *m_native_struct = nullptr;
    };

    obj_result OK(const GcPtr<Object> &object);

    obj_result OK();

    obj_result runtime_error(t_string error);



// builtin types

#define INSTANCE(NAME) static const char *name() { return #NAME; }

    class Float : public NativeInstance {
    public:
        INSTANCE(Float)

        explicit Float(double value) : m_value(value) {}

        [[nodiscard]] double get_value() const { return m_value; }

        [[nodiscard]] t_string str() const override { return fmt::format("{}", m_value); }


    private:
        double m_value;
    };

    struct ht_entry: public gc {
        GcPtr<Object> key;
        GcPtr<Object> value;
        size_t hash;

        ht_entry(const GcPtr<Object>& key, const GcPtr<Object>& value, size_t hash) {
            this->key = key;
            this->value = value;
            this->hash = hash;
        }
    };


    using hash_vector = std::vector<ht_entry*, gc_allocator<ht_entry*>>;

    class HashMap : public NativeInstance {
    public:
        INSTANCE(HashMap)

        HashMap() { expand(); }

        [[nodiscard]] t_string str() const override;

        std::expected<GcPtr<Object>, t_string> get(const GcPtr<Object> &key);

        std::expected<void, t_string> set(const GcPtr<Object> &key, const GcPtr<Object> &value);

        std::expected<void, t_string> remove(const GcPtr<Object> &key);

        std::expected<bool, t_string> has(const GcPtr<Object> &key);

        size_t size() const;

        size_t capacity() { return m_entries.size(); }

        hash_vector &get_entries() { return m_entries; }



    private:
        void expand();

        hash_vector m_entries;
        size_t m_size = 0;

        std::expected<void, t_string> set_entry(const GcPtr<Object> &key, const GcPtr<Object> &value);

        static std::expected<size_t, t_string> hash_key(const GcPtr<bond::Object> &key);
    };

    class Int : public NativeInstance {
    public:
        INSTANCE(Int)

        explicit Int(int64_t value) : m_value(value) {}

        [[nodiscard]] int64_t get_value() const { return m_value; }

        t_string str() const override { return fmt::format("{}", m_value); }

    private:
        int64_t m_value;
    };

    class Bytes : public NativeInstance {
    public:
        INSTANCE(Bytes)

        explicit Bytes(const std::vector<uint8_t> &value) : m_value(value) {}

        Bytes() = default;

        [[nodiscard]] std::vector<uint8_t> get_value() const { return m_value; }

        [[nodiscard]] t_string str() const override { return fmt::format("b'{}'", fmt::join(m_value, ", ")); }

    private:
        std::vector<uint8_t> m_value;
    };

    class Bool : public NativeInstance {
    public:
        INSTANCE(Bool)

        explicit Bool(bool value) : m_value(value) {}

        [[nodiscard]] bool get_value() const { return m_value; }

        [[nodiscard]] t_string str() const override { return m_value ? "true" : "false"; }

    private:
        bool m_value;
    };

    class String : public NativeInstance {
    public:
        INSTANCE(String)
        explicit String(t_string value) : m_value(std::move(value)) {}
        String() = default;

        String(char *base, size_t len) : m_value(base, len) {}

        [[nodiscard]] t_string get_value() const { return m_value; }

        [[nodiscard]] t_string &get_value_ref() { return m_value; }

        [[nodiscard]] t_string str() const override { return m_value; }

    private:
        t_string m_value;
    };

    class StringIterator : public NativeInstance {
    public:
        INSTANCE(StringIterator)
        explicit StringIterator(t_string value) : m_value(std::move(value)) {}

        [[nodiscard]] t_string get_value() const { return m_value; }

        [[nodiscard]] t_string str() const override { return fmt::format("<string iterator at {}>", (void *) this); }

        t_string m_value;
        size_t m_index = 0;
    };

    class None : public NativeInstance {
    public:
        INSTANCE(None)
        None() = default;

        [[nodiscard]] t_string str() const override { return "Nil"; }
    };

    class StringMap : public NativeInstance {
    public:
        INSTANCE(StringMap)
        StringMap() = default;

        StringMap(const t_map &map) : m_value(map) {}

        [[nodiscard]] t_map get_value() const { return m_value; }

        void set_value(t_map value) { m_value = std::move(value); }

        void set(const t_string &key, const GcPtr<Object> &obj);

        std::optional<GcPtr<Object>> get(const t_string &key);

        GcPtr<Object> get_unchecked(const t_string &key);

        bool has(const t_string &key) { return m_value.contains(key); }

    private:
        t_map m_value;
    };


    class Code : public NativeInstance {
    public:
        INSTANCE(Code)
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

        uint32_t current_index() { return (uint32_t)m_instructions.size(); }

        [[nodiscard]] t_string disassemble() const;

        GcPtr<Object> get_constant(size_t index) { return m_constants[index]; }

        uint32_t get_code(size_t index) { return m_instructions[index]; }

        uint32_t get_code_size() { return (uint32_t)m_instructions.size(); }

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
        std::unordered_map<t_string, uint32_t> m_string_map;
    };

    class Function : public NativeInstance {
    public:
        INSTANCE(Function)
        Function(t_string name, std::vector<std::shared_ptr<Param>> arguments, const GcPtr<Code> &code)
                : m_name(
                std::move(name)), m_arguments(std::move(arguments)), m_code(code) {}

        [[nodiscard]] t_string get_name() const { return m_name; }

        [[nodiscard]] std::vector<std::shared_ptr<Param>> &get_arguments() { return m_arguments; }

        [[nodiscard]] GcPtr<Code> get_code() const { return m_code; }

        void set_globals(const GcPtr<StringMap> &globals) { m_globals = globals; }

        [[nodiscard]] GcPtr<StringMap> get_globals() const { return m_globals; }

        void set_async() { m_is_async = true; }

        bool is_async() { return m_is_async; }


    private:
        t_string m_name;
        std::vector<std::shared_ptr<Param>> m_arguments;
        GcPtr<Code> m_code;
        GcPtr<StringMap> m_globals;
        bool m_is_async{false};
    };

    class Future : public NativeInstance {
    public:
        INSTANCE(Future)
        Future() = default;
        Future(const GcPtr<Object> &value) : m_value(value) {}
        [[nodiscard]] t_string str() const override { return fmt::format("<future at {}>", (void *) this); }

        void set_value(const GcPtr<Object> &value);

        [[nodiscard]] GcPtr<Object> get_value() {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this] { return m_ready; });
            return m_value;
        }

        bool is_ready() {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_ready;
        }

        void set_then(const GcPtr<Function> &func) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_then = func;
        }

    private:
        std::mutex m_mutex;
        std::condition_variable m_cv;
        bool m_ready = false;
        GcPtr<Object> m_value;
        std::optional<GcPtr<Function>> m_then {std::nullopt};
    };

    class Instance;

    class Struct : public NativeInstance {
    public:
        INSTANCE(Struct)

        Struct(t_string name, const std::vector<t_string> &fields) : m_name(std::move(name)), m_fields(fields) {}

        [[nodiscard]] t_string get_name() const { return m_name; }

        [[nodiscard]] std::vector<t_string> get_fields() const { return m_fields; }

        [[nodiscard]] GcPtr<Instance> create_instance(const t_map &fields);

        void add_method(const t_string &name, const GcPtr<Function> &func) { m_methods[name] = func; }

        [[nodiscard]] std::optional<GcPtr<Function>> get_method(const t_string &name) const;

        bool has_method(const t_string &name) const { return m_methods.contains(name); }

        void set_globals(const GcPtr<StringMap> &globals);

        [[nodiscard]] GcPtr<StringMap> get_globals() const { return m_globals; }

        std::unordered_map<t_string, GcPtr<Function>> &get_methods() { return m_methods; }

        t_string str() const override { return fmt::format("<struct {}>", m_name); }


    private:
        t_string m_name;
        std::vector<t_string> m_fields;
        std::unordered_map<t_string, GcPtr<Function>> m_methods;
        GcPtr<StringMap> m_globals;
    };


    class Instance : public NativeInstance {
    public:
        INSTANCE(Instance)
        Instance(Struct *type, t_map fields)
                : m_type(type), m_fields(std::move(fields)) {}

        [[nodiscard]] GcPtr<Struct> get_type() const { return m_type; }

        [[nodiscard]] t_map get_fields() const { return m_fields; }

        void set_fields(t_map fields) { m_fields = std::move(fields); }

        obj_result bind_method(const t_string &name);

        obj_result get_method(const t_string &name);

        obj_result get_field(const t_string &name);

        obj_result set_field(const t_string &name, const GcPtr<Object> &value);

        obj_result get_type();

        [[nodiscard]] t_string str() const override;

        Struct *get_struct() const { return m_type; }

        bool has_slot(Slot slot) override;


    private:
        Struct *m_type;
        t_map m_fields;

    };

    class NativeFunction : public NativeInstance {
    public:
        INSTANCE(NativeFunction)
        NativeFunction(t_string name, t_string doc, NativeFunctionPtr function)
                : m_name(std::move(name)), m_doc(std::move(doc)), m_function(std::move(function)) {}

        [[nodiscard]] t_string get_name() const { return m_name; }

        [[nodiscard]] t_string get_doc() const { return m_doc; }

        [[nodiscard]] NativeFunctionPtr get_function() const { return m_function; }

    private:
        t_string m_name;
        t_string m_doc;
        NativeFunctionPtr m_function;
    };

    class Module : public NativeInstance {
    public:
        INSTANCE(Module)
        explicit Module(t_string path, const GcPtr<StringMap> &globals) : m_globals(globals),
                                                                             m_path(std::move(path)) {}

        Module(t_string path, const t_map &objects);

        GcPtr<StringMap> get_globals() { return m_globals; }

        obj_result get_attribute(const t_string &name);

        t_string get_path() { return m_path; }

        void add_module(const t_string &name, const GcPtr<Module> &mod);

        t_string str() const override { return fmt::format("<module {}>", m_path); }

    private:
        GcPtr<StringMap> m_globals;
        t_string m_path;
    };


    class BoundMethod : public NativeInstance {
    public:
        INSTANCE(BoundMethod)
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
        INSTANCE(List)
        List() = default;

        List(const t_vector &elements);

        obj_result get_item(int64_t index);

        obj_result set_item(int64_t index, const GcPtr<Object> &item);

        obj_result size() const;

        void prepend(const GcPtr<Object> &item);

        void append(const GcPtr<Object> &item);

        void insert(int64_t index, const GcPtr<Object> &item);

        GcPtr<Object> pop();

        t_string str() const override;

        friend class ListIterator;

        size_t get_size() const;

        t_vector &get_elements() { return m_elements; }

    private:
        t_vector m_elements;

    };

    class ListIterator : public NativeInstance {
    public:
        INSTANCE(ListIterator)
        explicit ListIterator(const GcPtr<List> &list) : m_list(list) {}

        GcPtr<List> m_list;
        int64_t m_index;
    };


    class Result : public NativeInstance {
    public:
        INSTANCE(Result)
        Result(GcPtr<Object> value, bool is_error) : m_value(std::move(value)), m_error(is_error) {}

        [[nodiscard]] GcPtr<Object> get_value() const { return m_value; }

        [[nodiscard]] bool has_value() const { return !m_error; }

        [[nodiscard]] bool has_error() const { return m_error; }

        t_string str() const override { return fmt::format("{}({})", m_error ? "Error" : "Ok", m_value->str()); }

    private:
        GcPtr<Object> m_value;
        bool m_error;
    };

    class Closure : public NativeInstance {
    public:
        INSTANCE(Closure)
        Closure(GcPtr<Function> function, GcPtr<StringMap> up_values) : m_function(std::move(function)),
                                                                        m_up_values(std::move(up_values)) {}

        [[nodiscard]] GcPtr<Function> get_function() const { return m_function; }

        [[nodiscard]] GcPtr<StringMap> get_up_values() const { return m_up_values; }

    private:
        GcPtr<Function> m_function;
        GcPtr<StringMap> m_up_values;
    };


#define TRY(expr) if (auto result = (expr); !result) return std::unexpected(result.error())

    template<typename T>
    obj_result c_Default(const t_vector args) {
        T *value;
        auto res = parse_args(args, value);
        TRY(res);
        return OK(args[0]);
    }

    template<typename T, typename... Args>
    inline obj_result OK(Args &&...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    t_string get_type_name(const GcPtr<Object> &obj);


    template<typename... Values>
    obj_result parse_args(const t_vector &args, Values &... values) {
        if (args.size() != sizeof...(Values)) {
            return runtime_error(
                    fmt::format("Expected {} arguments, but {} were given", sizeof...(Values), args.size()));
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
                return runtime_error(
                        fmt::format("Expected argument {} to be of type {}, but got {}", I, typeid(ValueType).name(),
                                    get_type_name(args[I])));
            }

            return assign_args < I + 1 >(args, values...);  // Recursive call to assign the next argument
        }

        return OK();
    }

    GcPtr<Float> make_float(double value);

    GcPtr<List> make_list(const t_vector &values);

    GcPtr<Int> make_int(int64_t value);

    GcPtr<String> make_string(const t_string& value);

    GcPtr<NativeFunction> make_native_function(t_string name, t_string doc, const NativeFunctionPtr &function);

    GcPtr<Module> make_module(t_string path, const GcPtr<StringMap> &globals);

    GcPtr<NativeStruct>
    make_native_struct(t_string name, t_string doc, NativeFunctionPtr constructor, const method_map &methods);


    // init functions
    void init_closure();
    void init_result();
    void init_float();
    void init_int();
    void init_bool();
    void init_string();
    void init_none();
    void init_map();
    void init_struct();
    void init_instance_method();
    void init_function();
    void init_code();
    void init_module();
    void init_list();
    void init_hash_map();
    void init_future();
    void init_bytes();

    // result functions
    [[nodiscard]] GcPtr<Result> make_result(const GcPtr<Object>& value, bool is_error);

    [[nodiscard]] inline GcPtr<Result> make_ok(const GcPtr<Object> &value) {
        return make_result(value, false);
    }

    template<typename T, typename... Args>
    inline GcPtr<Result> make_ok(Args &&...args) {
        return make_ok(GcPtr<T>(new(GC) T(std::forward<Args>(args)...)));
    }

    [[nodiscard]] inline GcPtr<Result> make_error(const GcPtr<Object> &value) {
        return make_result(value, true);
    }

    template<typename T, typename... Args>
    inline GcPtr<Result> make_error(Args &&...args) {
        return make_error(GcPtr<T>(new(GC) T(std::forward<Args>(args)...)));
    }

}

// formatting
template<>
struct fmt::formatter<bond::GcPtr<bond::Object>> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const bond::GcPtr<bond::Object> &obj, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "{}", obj->str());
    }
};



// Variadic template to convert vector elements to a format argument pack.
template<typename... Args>
t_string format_impl(const t_string &format_string, const std::vector<fmt::format_context::format_arg> &args) {
    return fmt::vformat(format_string, fmt::basic_format_args<fmt::format_context>(args.data(), args.size()));
}


// Main function to call the format_impl with the vector of arguments.
t_string format_(const t_string &format_string, const std::vector<fmt::format_context::format_arg> &args);
std::expected<t_string, t_string> bond_format(const t_string &format_string, const bond::t_vector &args);