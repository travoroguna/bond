//
// Created by travor on 3/18/23.
//
#pragma once

namespace bond {
class BinaryOp;
class Unary;
class TrueLiteral;
class FalseLiteral;
class NumberLiteral;
class StringLiteral;
class NilLiteral;
class Identifier;
class List;
class GetItem;
class SetItem;
class Print;
class ExprStmnt;
class NewVar;
class Block;
class Assign;
class If;

class NodeVisitor {
public:
  NodeVisitor();
  virtual void visit_bin_op(BinaryOp *expr) = 0;
  virtual void visit_unary(Unary *expr) = 0;
  virtual void visit_true_lit(TrueLiteral *expr) = 0;
  virtual void visit_false_lit(FalseLiteral *expr) = 0;
  virtual void visit_num_lit(NumberLiteral *expr) = 0;
  virtual void visit_string_lit(StringLiteral *expr) = 0;
  virtual void visit_nil_lit(NilLiteral *expr) = 0;
  virtual void visit_identifier(Identifier *expr) = 0;
  virtual void visit_print(Print *stmnt) = 0;
  virtual void visit_expr_stmnt(ExprStmnt *stmnt) = 0;
  virtual void visit_new_var(NewVar *stmnt) = 0;
  virtual void visit_assign(Assign *stmnt) = 0;
  virtual void visit_block(Block *stmnt) = 0;
  virtual void visit_list(List *expr) = 0;
  virtual void visit_get_item(GetItem *expr) = 0;
  virtual void visit_set_item(SetItem *expr) = 0;
  virtual void visit_if(If *stmnt) = 0;
};
}
