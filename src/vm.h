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

namespace bond {

    class Frame{
    public:
        Frame() = default;
        explicit Frame(const GcPtr<Code> &code) { m_code = code; }
        GcPtr<Object> get_constant() { return m_code->get_constant(m_code->get_code(m_ip++)); }
        Opcode get_opcode() { return static_cast<Opcode>(m_code->get_code(m_ip++)); }
        SharedSpan get_span() { return m_code->get_span(m_ip); }

        void set_code(const GcPtr<Code> &code) { m_code = code; m_ip=0; }
        void set_globals(const GcPtr<Map> &globals) { m_globals = globals; }

        void set_global(const GcPtr<Object> &key, const GcPtr<Object> &value) { m_globals->set(key, value); }
        bool has_global(const GcPtr<Object> &key) { return m_globals->has(key); }
        GcPtr<Object> get_global(const GcPtr<Object> &key) { return m_globals->get_unchecked(key); }

    private:
        GcPtr<Code> m_code;
        size_t m_ip = 0;
        GcPtr<Map> m_globals;

    };

#define FRAME_MAX 1024

    class Vm: public Root{
    public:
        explicit Vm(Context *ctx) {
            m_ctx = ctx;
            m_True = GarbageCollector::instance().make_immortal<Bool>(true);
            m_False = GarbageCollector::instance().make_immortal<Bool>(false);
            m_Nil = GarbageCollector::instance().make_immortal<Nil>();
        }
        void run(const GcPtr<Code>& code);

    private:
        GcPtr<Bool> m_True;
        GcPtr<Bool> m_False;
        GcPtr<Nil> m_Nil;
        Context *m_ctx = nullptr;
        bool m_stop = false;

        size_t m_frame_pointer = 0;
        std::array<Frame, FRAME_MAX> m_frames;
        Frame *m_current_frame = nullptr;

        void exec();
        void runtime_error(const std::string &error, RuntimeError e, const SharedSpan &span);
        GcPtr<Map> m_globals = GarbageCollector::instance().make_immortal<Map>();
    };

} // bond

