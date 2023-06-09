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

        void visit_try(Try *stmnt) override;

        void visit_break(Break *stmnt) override;

        void visit_continue(Continue *stmnt) override;

        void visit_async_def(AsyncDef *stmnt) override;

        void visit_await(Await *expr) override;

        void visit_structured_assign(StructuredAssign *stmnt) override;


        bool m_in_function = false;
        bool m_in_closure = false;

    private:
        GcPtr<Code> m_code;
        Context *m_ctx;
        Scopes *m_scopes;

        void finish_generation();

        GcPtr<Function> create_function(FuncDef *stmnt);

        std::vector<std::vector<uint32_t>> m_break_stack;
        std::vector<std::vector<uint32_t>> m_continue_stack;

        void start_loop();

        void finish_loop(uint32_t loop_end, uint32_t loop_start);

        void func_def(FuncDef *stmnt, bool is_async);

        std::expected<std::string, std::string> path_resolver(const std::string &path);
    };

} // bond

#endif //BOND_CODE_H
