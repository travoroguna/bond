#include <iostream>
#include <fstream>
#include <filesystem>
#include "lsp/lsp.h"

#include "lexer.h"
#include "parser.h"
#include "context.h"
#include "code.h"
#include "vm.h"
#include "gc.h"

void execute_source(std::string &source, const char *path, bond::Vm &vm, bond::Context &ctx) {
    auto id = ctx.new_module(std::string(path));

    auto lexer = bond::Lexer(source, &ctx, id);
    auto tokens = lexer.tokenize();

    auto parser = bond::Parser(tokens, &ctx);
    auto nodes = parser.parse();

    if (ctx.has_error()) return;

    auto codegen = bond::CodeGenerator(&ctx, parser.get_scopes());
    auto bytecode = codegen.generate_code(nodes);

    if (ctx.has_error()) return;

    auto file = std::ofstream("out.bond");
    file << bytecode->dissasemble();
    file.close();

    fmt::print("[VM] start\n");
    vm.run(bytecode);
    fmt::print("[VM] end\n");
}

void run_repl(const std::string &lib_path) {
    bond::Context ctx(lib_path);

    auto vm = bond::Vm(&ctx);
    bond::GarbageCollector::instance().add_root(&vm);

    while (true) {
        auto path = std::string("<repl>");
        ctx.reset_error();
        std::string source;

        fmt::print("bond > ");

        while (char c = getchar()) {
            if (c == '\n') break;
            source += c;
        }

        if (source == "exit") break;

        execute_source(source, path.c_str(), vm, ctx);
    }
}

void run_file(const std::string &path, const std::string &lib_path) {
    auto src = bond::Context::read_file(std::string(path));
    bond::Context ctx(lib_path);

    auto vm = bond::Vm(&ctx);
    bond::GarbageCollector::instance().add_root(&vm);

    execute_source(src, path.c_str(), vm, ctx);
}

#ifdef _WIN32

int main(int32_t argc, char **argv) {
#else
    int main(int argc, const char *argv[]) {
#endif
    auto lib_path = std::filesystem::path(argv[0]).parent_path().string() + "/../libraries/";
    fmt::print("lib path: {} [{}]\n", lib_path, std::filesystem::exists(lib_path));

    if (argc > 200) {
        std::cout << "Usage: bond <script path>\n";
        return 1;
    } else if (argc == 1) {
        fmt::print("bond 0.0.0 \n");
        run_repl(lib_path);
    } else {
        if (std::string(argv[1]) == "--rpc") {
            bond::Rpc::log_message("Starting RPC server...\n");
            auto client = bond::LspClient();
            client.start();
            return 0;
        }

        if (!std::filesystem::exists(argv[1])) {
            fmt::print("File not found: {}\n", argv[1]);
            return 1;
        }
        auto full_path = std::filesystem::absolute(argv[1]);
        auto path = std::filesystem::path(argv[1]);
        std::filesystem::current_path(path.parent_path());

        run_file(full_path.string(), lib_path);
    }
}

#ifdef _WIN32

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
    fmt::print("bond 0.0.0 \n");
    return main(__argc, __argv);
}

#endif
