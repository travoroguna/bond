//
// Created by travor on 3/18/23.
//

#ifndef BOND_CODEGEN_H
#define BOND_CODEGEN_H

#include "nodevisitor.h"
#include "ast.h"
#include <cstdint>
#include <vector>
// #include "vm.h"
#include "../object.h"
#include "parser.h"

namespace bond {

    class CodeGenerator : public NodeVisitor {
    public:
        CodeGenerator(Context *ctx, Scopes *scopes);

        GcPtr<Code> generate_code(const std::vector<std::shared_ptr<Node>> &nodes);

        GcPtr<Code> generate_code(const std::shared_ptr<Node> &node, bool can_error, bool f_generation = true);

        void set_mode_repl() { m_is_repl = true; }

        void visit(BinaryOp *expr) override;

        void visit(Unary *expr) override;

        void visit(TrueLiteral *expr) override;

        void visit(FalseLiteral *expr) override;

        void visit(NumberLiteral *expr) override;

        void visit(StringLiteral *expr) override;

        void visit(NilLiteral *expr) override;

        void visit(ExprStmnt *stmnt) override;

        void visit(Identifier *expr) override;

        void visit(NewVar *stmnt) override;

        void visit(Assign *stmnt) override;

        void visit(Block *stmnt) override;

        void visit(ListLiteral *expr) override;

        void visit(GetItem *expr) override;

        void visit(SetItem *expr) override;

        void visit(If *stmnt) override;

        void visit(While *stmnt) override;

        void visit(Call *expr) override;

        void visit(For *stmnt) override;

        void visit(FuncDef *stmnt) override;

        void visit(Return *stmnt) override;

        void visit(ClosureDef *stmnt) override;

        void visit(StructNode *stmnt) override;

        void visit(GetAttribute *expr) override;

        void visit(SetAttribute *expr) override;

        void visit(ImportDef *stmnt) override;

        void visit(Try *stmnt) override;

        void visit(Break *stmnt) override;

        void visit(Continue *stmnt) override;

        void visit(AsyncDef *stmnt) override;

        void visit(Await *expr) override;

        void visit(StructuredAssign *stmnt) override;

        void visit(CallMethod *expr) override;

        void visit(ResultStatement *expr) override;

        void visit(DictLiteral *expr) override;

        bool m_in_function = false;
        bool m_in_closure = false;


    private:
        GcPtr<Code> m_code;
        Context *m_ctx;
        Scopes *m_scopes;
        bool m_is_repl = false;
        bool is_last {false};

        void finish_generation(bool can_error);

        GcPtr<Function> create_function(FuncDef *stmnt);

        std::vector<std::vector<uint32_t>> m_break_stack;
        std::vector<std::vector<uint32_t>> m_continue_stack;

        void start_loop();

        void finish_loop(uint32_t loop_end, uint32_t loop_start);

        void func_def(FuncDef *stmnt, bool is_async);
    };

} // bond

#endif //BOND_CODEGEN_H
