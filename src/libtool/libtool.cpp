//
// Created by Travor Oguna Oneya on 13/05/2023.
//

#include "../bond.h"
#include <plibsys.h>
#include <filesystem>
#include <fmt/core.h>
#include <memory>


int main(int argc, const char **argv) {
    if (argc < 2) {
        fmt::print("Usage: bond <filename>\n");
        return 1;
    }

    p_libsys_init();

    auto filename = argv[1];
    auto file = std::filesystem::path(filename);

    if (!std::filesystem::exists(file)) {
        fmt::print("File {} does not exist\n", filename);
        return 1;
    }

    auto library = p_library_loader_new(filename);
    if (library == nullptr) {
        fmt::print("Failed to load library {}\n", filename);
        fmt::print("Error: {}\n", p_library_loader_get_last_error(library));
        p_libsys_shutdown();
        return 1;
    }


    auto bond_init = (void (*)(bond::Context *, const std::string &)) p_library_loader_get_symbol(library,
                                                                                                  "bond_module_init");

    bond::GarbageCollector::instance().set_main_thread_id(std::this_thread::get_id());
    bond::GarbageCollector::instance().make_thread_storage();

    if (bond_init == nullptr) {
        fmt::print("Failed to load symbol bond_module_init from {}\n", filename);
        return 1;
    }

    auto ctx = bond::Context(file.parent_path().string());
    bond_init(&ctx, file.string());

    auto mod = ctx.get_module(file.string())->as<bond::Module>();

    fmt::print("Loaded module {}\n", mod->str());
    for (auto &[name, value]: mod->get_globals()->get_map()) {
        fmt::print("{} = {}\n", name->str(), value->str());
    }

    p_libsys_shutdown();
    return 0;
}