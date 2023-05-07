//
// Created by Travor Oguna Oneya on 12/04/2023.
//

#include "../../bond.h"

using namespace bond;

NativeErrorOr stop_gc(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    GarbageCollector::instance().stop_gc();
    return GarbageCollector::instance().make<Nil>();
}

NativeErrorOr resume_gc(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    GarbageCollector::instance().resume_gc();
    return GarbageCollector::instance().make<Nil>();
}

NativeErrorOr collect(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
//    GarbageCollector::instance().collect();
    return GarbageCollector::instance().make<Nil>();
}

NativeErrorOr get_immortal_count(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    auto s = GarbageCollector::instance().get_immortal_count();
    return GarbageCollector::instance().make<Integer>(s);
}

NativeErrorOr get_alloc_count(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    auto s = GarbageCollector::instance().get_alloc_count();
    return GarbageCollector::instance().make<Integer>(s);
}


NativeErrorOr set_alloc_limit(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(limit, Integer, 0, arguments);
    GarbageCollector::instance().set_alloc_limit(limit->get_value());
    return GarbageCollector::instance().make<Nil>();
}

NativeErrorOr get_alloc_limit(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    auto s = GarbageCollector::instance().get_alloc_limit();
    return GarbageCollector::instance().make<Integer>(s);
}

EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    GarbageCollector::instance().set_gc(ctx->gc());
    std::unordered_map<std::string, NativeFunctionPtr> io_module = {
            {"set_allocation_limit", set_alloc_limit},
            {"get_allocation_count", get_alloc_count},
            {"get_allocation_limit", get_alloc_limit},
            {"get_immortal_count",   get_immortal_count},
            {"collect",              collect},
            {"stop_gc",              stop_gc},
            {"resume_gc",            resume_gc},
    };
    auto module = ctx->gc()->make<Module>(path, io_module);
    ctx->add_module(path, module);
}

