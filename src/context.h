//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once

#include <unordered_map>
#include <string>
#include <fmt/core.h>
#include <memory>
#include <fstream>
#include "span.h"

namespace bond {
    class Context {
    public:
        uint32_t new_module(std::string &path);

        void error(const std::shared_ptr<Span>& span, const std::basic_string<char>& err) {
            auto contents = read_file(m_modules[span->module_id]);

            auto line_start = contents.rfind('\n', span->start);
            auto line_end = contents.rfind('\n', span->end);

            auto line_source = contents.substr(line_start, line_end);

            fmt::print("Error: {}\n", err);
            fmt::print("{}\n", line_source);

            for (int i = 0; i < line_start - span->start; i ++) {
                fmt::print(" ");
            }

            for (int i = 0; i < line_end - line_start; i ++) {
                fmt::print("^");
            }

            fmt::print("\n");
        }

        static std::string read_file(const std::string& path);

    private:
        std::unordered_map<uint32_t, std::string> m_modules;
    };
}
