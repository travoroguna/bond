//
// Created by travor on 3/18/23.
//

#include "vm.h"

namespace bond {

    void Vm::run(const GcPtr<Code>& code) {
        auto frame = &m_frames[m_frame_pointer++];
        frame->set_code(code);
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
                err = fmt::format("Division: by zero {}", error);\
                break;
        }

        m_stop = true;
        m_ctx->error(span, err);
    }

    void Vm::exec() {
        if (m_frame_pointer == 0) return;

        while (!m_stop) {
            auto opcode = m_current_frame->get_opcode();

            switch (opcode) {

                case Opcode::LOAD_CONST:
                    push(m_current_frame->get_constant());
                    break;

                case Opcode::BIN_ADD: {
                    auto right = pop();
                    auto left = pop();

                    auto result = left->$add(right);

                    if (!result.has_value()) {
                        runtime_error(fmt::format("unable to add {} and {}", right->str(), left->str()), result.error(), m_current_frame->get_span());
                        continue;
                    }

                    push(result.value());
                    fmt::print("result is {}\n", result.value()->str());
                    break;
                }
                case Opcode::BIN_SUB:
                    break;
                case Opcode::BIN_MUL:
                    break;
                case Opcode::BIN_DIV:
                    break;
                case Opcode::RETURN:
                    m_stop = true;
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
            }
        }

    }

}; // bond