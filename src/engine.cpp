//
// Created by travor on 18/07/2023.
//

#include "engine.h"
#include "import.h"


namespace bond {
    void bond::Engine::run_repl() {
        auto vm = bond::Vm(&m_context);
        auto id = m_context.new_module(std::string("<repl>"));
        bond::Scopes scope = bond::Scopes(&m_context);


        while (true) {
            auto path = std::string("<repl>");
            m_context.reset_error();
            std::string source;

            fmt::print("bond > ");

            while (char c = getchar()) {
                if (c == '\n') break;
                source += c;
            }

            if (source == "exit") break;

            auto lexer = bond::Lexer(source, &m_context, id);
            if (m_context.has_error()) continue;
            auto parser = bond::Parser(lexer.tokenize(), &m_context);
            parser.set_scopes(&scope);
            if (m_context.has_error()) continue;

            auto codegen = bond::CodeGenerator(&m_context, &scope);
            auto bytecode = codegen.generate_code(parser.parse());
            if (m_context.has_error()) continue;

            vm.run(bytecode);

        }
    }

    void Engine::execute_source(std::string &source, const char *path, Vm &vm) {
        auto id = m_context.new_module(std::string(path));

        auto lexer = bond::Lexer(source, &m_context, id);
        auto tokens = lexer.tokenize();

        auto parser = bond::Parser(tokens, &m_context);
        auto nodes = parser.parse();

        if (m_context.has_error()) return;

        auto codegen = bond::CodeGenerator(&m_context, parser.get_scopes());
        auto bytecode = codegen.generate_code(nodes);

        if (m_context.has_error()) return;

        auto file = std::ofstream("out.bond");
        file << bytecode->disassemble();
        file.close();

        vm.run(bytecode);
    }

    void Engine::run_file(const std::string &path) {
        auto src = bond::Context::read_file(std::string(path));
        auto vm = bond::Vm(&m_context);
        execute_source(src, path.c_str(), vm);
    }

    void Engine::add_core_module(const GcPtr<Module> &mod) {
        core_module->add_module(mod->get_path(), mod);
    }

    std::unique_ptr<Engine> create_engine(const std::string &lib_path, const std::vector<std::string> &args) {
        GC_INIT();

        GC_set_warn_proc([](char *msg, GC_word arg) {
            fmt::print("{} {}\n", msg, arg);
        });

        bond::init_caches();
        bond::build_core_module();

        return std::make_unique<Engine>(lib_path, args);
    }

    std::unique_ptr<Engine> create_engine(const std::string &lib_path) {
        return create_engine(lib_path, std::vector<std::string>());
    }
}