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

    auto codegen = bond::CodeGenerator(&ctx);
    auto bytecode = codegen.generate_code(nodes);

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
    auto str0 = bond::GarbageCollector::instance().make<bond::String>("hello");
    auto str1 = bond::GarbageCollector::instance().make<bond::String>("hello");
    auto str2 = bond::GarbageCollector::instance().make<bond::String>("bello");


    fmt::print("hashes: {} and {}, is_equal: {}\n", str0->hash(), str1->hash(), str0->equal(str1));

    auto map= bond::GarbageCollector::instance().make<bond::Map>();
    map->set(str0, str1);

    fmt::print("hello in map? same key {}, different key {}, test key {}\n", map->has(str0), map->has(str1), map->has(str2));

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
