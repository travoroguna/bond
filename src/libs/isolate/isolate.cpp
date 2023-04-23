//
// Created by Travor Oguna Oneya on 12/04/2023.
//

#include <utility>
#include "../../bond.h"

#include <chrono>

#include <plibsys.h>


using namespace bond;
Context *m_ctx;


class Future : public Object {
public:
    Future() : m_mutex(p_mutex_new()) {}

    ~Future() override {
        if (m_thread != nullptr) {
//            p_uthread_unref(m_thread);
        }
//        p_mutex_free(m_mutex);
    }


    bool has_result() {
        p_mutex_lock(m_mutex);
        auto res = m_result.get() != nullptr;
        p_mutex_unlock(m_mutex);

        return res;
    }

    GcPtr<Object> get_result() {
        return m_result;
    }

    void set_result(GcPtr<Object> result) {
        p_mutex_lock(m_mutex);
        m_result = std::move(result);
        p_mutex_unlock(m_mutex);
    }

    void mark() override {
        if (m_result.get() != nullptr) m_result->mark();
    }

    void unmark() override {
        if (m_result.get() != nullptr) m_result->unmark();
    }


    bool equal(const GcPtr<Object> &other) override { return false; }

    size_t hash() override { return 0; }

    std::string str() override { return "Future"; }

    OBJ_RESULT $get_attribute(const GcPtr<Object> &index) override {
        if (!index->is<String>()) return std::unexpected(RuntimeError::AttributeNotFound);
        auto name = index->as<String>()->get_value();
        if (!m_attributes.contains(name)) return std::unexpected(RuntimeError::AttributeNotFound);
        return m_attributes[name];
    }

    void set_ptr(PUThread *thread) {
        m_thread = thread;
        p_uthread_ref(m_thread);
    }

private:
    PMutex *m_mutex;
    PUThread *m_thread = nullptr;
    GcPtr<Object> m_result = nullptr;

    std::unordered_map<std::string, GcPtr<Object>> m_attributes = {
            {"has_result", m_ctx->gc()->make<NativeFunction>(
                    [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                        ASSERT_ARG_COUNT(0, arguments);
                        return GarbageCollector::instance().make<Bool>(has_result());
                    }
            )},
            {"get_result", m_ctx->gc()->make<NativeFunction>(
                    [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                        ASSERT_ARG_COUNT(0, arguments);
                        if (has_result()) {
                            return get_result();
                        } else {
                            return Err("Future has no results");
                        }
                    }
            )},
    };
};

struct ThreadData {
    Context *ctx;
    GcPtr<Function> function;
    GcPtr<ListObj> args;
    GcPtr<Future> future;

    ThreadData(Context *ctx, const GcPtr<Function> &function, const GcPtr<ListObj> &args, const GcPtr<Future> &future)
            : ctx(ctx), function(function), args(args), future(future) {}
};


ppointer isolate_thread(ppointer arg) {
    auto data = (ThreadData *) arg;
    auto ctx = Context(m_ctx->get_lib_path());


    auto vm = Vm(&ctx);
    m_ctx->gc()->add_root(&vm);
    GarbageCollector::instance().add_root(&vm);
    GarbageCollector::instance().stop_gc();

    auto function = data->function;
    auto args = data->args->get_list();
    auto future = data->future;

    vm.push(future);
    vm.push(function);
    for (auto &a: args) {
        vm.push(a);
    }
    vm.call_function(function, args);
    vm.exec();

    GarbageCollector::instance().resume_gc();
    if (!vm.had_error()) {
        auto result = vm.pop();
        future->set_result(GarbageCollector::instance().make<BondResult>(false, result));
    } else {
        auto error = vm.pop();
        future->set_result(GarbageCollector::instance().make<BondResult>(true, error));
    }

    GarbageCollector::instance().remove_root(&vm);
    m_ctx->gc()->remove_root(&vm);
    delete data;
    return nullptr;
}

NativeErrorOr start_isolate(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(2, arguments);

    DEFINE(function, Function, 0, arguments);
    DEFINE(args, ListObj, 1, arguments);

    GarbageCollector::instance().stop_gc();


    auto future = m_ctx->gc()->make<Future>();
    auto t = new ThreadData(m_ctx, function, args, future);
    auto thread = p_uthread_create(isolate_thread, t, true);
    future->set_ptr(thread);

//    p_uthread_join(thread);
    fmt::print("Starting isolate, {}, {}\n", function->str(), args->str());

    return future;
}


NativeErrorOr sleep(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);

    DEFINE(duration, Integer, 0, arguments);

    std::this_thread::sleep_for(std::chrono::milliseconds(duration->get_value()));

    return GarbageCollector::instance().make<Nil>();
}


EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    m_ctx = ctx;
//    GarbageCollector::instance().set_gc(m_ctx->gc());

    fmt::print("ISOLATE {} ?????????????????????????\n", m_ctx->gc()->get_roots().size());

    for (auto root: m_ctx->gc()->get_roots()) {
        GarbageCollector::instance().add_root(root);
    }

    std::unordered_map<std::string, NativeFunctionPtr> io_module = {
            {"start_isolate", start_isolate},
            {"sleep",         sleep}

    };

    auto module = ctx->gc()->make<Module>(path, io_module);
    ctx->add_module(path, module);
}