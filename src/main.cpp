#include <iostream>
#include <cstdint>

#include "lexer.h"
#include "parser.h"
#include "context.h"
#include "code.h"
#include "vm.h"
#include "gc.h"


void execute_source(std::string& source, const char *path, bond::Vm &vm, bond::Context &ctx) {
    auto id = ctx.new_module(std::string(path));

    auto lexer = bond::Lexer(source, &ctx, id);
    auto tokens = lexer.tokenize();

    auto parser = bond::Parser(tokens, &ctx);
    auto nodes = parser.parse();

    if (ctx.has_error()) return;

    auto codegen = bond::CodeGenerator(&ctx, parser.get_scopes());
    auto bytecode = codegen.generate_code(nodes);

  if (ctx.has_error()) return;
    fmt::print("disassembly of opcode\n{}\n", bytecode->dissasemble());

    vm.run(bytecode);
}


void run_repl() {
    bond::Context ctx;

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

void run_file(const char *path) {
    auto src = bond::Context::read_file(std::string(path));
    bond::Context ctx;

    auto vm = bond::Vm(&ctx);
    bond::GarbageCollector::instance().add_root(&vm);

    execute_source(src, path, vm, ctx);
}



int main(int32_t argc, const char * argv[]) {
   if (argc > 2) {
       std::cout << "Usage: bond <script path>\n";
       return 1;
   }
   else if (argc == 1){
       fmt::print("bond 0.0.0 \n");
       run_repl();
   } else {
       run_file(argv[1]);
   }

}
