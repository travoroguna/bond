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
        uint32_t new_module(std::string const& path);

        void error(const std::shared_ptr<Span>& span, const std::basic_string<char>& err) {
            m_has_error = true;
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

        static std::string read_file(std::string const& path);
        bool has_error() const { return m_has_error; }
        bool has_module(uint32_t id) const { return m_modules.find(id) != m_modules.end(); }
        std::string get_module_name(uint32_t id) const { return m_modules.at(id); }

    private:
        std::unordered_map<uint32_t, std::string> m_modules;
        bool m_has_error = false;
    };
}
