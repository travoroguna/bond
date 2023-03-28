//
// Created by travor on 3/18/23.
//

#include "vm.h"

namespace bond {

    void Vm::run(const GcPtr<Code> &code) {
        m_stop = false;
        auto frame = &m_frames[m_frame_pointer++];
        frame->set_code(code);
        frame->set_globals(m_globals);
        m_current_frame = frame;
        exec();
    }

    void Vm::runtime_error(const std::string &error, RuntimeError e, const SharedSpan &span) {
        std::string err;
        switch (e) {
            case RuntimeError::TypeError:
                err = fmt::format("TypeError: {}", error);
                break;
            case RuntimeError::Unimplemented:
                err = fmt::format("Unimplemented: {}", error);
                break;
            case RuntimeError::DivisionByZero:
                err = fmt::format("Division by zero, {}", error);\
                break;
            case RuntimeError::GenericError:
                err = error;
        }

        m_stop = true;
        m_ctx->error(span, err);
    }


#define BINARY_OP(X) { \
    auto right = pop(); \
    auto left = pop();  \
    auto result = left->$##X(right); \
    if (!result.has_value()) { \
        runtime_error(fmt::format("unable to " #X " {} and {}", left->str(), right->str()), result.error(), m_current_frame->get_span());\
        continue;\
    }\
    push(result.value());\
    break;\
    }


    void Vm::exec() {
        if (m_frame_pointer == 0) return;

        while (!m_stop) {
            auto opcode = m_current_frame->get_opcode();

            switch (opcode) {
                case Opcode::LOAD_CONST:
                    push(m_current_frame->get_constant());
                    break;

                case Opcode::BIN_ADD: BINARY_OP(add)
                case Opcode::BIN_SUB: BINARY_OP(sub)
                case Opcode::BIN_MUL: BINARY_OP(mul)
                case Opcode::BIN_DIV: BINARY_OP(div)
                case Opcode::RETURN:
                    if (m_stack_ptr > 0) {
                        fmt::print("  {}\n", pop()->str());
                    }
                    m_stop = true;
                    m_frame_pointer--;
                    break;
                case Opcode::PUSH_TRUE:
                    push(m_True);
                    break;
                case Opcode::PUSH_FALSE:
                    push(m_False);
                    break;
                case Opcode::PUSH_NIL:
                    push(m_Nil);
                    break;
                case Opcode::PRINT:
                    fmt::print("{}\n", pop()->str());
                    break;
                case Opcode::LOAD_GLOBAL: {
                    auto name = m_current_frame->get_constant();
                    if (!m_current_frame->has_global(name)) {
                        auto err = fmt::format("Global variable {} does not exist", name->as<String>()->get_value());
                        runtime_error(err, RuntimeError::GenericError, m_current_frame->get_span());
                        continue;
                    }
                    push(m_current_frame->get_global(name));
                    break;
                }
                case Opcode::SET_GLOBAL: {
                    auto name = m_current_frame->get_constant();
                    auto expr = pop();

                    m_current_frame->set_global(name, expr);
                    break;
                }
                case Opcode::LOAD_FAST:
                    break;
                case Opcode::STORE_FAST:
                    break;
            }
        }

    }

}; // bond