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

#include "code.h"
#include "object.h"
#include "context.h"
#include "builtins.h"

#include <cassert>


namespace bond {

    class Frame {
    public:
        Frame() = default;

        explicit Frame(const GcPtr<Code> &code) { m_code = code; }

        GcPtr<Object> get_constant() { return m_code->get_constant(m_code->get_code(m_ip++)); }

        Opcode get_opcode() { return static_cast<Opcode>(m_code->get_code(m_ip++)); }

        uint32_t get_oprand() { return m_code->get_code(m_ip++); }

        SharedSpan get_span() { return m_code->get_span(m_ip - 1); }

        void set_code(const GcPtr<Code> &code) {
            m_code = code;
            m_ip = 0;
        }

        void set_function(const GcPtr<Function> &function) {
            m_function = function;
            set_code(function->get_code());
        }

        GcPtr<Function> get_function() { return m_function; }

        void set_globals(const GcPtr<Map> &globals) { m_globals = globals; }

        void jump_absolute(size_t ip) { m_ip = ip; }

        void set_locals(const GcPtr<Map> &locals) { m_locals = locals; }

        void set_global(const GcPtr<Object> &key, const GcPtr<Object> &value) { m_globals->set(key, value); }

        bool has_global(const GcPtr<Object> &key) { return m_globals->has(key); }

        GcPtr<Object> get_global(const GcPtr<Object> &key) { return m_globals->get_unchecked(key); }

        void set_local(const GcPtr<Object> &key, const GcPtr<Object> &value) { m_locals->set(key, value); }

        bool has_local(const GcPtr<Object> &key) { return m_locals->has(key); }

        GcPtr<Object> get_local(const GcPtr<Object> &key) { return m_locals->get_unchecked(key); }

        void mark() {
            m_code->mark();
            m_globals->mark();
            m_locals->mark();
        }

        void unmark() {
            m_code->unmark();
            m_globals->unmark();
            m_locals->unmark();
        }

    private:
        GcPtr<Function> m_function;
        GcPtr<Code> m_code;
        size_t m_ip = 0;
        GcPtr<Map> m_locals;
        GcPtr<Map> m_globals;
    };


    class Module : public Object {
    public:
        explicit Module(std::string path, const GcPtr<Map> &globals) : m_globals(globals), m_path(std::move(path)) {}

        Module(std::string path, const std::unordered_map<std::string, NativeFunctionPtr> &functions) : m_path(
                std::move(path)) {
            m_globals = GarbageCollector::instance().make<Map>();
            for (auto const &[name, function]: functions) {
                m_globals->set(GarbageCollector::instance().make<String>(name),
                               GarbageCollector::instance().make<NativeFunction>(function));
            }
        }

        Module(std::string path, const std::unordered_map<std::string, GcPtr<Object>> &objects) {
            m_path = std::move(path);

            m_globals = GarbageCollector::instance().make<Map>();
            for (auto const &[name, object]: objects) {
                m_globals->set(GarbageCollector::instance().make<String>(name), object);
            }
        }

        void mark() override {
            Object::mark();
            m_globals->mark();
        }

        void unmark() override {
            Object::mark();
            m_globals->unmark();
        }

        OBJ_RESULT $get_attribute(const GcPtr<Object> &index) override {
            if (m_globals->has(index)) {
                return m_globals->get_unchecked(index);
            }
            return std::unexpected(RuntimeError::AttributeNotFound);
        }

        OBJ_RESULT $set_attribute(const GcPtr<Object> &index, const GcPtr<Object> &value) override {
            if (!m_globals->has(index)) {
                return std::unexpected(RuntimeError::AttributeNotFound);
            }

            m_globals->set(index, value);
            return value;
        }

        std::string str() override { return fmt::format("<module '{}' at {}>", m_path, (void *) this); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

        GcPtr<Map> get_globals() { return m_globals; }

    private:
        GcPtr<Map> m_globals;
        std::string m_path;
    };


#define FRAME_MAX 1024

    class Vm : public Root {
    public:
        explicit Vm(Context *ctx) {
            m_ctx = ctx;
            m_True = GarbageCollector::instance().make_immortal<Bool>(true);
            m_False = GarbageCollector::instance().make_immortal<Bool>(false);
            m_Nil = GarbageCollector::instance().make_immortal<Nil>();
            m_globals = GarbageCollector::instance().make_immortal<Map>();
            assert(m_globals.get() != nullptr);
            add_builtins_to_globals(m_globals);
        }

        void run(const GcPtr<Code> &code);

        bool had_error() { return m_has_error; }

        GcPtr<Map> get_globals() { return m_globals; }

        void call_function(const GcPtr<Function> &function, const std::vector<GcPtr<Object>> &args);

        void exec();


    private:
        GcPtr<Bool> m_True;
        GcPtr<Bool> m_False;
        GcPtr<Nil> m_Nil;
        Context *m_ctx = nullptr;
        bool m_stop = false;
        bool m_has_error = false;

        size_t m_frame_pointer = 0;
        std::array<Frame, FRAME_MAX> m_frames;
        Frame *m_current_frame = nullptr;

        void mark() override;

        void unmark() override;


        void runtime_error(const std::string &error, RuntimeError e, const SharedSpan &span);

        void runtime_error(const std::string &error, RuntimeError e);


        std::expected<GcPtr<Module>, std::string> load_dynamic_lib(const std::string &path, std::string &alias);

        GcPtr<Map> m_globals;

        void print_stack();

        void create_instance(const GcPtr<Struct> &_struct, const std::vector<GcPtr<Object>> &args);

        std::expected<GcPtr<Module>, std::string> create_module(const std::string &path, std::string &alias);

        void call_bound_method(const GcPtr<BoundMethod> &bound_method, std::vector<GcPtr<Object>> &args);

        std::expected<std::string, std::string> path_resolver(const std::string &path);
    };

};