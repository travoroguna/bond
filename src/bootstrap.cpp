//
// Created by travor on 09/09/2023.
//
#include "engine.h"
#include "import.h"
#include "runtime.h"


int main(int argc, char **argv) {
// name of the executable will be the name of the archive
// The archive and executable must be in the same directory

    auto exe_path = std::filesystem::path(bond::get_exe_path()).parent_path().string() + "/";
    auto lib_path = exe_path + "libraries/";

    auto engine = bond::create_engine(lib_path, std::vector<std::string, gc_allocator<std::string>>(argv, argv + argc));
    engine->set_checker(false);

    auto exe_name = std::filesystem::path(bond::get_exe_path()).filename().stem().string();
    auto archive_name = exe_name + ".bar";

    auto vm = bond::Vm(engine->get_context());
    bond::set_current_vm(&vm);

    auto full_path = exe_path + archive_name;

    if (!std::filesystem::exists(full_path)) {
        fmt::print("Archive not found: {}\n", full_path);
        return 1;
    }

    auto res = bond::Import::instance().import_archive(engine->get_context(), exe_path + archive_name);

    if (!res) {
        fmt::print("Failed to load archive: {}\n", res.error());
        return 1;
    }

    if (!res) {
        fmt::print("Failed to load archive: {}\n", res.error());
        return 1;
    }

    vm.run(res.value());

    return vm.had_error() or engine->get_context()->has_error() ? 1 : 0;
}