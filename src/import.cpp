//
// Created by travor on 15/07/2023.
//

#include <expected>
#include <string>
#include <fmt/core.h>
#include "import.h"
#include "compiler/context.h"
#include "object.h"
#include "compiler/parser.h"
#include "vm.h"

#ifdef _WIN32


#else
#include <dlfcn.h>
#endif


#include "compiler/bfmt.h"
#include "api.h"
#include "conv.hpp"

namespace bond {
    std::expected<t_string, t_string> path_resolver(Context *ctx, const t_string &path) {
#ifdef _WIN32
        auto test_compiled_native = fmt::format("{}{}.dll", ctx->get_lib_path(), path);
        auto test_compiled_c_path = path + ".dll";
#else
        auto lib_p = t_string("lib") + path + ".so";
        auto test_compiled_native = t_string(ctx->get_lib_path()) + lib_p;
        const auto& test_compiled_c_path = lib_p;
#endif

        auto test_native = fmt::format("{}{}.bd", ctx->get_lib_path(), path);
        auto test_user = path + ".bd";


        std::array<t_string, 4> paths = {test_compiled_native, test_compiled_c_path, test_native, test_user};

        for (auto &p: paths) {
            if (std::filesystem::exists(p.c_str())) {
                return std::filesystem::canonical(p.c_str()).string();
            }
        }

        if (std::filesystem::exists(path.c_str())) {
            auto ext = std::filesystem::path(path.c_str()).extension();
            if (ext == ".bd" || ext == ".dll" || ext == ".so") {
                return std::filesystem::canonical(path.c_str()).string();
            }
        }

        return std::unexpected(fmt::format("failed to resolve path {}", path));
    }

    std::expected<GcPtr<Module>, t_string>
    load_dynamic_lib(Context *ctx, const t_string &path, const t_string &alias) {
        auto mod = bond::Mod(path);

#ifdef _WIN32
        auto handle = LoadLibrary(path.c_str());
        if (!handle) {
            return std::unexpected(fmt::format("failed to load dynamic library {}: {}", path, GetLastError()));
        }

        // void bond_module_init(bond::Context *ctx, const char *path)
        auto init = (void (*)(bond::Context *, bond::Vm*, bond::Mod* mod)) GetProcAddress(handle, "bond_module_init");

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


        auto init = (void (*)(bond::Context *, bond::Vm*, bond::Mod*))  dlsym(handle, "bond_module_init");
        if (!init) {
            return std::unexpected(fmt::format("failed to load dynamic library {}: {}, missing init function 'bond_module_init'", path, dlerror()));
        }

#endif
        init(ctx, get_current_vm(), &mod);
        return mod.build();
    }

    std::expected<GcPtr<Module>, t_string>
    create_module(Context *m_ctx, const t_string &path, const t_string &alias);

    std::expected<GcPtr<Module>, t_string>
    create_package(Context *m_ctx, const t_string &path, const t_string &alias) {
        //do we load all the files in the directory ending in bd
        //what about sub-directories

        auto mod_map = Runtime::ins()->MAP_STRUCT->create_instance<StringMap>();

        for (const auto &entry: std::filesystem::directory_iterator(path.c_str())) {
            auto ext = entry.path().extension();
            if (ext == ".bd" || ext == ".dll" || ext == ".so") {
                auto a = entry.path().stem().string();
                auto res = create_module(m_ctx, std::filesystem::absolute(entry).string(), a.c_str());
                if (!res) {
                    return std::unexpected(res.error());
                }
                mod_map->set(a, res.value());
            } else if (std::filesystem::is_directory(entry.path())) {
                auto a = split_at_last_occur(split_at_last_occur(entry.path().stem().string(), ':'), '/');

                auto res = create_package(m_ctx, entry.path().string(), a);
                if (!res) {
                    return std::unexpected(res.error());
                }
                mod_map->set(a, res.value());
            }
        }

        return Runtime::ins()->MODULE_STRUCT->create_instance<Module>(alias, mod_map);
    }

    std::expected<GcPtr<Module>, t_string>
    create_module(Context *m_ctx, const t_string &path, const t_string &alias) {
        if (std::filesystem::is_directory(path.c_str())) {
            return create_package(m_ctx, path, alias);
        }

        auto res = path_resolver(m_ctx, path);

        if (!res) {
            return std::unexpected(res.error());
        }

        auto resolved_path = res.value();

        if (m_ctx->has_module(resolved_path.c_str())) {
            return m_ctx->get_module(resolved_path.c_str())->as<Module>();
        }

        if (resolved_path.ends_with(".dll") || resolved_path.ends_with(".so")) {
            return load_dynamic_lib(m_ctx, resolved_path.c_str(), alias.c_str());
        }


        auto pre_cwd = std::filesystem::current_path();
        std::filesystem::current_path(std::filesystem::path(resolved_path.c_str()).parent_path());

        auto id = m_ctx->new_module(resolved_path.c_str());
        auto source = bond::Context::read_file(resolved_path.c_str());

        auto lexer = Lexer(source, m_ctx, id);
        auto parser = Parser(lexer.tokenize(), m_ctx);

        auto nodes = parser.parse();
        auto code_gen = CodeGenerator(m_ctx, parser.get_scopes());

        auto code = code_gen.generate_code(nodes);

        if (m_ctx->has_error()) {
            return std::unexpected(fmt::format("unable to import module {}", resolved_path));
        }

        auto pre_vm = get_current_vm();
        auto vm = Vm(m_ctx);
        set_current_vm(&vm);
        vm.run(code);
        set_current_vm(pre_vm);


        if (vm.had_error() or m_ctx->has_error()) {
            return std::unexpected(fmt::format("unable to import module {}", resolved_path));
        }

        std::filesystem::current_path(pre_cwd);

        auto mod = Runtime::ins()->MODULE_STRUCT->create_instance<Module>(resolved_path, vm.get_globals());
        m_ctx->add_module(resolved_path.c_str(), mod);
        return mod;
    }



    // import parser
    // import "core:io" as io; -> core inbuilt module
    // import "library:io" as io; -> libraries
    // import "file/io" as io; -> local files

    std::expected<GcPtr<Object>, t_string>
    Import::import_module(Context *ctx, const t_string &path, t_string &alias) {
        if (m_modules.contains(path)) {
            return m_modules[path];
        }


        if (ctx->has_module(path.c_str())) {
            return ctx->get_module(path.c_str())->as<Module>();
        }


        if (path.starts_with("core:")) {
            auto p = path.substr(5);
            if (p.empty()) {
                return std::unexpected("invalid core import expected name after core:, e.g. core:io");
            }

            auto res = core_module->get_attribute(p);
            if (!res) {
                return std::unexpected(fmt::format("failed to import core module {}", p));
            }

            return res.value();
        }

        if (path.starts_with("core")) return core_module;


        // TODO: add support for library imports
        auto mod = create_module(ctx, path, alias);
        if (!mod) {
            return mod;
        }
        m_modules[path] = mod.value();
        return mod;
    }


    std::expected<GcPtr<Code>, t_string> Import::import_archive(Context *ctx, const t_string &path) {
        if (!std::filesystem::exists(path.c_str())) {
            return std::unexpected(fmt::format("archive {} does not exist", path));
        }

        auto res = read_archive_file(path);
        TRY(res);

        m_context = ctx;
        m_compiled_archive = res.value();
        return res.value()[0];
    }

    std::expected<GcPtr<Module>, t_string> Import::get_pre_compiled(uint32_t id) {
        if (m_compiled_modules.contains(id)) return m_compiled_modules[id];

        auto code = m_compiled_archive[id];
        auto pre_vm = get_current_vm();
        auto vm = Vm(m_context);
        set_current_vm(&vm);
        vm.run(code);
        set_current_vm(pre_vm);

        if (vm.had_error() or m_context->has_error()) {
            return std::unexpected(fmt::format("failed to import module {} from archive", id));
        }

        auto module = Runtime::ins()->MODULE_STRUCT->create_instance<Module>(std::to_string(id), vm.get_globals());
        m_compiled_modules[id] = module;
        return module;
    }

    obj_result resolve_core(const t_string &path) {
        return core_module->get_attribute(path);
    }

}