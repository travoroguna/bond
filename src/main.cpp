#include "engine.h"
#include "conv.hpp"
#include "core/build.h"
#include "import.h"
#include "api.h"
#include <argumentum/argparse-h.h>


int main(int32_t argc, char **argv) {
#ifdef _WIN32
    auto lib_path = std::filesystem::path(argv[0]).parent_path().string() + "/../libraries/";
#else
    auto lib_path = "/usr/local/libraries/bond/";
#endif

    auto args = std::vector<std::string, gc_allocator<std::string>>(argv, argv + argc);
    auto engine = bond::create_engine(lib_path, args);

    std::string file;
    bool build;

    using namespace argumentum;
    auto parser = argument_parser{};
    auto params = parser.params();
    parser.config().program(argv[0]).description("Bond programming language");
    params.add_parameter(file, "f").help("File to run");
    params.add_parameter(build, "--build-archive", "-b")
            .nargs(0)
            .help("Build archive from file");


    if (argc == 1) {
        engine->run_repl();
        return 0;
    }

    if (!parser.parse_args(argc, argv, 1)) {
        return 1;
    }

    if (!std::filesystem::exists(file)) {
        fmt::print("File not found: {}\n", file);
        return 1;
    }
    auto f_path = std::filesystem::absolute(file);

    auto path = std::filesystem::path(file);
    std::filesystem::current_path(path.parent_path());

    auto full_path = f_path.string();

    auto is_archive = bond::may_be_bar_file(full_path);

    if (build) {
        if (is_archive) {
            fmt::print("File is already an archive: {}\n", full_path);
            return 1;
        }

        auto builder = bond::Build(lib_path, full_path);
        auto res = builder.build();

        if (!res) {
            fmt::print("Build failed: {}\n", res.error());
            return 1;
        }

        fmt::print("Build succeeded: {}\n", full_path);
        return 0;
    }

    if (is_archive) {
        auto res = bond::Import::instance().import_archive(engine->get_context(), full_path);
        if (!res) {
            fmt::print("Failed to load archive: {}\n", res.error());
            return 1;
        }

        auto pre = bond::get_current_vm();
        auto vm = bond::Vm(engine->get_context());
        bond::set_current_vm(&vm);

        vm.run(res.value());

        bond::set_current_vm(pre);
        return vm.had_error() or engine->get_context()->has_error() ? 1 : 0;
    }

    else {
        engine->run_file(full_path);
        return engine->get_context()->has_error() ? 1 : 0;
    }

}
