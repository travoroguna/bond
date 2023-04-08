//
// Created by travor on 3/18/23.
//

#include "vm.h"

namespace bond {
#define CALL_USER_IMPLEMENTATION(OBJ, ROUTER, NAME, ...) \
    auto result = (OBJ)->as<StructInstance>()->$##ROUTER(__VA_ARGS__);\
    if (!result.has_value()) {\
    runtime_error((#NAME " is not defined"),\
            result.error(),\
            m_current_frame->get_span());\
    continue;\
}\
std::vector<GcPtr<Object>> args{__VA_ARGS__};\
call_bound_method(dynamic_cast<BoundMethod*>(result.value().get()), args)

    void Vm::run(const GcPtr<Code> &code) {
        m_stop = false;
        auto func =
                GarbageCollector::instance().make<Function>(code, std::vector<std::pair<std::string, SharedSpan>>(),
                                                            "__main__");

        func->set_globals(m_globals);
        push(func);
        call_function(func, std::vector<GcPtr<Object>>());
        exec();
    }

    void Vm::call_function(const GcPtr<Function> &function, const std::vector<GcPtr<Object>> &args) {
        GarbageCollector::instance().stop_gc();
        auto frame = &m_frames[m_frame_pointer++];

        if (m_frame_pointer >= FRAME_MAX) {
            runtime_error("stack overflow", RuntimeError::GenericError, m_current_frame->get_span());
            return;
        }
        auto params = function->get_params();

        if (args.size() != params.size()) {
            runtime_error(fmt::format("expected {} arguments, got {}", params.size(), args.size()),
                          RuntimeError::GenericError,
                          m_current_frame->get_span());
            return;
        }

        auto local_args = GarbageCollector::instance().make<Map>();

        for (size_t i = 0; i < args.size(); i++) {
            local_args->set(GarbageCollector::instance().make<String>(params[i].first), args[i]);
        }

        frame->set_function(function);

        frame->set_globals(function->get_globals());
        frame->set_locals(local_args);
        m_current_frame = frame;
        GarbageCollector::instance().resume_gc();
    }

    void Vm::call_bound_method(const GcPtr<BoundMethod> &bound_method, std::vector<GcPtr<Object>> &args) {
        auto method = bound_method->get_method()->as<Function>();

        auto params = method->get_params();
        if (params.size() - 1 != args.size()) {
            runtime_error(fmt::format("expected {} arguments, got {}", params.size() - 1,
                                      args.size()),
                          RuntimeError::GenericError,
                          m_current_frame->get_span());
            return;
        }

        args.insert(args.begin(), bound_method->get_receiver());
        call_function(method, args);
    }

    void Vm::create_instance(const GcPtr<Struct> &_struct, const std::vector<GcPtr<Object>> &args) {
        auto variables = _struct->get_instance_variables();
        if (variables.size() != args.size()) {
            runtime_error(fmt::format("expected {} arguments, got {}", variables.size(), args.size()),
                          RuntimeError::GenericError,
                          m_current_frame->get_span());
            return;
        }

        GarbageCollector::instance().stop_gc();
        auto instance = GarbageCollector::instance().make<StructInstance>(_struct);
        for (size_t i = 0; i < args.size(); i++) {
            instance->set_attr(variables[i], args[i]);
        }

        push(instance);
        GarbageCollector::instance().resume_gc();
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
            default:
                err = error;
                break;
        }

        m_stop = true;
        m_ctx->error(span, err);
    }

#define BINARY_OP(X) { \
    GarbageCollector::instance().stop_gc();                   \
    auto right = pop(); \
    auto left = pop();  \
    auto result = left->$##X(right); \
    if (!result.has_value()) { \
        runtime_error(fmt::format("unable to " #X " {} and {}", left->str(), right->str()), result.error(), m_current_frame->get_span());\
        continue;\
    }\
    push(result.value());                                    \
    GarbageCollector::instance().resume_gc();                   \
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
                case Opcode::LOAD_CONST:
                    push(m_current_frame->get_constant());
                    break;

                case Opcode::CREATE_FUNCTION: {
                    auto func = m_current_frame->get_constant()->as<Function>();
                    func->set_globals(m_globals);
                    push(func);
                    break;
                }

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
                    if (m_frame_pointer == 1) {
                        m_stop = true;
                        break;
                    }
                    m_frame_pointer--;
                    m_current_frame = &m_frames[m_frame_pointer - 1];
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
//        if (!m_current_frame->has_global(name)) {
//          auto err = fmt::format("Global variable {} is not defined at this point",
//                                 name->as<String>()->get_value());
//          runtime_error(err, RuntimeError::GenericError, m_current_frame->get_span());
//          continue;
//        }
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
//        if (!m_current_frame->has_local(name)) {
//          auto err = fmt::format("Local variable {} does not exist", name->as<String>()->get_value());
//          runtime_error(err, RuntimeError::GenericError, m_current_frame->get_span());
//          continue;
//        }
                    push(m_current_frame->get_local(name));
                    break;
                }

                case Opcode::POP_TOP:
                    pop();
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

                    if (list->is<StructInstance>()) {
                        CALL_USER_IMPLEMENTATION(list, get_item, __get_item__, index);
                        break;
                    }

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

                    if (list->is<StructInstance>()) {
                        CALL_USER_IMPLEMENTATION(list, set_item, __set_item__, index, value);
                        break;
                    }

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
                    GarbageCollector::instance().stop_gc();
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
                    GarbageCollector::instance().resume_gc();
                    break;
                }

                case Opcode::JUMP: {
                    auto position = m_current_frame->get_oprand();
                    m_current_frame->jump_absolute(position);
                    break;
                }

                case Opcode::OR: {
                    GarbageCollector::instance().stop_gc();

                    auto right = pop();
                    auto left = pop();

                    auto res = left->$_bool();

                    if (!res.has_value()) {
                        runtime_error(fmt::format("unable to convert {} to bool", left->str()),
                                      res.error(),
                                      m_current_frame->get_span());
                        GarbageCollector::instance().resume_gc();
                        continue;
                    }

                    if (res.value()->as<Bool>()->get_value()) {
                        push(left);
                    } else {
                        push(right);
                    }
                    GarbageCollector::instance().resume_gc();
                    break;
                }

                case Opcode::AND: {
                    GarbageCollector::instance().stop_gc();
                    auto right = pop();
                    auto left = pop();

                    auto l_bool = left->$_bool();
                    auto r_bool = right->$_bool();

                    if (!l_bool) {
                        runtime_error(fmt::format("unable to convert {} to bool", left->str()),
                                      l_bool.error(),
                                      m_current_frame->get_span());
                        GarbageCollector::instance().resume_gc();
                        continue;
                    }

                    if (!r_bool) {
                        runtime_error(fmt::format("unable to convert {} to bool", right->str()),
                                      r_bool.error(),
                                      m_current_frame->get_span());
                        GarbageCollector::instance().resume_gc();
                        continue;
                    }

                    auto l = l_bool.value()->as<Bool>()->get_value();
                    auto r = r_bool.value()->as<Bool>()->get_value();

                    if (l && r) {
                        push(right);
                    } else {
                        push(l ? right : left);
                    }
                    GarbageCollector::instance().resume_gc();
                    break;
                }

                case Opcode::ITER: {
                    GarbageCollector::instance().stop_gc();
                    auto expr = pop();
                    auto iter = expr->$iter(expr);

                    if (!iter.has_value()) {
                        runtime_error(fmt::format("unable to iterate over {}", expr->str()),
                                      iter.error(),
                                      m_current_frame->get_span());
                        GarbageCollector::instance().resume_gc();
                        continue;
                    }

                    push(iter.value());
                    GarbageCollector::instance().resume_gc();
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
                    GarbageCollector::instance().stop_gc();
                    auto next = peek()->$has_next();
                    auto jump_pos = m_current_frame->get_oprand();

                    if (!next.has_value()) {
                        runtime_error(fmt::format("unable to get next item from {}", peek()->str()),
                                      next.error(),
                                      m_current_frame->get_span());
                        GarbageCollector::instance().resume_gc();
                        continue;
                    }

                    auto jump_condition = next.value()->$_bool();

                    if (!jump_condition.has_value()) {
                        runtime_error(fmt::format("unable to convert {} to bool", next.value()->str()),
                                      jump_condition.error(),
                                      m_current_frame->get_span());
                        GarbageCollector::instance().resume_gc();
                        continue;
                    }

                    if (!jump_condition.value()->as<Bool>()->get_value()) {
                        m_current_frame->jump_absolute(jump_pos);
                    }
                    GarbageCollector::instance().resume_gc();
                    break;

                }

                case Opcode::CALL: {
                    auto arg_count = m_current_frame->get_oprand();
                    std::vector<GcPtr<Object>> args;

                    for (int i = 0; i < arg_count; i++) {
                        args.insert(args.begin(), pop());
                    }

                    auto func = pop();

                    if (func->is<NativeFunction>()) {
                        auto f = func->as<NativeFunction>();
                        auto result = f->get_fn()(args);

                        if (result.has_value()) {
                            push(result.value());
                            continue;
                        }
                        runtime_error(result.error().message, result.error().error, m_current_frame->get_span());
                    } else if (func->is<Function>()) {
                        auto f = func->as<Function>();
                        call_function(f, args);
                        break;
                    } else if (func->is<Struct>()) {
                        auto st = func->as<Struct>();
                        create_instance(st, args);
                        break;
                    } else if (func->is<BoundMethod>()) {
                        auto bm = func->as<BoundMethod>();
                        call_bound_method(bm, args);
                        break;
                    }
                    runtime_error(fmt::format("{} is not callable", func->str()), RuntimeError::GenericError,
                                  m_current_frame->get_span());
                    continue;
                }
                case Opcode::CREATE_STRUCT: {
                    auto st = m_current_frame->get_constant();
                    st->as<Struct>()->set_globals(m_globals);
                    push(st);
                    break;
                }
                case Opcode::GET_ATTRIBUTE: {
                    auto attr = m_current_frame->get_constant();
                    auto obj = pop();
                    auto result = obj->$get_attribute(attr);

                    if (!result.has_value()) {
                        runtime_error(fmt::format("unable to get attribute {} from {}", attr->str(), obj->str()),
                                      result.error(),
                                      m_current_frame->get_span());
                        continue;
                    }

                    push(result.value());
                    break;
                }
                case Opcode::SET_ATTRIBUTE: {
                    auto attr = m_current_frame->get_constant();

                    auto value = pop();
                    auto obj = pop();

                    auto result = obj->$set_attribute(attr, value);
                    if (!result.has_value()) {
                        runtime_error(fmt::format("unable to set attribute {} as {}", attr->str(), obj->str()),
                                      result.error(),
                                      m_current_frame->get_span());
                        continue;
                    }
                    push(result.value());
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