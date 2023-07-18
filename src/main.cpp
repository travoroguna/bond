#include <fstream>
#include <filesystem>
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/context.h"
#include "compiler/codegen.h"
#include "vm.h"
#include "gc.h"
#include "core/core.h"
#include <plibsys.h>


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
    file << bytecode->disassemble();
    file.close();

//    try {
    fmt::print("[VM] start\n");
    vm.run(bytecode);
    fmt::print("[VM] end\n");
//    }
//    catch (std::exception &e) {
//        fmt::print("Error in application\n");
//        vm.runtime_error(e.what(), bond::RuntimeError::GenericError);
//    }
}

void run_repl(const std::string &lib_path, const std::vector<std::string> &args) {
    bond::Context ctx(lib_path);
    ctx.set_args(args);

    auto vm = bond::Vm(&ctx);
    auto id = ctx.new_module(std::string("<repl>"));
    bond::Scopes scope = bond::Scopes(&ctx);


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

        auto lexer = bond::Lexer(source, &ctx, id);
        if (ctx.has_error()) continue;
        auto parser = bond::Parser(lexer.tokenize(), &ctx);
        parser.set_scopes(&scope);
        if (ctx.has_error()) continue;

        auto codegen = bond::CodeGenerator(&ctx, &scope);
        auto bytecode = codegen.generate_code(parser.parse());
        if (ctx.has_error()) continue;

        vm.run(bytecode);

    }
}

void run_file(const std::string &path, const std::string &lib_path, const std::vector<std::string> &args) {
    auto src = bond::Context::read_file(std::string(path));
    bond::Context ctx(lib_path);
    ctx.set_args(args);

    auto vm = bond::Vm(&ctx);

    execute_source(src, path.c_str(), vm, ctx);
}


int main(int32_t argc, char **argv) {
    GC_INIT();
    GC_enable_incremental();
    GC_set_warn_proc([](char *msg, GC_word arg) {
//        fmt::print("GC warning: {}\n", msg);
    });

    bond::init_caches();
    bond::build_core_module();


#ifdef _WIN32
    auto lib_path = std::filesystem::path(argv[0]).parent_path().string() + "/../libraries/";
#else
    auto lib_path = "/usr/local/libraries/";
#endif


    fmt::print("lib path: {} [{}]\n", lib_path, std::filesystem::exists(lib_path));
    p_libsys_init();


    auto args = std::vector<std::string>(argv, argv + argc);

    if (argc == 1) {
        fmt::print("bond 0.0.0 \n");
        run_repl(lib_path, args);
    }
    else {
        if (!std::filesystem::exists(argv[1])) {
            fmt::print("File not found: {}\n", argv[1]);
            return 1;
        }
        auto full_path = std::filesystem::absolute(argv[1]);

        auto path = std::filesystem::path(argv[1]);
        std::filesystem::current_path(path.parent_path());
        run_file(full_path.string(), lib_path, args);
    }

    p_libsys_shutdown();
}

#ifdef _WIN32

#include <windows.h>

int APIENTRY WinMain([[maybe_unused]] HINSTANCE hInstance,
                     [[maybe_unused]] HINSTANCE hPrevInstance,
                     [[maybe_unused]] LPSTR lpCmdLine, [[maybe_unused]] int nCmdShow) {
        return main(__argc, __argv);
}

#endif
