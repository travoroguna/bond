#include <filesystem>
#include "../../bond.h"


using namespace bond;
GarbageCollector *m_gc;
Context *m_ctx;

auto path_exists(const std::vector<GcPtr<GcObject>> &arguments) -> NativeErrorOr {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(path, String, 0, arguments);

    return m_gc->make<Bool>(std::filesystem::exists(path->get_value()));
}

auto path_is_file(const std::vector<GcPtr<GcObject>> &arguments) -> NativeErrorOr {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(path, String, 0, arguments);

    return m_gc->make<Bool>(std::filesystem::is_regular_file(path->get_value()));
}

auto path_is_directory(const std::vector<GcPtr<GcObject>> &arguments) -> NativeErrorOr {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(path, String, 0, arguments);

    return m_gc->make<Bool>(std::filesystem::is_directory(path->get_value()));
}

auto path_current_working_directory(const std::vector<GcPtr<GcObject>> &arguments) -> NativeErrorOr {
    ASSERT_ARG_COUNT(0, arguments);
    return m_gc->make<String>(std::filesystem::current_path().string());
}

auto path_absolute(const std::vector<GcPtr<GcObject>> &arguments) -> NativeErrorOr {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(path, String, 0, arguments);

    if (!std::filesystem::exists(path->get_value()))
        return Err(fmt::format("Path '{}' does not exist", path->get_value()));
    return Ok(m_gc->make<String>(std::filesystem::current_path().string()));
}

auto path_os_name(const std::vector<GcPtr<GcObject>> &arguments) -> NativeErrorOr {
    ASSERT_ARG_COUNT(0, arguments);

#ifdef _WIN32
    return m_gc->make<String>("Windows");
#else
    return m_gc->make<String>("Linux");
#endif
}


class Path : public GcObject {
public:
    std::string str() override {
        return "Path";
    }

    OBJ_RESULT $get_attribute(const GcPtr<GcObject> &index) override {
        auto name = index->as<String>()->get_value();
        if (m_map.contains(name)) return m_map[name];
        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    bool equal(const GcPtr<GcObject> &other) override { return false; }

    size_t hash() override { return 0; }

    void mark() override {
        GcObject::mark();
        for (auto &[_, attr]: m_map) {
            attr->mark();
        }
    }

    void unmark() override {
        GcObject::unmark();
        for (auto &[_, attr]: m_map) {
            attr->unmark();
        }
    }

private:
    std::unordered_map<std::string, GcPtr<GcObject>> m_map = {
            {"exists",                    m_gc->make<NativeFunction>(path_exists, "exists")},
            {"is_file",                   m_gc->make<NativeFunction>(path_is_file, "is_file")},
            {"is_directory",              m_gc->make<NativeFunction>(path_is_directory, "is_directory")},
            {"current_working_directory", m_gc->make<NativeFunction>(path_current_working_directory,
                                                                     "current_working_directory")},
            {"absolute",                  m_gc->make<NativeFunction>(path_absolute, "absolute")},
            {"os_name",                   m_gc->make<NativeFunction>(path_os_name, "os_name")},
    };
};


EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    m_gc = ctx->gc();
    m_ctx = ctx;
    GarbageCollector::instance().set_gc(ctx->gc());

    std::vector<GcPtr<GcObject>> args;
    for (auto &arg: ctx->get_args()) {
        args.emplace_back(ctx->gc()->make<String>(arg));
    }

    std::unordered_map<std::string, GcPtr<GcObject>> io_module = {
            {"Path", m_gc->make<Path>()},
            {"args", m_gc->make<ListObj>(args)},
    };

    auto module = ctx->gc()->make<Module>(path, io_module);
    ctx->add_module(path, module);
}