//
// Created by Travor Oguna Oneya on 09/04/2023.
//

#include "../../bond.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace bond;
GarbageCollector *m_gc;


NativeErrorOr print_ln(const std::vector<GcPtr<Object>> &arguments) {
    for (auto const &arg: arguments) {
        fmt::print("{} ", arg->str());
    }
    fmt::print("\n");
    return m_gc->make<Nil>();
}

NativeErrorOr print(const std::vector<GcPtr<Object>> &arguments) {
    for (auto const &arg: arguments) {
        fmt::print("{}", arg->str());
    }
    return m_gc->make<Nil>();
}


NativeErrorOr read_stdin(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    std::string line;
    std::getline(std::cin, line);
    return m_gc->make<String>(line);
}


class File : public Object {
public:
    File(std::string const &path, std::string mode) : m_path(path), m_mode(std::move(mode)) {
        m_file.open(path, std::ios::in | std::ios::out);
    }

    std::string read_all() {
        std::stringstream buffer;
        buffer << m_file.rdbuf();
        return buffer.str();
    }

    void close() {
        m_file.close();
    }

    std::string str() override {
        return fmt::format("File({})", m_path);
    }

    OBJ_RESULT $get_attribute(const GcPtr<Object> &index) override {
        if (!index->is<String>()) return std::unexpected(RuntimeError::AttributeNotFound);

        auto name = index->as<String>()->get_value();

        if (m_methods.find(name) != m_methods.end()) {
            return m_methods[name];
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    void mark() override {
        Object::mark();
        for (auto const &[name, method]: m_methods) {
            method->mark();
        }
    }

    void unmark() override {
        Object::unmark();
        for (auto const &[name, method]: m_methods) {
            method->unmark();
        }
    }

    bool equal(const GcPtr<Object> &other) override { return false; }

    size_t hash() override { return 0; }

private:
    std::string m_path;
    std::string m_mode;
    std::fstream m_file;

    std::unordered_map<std::string, GcPtr<Object>> m_methods = {
            {"read_all",  m_gc->make<NativeFunction>(
                    [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                        ASSERT_ARG_COUNT(0, arguments);
                        return m_gc->make<String>(this->read_all());
                    })},
            {"close",     m_gc->make<NativeFunction>(
                    [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                        ASSERT_ARG_COUNT(0, arguments);
                        this->close();
                        return m_gc->make<Nil>();
                    })},
            {"write_all", m_gc->make<NativeFunction>(
                    [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                        ASSERT_ARG_COUNT(1, arguments);
                        DEFINE(content, String, 0, arguments);
                        m_file << content->get_value();
                        return m_gc->make<Nil>();
                    })},
            {"file_size", m_gc->make<NativeFunction>(
                    [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                        ASSERT_ARG_COUNT(0, arguments);
                        return m_gc->make<Integer>(std::filesystem::file_size(m_path));
                    })},
    };

};


NativeErrorOr open_file(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(2, arguments);
    DEFINE(path, String, 0, arguments);
    DEFINE(mode, String, 1, arguments);

    if (!std::filesystem::exists(path->get_value()))
        return Err(fmt::format("File {} does not exist", path->get_value()));
    return Ok(m_gc->make<File>(path->get_value(), mode->get_value()));
}


EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    GarbageCollector::instance().set_gc(ctx->gc());
    m_gc = ctx->gc();
    std::unordered_map<std::string, NativeFunctionPtr> io_module = {
            {"println", print_ln},
            {"print",   print},
            {"read",    read_stdin},
            {"open",    open_file},
    };

    fmt::print("[IO] module loaded, {} == {}\n", (void *) m_gc, (void *) &GarbageCollector::instance());
    auto module = ctx->gc()->make<Module>(path, io_module);
    ctx->add_module(path, module);
}
