//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once

#include <unordered_map>
#include <string>
#include <fmt/core.h>
#include <memory>
#include <fstream>
#include <filesystem>
#include "span.h"

namespace bond {
    class Context {
    public:
        uint32_t new_module(std::string const& path);

        void error(const std::shared_ptr<Span>& span, const std::basic_string<char>& err) {
            m_has_error = true;

            if (m_modules[span->module_id] == "<repl>"){
                fmt::print("Error: {}\n", err);
                return;
            }

            auto contents = read_file(m_modules[span->module_id]);

            auto line_start = contents.rfind('\n', span->start);
            if (line_start == std::string::npos) line_start = 0;

            auto line_end = contents.find('\n', span->end);
            if (line_end > contents.length() - 1) line_end = contents.length() - 1;

            auto diff = span->line > 1 ? 1 : 0;

            auto line_source = contents.substr(line_start + diff, line_end - line_start - diff);

            fmt::print("\nError at: {}:{}:{}\n", std::filesystem::absolute(get_module_name(span->module_id)).string(),
                       span->line, span->end - line_start);
            fmt::print("  {}\n", line_source);

            fmt::print("  ");
            for (int i = 0; i < span->start - line_start - diff; i++) {
                fmt::print(" ");
            }

            for (int i = 0; i < span->end - span->start; i++) {
                fmt::print("^");
            }

            fmt::print("\n  {}\n\n", err);

        }

        static std::string read_file(std::string const& path);
        bool has_error() const { return m_has_error; }
        void reset_error() { m_has_error = false; }
        bool has_module(uint32_t id) const { return m_modules.find(id) != m_modules.end(); }
        std::string get_module_name(uint32_t id) const { return m_modules.at(id); }

    private:
        std::unordered_map<uint32_t, std::string> m_modules;
        bool m_has_error = false;
    };
}
