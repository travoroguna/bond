#include "api.h"
#include "core/build.h"
#include "engine.h"
#include "import.h"
#include "runtime.h"
#include <argumentum/argparse-h.h>


int main(int32_t argc, char **argv) {
#ifdef _WIN32
    auto lib_path =
            std::filesystem::path(bond::get_exe_path()).parent_path().string() +
            "/../libraries/";

    if (!std::filesystem::exists(lib_path)) {
        lib_path =
                std::filesystem::path(bond::get_exe_path()).parent_path().string() +
                "/";
    }

    fmt::print("lib_path: {}\n", lib_path);
#else
    std::string lib_path = "/usr/local/libraries/";

    if (!std::filesystem::exists(lib_path)) {
      lib_path =
          std::filesystem::path(bond::get_exe_path()).parent_path().string() +
          "/";
    }

#endif

    auto args = std::vector<std::string, gc_allocator<std::string>>(argv, argv + argc);
    auto engine = bond::create_engine(lib_path, args);

    std::string file;
    bool build;
    bool experimental_type_checker;
    bool enable_generational_gc;
    bool enable_disassembler;

    using namespace argumentum;
    auto parser = argument_parser{};
    auto params = parser.params();
    parser.config().program(argv[0]).description("Bond programming language");
    params.add_parameter(file, "f").help("File to run");
    params.add_parameter(build, "--build-archive", "-b")
            .nargs(0)
            .help("Build archive from file");
    params
            .add_parameter(experimental_type_checker, "--experimental-type-checker",
                           "-c")
            .nargs(0)
            .help("turn on experimental type checking");

    params.add_parameter(enable_generational_gc, "--enable-generational-gc")
            .nargs(0)
            .help("turn on generational garbage collection");

    params.add_parameter(enable_disassembler, "--disassemble")
            .nargs(0)
            .help("turn on disassembler");

    if (!parser.parse_args(argc, argv, 1)) {
        return 1;
    }

    if (enable_generational_gc) {
        GC_enable_incremental();
    }

    if (argc == 1) {
        engine->run_repl();
        return 0;
    }


    engine->set_checker(experimental_type_checker);
    engine->set_disassembler(enable_disassembler);

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

    int exit_code;

    auto vm = bond::Vm(engine->get_context());
    bond::set_current_vm(&vm);

    if (is_archive) {
        auto res = bond::Import::instance().import_archive(engine->get_context(),
                                                           full_path);
        if (!res) {
            fmt::print("Failed to load archive: {}\n", res.error());
            return 1;
        }
        vm.run(res.value());
        exit_code = vm.had_error() or engine->get_context()->has_error() ? 1 : 0;
    } else {
        engine->run_file(full_path);
        exit_code = engine->get_context()->has_error() ? 1 : 0;
    }

    bond::Runtime::ins()->exit();
    return exit_code;
}
