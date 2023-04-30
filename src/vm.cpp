//
// Created by travor on 3/18/23.
//

#include "vm.h"
#include "lexer.h"
#include "parser.h"
#include "code.h"
#include "result.h"

#include <filesystem>
#include <array>

#ifdef _WIN32

#include <Windows.h>

#else
#include <dlfcn.h>
#endif

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
            if (m_current_frame != nullptr) {
                runtime_error("stack overflow", RuntimeError::GenericError, m_current_frame->get_span());
            } else {
                runtime_error("stack overflow", RuntimeError::GenericError);
            }
            return;
        }
        auto params = function->get_params();

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


    std::expected<std::string, std::string> Vm::path_resolver(const std::string &path) {
#ifdef _WIN32
        auto test_compiled_native = m_ctx->get_lib_path() + path + ".dll";
        auto test_compiled_c_path = path + ".dll";
#else
        auto test_compiled_native = m_ctx->get_lib_path() + path + ".so";
        auto test_compiled_c_path = path + ".so";
#endif

        auto test_native = m_ctx->get_lib_path() + path + ".bd";
        auto test_user = path + ".bd";


        std::array<std::string, 4> paths = {test_compiled_native, test_compiled_c_path, test_native, test_user};

        for (auto &p: paths) {
            fmt::print("testing path: {}\n", p);
            if (std::filesystem::exists(p)) {
                return p;
            }
        }
//
        return std::unexpected(fmt::format("failed to resolve path {}", path));
    }

    std::expected<GcPtr<Module>, std::string> Vm::load_dynamic_lib(const std::string &path, std::string &alias) {
#ifdef _WIN32
        auto handle = LoadLibrary(path.c_str());
        if (!handle) {
            return std::unexpected(fmt::format("failed to load dynamic library {}: {}", path, GetLastError()));
        }

        // void bond_module_init(bond::Context *ctx, const char *path)
        auto init = (void (*)(bond::Context *, const std::string &)) GetProcAddress(handle, "bond_module_init");

        if (!init) {
            return std::unexpected(
                    fmt::format("failed to load dynamic library {}: {}, missing init function 'bond_module_init'", path,
                                GetLastError()));
        }


#else
        auto handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) {
            return std::unexpected(fmt::format("failed to load dynamic library {}: {}", path, dlerror()));
        }


        auto init = (void (*)(bond::Context *, const std::string &))  dlsym(handle, "bond_module_init");
        if (!init) {
            return std::unexpected(fmt::format("failed to load dynamic library {}: {}, missing init function 'bond_module_init'", path, dlerror()));
        }

#endif
        init(m_ctx, path);

        auto mod = m_ctx->get_module(path);

        fmt::print("gc addr: {}\n", (void *) &GarbageCollector::instance());

        for (auto &[k, v]: mod->as<Module>()->get_globals()->get_map()) {
            fmt::print("{}: {}\n", k->str(), v->str());
        }
        return mod->as<Module>();
    }

    std::expected<GcPtr<Module>, std::string> Vm::create_module(const std::string &path, std::string &alias) {
        auto res = path_resolver(path);

        if (!res) {
            return std::unexpected(res.error());
        }

        auto resolved_path = res.value();

        if (m_ctx->has_module(resolved_path)) {
            return m_ctx->get_module(resolved_path)->as<Module>();
        }

        if (resolved_path.ends_with(".dll") || resolved_path.ends_with(".so")) {
            return load_dynamic_lib(resolved_path, alias);
        }

        auto id = m_ctx->new_module(resolved_path);
        auto source = bond::Context::read_file(resolved_path);

        auto lexer = Lexer(source, m_ctx, id);
        auto parser = Parser(lexer.tokenize(), m_ctx);

        auto nodes = parser.parse();
        auto code_gen = CodeGenerator(m_ctx, parser.get_scopes());

        auto code = code_gen.generate_code(nodes);

        auto vm = Vm(m_ctx);
        vm.run(code);

        GarbageCollector::instance().add_root(&vm);

        if (vm.had_error()) {
            return std::unexpected(fmt::format("unable to import module {}", resolved_path));
        }

        GarbageCollector::instance().remove_root(&vm);

        GarbageCollector::instance().stop_gc();
        auto mod = GarbageCollector::instance().make<Module>(resolved_path, vm.get_globals());
        GarbageCollector::instance().resume_gc();

        m_ctx->add_module(resolved_path, mod);

        return mod;
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
        m_has_error = true;


        push(GarbageCollector::instance().make<String>(err));

        for (size_t i = m_frame_pointer - 1; i > 0; i--) {
            m_ctx->error(m_frames[i].get_span(), "");
        }

        m_ctx->error(span, err);
    }

    void Vm::runtime_error(const std::string &error, RuntimeError e) {
        m_stop = true;
        m_has_error = true;

        push(GarbageCollector::instance().make<String>(error));
    }


#define BINARY_OP(X) { \
    GarbageCollector::instance().stop_gc();                   \
    auto right = pop(); \
    auto left = pop(); \
    if (left->is<StructInstance>()) { \
        CALL_USER_IMPLEMENTATION(left, X, __##X##__, right);\
        break; \
    }                   \
    auto result = left->$##X(right); \
    if (!result.has_value()) { \
        runtime_error(fmt::format("unable to " #X " values of type {} and {}", left.type_name(), right.type_name()), result.error(), m_current_frame->get_span());\
        continue;\
    }\
    push(result.value());                                    \
    GarbageCollector::instance().resume_gc();                   \
    break;\
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

                case Opcode::IMPORT: {
                    auto path = pop()->as<String>()->get_value();
                    auto constant = m_current_frame->get_constant();
                    auto alias = constant->as<String>()->get_value();

                    GarbageCollector::instance().stop_gc();
                    auto module = create_module(path, alias);
                    if (!module.has_value()) {
                        runtime_error(module.error(), RuntimeError::GenericError, m_current_frame->get_span());
                        GarbageCollector::instance().resume_gc();
                        continue;
                    }
                    GarbageCollector::instance().resume_gc();
                    m_current_frame->set_global(constant, module.value());
                    break;
                }
                case Opcode::BIN_ADD: BINARY_OP(add)
                case Opcode::BIN_SUB: BINARY_OP(sub)
                case Opcode::BIN_MUL: BINARY_OP(mul)
                case Opcode::BIN_DIV: BINARY_OP(div)
                case Opcode::BIN_MOD: BINARY_OP(mod)
                case Opcode::NE: BINARY_OP(ne)
                case Opcode::EQ: BINARY_OP(eq)
                case Opcode::LT: BINARY_OP(lt)
                case Opcode::LE: BINARY_OP(le)
                case Opcode::GT: BINARY_OP(gt)
                case Opcode::GE: BINARY_OP(ge)
                case Opcode::TRY: {
                    if (peek()->is<BondResult>()) {
                        auto result = pop()->as<BondResult>();
                        auto jmp = m_current_frame->get_oprand();
                        if (result->is_error()) {
                            if (m_frame_pointer == 1) {
                                runtime_error(result->str(), RuntimeError::GenericError, m_current_frame->get_span());
                                break;
                            }
                            push(result);
                            break;
                        } else {
                            push(result->get());
                            m_current_frame->jump_absolute(jmp);
                            break;
                        }
                    }
                    runtime_error("try statement expects a BondResult", RuntimeError::GenericError,
                                  m_current_frame->get_span());
                    break;
                }
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
                    if (peek()->is<BondResult>()) {
                        runtime_error(fmt::format("result must be handled: {}", pop()->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }
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
                    auto iter = expr->$iter();

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

                        GarbageCollector::instance().stop_gc();
                        auto result = f->get_fn()(args);
                        GarbageCollector::instance().resume_gc();

                        if (result.has_value()) {
                            push(result.value());
                            GarbageCollector::instance().collect_if_needed();
                            continue;
                        }
                        runtime_error(result.error().message, result.error().error, m_current_frame->get_span());
                        continue;

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

                    auto res = func->call__(args);
                    if (res.has_value()) {
                        push(res.value());
                        continue;
                    }

                    runtime_error(fmt::format("{}", res.error()), RuntimeError::GenericError,
                                  m_current_frame->get_span());
                    continue;

                    auto result = func->$call(args);
                    if (result.has_value()) {
                        push(result.value());
                        continue;
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

                case Opcode::BREAK:
                case Opcode::CONTINUE:
                    m_current_frame->jump_absolute(m_current_frame->get_oprand());
                    break;
                case Opcode::UNARY_SUB: {
                    auto obj = pop();
                    if (obj->is<Integer>()) {
                        push(GarbageCollector::instance().make<Integer>(-obj->as<Integer>()->get_value()));
                        break;
                    } else if (obj->is<Float>()) {
                        push(GarbageCollector::instance().make<Float>(-obj->as<Float>()->get_value()));
                        break;
                    }

                    runtime_error(fmt::format("unable to apply unary - to {}", obj->str()),
                                  RuntimeError::GenericError,
                                  m_current_frame->get_span());
                    continue;
                }
                case Opcode::NOT: {
                    auto obj = pop();
                    auto res = obj->$_bool();

                    if (!res.has_value()) {
                        runtime_error(fmt::format("unable to convert {} to bool", obj->str()),
                                      res.error(),
                                      m_current_frame->get_span());
                        continue;
                    }
                    push(GarbageCollector::instance().make<Bool>(!res.value()->as<Bool>()->get_value()));
                    break;
                }

                case Opcode::BIT_OR: {
                    auto right = pop();
                    auto left = pop();

                    if (!right->is<Integer>() or !left->is<Integer>()) {
                        runtime_error(fmt::format("unable to apply | to {} and {}", left->str(), right->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }

                    auto res = left->as<Integer>()->get_value() | right->as<Integer>()->get_value();
                    push(GarbageCollector::instance().make<Integer>(res));
                    break;
                }

                case Opcode::BIT_AND: {
                    auto right = pop();
                    auto left = pop();

                    if (!right->is<Integer>() or !left->is<Integer>()) {
                        runtime_error(fmt::format("unable to apply & to {} and {}", left->str(), right->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }

                    auto res = left->as<Integer>()->get_value() & right->as<Integer>()->get_value();
                    push(GarbageCollector::instance().make<Integer>(res));
                    break;
                }

                case Opcode::BIT_XOR: {
                    auto right = pop();
                    auto left = pop();

                    if (!right->is<Integer>() or !left->is<Integer>()) {
                        runtime_error(fmt::format("unable to apply ^ to {} and {}", left->str(), right->str()),
                                      RuntimeError::GenericError,
                                      m_current_frame->get_span());
                        continue;
                    }

                    auto res = left->as<Integer>()->get_value() ^ right->as<Integer>()->get_value();
                    push(GarbageCollector::instance().make<Integer>(res));
                    break;
                }

            }

        }
    }

    void Vm::mark() {
        Root::mark();
        for (size_t i = 0; i < m_frame_pointer; i++) {
            m_frames[i].mark();
        }
        m_globals.mark();
        if (m_current_frame) m_current_frame->mark();
        m_ctx->mark();
    }

    void Vm::unmark() {
        Root::unmark();

        for (size_t i = 0; i < m_frame_pointer; i++) { ;
            m_frames[i].unmark();
        }
        m_globals.unmark();
        if (m_current_frame) m_current_frame->unmark();
        m_ctx->unmark();
    }

}; // bond