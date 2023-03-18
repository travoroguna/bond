#include <iostream>
#include <cstdint>

#include "lexer.h"
#include "context.h"


void execute_source(std::string& source);


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

        auto lexer = bond::Lexer(source, &ctx, id);
        auto tokens = lexer.tokenize();

        for (auto t: tokens){
            fmt::print("{}\n", t.as_string());
        }
    }
}


void run_file(std::string& path) {

}


int main(int32_t argc, const char * argv[]) {
    fmt::print("bond 0.0.0 {}\n", argc);

    if (argc > 1) {
        std::cout << "Usage: bond <script path>\n";
        return 1;
    }
    else if (argc == 1){
        run_repl();
    } else {
        run_file((std::string &) argv[1]);
    }
}
