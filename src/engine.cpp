//
// Created by travor on 18/07/2023.
//

#include "engine.h"
#include "import.h"
#include "lsp/resolver.h"
#include <replxx.hxx>
#include "compiler/lexer.h"
#include <cmath>

namespace bond {

    size_t levenshteinDist(const std::string &word1, const std::string &word2) {
        size_t size1 = word1.size();
        size_t size2 = word2.size();
        //int verif[size1 + 1][size2 + 1]; // Verification matrix i.e. 2D array which will store the calculated distance.

        std::vector<std::vector<int>> verif(size1 + 1, std::vector<int>(size2 + 1));

        // If one of the words has zero length, the distance is equal to the size of the other word.
        if (size1 == 0)
            return size2;
        if (size2 == 0)
            return size1;

        // Sets the first row and the first column of the verification matrix with the numerical order from 0 to the length of each word.
        for (int i = 0; i <= size1; i++)
            verif[i][0] = i;
        for (int j = 0; j <= size2; j++)
            verif[0][j] = j;

        // Verification step / matrix filling.
        for (int i = 1; i <= size1; i++) {
            for (int j = 1; j <= size2; j++) {
                // Sets the modification cost.
                // 0 means no modification (i.e. equal letters) and 1 means that a modification is needed (i.e. unequal letters).
                int cost = (word2[j - 1] == word1[i - 1]) ? 0 : 1;

                // Sets the current position of the matrix as the minimum value between a (deletion), b (insertion) and c (substitution).
                // a = the upper adjacent value plus 1: verif[i - 1][j] + 1
                // b = the left adjacent value plus 1: verif[i][j - 1] + 1
                // c = the upper left adjacent value plus the modification cost: verif[i - 1][j - 1] + cost
                verif[i][j] = std::min(
                        std::min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                        verif[i - 1][j - 1] + cost
                );
            }
        }

        // The last position of the matrix will contain the Levenshtein distance.
        return verif[size1][size2];
    }

    void bond::Engine::run_repl() {
        fmt::print("bond {} on {}\n", BOND_VERSION, COMPILER_VERSION);
        auto id = m_context.new_module(std::string("<repl>"));

        auto globals = Runtime::ins()->MAP_STRUCT->create_instance<StringMap>();
        Scopes scopes(&m_context);

        auto vm = bond::Vm(&m_context, globals);
        bond::set_current_vm(&vm);

        replxx::Replxx rx;
        rx.install_window_change_handler();

        std::string history_file{"./history.txt"};
        rx.history_load(history_file);

        // set the max history size
        rx.set_max_history_size(128);
        rx.set_max_hint_rows(3);
        rx.set_word_break_characters(".");

        using Color = replxx::Replxx::Color;
        auto comp_green = replxx::Replxx::Color::GREEN;

        auto keywords = get_keywords();


        rx.set_highlighter_callback(
                [&](std::string const &input, replxx::Replxx::colors_t &colors) {
            Lexer lexer(input, &m_context, id);
            lexer.disable_reporting();

            auto tokens = lexer.tokenize();

            if (m_context.has_error()) m_context.reset_error();
            if (tokens.empty()) return;

            colors.assign(input.size() + 1, Color::WHITE);

            auto assign_range = [&](const auto &range, Color color) {
                for (auto i = range->start; i < range->end; i++) {
                    colors[i] = color;
                }
            };

            for (auto &token: tokens) {
                if (keywords.contains(token.get_lexeme())) {
                    assign_range(token.get_span(), Color::BRIGHTMAGENTA);
                } else if (token.get_type() == TokenType::INTEGER or token.get_type() == TokenType::FLOAT) {
                    assign_range(token.get_span(), Color::BRIGHTBLUE);
                } else if (token.get_type() == TokenType::STRING) {
                    assign_range(token.get_span(), Color::BRIGHTCYAN);
                    colors[token.get_span()->start] = Color::YELLOW;
                    colors[token.get_span()->end - 1] = Color::YELLOW;
                } else if (token.get_type() == TokenType::IDENTIFIER) {
                    assign_range(token.get_span(), Color::WHITE);
                } else if (token.get_type() == TokenType::SEMICOLON) {
                    assign_range(token.get_span(), Color::BRIGHTRED);
                } else {
                    assign_range(token.get_span(), Color::DEFAULT);
                }
            }

        });


        rx.set_completion_callback([&](std::string const &input, int &contextLen) -> replxx::Replxx::completions_t {
            if (input.starts_with(".")) return {};

            std::vector<replxx::Replxx::Completion> hints;

            for (auto &[name, _]: globals->get_value()) {
                if (levenshteinDist(name.c_str(), input) < 5)
                    hints.emplace_back(name.c_str());
            }

            for (auto &[name, _]: keywords) {
                if (levenshteinDist(name, input) < 5)
                    hints.emplace_back(name, comp_green);
            }

            return hints;
        });


        rx.set_hint_callback([&](std::string const &input, int &contextLen, Color &color) -> replxx::Replxx::hints_t {
            if (input.starts_with(".")) return {};
            std::vector<std::string> hints;

            for (auto &[name, _]: globals->get_value()) {
                if (levenshteinDist(name.c_str(), input) < 5)
                    hints.emplace_back(name.c_str());
            }

            for (auto &[name, _]: keywords) {
                if (levenshteinDist(name, input) < 5)
                    hints.emplace_back(name);
            }

            return hints;
//            if (input.starts_with(".")) return {};
//
//            std::vector<std::string> hints{""};
//
//            Lexer lexer(input, &m_context, id);
//            lexer.disable_reporting();
//
//            Parser parser(lexer.tokenize(), &m_context);
//
//            for (auto &[rep, _]: lexer.get_error_spans()) {
//                hints.emplace_back(rep);
//            }
//
//            if (m_context.has_error()) {
//                m_context.reset_error();
//                return hints;
//            }
//
//            parser.set_scopes(&scopes);
//            parser.disable_reporting();
//            parser.parse();
//
//            for (auto &[rep, _]: parser.get_diagnostics()) {
//                hints.emplace_back(rep);
//            }
//
//            m_context.reset_error();
//            return hints;
        });

        rx.set_indent_multiline(true);
        rx.set_complete_on_empty(true);
        rx.set_double_tab_completion(false);
        rx.enable_bracketed_paste();

        auto prompt = std::string{"\x1b[1;32mbond > \x1b[0m"};

        using command = std::function<void()>;

        bool run = true;
        bool diss= false;


        std::unordered_map<std::string, command> commands;

        commands[".exit"] = [&]() {
            run = false;
        };

        commands[".diss"] = [&]() {
            diss = !diss;
            fmt::print("disassembly {}\n", diss ? "enabled" : "disabled");
        };

        const char *help = R"(
        commands:
            .exit - exit the repl
            .diss - toggle disassembly
            .help - print this help message

        key binds:
            <CTRL><ENTER> - insert newline
            <CTRL><R>     - reverse search history
        )";

        commands[".help"] = [&]() {
            fmt::print("{}\n", help);
        };

        fmt::print("enter .help for help\n");
        while (run) {
            char const *cinput{nullptr};
            do {
                cinput = rx.input(prompt);
            } while ((cinput == nullptr) && (errno == EAGAIN));

            if (cinput == nullptr) {
                break;
            }

            std::string input{cinput};

            if (input.starts_with(".")) {
                if (commands.contains(input)) {
                    commands[input]();
                    continue;
                    rx.history_add(input);
                }
                fmt::print("unknown command {}\n", input);
                continue;
            }

            // add to history
            rx.history_add(input);

            // save history
            rx.history_save(history_file);


            auto vm = bond::Vm(&m_context, globals);
            bond::set_current_vm(&vm);

            auto path = std::string("<repl>");
            m_context.reset_error();

            auto lexer = bond::Lexer(cinput, &m_context, id);
            if (m_context.has_error()) continue;
            auto parser = bond::Parser(lexer.tokenize(), &m_context);
            parser.set_scopes(&scopes);

            if (m_context.has_error()) {
                m_context.reset_error();
                continue;
            }

            auto codegen = bond::CodeGenerator(&m_context, &scopes);
            codegen.set_mode_repl();

            // FIXME: crashes and failed compilations cause globals
            // and scopes to be out of sync, this is a hack to fix that
            {
                std::vector<std::string> keys;
                for (auto &[k, o]: globals->get_value()) {
                    if (o.get() != nullptr)
                        keys.emplace_back(k);
                }

                scopes.remove_if_not_in_list(keys);
            }

            auto bytecode = codegen.generate_code(parser.parse());

            if (m_context.has_error()) {
                m_context.reset_error();
                continue;
            }

            if (diss)
                fmt::print("{}\n", bytecode->disassemble());

            vm.run(bytecode);
            auto top = vm.get_repl_result();

            if (top.has_value()) {
                assert(top.value().get() != nullptr && "top is null, report this as a bug");
                fmt::print("{}\n", top.value()->str());
            }
        }

    }

    void Engine::execute_source(std::string &source, const char *path, Vm &vm) {
        auto id = m_context.new_module(std::string(path));

        auto lexer = bond::Lexer(source, &m_context, id);
        auto tokens = lexer.tokenize();

        auto parser = bond::Parser(tokens, &m_context);
        auto nodes = parser.parse();

        if (m_context.has_error()) return;

        if (m_check) {
            auto resolver = bond::lsp::Resolver(&m_context, nodes);
            auto res = resolver.resolve();

            if (!res) {
                for (auto &err: res.error()) {
                    m_context.error(err.m_span, err.m_message.c_str());
                }
            }

            m_context.reset_error();
        }

        auto codegen = bond::CodeGenerator(&m_context, parser.get_scopes());

        auto bytecode = codegen.generate_code(nodes);

        if (m_context.has_error()) return;

//        fmt::print("{}\n", bytecode->disassemble());

        auto file = std::ofstream("out.bond");
        file << bytecode->disassemble();
        file.close();

        vm.run(bytecode);

    }

    void Engine::run_file(const std::string &path) {
        auto src = bond::Context::read_file(std::string(path));
        execute_source(src, path.c_str(), *get_current_vm());
    }

    void Engine::add_core_module(const GcPtr<Module> &mod) {
        core_module->add_module(mod->get_path(), mod);
    }

    Engine *
    create_engine(const std::string &lib_path, const std::vector<std::string, gc_allocator<std::string>> &args) {
        GC_INIT();

        GC_set_all_interior_pointers(1);

        GC_set_warn_proc([](char *msg, GC_word arg) {
            printf(msg,  arg);
        });

        Runtime::ins()->init();
        bond::build_core_module();
        bond::lsp::init_symbols();

        return new(GC) Engine(lib_path, args);
    }

    Engine *create_engine(const std::string &lib_path) {
        return create_engine(lib_path, std::vector<std::string, gc_allocator<std::string>>());
    }
}