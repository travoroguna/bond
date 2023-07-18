//
// Created by Travor Oguna Oneya on 15/03/2023.
//

#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <fstream>
#include <filesystem>
#include <utility>
#include "fmt/core.h"
#include "span.h"
#include "../gc.h"

namespace bond {
    class Context {
    public:
        explicit Context(std::string lib_path)  {
            m_lib_path = std::move(lib_path);
        }

        void set_args(const std::vector<std::string> &args) { m_args = args; }

        [[nodiscard]] std::vector<std::string> get_args() const { return m_args; }

        [[nodiscard]] std::string get_lib_path() const { return m_lib_path; }

        [[nodiscard]] uint32_t new_module(std::string const &path);

        void error(const std::shared_ptr<Span> &span, const std::basic_string<char> &err);

        static std::string read_file(std::string const &path);

        [[nodiscard]] bool has_error() const { return m_has_error; }

        void reset_error() { m_has_error = false; }

        [[nodiscard]] bool has_module(uint32_t id) const { return m_modules.find(id) != m_modules.end(); }

        [[nodiscard]] std::string get_module_name(uint32_t id) const { return m_modules.at(id); }

        [[nodiscard]] bool has_module(std::string const &path) const {
            return m_compiled_modules.find(path) != m_compiled_modules.end();
        }

        [[nodiscard]] GcPtr<GcObject> get_module(std::string const &path) const { return m_compiled_modules.at(path); }

        void add_module(std::string const &path, const GcPtr<GcObject> &module) { m_compiled_modules[path] = module; }


    private:
        std::unordered_map<uint32_t, std::string> m_modules;
        std::unordered_map<std::string, GcPtr<GcObject>> m_compiled_modules;
        bool m_has_error = false;
        std::string m_lib_path;
        std::vector<std::string> m_args;
    };
}
