//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include <sstream>
#include <algorithm>
#include "context.h"

namespace bond {
std::string Context::read_file(std::string const &path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

uint32_t Context::new_module(std::string const &path) {
    auto val = std::find_if(std::begin(m_modules), std::end(m_modules),
                            [&path](auto &&p) { return p.second==path; });

    if (val==std::end(m_modules)) {
        auto l = m_modules.size();
        m_modules[l] = path;
        return l;
    }

    return val->first;
}

void Context::error(const std::shared_ptr<Span> &span, const std::basic_string<char> &err) {
    m_has_error = true;

    if (m_modules[span->module_id]=="<repl>") {
        fmt::print("Error: {}\n", err);
        return;
    }

    auto contents = read_file(m_modules[span->module_id]);

    auto line_start = contents.rfind('\n', span->start);
    if (line_start==std::string::npos) line_start = 0;

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

};