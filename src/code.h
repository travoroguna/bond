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
#include "parser.h"

namespace bond {

    class CodeGenerator : public NodeVisitor {
    public:
        CodeGenerator(Context *ctx, Scopes *scopes);

        GcPtr<Code> generate_code(const std::vector<std::shared_ptr<Node>> &nodes);

        GcPtr<Code> generate_code(const std::shared_ptr<Node> &node);

        void visit_bin_op(BinaryOp *expr) override;

        void visit_unary(Unary *expr) override;

        void visit_true_lit(TrueLiteral *expr) override;

        void visit_false_lit(FalseLiteral *expr) override;

        void visit_num_lit(NumberLiteral *expr) override;

        void visit_string_lit(StringLiteral *expr) override;

        void visit_nil_lit(NilLiteral *expr) override;

        void visit_print(Print *stmnt) override;

        void visit_expr_stmnt(ExprStmnt *stmnt) override;

        void visit_identifier(Identifier *expr) override;

        void visit_new_var(NewVar *stmnt) override;

        void visit_assign(Assign *stmnt) override;

        void visit_block(Block *stmnt) override;

        void visit_list(List *expr) override;

        void visit_get_item(GetItem *expr) override;

        void visit_set_item(SetItem *expr) override;

        void visit_if(If *stmnt) override;

        void visit_while(While *stmnt) override;

        void visit_call(Call *expr) override;

        void visit_for(For *stmnt) override;

        void visit_func_def(FuncDef *stmnt) override;

        void visit_return(Return *stmnt) override;

        void visit_closure_def(ClosureDef *stmnt) override;

        void visit_struct(StructNode *stmnt) override;

        void visit_get_attribute(GetAttribute *expr) override;

        void visit_set_attribute(SetAttribute *expr) override;

        void visit_import(ImportDef *stmnt) override;


        bool m_in_function = false;
        bool m_in_closure = false;

    private:
        GcPtr<Code> m_code;
        Context *m_ctx;
        Scopes *m_scopes;

        void finish_generation();

        GcPtr<Function> create_function(FuncDef *stmnt);
    };

} // bond

#endif //BOND_CODE_H
