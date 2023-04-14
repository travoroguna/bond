//
// Created by Travor Oguna Oneya on 12/04/2023.
//

#include <thread>
#include <mutex>
#include <utility>
#include "../bond.h"
#include <chrono>


using namespace bond;
Context *m_ctx;


class Future : public Object {
public:
    Future() = default;

    bool has_result() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_result.get() != nullptr;
    }

    GcPtr<Object> get_result() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_result;
    }

    void set_result(GcPtr<Object> result) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_result = std::move(result);
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


private:
    std::mutex m_mutex;
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

NativeErrorOr start_isolate(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(2, arguments);

    DEFINE(function, Function, 0, arguments);
    DEFINE(args, ListObj, 1, arguments);

    auto ctx = Context(m_ctx->get_lib_path());
    auto vm = Vm(&ctx);
    GarbageCollector::instance().add_root(&vm);
    auto future = GarbageCollector::instance().make<Future>();

    GarbageCollector::instance().stop_gc();
    std::thread t([&vm, &function, &args, &future]() {
        vm.push(future);
        vm.call_function(function, args->get_list());
        vm.exec();

        GarbageCollector::instance().stop_gc();
        if (!vm.had_error()) {
            auto result = vm.pop();
            future->set_result(GarbageCollector::instance().make<BondResult>(false, result));
        } else {
            auto error = vm.pop();
            future->set_result(GarbageCollector::instance().make<BondResult>(true, error));
        }
    });

    t.join();
    return future;
}


NativeErrorOr sleep(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);

    DEFINE(duration, Number, 0, arguments);

    std::this_thread::sleep_for(std::chrono::milliseconds((int) duration->get_value()));
    return GarbageCollector::instance().make<Nil>();
}


EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    m_ctx = ctx;
//    GarbageCollector::instance().set_gc(ctx->gc());

    std::unordered_map<std::string, NativeFunctionPtr> io_module = {
            {"start_isolate", start_isolate},
            {"sleep",         sleep}

    };

    auto module = ctx->gc()->make<Module>(path, io_module);
    ctx->add_module(path, module);
}