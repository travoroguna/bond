//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#include <sstream>
#include <algorithm>
#include "context.h"


namespace bond {
    std::string Context::read_file(const std::string& path) {
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    uint32_t Context::new_module(std::string &path) {
        auto val = std::find_if(std::begin(m_modules), std::end(m_modules),
                                [&path](auto &&p) { return p.second == path; });

        if (val == std::end(m_modules)) {
            auto l = m_modules.size();
            m_modules[l] = path;
            return l;
        }

        return val->first;
    }
};