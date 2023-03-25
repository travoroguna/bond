//
// Created by travor on 3/18/23.
//

#ifndef BOND_CODE_H
#define BOND_CODE_H

#include "nodevisitor.h"
#include "ast.h"
#include <cstdint>
#include <vector>
// #include "vm.h"
#include "object.h"


namespace bond {

    class CodeGenerator : public NodeVisitor {
    public:
        explicit CodeGenerator(Context *ctx);

        GcPtr <Code> generate_code(const SharedExpr& expr);
        void visit_bin_op(BinaryOp* expr) override;
        void visit_unary(Unary* expr) override;
        void visit_true_lit(TrueLiteral* expr) override;
        void visit_false_lit(FalseLiteral* expr) override;
        void visit_num_lit(NumberLiteral* expr) override;
        void visit_string_lit(StringLiteral* expr) override;
        void visit_nil_lit(NilLiteral* expr) override;
    private:
        GcPtr<Code> m_code;
        Context *m_ctx;
    };

} // bond

#endif //BOND_CODE_H
