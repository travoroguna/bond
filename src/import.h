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

        std::expected<GcPtr<Object>, t_string>
        import_module(Context *ctx, const t_string &path, t_string &alias);

        std::expected<GcPtr<Code>, t_string> import_archive(Context *ctx, const t_string &path);

        std::expected<GcPtr<Module>, t_string> get_pre_compiled(uint32_t id);


    private:
        t_map m_modules;
        std::unordered_map<uint32_t, GcPtr<Module>> m_compiled_modules;
        std::unordered_map<uint32_t, GcPtr<Code>> m_compiled_archive;
        Context *m_context{nullptr};

    };

    std::expected<t_string, t_string> path_resolver(Context *ctx, const t_string &path);

    obj_result resolve_core(const t_string &path);
}


