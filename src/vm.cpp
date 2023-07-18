//
// Created by travor on 3/18/23.
//

#include "vm.h"
#include "import.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/codegen.h"

#include <filesystem>
#include <array>

#ifdef _WIN32

#include <Windows.h>

#else
#include <dlfcn.h>
#endif

namespace bond {

#define TODO() runtime_error("not implemented", RuntimeError::GenericError, m_current_frame->get_span())
#define TO_BOOL(X) BOOL_STRUCT->create({(X)}).value()->as<Bool>();

    void Vm::run(const GcPtr<Code> &code) {
        m_stop = false;

        auto func = FUNCTION_STRUCT->create_immortal<Function>("__main__",
                                                               std::vector<std::pair<std::string, SharedSpan>>(), code);

        func->set_globals(m_globals);
        push(func);
        call_function(func, t_vector());
        fmt::print("func\n{}\n", func->get_code()->disassemble());
        exec();
    }


    void Vm::call_function(const GcPtr<Function> &function, const t_vector &args, const GcPtr<Map> &locals) {

        auto frame = &m_frames[m_frame_pointer];
        m_frame_pointer++;


        if (m_frame_pointer >= FRAME_MAX) {
            if (m_current_frame != nullptr) {
                runtime_error("stack overflow", RuntimeError::GenericError, m_current_frame->get_span());
            } else {
                runtime_error("stack overflow", RuntimeError::GenericError);
            }
            return;
        }
        auto params = function->get_arguments();

        if (args.size() != params.size()) {
            if (m_current_frame != nullptr) {
                runtime_error(fmt::format("expected {} arguments, got {}", params.size(), args.size()),
                              RuntimeError::GenericError,
                              m_current_frame->get_span());
            } else {
                runtime_error(fmt::format("expected {} arguments, got {}", params.size(), args.size()),
                              RuntimeError::GenericError);
            }

            return;
        }

        auto local_args = MAP_STRUCT->create_instance<Map>();

        for (size_t i = 0; i < args.size(); i++) {
            local_args->set(params[i].first, args[i]);
        }

        if (locals) {
            for (auto &[name, obj]: locals->get_value()) {
                local_args->set(name, obj);
            }
        }

        frame->set_function(function);
        frame->set_globals(function->get_globals());
        frame->set_locals(local_args);

        m_current_frame = frame;
    }

    void Vm::setup_bound_call(const GcPtr<Object> &instance, const GcPtr<Function> &function, t_vector &args) {
        auto params = function->get_arguments();
        if (params.size() - 1 != args.size()) {
            runtime_error(fmt::format("expected {} arguments, got {}", params.size() - 1,
                                      args.size()),
                          RuntimeError::GenericError,
                          m_current_frame->get_span());
            return;
        }

        args.insert(args.begin(), instance);
        call_function(function, args);
    }

    void Vm::call_bound_method(const GcPtr<BoundMethod> &bound_method, t_vector &args) {
        auto method = bound_method->get_method()->as<Function>();
        setup_bound_call(bound_method->get_instance(), method, args);
    }


    void Vm::create_instance(const GcPtr<Struct> &_struct, const t_vector &args) {
        auto variables = _struct->get_fields();
        if (variables.size() != args.size()) {
            runtime_error(fmt::format("expected {} arguments, got {}", variables.size(), args.size()),
                          RuntimeError::GenericError,
                          m_current_frame->get_span());
            return;
        }

        t_map fields;

        for (size_t i = 0; i < args.size(); i++) {
            fields[variables[i]] = args[i];
        }

        push(_struct->create_instance(fields));
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
                err = fmt::format("Division by zero, {}", error);
                break;
            case RuntimeError::AttributeNotFound:
                err = fmt::format("Attribute not found, {}", error);
            default:
                err = error;
                break;
        }

        m_stop = true;
        m_has_error = true;


        push(make_string(err));

        for (size_t i = 0; i < m_frame_pointer; i++) {
            if (m_frames[i].get_function().get() == nullptr) continue;
            m_ctx->error(m_frames[i].get_span(), "");
        }

//        m_ctx->error(span, err);
        fmt::print(" {}\n", err);
    }

    void Vm::runtime_error(const std::string &error) {
        runtime_error(error, RuntimeError::GenericError);
    }

    void Vm::runtime_error(const std::string &error, RuntimeError e) {
        if (m_current_frame != nullptr) {
            runtime_error(error, e, m_current_frame->get_span());
        }

        m_stop = true;
        m_has_error = true;

        push(make_string(error));
    }

    void Vm::bin_op(Slot slot, const std::string &op_name) {
        if (peek(1)->is<NativeInstance>() and peek()->is<NativeInstance>()) {

            auto right = pop()->as<NativeInstance>();
            auto left = pop()->as<NativeInstance>();
            auto result = call_slot(slot, left, {right}, "unable to {} values of type {} and {}", op_name,
                                    get_type_name(left), get_type_name(right));
            push(result);
            return;
        }

        //TODO: better error reporting
        runtime_error(fmt::format("unable to {} values of type {} and {}", op_name, peek(1).type_name(),
                                  peek().type_name()), RuntimeError::TypeError);
    }

    void Vm::compare_op(Slot slot, const std::string &op_name) {
        if (!peek(1)->is<NativeInstance>() or !peek()->is<NativeInstance>()) {
            runtime_error(fmt::format("unable to {} values of type {} and {}", op_name, peek(1).type_name(),
                                      peek().type_name()), RuntimeError::TypeError);
            return;
        }

        auto right = pop()->as<NativeInstance>();
        auto left = pop()->as<NativeInstance>();
        if (left->has_slot(slot)) {
            auto result = call_slot(slot, left, {right}, "unable to {} values of type {} and {}", op_name,
                                    get_type_name(left), get_type_name(right));
            push(result);
            return;
        }
        auto result = call_slot(slot, right, {left}, "unable to {} values of type {} and {}", op_name,
                                get_type_name(left), get_type_name(right));
        push(result);
    }

    template<typename ...T>
    [[nodiscard]] GcPtr<Object>
    Vm::call_slot(Slot slot, const GcPtr<Object> &instance, const t_vector &args, fmt::format_string<T...> fmt,
                  T &&... fmt_args) {
        if (!instance->is<NativeInstance>()) {
            runtime_error(vformat(fmt, fmt::make_format_args(fmt_args...)));
            m_stop = true;
            return nullptr;
        }

        if (instance->is<Instance>() and slot != Slot::GET_ATTR and slot != Slot::SET_ATTR) {
            auto result = instance->as<Instance>()->call_slot(slot, {});

            if (!result.has_value()) {
                auto error = vformat(fmt, fmt::make_format_args(fmt_args...));
                runtime_error(fmt::format("{}  \nError {}", error, result.error()));
                m_stop = true;
                return nullptr;
            }

            auto args_ = const_cast<t_vector &>(args);
            setup_bound_call(instance, result.value()->as<Function>(), args_);
            exec(m_frame_pointer);

            if (m_stop) {
                auto err = vformat(fmt, fmt::make_format_args(fmt_args...));
                runtime_error(err);
                return nullptr;
            }

            return pop();
        }

        auto result = instance->as<NativeInstance>()->call_slot(slot, args);

        if (!result.has_value()) {
            auto error = vformat(fmt, fmt::make_format_args(fmt_args...));
            runtime_error(fmt::format("{}  \nError {} {}", error,
                                      instance->as<NativeInstance>()->get_native_struct()->get_name(), result.error()));
            return nullptr;
        }

        return *result;
    }

    void Vm::call_object(const GcPtr<Object> &func, t_vector &args) {
        if (func->is<NativeFunction>()) {
            auto f = func->as<NativeFunction>();

            auto result = f->get_function()(args);

            if (result.has_value()) {
                push(result.value());
                return;
            }
            runtime_error(result.error());
            return;

        } else if (func->is<Function>()) {
            auto f = func->as<Function>();
            call_function(f, args);
            return;
        } else if (func->is<Struct>()) {
            auto st = func->as<Struct>();
            create_instance(st, args);
            return;
        } else if (func->is<BoundMethod>()) {
            auto bm = func->as<BoundMethod>();
            call_bound_method(bm, args);
            return;
        } else if (func->is<NativeStruct>()) {
            auto st = func->as<NativeStruct>();
            auto res = st->get_constructor()(args);

            if (res.has_value()) {
                push(res.value());
                return;
            }
            runtime_error(res.error());
            return;
        } else if (func->is<Closure>()) {
            auto cl = func->as<Closure>();
            call_function(cl->get_function(), args, cl->get_up_values());
            return;
        }
        runtime_error(fmt::format("{} is not callable", func->str()));
    }


    void Vm::exec(uint32_t stop_frame) {
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
                case Opcode::IMPORT: {

                    auto path = pop()->as<String>()->get_value();
                    auto constant = m_current_frame->get_constant();
                    auto alias = constant->as<String>()->get_value();


                    auto module = Import::instance().import_module(m_ctx, path, alias);

                    if (!module.has_value()) {
                        runtime_error(module.error(), RuntimeError::GenericError, m_current_frame->get_span());
                        continue;
                    }

                    if (m_ctx->has_error()) m_stop = true;
                    m_current_frame->set_global(alias, module.value());
                    break;
                }
                case Opcode::BIN_ADD:
                    bin_op(Slot::BIN_ADD, "add");
                    break;

                case Opcode::BIN_SUB:
                    bin_op(Slot::BIN_SUB, "subtract");
                    break;

                case Opcode::BIN_MUL:
                    bin_op(Slot::BIN_MUL, "multiply");
                    break;

                case Opcode::BIN_DIV:
                    bin_op(Slot::BIN_DIV, "divide");
                    break;

                case Opcode::BIN_MOD:
                    bin_op(Slot::BIN_MOD, "modulo");
                    break;

                case Opcode::NE:
                    compare_op(Slot::NE, "compare (!=)");
                    break;
                case Opcode::EQ:
                    compare_op(Slot::EQ, "compare (==)");
                    break;
                case Opcode::LT:
                    bin_op(Slot::LT, "compare (<)");
                    break;
                case Opcode::LE:
                    bin_op(Slot::LE, "compare (<=)");
                    break;
                case Opcode::GT:
                    bin_op(Slot::GT, "compare (>)");
                    break;
                case Opcode::GE:
                    bin_op(Slot::GE, "compare (>=)");
                    break;
                case Opcode::TRY: {
                    if (peek()->is<Result>()) {
                        auto result = pop()->as<Result>();
                        auto jmp = m_current_frame->get_oprand();
                        if (result->has_error()) {
                            if (m_frame_pointer == 1) {
                                runtime_error(result->str(), RuntimeError::GenericError, m_current_frame->get_span());
                                break;
                            }
                            push(result);
                            break;
                        } else {
                            push(result->get_value());
                            m_current_frame->jump_absolute(jmp);
                            break;
                        }
                    }
                    runtime_error("try statement expects a Result", RuntimeError::GenericError,
                                  m_current_frame->get_span());
                    break;
                }
                case Opcode::RETURN: {
                    if (m_frame_pointer == stop_frame) {
                        m_frame_pointer--;
                        m_current_frame = &m_frames[m_frame_pointer - 1];
                        return;
                    }

                    if (m_frame_pointer == 1) {
                        m_stop = true;
                        break;
                    }
                    m_frame_pointer--;
                    m_current_frame = &m_frames[m_frame_pointer - 1];
                    break;
                }
                case Opcode::PUSH_TRUE:
                    push(m_True);
                    break;
                case Opcode::PUSH_FALSE:
                    push(m_False);
                    break;
                case Opcode::PUSH_NIL:
                    push(m_Nil);
                    break;
                case Opcode::LOAD_GLOBAL: {
                    auto name = m_current_frame->get_constant()->as<String>()->get_value();
                    if (!m_current_frame->has_global(name)) {
                        auto err = fmt::format("Global variable {} is not defined at this point",
                                               name);
                        runtime_error(err, RuntimeError::GenericError, m_current_frame->get_span());
                        continue;
                    }
                    push(m_current_frame->get_global(name));

                    break;
                }
                case Opcode::CREATE_GLOBAL: {
                    auto name = m_current_frame->get_constant()->as<String>()->get_value();
                    auto expr = pop();
                    m_current_frame->set_global(name, expr);
                    break;
                }
                case Opcode::STORE_GLOBAL: {
                    auto name = m_current_frame->get_constant()->as<String>()->get_value();
                    auto expr = peek();

                    m_current_frame->set_global(name, expr);
                    break;
                }

                case Opcode::CREATE_LOCAL: {


                    auto name = m_current_frame->get_constant()->as<String>()->get_value();;
                    auto expr = pop();

                    m_current_frame->set_local(name, expr);
                    break;
                }

                case Opcode::STORE_FAST: {
                    auto name = m_current_frame->get_constant()->as<String>()->get_value();;
                    auto expr = peek();

                    m_current_frame->set_local(name, expr);
                    break;
                }

                case Opcode::LOAD_FAST: {
                    auto name = m_current_frame->get_constant()->as<String>()->get_value();
//        if (!m_current_frame->has_local(name)) {
//          auto err = fmt::format("Local variable {} does not exist", name->as<String>()->get_value());
//          runtime_error(err, RuntimeError::GenericError, m_current_frame->get_span());
//          continue;
//        }
                    push(m_current_frame->get_local(name));
                    break;
                }

                case Opcode::POP_TOP: {
                    if (peek()->is<Result>()) {
                        runtime_error(fmt::format("result must be handled: {}", pop()->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }
                    pop();
                }
                    break;

                case Opcode::BUILD_LIST: {


                    auto size = m_current_frame->get_oprand();
                    auto list = LIST_STRUCT->create_instance<List>();

                    for (int i = 0; i < size; i++) {
                        list->prepend(pop());
                    }
                    push(list);
                    break;
                }

                case Opcode::GET_ITEM: {
                    auto index = pop();
                    auto list = pop();

                    push(call_slot(Slot::GET_ITEM, list, {index}, "unable to get item"));
                    break;
                }

                case Opcode::SET_ITEM: {
                    auto value = pop();
                    auto index = pop();
                    auto list = pop();

                    push(call_slot(Slot::SET_ITEM, list, {index, value}, "unable to set item"));
                    break;
                }

                case Opcode::JUMP_IF_FALSE: {
                    auto position = m_current_frame->get_oprand();
                    auto cond = pop();

                    auto res = BOOL_STRUCT->create({cond}).value()->as<Bool>();

                    if (!res->get_value()) {
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

                    auto res = BOOL_STRUCT->create({left}).value()->as<Bool>();

                    if (res->get_value()) {
                        push(left);
                    } else {
                        push(right);
                    }
                    break;
                }

                case Opcode::AND: {


                    auto right = pop();
                    auto left = pop();

                    auto l_bool = BOOL_STRUCT->create({left}).value()->as<Bool>();
                    auto r_bool = BOOL_STRUCT->create({right}).value()->as<Bool>();

                    auto l = l_bool->get_value();
                    auto r = r_bool->get_value();

                    if (l && r) {
                        push(right);
                    } else {
                        push(l ? right : left);
                    }
                    break;
                }

                case Opcode::ITER: {
                    auto expr = pop();

                    auto iter = call_slot(Slot::ITER, expr, {},
                                          "unable to iterate over expression\n  __iter__ is not defined");

                    if (iter.get() == nullptr) {
                        continue;
                    }

                    auto iterator = iter->as<NativeInstance>();
                    if (iterator.get() == nullptr) {
                        runtime_error(fmt::format("unable to iterate over {}\n  iterator is not an instance",
                                                  iterator->str()));
                        continue;
                    }

                    if (!iterator->has_slot(Slot::NEXT)) {
                        runtime_error(
                                fmt::format("unable to iterate over {}, __next__ is not implemented", iterator->str()),
                                RuntimeError::GenericError,
                                m_current_frame->get_span());
                        continue;
                    }

                    if (!iterator->has_slot(Slot::HAS_NEXT)) {
                        runtime_error(fmt::format("unable to iterate over {}, __has_next__ is not implemented",
                                                  iterator->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }

                    push(iterator);
                    break;
                }

                case Opcode::ITER_NEXT: {
                    auto next = call_slot(Slot::NEXT, peek(), {}, "unable to get next item");
                    if (next.get() == nullptr) {
                        continue;
                    }
                    auto local = m_current_frame->get_constant()->as<String>()->get_value();
                    m_current_frame->set_local(local, next);
                    break;
                }

                case Opcode::ITER_END: {
                    auto next = call_slot(Slot::HAS_NEXT, peek(), {}, "unable to get next item on {}",
                                          get_type_name(peek()));
                    if (next.get() == nullptr) {
                        continue;
                    }
                    auto jump_pos = m_current_frame->get_oprand();
                    auto jump_condition = TO_BOOL(next);

                    if (!jump_condition->get_value()) {
                        m_current_frame->jump_absolute(jump_pos);
                    }

                    break;

                }

                case Opcode::CALL: {
                    auto arg_count = m_current_frame->get_oprand();
                    m_args.clear();

                    for (int i = 0; i < arg_count; i++) {
                        m_args.insert(m_args.begin(), pop());
                    }

                    call_object(pop(), m_args);
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

                    if (obj->is<NativeInstance>()) {
                        auto result = obj->as<NativeInstance>()->get_attr(attr->as<String>()->get_value());
                        if (result.has_value()) {
                            auto res = result.value();
                            if (res.has_value()) {
                                push(res.value());
                                break;
                            } else {
                                runtime_error(
                                        fmt::format("unable to get attribute {} of {}\n  {}", attr->str(), obj->str(),
                                                    res.error()));
                                continue;
                            }
                        }
                    }
                    push(call_slot(Slot::GET_ATTR, obj, {attr}, "unable to get attribute {} of {}", attr->str(),
                                   obj->str()));
                    break;
                }
                case Opcode::SET_ATTRIBUTE: {
                    auto attr = m_current_frame->get_constant();

                    auto value = pop();
                    auto obj = pop();

                    if (obj->is<NativeInstance>()) {
                        auto result = obj->as<NativeInstance>()->set_attr(attr->as<String>()->get_value(), value);
                        if (result.has_value()) {
                            auto res = result.value();
                            if (res.has_value()) {
                                push(res.value());
                                break;
                            } else {
                                runtime_error(
                                        fmt::format("unable to set attribute {} of {}\n  {}", attr->str(), obj->str(),
                                                    res.error()));
                                continue;
                            }
                        }
                    }
                    push(call_slot(Slot::SET_ATTR, obj, {attr, value}, "unable to set attribute {} of {}", attr->str(),
                                   obj->str()));
                    break;
                }

                case Opcode::BREAK:
                case Opcode::CONTINUE:
                    m_current_frame->jump_absolute(m_current_frame->get_oprand());
                    break;
                case Opcode::MAKE_OK: {
                    push(make_result(pop(), false));
                    break;
                }
                case Opcode::MAKE_ERROR: {
                    push(make_result(pop(), true));
                    break;
                }
                case Opcode::UNARY_SUB: {
                    auto obj = pop();
                    if (obj->is<Int>()) {
                        push(make_int(-obj->as<Int>()->get_value()));
                        break;
                    } else if (obj->is<Float>()) {
                        push(make_float(-obj->as<Float>()->get_value()));
                        break;
                    }

                    runtime_error(fmt::format("unable to apply unary - to {}", obj->str()),
                                  RuntimeError::GenericError,
                                  m_current_frame->get_span());
                    continue;
                }
                case Opcode::NOT: {
                    auto obj = pop();
                    auto res = BOOL_STRUCT->create({obj}).value()->as<Bool>();
                    push(AS_BOOL(!res->get_value()));
                    break;
                }

                case Opcode::BIT_OR: {
                    auto right = pop();
                    auto left = pop();

                    if (!right->is<Int>() or !left->is<Int>()) {
                        runtime_error(fmt::format("unable to apply | to {} and {}", left->str(), right->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }

                    auto res = left->as<Int>()->get_value() | right->as<Int>()->get_value();
                    push(make_int(res));
                    break;
                }

                case Opcode::BIT_AND: {
                    auto right = pop();
                    auto left = pop();

                    if (!right->is<Int>() or !left->is<Int>()) {
                        runtime_error(fmt::format("unable to apply & to {} and {}", left->str(), right->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }

                    auto res = left->as<Int>()->get_value() & right->as<Int>()->get_value();
                    push(make_int(res));
                    break;
                }

                case Opcode::BIT_XOR: {
                    auto right = pop();
                    auto left = pop();

                    if (!right->is<Int>() or !left->is<Int>()) {
                        runtime_error(fmt::format("unable to apply ^ to {} and {}", left->str(), right->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }

                    auto res = left->as<Int>()->get_value() ^ right->as<Int>()->get_value();
                    push(make_int(res));
                    break;
                }

                case Opcode::CALL_METHOD: {
                    auto arg_size = m_current_frame->get_oprand();

                    t_vector args;
                    for (size_t i = 0; i < arg_size; i++) {
                        args.insert(args.begin(), pop());
                    }

                    auto name = pop()->as<String>()->get_value();
                    auto obj = pop();

                    if (!obj->is<NativeInstance>()) {
                        runtime_error(fmt::format("method {} of {} does not exist", name, obj->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                    }
                    else {
                        auto o = obj->as<NativeInstance>();
                        if (o->has_method(name)) {
                            auto res = o->call_method(name, args);

                            if (!res.has_value()) {
                                runtime_error(fmt::format("unable to call method\n  {}", name, res.error()),
                                              RuntimeError::GenericError,
                                              m_current_frame->get_span());
                                break;
                            }
                            push(res.value());
                            break;
                        }
                    }


                    if (obj->is<Instance>()) {
                        auto o = obj->as<Instance>();
                        auto meth = o->get_method(name);

                        if (!meth.has_value()) {
                            runtime_error(fmt::format("attribute {} is not callable \n  {}", name, meth.error()));
                            break;
                        }

                        setup_bound_call(o, meth.value()->as<Function>(), args);
                        break;
                    } else if (obj->is<Struct>()) {
                        auto o = obj->as<Struct>();
                        auto meth = o->get_method(name);

                        if (!meth.has_value()) {
                            runtime_error(
                                    fmt::format("static method {} does not exist in struct {}", name, o->get_name()));
                            break;
                        }
                        call_function(meth.value(), args);
                        break;
                    } else if (obj->is<Module>()) {
                        auto o = obj->as<Module>();
                        auto meth = o->get_attribute(name);

                        if (!meth.has_value()) {
                            runtime_error(fmt::format("method {} does not exist in module {}", name, o->get_path()));
                            break;
                        }
                        call_object(meth.value(), args);
                        break;
                    }

                    runtime_error(fmt::format("method {} of type {} does not exist", name, get_type_name(obj)),
                                  RuntimeError::AttributeNotFound,
                                  m_current_frame->get_span());
                    break;
                }

                case Opcode::UNPACK_SEQ: {
                    auto obj = pop();
                    auto count = m_current_frame->get_oprand();

                    //check if instance
                    if (!obj->is<NativeInstance>()) {
                        runtime_error(fmt::format("unable to unpack {}", obj->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }

                    auto o = obj->as<NativeInstance>();

                    // check iter
                    if (!o->has_slot(Slot::ITER)) {
                        runtime_error(fmt::format("object of type {} is not iterable", get_type_name(o)));
                        continue;
                    }

                    auto iter = call_slot(Slot::ITER, o, {}, "unable to unpack object of type {}", get_type_name(obj));

                    if (iter.get() == nullptr) {
                        runtime_error(fmt::format("object of type {} is not iterable", get_type_name(obj)));
                        return;
                    }

                    auto the_iterator = iter->as<NativeInstance>();

                    //check next and has_next
                    if (!the_iterator->has_slot(Slot::NEXT) or !the_iterator->has_slot(Slot::HAS_NEXT)) {
                        runtime_error(fmt::format(
                                "object of type {} is not iterable, __next__ or __has_next__ is not defined",
                                get_type_name(obj)));
                        continue;
                    }

                    // unpack
                    size_t i = 0;
                    while (true) {
                        auto has_next = call_slot(Slot::HAS_NEXT, the_iterator, {},
                                                  "unable to unpack object of type {}",
                                                  get_type_name(obj));
                        if (has_next.get() == nullptr) {
                            break;
                        }

                        auto cond = TO_BOOL(has_next);
                        if (!cond->get_value()) {
                            break;
                        }

                        auto next = call_slot(Slot::NEXT, the_iterator, {}, "unable to unpack object of type {}",
                                              get_type_name(obj));
                        if (next.get() == nullptr) {
                            break;
                        }

                        push(next);
                        i++;
                    }

                    if (i != count) {
                        runtime_error(fmt::format("unable to unpack object of type {}, expected {} elements, got {}",
                                                  get_type_name(obj), count, i));
                    }
                    break;
                }
                case Opcode::CREATE_CLOSURE: {
                    auto func = m_current_frame->get_constant()->as<Function>();
                    func->set_globals(m_current_frame->get_globals());
                    auto closure = CLOSURE_STRUCT->create_instance<Closure>(func->as<Function>(),
                                                                            m_current_frame->get_locals());
                    m_current_frame->set_local(func->get_name(), closure);
                }

            }

//            collect_if_needed();
        }
    }

}; // bond