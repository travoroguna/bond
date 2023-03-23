//
// Created by travor on 3/18/23.
//

#ifndef BOND_CODE_H
#define BOND_CODE_H

#include "nodevisitor.h"
#include "ast.h"
#include <cstdint>
#include <vector>
#include "vm.h"

namespace bond {
    enum class Opcode : uint32_t{
        LOAD_CONST, BIN_ADD, BIN_SUB, BIN_MUL, BIN_DIV, RETURN,
        PUSH_TRUE, PUSH_FALSE, PUSH_NIL
    };

    class Code{
        public:
            Code() {}
            Code(std::vector<uint32_t> code, std::vector<Object*> constants)
                : m_code{code}, m_constants{constants} {}
            std::vector<uint32_t> get_code() const { return m_code; }
            std::vector<Object*> get_constants() const { return m_constants; }

            uint32_t add_constant(Object* obj) { m_constants.push_back(obj);  return m_constants.size() - 1; }
            void add_code(Opcode code, SharedSpan span);
            void add_code(Opcode code, uint32_t oprand, SharedSpan span);
  

        private:
            std::vector<uint32_t> m_code{};
            std::vector<Object*> m_constants{};
            std::vector<SharedSpan> m_spans{};


    };

    class CodeGenerator : public NodeVisitor {
    public:
        CodeGenerator(Context *ctx);

        Code *generate_code(SharedExpr expr);
        void visit_bin_op(BinaryOp* expr) override;
        void visit_unary(Unary* expr) override;
        void visit_true_lit(TrueLiteral* expr) override;
        void visit_false_lit(FalseLiteral* expr) override;
        void visit_num_lit(NumberLiteral* expr) override;
        void visit_string_lit(StringLiteral* expr) override;
        void visit_nil_lit(NilLiteral* expr) override;
    private:
        Code *m_code;
        Context *m_ctx;
    };

} // bond

#endif //BOND_CODE_H
