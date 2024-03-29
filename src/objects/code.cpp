//
// Created by travor on 06/07/2023.
//
#include "../object.h"
#include "../runtime.h"
#include <sstream>


namespace bond {
    size_t Code::simple_instruction(std::stringstream &ss, const char *name, size_t offset) const {
        ss << fmt::format("{}\n", name);
        return offset + 1;
    }

    size_t Code::constant_instruction(std::stringstream &ss, const char *name, size_t offset) const {
        auto constant = m_constants[m_instructions[offset + 1]];
        ss << fmt::format("{:<16} {:0>4}, {:<16}\n", name, m_instructions[offset + 1], constant->str());
        return offset + 2;
    }

    size_t Code::oprand_instruction(std::stringstream &ss, const char *name, size_t offset) const {
        ss << fmt::format("{:<16} {:<4}\n", name, m_instructions[offset + 1]);
        return offset + 2;
    }

    uint32_t Code::add_constant(const GcPtr<Object> &obj) {
        if (instanceof<Int>(obj.get())) {
            auto value = obj->as<Int>()->get_value();
            if (m_int_map.contains(value)) {
                return m_int_map[value];
            } else {
                m_int_map[value] = m_constants.size();
                m_constants.push_back(obj);
                return m_constants.size() - 1;
            }
        } else if (instanceof<Float>(obj.get())) {
            auto value = obj->as<Float>()->get_value();
            if (m_float_map.contains(value)) {
                return m_float_map[value];
            } else {
                m_float_map[value] = m_constants.size();
                m_constants.push_back(obj);
                return m_constants.size() - 1;
            }
        }

        m_constants.push_back(obj);
        return m_constants.size() - 1;
    }


    void Code::add_ins(Opcode code, const SharedSpan &span) {
        m_instructions.push_back(static_cast<uint8_t>(code));
        m_spans.push_back(span);
    }

    void Code::add_ins(Opcode code, uint32_t oprand, const SharedSpan &span) {
        m_instructions.push_back(static_cast<uint8_t>(code));
        m_instructions.push_back(oprand);
        m_spans.push_back(span);
        m_spans.push_back(span);
    }


    t_string Code::disassemble() const {
        std::stringstream ss;
        size_t count = 0;

#define SIMPLE_INSTRUCTION(name) \
        case Opcode::name:    \
            count = simple_instruction(ss, #name, count);\
            break

#define CONSTANT_INSTRUCTION(name) \
        case Opcode::name:    \
            count = constant_instruction(ss, #name, count);\
            break
#define OPRAND_INSTRUCTION(name) \
        case Opcode::name:    \
            count = oprand_instruction(ss, #name, count);\
            break

        size_t pre_line = 0;

        while (count < m_instructions.size()) {
            auto opcode = static_cast<Opcode>(m_instructions[count]);

            if (pre_line != m_spans[count]->line) {
                ss << fmt::format("{:6}  {:04} ", m_spans[count]->line, count);
                pre_line = m_spans[count]->line;

            } else {
                ss << fmt::format("{:6}  {:04} ", "", count);
            }

            switch (opcode) {
                CONSTANT_INSTRUCTION(LOAD_CONST);
                CONSTANT_INSTRUCTION(STORE_GLOBAL);
                CONSTANT_INSTRUCTION(LOAD_GLOBAL);
                CONSTANT_INSTRUCTION(CREATE_GLOBAL);
                CONSTANT_INSTRUCTION(CREATE_LOCAL);
                CONSTANT_INSTRUCTION(STORE_FAST);
                CONSTANT_INSTRUCTION(LOAD_FAST);
                CONSTANT_INSTRUCTION(ITER_NEXT);
                CONSTANT_INSTRUCTION(GET_ATTRIBUTE);
                CONSTANT_INSTRUCTION(SET_ATTRIBUTE);
                CONSTANT_INSTRUCTION(IMPORT);
                CONSTANT_INSTRUCTION(MAKE_ASYNC);
                CONSTANT_INSTRUCTION(CREATE_CLOSURE);
                CONSTANT_INSTRUCTION(CREATE_CLOSURE_EX);


                OPRAND_INSTRUCTION(UNPACK_SEQ);
                OPRAND_INSTRUCTION(BUILD_LIST);
                OPRAND_INSTRUCTION(BUILD_DICT);
                OPRAND_INSTRUCTION(JUMP_IF_FALSE);
                OPRAND_INSTRUCTION(JUMP);
                OPRAND_INSTRUCTION(CALL);
                OPRAND_INSTRUCTION(ITER_END);
                OPRAND_INSTRUCTION(CREATE_FUNCTION);
                OPRAND_INSTRUCTION(CREATE_STRUCT);
                OPRAND_INSTRUCTION(TRY);
                OPRAND_INSTRUCTION(BREAK);
                OPRAND_INSTRUCTION(CONTINUE);
                OPRAND_INSTRUCTION(CALL_METHOD);
                OPRAND_INSTRUCTION(IMPORT_PRE_COMPILED);

                SIMPLE_INSTRUCTION(BIT_OR);
                SIMPLE_INSTRUCTION(BIT_XOR);
                SIMPLE_INSTRUCTION(BIT_AND);
                SIMPLE_INSTRUCTION(ITER);
                SIMPLE_INSTRUCTION(BIN_ADD);
                SIMPLE_INSTRUCTION(BIN_SUB);
                SIMPLE_INSTRUCTION(BIN_MUL);
                SIMPLE_INSTRUCTION(NOT);
                SIMPLE_INSTRUCTION(UNARY_SUB);
                SIMPLE_INSTRUCTION(BIN_DIV);
                SIMPLE_INSTRUCTION(BIN_MOD);
                SIMPLE_INSTRUCTION(RETURN);
                SIMPLE_INSTRUCTION(PUSH_TRUE);
                SIMPLE_INSTRUCTION(PUSH_FALSE);
                SIMPLE_INSTRUCTION(PUSH_NIL);
                SIMPLE_INSTRUCTION(NE);
                SIMPLE_INSTRUCTION(EQ);
                SIMPLE_INSTRUCTION(LE);
                SIMPLE_INSTRUCTION(GT);
                SIMPLE_INSTRUCTION(GE);
                SIMPLE_INSTRUCTION(LT);
                SIMPLE_INSTRUCTION(POP_TOP);
                SIMPLE_INSTRUCTION(GET_ITEM);
                SIMPLE_INSTRUCTION(SET_ITEM);
                SIMPLE_INSTRUCTION(OR);
                SIMPLE_INSTRUCTION(AND);
                SIMPLE_INSTRUCTION(AWAIT);
                SIMPLE_INSTRUCTION(MAKE_OK);
                SIMPLE_INSTRUCTION(MAKE_ERROR);
            }

        }

#undef SIMPLE_INSTRUCTION
#undef CONSTANT_INSTRUCTION

        for (auto &constant: m_constants) {
            if (constant->is<Function>()) {
                auto func = constant->as<Function>();
                ss << fmt::format("\ndisassembly of function {}\n", func->get_name());
                ss << func->get_code()->disassemble();
            } else if (constant->is<Struct>()) {
                auto st = constant->as<Struct>();
                ss << fmt::format("\ndisassembly of struct {}\n", st->get_name());

                for (auto &[name, method]: st->get_methods()) {
                    ss << fmt::format("\ndisassembly of {}.{}\n", st->get_name(), name);
                    ss << method->as<Function>()->get_code()->disassemble();
                }
            }
        }
        return ss.str();
    }


    obj_result Code_construct(const t_vector &args) {
        Code *code;
        auto opt = parse_args(args, code);
        TRY(opt);
        return GcPtr<Code>(code);
    }

    void init_code() {
        Runtime::ins()->CODE_STRUCT = make_immortal<NativeStruct>("Code", "Code()", Code_construct);
    }
}