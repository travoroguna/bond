#include <iostream>
#include <cstdint>

#include "lexer.h"
#include "parser.h"
#include "context.h"
#include "code.h"
#include "vm.h"


void execute_source(std::string& source, const char *path) {
    bond::Context ctx;
    auto id = ctx.new_module(std::string(path));

    auto lexer = bond::Lexer(source, &ctx, id);
    auto tokens = lexer.tokenize();

    for (auto t: tokens){
        fmt::print("{}\n", t.as_string());
    }

    auto parser = bond::Parser(tokens, &ctx);
    auto expr = parser.parse();

    if (ctx.has_error()) return;

    auto codegen = bond::CodeGenerator(&ctx);
    auto bytecode = codegen.generate_code(expr);

}


void run_repl() {
    while (true) {
        bond::Context ctx;
        auto path = std::string("<repl>");
        auto id = ctx.new_module(path);

        std::string source;

        fmt::print("bond > ");

        while (char c = getchar()) {
            if (c == '\n') break;
            source += c;
        }

        if (source == "exit") break;

        execute_source(source, path.c_str());
    }
}

void run_file(const char *path) {
    auto src = bond::Context::read_file(std::string(path));
    execute_source(src, path);
}



int main(int32_t argc, const char * argv[]) {
   fmt::print("bond 0.0.0 {}\n", argc);

   if (argc > 2) {
       std::cout << "Usage: bond <script path>\n";
       return 1;
   }
   else if (argc == 1){
       run_repl();
   } else {
       run_file(argv[1]);
   }
}
