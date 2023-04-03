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
  frame->set_locals(GarbageCollector::instance().make<Map>());
  m_current_frame = frame;
  exec();
}

void Vm::runtime_error(const std::string &error, RuntimeError e, const SharedSpan &span) {
  std::string err;
  switch (e) {
    case RuntimeError::TypeError:err = fmt::format("TypeError: {}", error);
      break;
    case RuntimeError::Unimplemented:err = fmt::format("Unimplemented: {}", error);
      break;
    case RuntimeError::DivisionByZero:err = fmt::format("Division by zero, {}", error);\
      break;
    case RuntimeError::GenericError:err = error;
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

void Vm::print_stack() {
  fmt::print("Stack: ");
  for (size_t i = 0; i < m_stack_ptr; i++) {
    fmt::print("{} ", m_stack[i]->str());
  }
  fmt::print("\n");
}

void Vm::exec() {
  if (m_frame_pointer == 0) return;

  while (!m_stop) {
    auto opcode = m_current_frame->get_opcode();

    switch (opcode) {
      case Opcode::LOAD_CONST:push(m_current_frame->get_constant());
        break;

      case Opcode::BIN_ADD: BINARY_OP(add)
      case Opcode::BIN_SUB: BINARY_OP(sub)
      case Opcode::BIN_MUL: BINARY_OP(mul)
      case Opcode::BIN_DIV: BINARY_OP(div)
      case Opcode::NE: BINARY_OP(ne)
      case Opcode::EQ: BINARY_OP(eq)
      case Opcode::LT: BINARY_OP(lt)
      case Opcode::LE: BINARY_OP(le)
      case Opcode::GT: BINARY_OP(gt)
      case Opcode::GE: BINARY_OP(ge)
      case Opcode::RETURN:
        if (m_stack_ptr > 0) {
          fmt::print("  {}\n", pop()->str());
        }
        m_stop = true;
        m_frame_pointer--;
        break;
      case Opcode::PUSH_TRUE:push(m_True);
        break;
      case Opcode::PUSH_FALSE:push(m_False);
        break;
      case Opcode::PUSH_NIL:push(m_Nil);
        break;
      case Opcode::PRINT:fmt::print("{}\n", pop()->str());
        break;
      case Opcode::LOAD_GLOBAL: {
        auto name = m_current_frame->get_constant();
        if (!m_current_frame->has_global(name)) {
          auto err = fmt::format("Global variable {} is not defined at this point",
                                 name->as<String>()->get_value());
          runtime_error(err, RuntimeError::GenericError, m_current_frame->get_span());
          continue;
        }
        push(m_current_frame->get_global(name));

        break;
      }
      case Opcode::CREATE_GLOBAL: {
        auto name = m_current_frame->get_constant();
        auto expr = pop();

        m_current_frame->set_global(name, expr);
        break;
      }
      case Opcode::STORE_GLOBAL: {
        auto name = m_current_frame->get_constant();
        auto expr = peek();

        m_current_frame->set_global(name, expr);
        break;
      }

      case Opcode::CREATE_LOCAL: {
        auto name = m_current_frame->get_constant();
        auto expr = pop();

        m_current_frame->set_local(name, expr);
        break;
      }

      case Opcode::STORE_FAST: {
        auto name = m_current_frame->get_constant();
        auto expr = peek();

        m_current_frame->set_local(name, expr);
        break;
      }

      case Opcode::LOAD_FAST: {
        auto name = m_current_frame->get_constant();
        if (!m_current_frame->has_local(name)) {
          auto err = fmt::format("Local variable {} does not exist", name->as<String>()->get_value());
          runtime_error(err, RuntimeError::GenericError, m_current_frame->get_span());
          continue;
        }
        push(m_current_frame->get_local(name));
        break;
      }

      case Opcode::POP_TOP:pop();
        break;

      case Opcode::BUILD_LIST: {
        auto size = m_current_frame->get_oprand();
        auto list = GarbageCollector::instance().make<ListObj>();

        for (int i = 0; i < size; i++) {
          list->prepend(pop());
        }
        push(list);
        break;
      }

      case Opcode::GET_ITEM: {
        auto index = pop();
        auto list = pop();

        auto result = list->$get_item(index);
        if (!result.has_value()) {
          runtime_error(fmt::format("unable to get item {} from {}", index->str(), list->str()),
                        result.error(), m_current_frame->get_span());
          continue;
        }
        push(result.value());
        break;
      }

      case Opcode::SET_ITEM: {
        auto value = pop();
        auto index = pop();
        auto list = pop();

        auto result = list->$set_item(index, value);
        if (!result.has_value()) {
          runtime_error(fmt::format("unable to set item {} to {} in {}", index->str(), value->str(),
                                    list->str()), result.error(), m_current_frame->get_span());
          continue;
        }

        push(result.value());
        break;
      }

      case Opcode::JUMP_IF_FALSE: {
        auto position = m_current_frame->get_oprand();
        auto cond = pop();

        auto res = cond->$_bool();

        if (!res.has_value()) {
          runtime_error(fmt::format("unable to convert {} to bool", cond->str()),
                        res.error(),
                        m_current_frame->get_span());
          continue;
        }

        if (!res.value()->as<Bool>()->get_value()) {
          m_current_frame->jump_absolute(position);
        }
        break;
      }

      case Opcode::JUMP: {
        auto position = m_current_frame->get_oprand();
        m_current_frame->jump_absolute(position);
        break;
      }

      case Opcode::OR: {
        auto right = pop();
        auto left = pop();

        auto res = left->$_bool();

        if (!res.has_value()) {
          runtime_error(fmt::format("unable to convert {} to bool", left->str()),
                        res.error(),
                        m_current_frame->get_span());
          continue;
        }

        if (res.value()->as<Bool>()->get_value()) {
          push(left);
        } else {
          push(right);
        }
        break;
      }

      case Opcode::AND: {
        auto right = pop();
        auto left = pop();

        auto l_bool = left->$_bool();
        auto r_bool = right->$_bool();

        if (!l_bool) {
          runtime_error(fmt::format("unable to convert {} to bool", left->str()),
                        l_bool.error(),
                        m_current_frame->get_span());
          continue;
        }

        if (!r_bool) {
          runtime_error(fmt::format("unable to convert {} to bool", right->str()),
                        r_bool.error(),
                        m_current_frame->get_span());
          continue;
        }

        auto l = l_bool.value()->as<Bool>()->get_value();
        auto r = r_bool.value()->as<Bool>()->get_value();

        if (l && r) {
          push(right);
        } else {
          push(l ? right : left);
        }
        break;
      }

      case Opcode::ITER: {
        auto expr = pop();
        auto iter = expr->$iter(expr);

        if (!iter.has_value()) {
          runtime_error(fmt::format("unable to iterate over {}", expr->str()),
                        iter.error(),
                        m_current_frame->get_span());
          continue;
        }

        push(iter.value());
        break;
      }

      case Opcode::ITER_NEXT: {
        auto next = peek()->$next();

        if (!next.has_value()) {
          runtime_error(fmt::format("unable to get next item from {}", peek()->str()),
                        next.error(),
                        m_current_frame->get_span());
          continue;
        }

        auto local = m_current_frame->get_constant();
        m_current_frame->set_local(local, next.value());
        break;
      }

      case Opcode::ITER_END: {
        auto next = peek()->$has_next();
        auto jump_pos = m_current_frame->get_oprand();

        if (!next.has_value()) {
          runtime_error(fmt::format("unable to get next item from {}", peek()->str()),
                        next.error(),
                        m_current_frame->get_span());
          continue;
        }

        auto jump_condition = next.value()->$_bool();

        if (!jump_condition.has_value()) {
          runtime_error(fmt::format("unable to convert {} to bool", next.value()->str()),
                        jump_condition.error(),
                        m_current_frame->get_span());
          continue;
        }

        if (!jump_condition.value()->as<Bool>()->get_value()) {
          m_current_frame->jump_absolute(jump_pos);
        }
        break;

      }

    }
  }

}

void Vm::mark() {
  for (size_t i = 0; i < m_frame_pointer; i++) {
    m_frames[i].mark();
  }
  Root::mark();
}

void Vm::unmark() {
  for (size_t i = 0; i < m_frame_pointer; i++) {
    m_frames[i].unmark();
  }
  Root::unmark();
}

}; // bond