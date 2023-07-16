//
// Created by travor on 15/07/2023.
//

#pragma once
#include "object.h"

namespace bond {
    // import parser
    // import "core:io" as io; -> core inbuilt module
    // import "library:io" as io; -> libraries
    // import "file/io" as io; -> local files

    extern GcPtr<Module> core_module;

    class Import {
    public:
        Import() = default;

        static Import& instance() {
            static Import instance;
            return instance;
        }

        std::expected<GcPtr<Object>, std::string> import_module(Context* ctx, const std::string& path, std::string& alias);

    private:
        t_map m_modules;
    };
}


