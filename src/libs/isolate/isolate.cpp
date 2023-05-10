//
// Created by Travor Oguna Oneya on 12/04/2023.
//

#include <utility>
#include "../../bond.h"
#include <cassert>
#include <chrono>

#include <plibsys.h>


PMutex *iso_mutex;

using namespace bond;
Context *m_ctx;

struct ThreadData {
    Context *ctx;
    GcPtr<Function> function;
    GcPtr<ListObj> args;
    GcPtr<Future> future;
    Vm *m_vm;

    ThreadData(Context *ctx, const GcPtr<Function> &function, const GcPtr<ListObj> &args, const GcPtr<Future> &future,
               Vm *vm)
            : ctx(ctx), function(function), args(args), future(future), m_vm(vm) {}
};


class Mutex : public Object {
public:
    Mutex() {
        m_mutex = p_mutex_new();
    }

    ~Mutex() override {
//        p_mutex_free(m_mutex);
    }

    NativeErrorOr lock(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        p_mutex_lock(m_mutex);
        return Globs::BondNil;
    }

    NativeErrorOr unlock(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        p_mutex_unlock(m_mutex);
        return Globs::BondNil;
    }

    bool equal(const GcPtr<bond::Object> &other) override {
        if (!other->is<Mutex>()) return false;
        auto o = other->as<Mutex>();
        return o->m_mutex == m_mutex;
    }

    size_t hash() override {
        return (size_t) m_mutex;
    }

    std::string str() override {
        return fmt::format("<Mutex instance at {}>", (void *) m_mutex);
    }

    std::expected<GcPtr<Object>, RuntimeError> $get_attribute(const GcPtr<bond::Object> &index) override {
        auto idx = index->as<String>()->get_value();
        if (idx == "lock") {
            return MAKE_METHOD(lock);
        } else if (idx == "unlock") {
            return MAKE_METHOD(unlock);
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

private:
    PMutex *m_mutex;
};


NativeErrorOr c_Mutex(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    return m_ctx->gc()->make<Mutex>();
}

ppointer isolate_thread(ppointer arg) {
    m_ctx->gc()->make_thread_storage();
    m_ctx->gc()->stop_gc();

    auto data = (ThreadData *) arg;
    auto ctx = Context(m_ctx->get_lib_path());


    auto vm = *data->m_vm;
    m_ctx->gc()->add_root(&vm);

    auto function = data->function;
    auto args = data->args->get_list();
    auto future = data->future;
    m_ctx->gc()->resume_gc();
    vm.call_function(function, args);

    vm.exec();

    m_ctx->gc()->stop_gc();
    if (!vm.had_error()) {
        auto result = vm.pop();
        future->set_value(m_ctx->gc()->make<BondResult>(false, result));
    } else {
        auto error = vm.pop();
        future->set_value(m_ctx->gc()->make<BondResult>(true, error));
    }

    assert(future->has_value());

    m_ctx->gc()->resume_gc();

    m_ctx->gc()->remove_thread_storage(std::this_thread::get_id());

    delete data;
    return nullptr;
}

NativeErrorOr start_isolate(const std::vector<GcPtr<Object>> &arguments) {
    p_mutex_lock(iso_mutex);

    ASSERT_ARG_COUNT(2, arguments);

    DEFINE(function, Function, 0, arguments);
    DEFINE(args, ListObj, 1, arguments);

    auto vm = new Vm(m_ctx);
    vm->push(function);
    vm->push(args);


    auto future = m_ctx->gc()->make<Future>();
    vm->push(future);
    auto t = new ThreadData(m_ctx, function, args, future, vm);
    auto thread = p_uthread_create(isolate_thread, t, false);

    fmt::print("Starting isolate, {}, {}\n", function->str(), args->str());

    p_mutex_unlock(iso_mutex);

    return future;
}


NativeErrorOr sleep_ms(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);

    DEFINE(duration, Integer, 0, arguments);

    std::this_thread::sleep_for(std::chrono::milliseconds(duration->get_value()));

    return Globs::BondNil;
}

NativeErrorOr sleep_(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(duration, Integer, 0, arguments);

    std::this_thread::sleep_for(std::chrono::seconds(duration->get_value()));

    return Globs::BondNil;
}

NativeErrorOr wait_until_completed(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(c_list, ListObj, 0, arguments);


    std::vector<GcPtr<Object>> results;
    auto args = c_list->get_list();

    auto root = bond::Root();
    m_ctx->gc()->add_root(&root);

    for (auto &arg: args) {
        if (!arg->is<Future>()) return std::unexpected(FunctionError("Expected future", RuntimeError::GenericError));
    }


    while (!args.empty()) {
        for (auto &arg: args) {
            auto future = arg->as<Future>();
            if (future->get_value().get() != nullptr) {
                root.push(future->get_value());
                results.push_back(future->get_value());
            }
        }


        auto it = std::remove_if(args.begin(), args.end(),
                                 [](const GcPtr<Object> &arg) {
                                     return arg->as<Future>()->has_value();
                                 }
        );


        args.erase(it, args.end());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    m_ctx->gc()->remove_root(&root);
    return m_ctx->gc()->make<ListObj>(results);
}


EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    m_ctx = ctx;
    GarbageCollector::instance().set_gc(ctx->gc());
    auto gc = ctx->gc();
    iso_mutex = p_mutex_new();


    std::unordered_map<std::string, GcPtr<Object>> io_module = {
            {"start_isolate",        gc->make<NativeFunction>(start_isolate)},
            {"sleep",                gc->make<NativeFunction>(sleep_)},
            {"sleep_ms",             gc->make<NativeFunction>(sleep_ms)},
            {"wait_until_completed", gc->make<NativeFunction>(wait_until_completed)},
            {"Mutex",                gc->make<NativeFunction>(c_Mutex)}
    };

    auto module = ctx->gc()->make<Module>(path, io_module);
    ctx->add_module(path, module);
}