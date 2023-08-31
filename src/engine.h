//
// Created by travor on 18/07/2023.
//

#pragma once

#include <fstream>
#include <filesystem>
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/context.h"
#include "compiler/codegen.h"
#include "vm.h"
#include "api.h"
#include "gc.h"
#include "core/core.h"
#include <memory>
#include <utility>

namespace bond {
    class Engine: public gc {
    private:
        std::string m_lib_path;
        std::vector<std::string, gc_allocator<std::string>> m_args;
        Context m_context;
        bool m_check{false};


    public:
        Engine(std::string lib_path, std::vector<std::string, gc_allocator<std::string>> args)
            : m_lib_path(std::move(lib_path)), m_args(std::move(args)), m_context(m_lib_path) {
            m_context.set_args(m_args);
        }

        Context* get_context() { return &m_context; }
        void run_repl();
        void execute_source(std::string &source, const char *path, bond::Vm &vm);
        void run_file(const std::string &path);
        static void add_core_module(const GcPtr<Module>& mod);
        void set_checker(bool check) { m_check = check; }
    };

    Engine* create_engine(const std::string& lib_path, const std::vector<std::string, gc_allocator<std::string>>& args);
    Engine* create_engine(const std::string& lib_path);

}


