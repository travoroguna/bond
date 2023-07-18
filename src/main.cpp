#include "engine.h"
#include "conv.hpp"


int main(int32_t argc, char **argv) {
#ifdef _WIN32
    auto lib_path = std::filesystem::path(argv[0]).parent_path().string() + "/../libraries/";
#else
    auto lib_path = "/usr/local/libraries/bond/";
#endif

    auto args = std::vector<std::string>(argv, argv + argc);
    auto engine = bond::create_engine(lib_path, args);


    if (argc == 1) {
        engine->run_repl();
    }
    else {
        if (!std::filesystem::exists(argv[1])) {
            fmt::print("File not found: {}\n", argv[1]);
            return 1;
        }
        auto full_path = std::filesystem::absolute(argv[1]);

        auto path = std::filesystem::path(argv[1]);
        std::filesystem::current_path(path.parent_path());
        engine->run_file(argv[1]);
    }
}
