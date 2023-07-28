//
// Created by travor on 15/07/2023.
//

#pragma once

#include "object.h"

namespace bond {
    extern GcPtr<Module> core_module;

    class Import {
    public:
        Import() = default;

        static Import &instance() {
            static Import instance;
            return instance;
        }

        std::expected<GcPtr<Object>, std::string>
        import_module(Context *ctx, const std::string &path, std::string &alias);

        std::expected<GcPtr<Code>, std::string> import_archive(Context *ctx, const std::string &path);

        std::expected<GcPtr<Module>, std::string> get_pre_compiled(uint32_t id);


    private:
        t_map m_modules;
        std::unordered_map<uint32_t, GcPtr<Module>> m_compiled_modules;
        std::unordered_map<uint32_t, GcPtr<Code>> m_compiled_archive;
        Context *m_context{nullptr};

    };

    std::expected<std::string, std::string> path_resolver(Context *ctx, const std::string &path);

    obj_result resolve_core(const std::string &path);
}


