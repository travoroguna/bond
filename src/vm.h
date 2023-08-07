//
// Created by travor on 3/18/23.
//

#pragma once

#include <utility>
#include <vector>
#include <expected>
#include <type_traits>
#include <fmt/core.h>
#include <array>

#include "compiler/codegen.h"
#include "object.h"
#include "compiler/context.h"
#include "builtins.h"

#include <cassert>


namespace bond {
#define TO_BOOL(X) BOOL_STRUCT->create({(X)}).value()->as<Bool>()


    class Frame {
    public:
        Frame() = default;

        explicit Frame(const GcPtr<Code> &code) { m_code = code; }

        GcPtr<Object> get_constant() { return m_code->get_constant(m_code->get_code(m_ip++)); }

        Opcode get_opcode() { return static_cast<Opcode>(m_code->get_code(m_ip++)); }

        uint32_t get_oprand() { return m_code->get_code(m_ip++); }

        SharedSpan get_span() {
            if (m_ip == 0) {
                return m_code->get_span(0);
            }
            return m_code->get_span(m_ip - 1 );
        }

        void set_code(const GcPtr<Code> &code) {
            m_code = code;
            m_ip = 0;
        }

        void set_function(const GcPtr<Function> &function) {
            m_function = function;
            set_code(function->get_code());
        }

        GcPtr<Function> get_function() { return m_function; }

        void set_globals(const GcPtr<StringMap> &globals) { m_globals = globals; }

        void jump_absolute(size_t ip) { m_ip = ip; }

        void set_locals(const GcPtr<StringMap> &locals) { m_locals = locals; }

        void set_global(const std::string &key, const GcPtr<Object> &value) { m_globals->set(key, value); }

        bool has_global(const std::string &key) { return m_globals->has(key); }

        GcPtr<Object> get_global(const std::string &key) { return m_globals->get_unchecked(key); }

        void set_local(const std::string &key, const GcPtr<Object> &value) { m_locals->set(key, value); }

        bool has_local(const std::string &key) { return m_locals->has(key); }

        GcPtr<Object> get_local(const std::string &key) { return m_locals->get_unchecked(key); }

        GcPtr<StringMap> get_locals() { return m_locals; }
        GcPtr<StringMap> get_globals() { return m_globals; }

        void clear() {
            m_locals.reset();
            m_globals.reset();
            m_function.reset();
            m_code.reset();
            m_ip = 0;
        }


    private:
        GcPtr<Function> m_function;
        GcPtr<Code> m_code;
        size_t m_ip = 0;
        GcPtr<StringMap> m_locals;
        GcPtr<StringMap> m_globals;
    };




#define FRAME_MAX 1024

    using VectorArgs = std::vector<std::shared_ptr<Param>>;


    class Vm {
    public:
        explicit Vm(Context *ctx) {
            m_ctx = ctx;
            m_True = C_TRUE;
            m_False = C_FALSE;
            m_Nil = C_NONE;
            m_globals = MAP_STRUCT->create_instance<StringMap>();
            assert(m_globals.get() != nullptr);
            add_builtins_to_globals(m_globals);
        }

        Vm(Context *ctx, const GcPtr<StringMap> &globals) {
            m_ctx = ctx;
            m_True = C_TRUE;
            m_False = C_FALSE;
            m_Nil = C_NONE;
            m_globals = globals;
            add_builtins_to_globals(m_globals);
        }

        void run(const GcPtr<Code> &code);

        bool has_top() { return m_stack.size() > 0; }

        bool had_error() { return m_has_error; }

        GcPtr<StringMap> get_globals() { return m_globals; }

        void set_globals(const GcPtr<StringMap> &globals) { m_globals = globals; }

        void call_function(const GcPtr<Function> &function, const t_vector  &args, const GcPtr<StringMap> &locals = nullptr);

        void exec(uint32_t stop_frame = 0);

        void runtime_error(const std::string &error, RuntimeError e, const SharedSpan &span);

        void runtime_error(const std::string &error, RuntimeError e);

        template <typename ...T>
        [[nodiscard]] GcPtr<Object> call_slot(Slot slot, const GcPtr<Object>& instance, const t_vector & args, fmt::format_string<T...> fmt, T&&... fmt_args);

        std::expected<GcPtr<Object>, std::string> call_slot(Slot slot, const GcPtr<Object>& instance, const t_vector & args);



        inline GcPtr<Object> pop() {
            auto res = peek();
            m_stack.pop_back();
            return res;
        }

        bool call_object_ex(const GcPtr <Object> &obj, t_vector &args);


    private:
        GcPtr<Bool> m_True;
        GcPtr<Bool> m_False;
        GcPtr<None> m_Nil;
        Context *m_ctx = nullptr;
        bool m_stop = false;
        bool m_has_error = false;

        size_t m_frame_pointer = 0;
        std::array<Frame, FRAME_MAX> m_frames;
        Frame *m_current_frame = nullptr;
        t_vector m_args;

        std::expected<GcPtr<Module>, std::string> load_dynamic_lib(const std::string &path, std::string &alias);

        GcPtr<StringMap> m_globals;

        void create_instance(const GcPtr<Struct> &_struct, const t_vector &args);

        std::expected<GcPtr<Module>, std::string> create_module(const std::string &path, std::string &alias);

        void call_bound_method(const GcPtr<BoundMethod> &bound_method, t_vector  &args);

        std::expected<std::string, std::string> path_resolver(const std::string &path);

        void runtime_error(const std::string &error);

        void bin_op(Slot slot, const std::string &op_name);


        void setup_bound_call(const GcPtr<Object>& instance, const GcPtr<Function>& function, t_vector & args);

        inline void push(GcPtr<Object> const &obj) { m_stack.push_back(obj); }

        inline GcPtr<Object> peek() { return m_stack.back(); }
        inline GcPtr<Object> peek(size_t rel_index) { return m_stack[m_stack.size() - 1 - rel_index]; }

        t_vector m_stack;

        void compare_op(Slot slot, const std::string &op_name);

        void call_native_function(const GcPtr <Object> &func, t_vector &args);

        void call_script_function(const GcPtr <Object> &func, t_vector &args);

        void call_struct(const GcPtr <Object> &func, t_vector &args);

        void call_bound_method(const GcPtr <Object> &func, t_vector &args);

        void call_native_struct(const GcPtr <Object> &func, t_vector &args);

        void call_closure(const GcPtr <Object> &func, t_vector &args);

        void update_frame_pointer();

        void check_argument_count(const t_vector &args, const VectorArgs &params);

        static void
        set_local_arguments(Frame *frame, const t_vector &args,
                            const VectorArgs &params,
                            const GcPtr <StringMap> &locals);

        void call_object(const GcPtr <Object> &func, t_vector &args);


    };

};