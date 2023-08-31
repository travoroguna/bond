//
// Created by travor on 02/08/2023.
//


#include "../../bond.h"
#include <filesystem>


namespace bond::fs {
    static obj_result exists(const t_vector &args) {
        String *path;
        TRY(parse_args(args, path));
        return AS_BOOL(std::filesystem::exists(path->get_value().c_str()));
    }

    static obj_result full_path(const t_vector &args) {
        String *path;
        TRY(parse_args(args, path));

        if (!std::filesystem::exists(path->get_value().c_str())) {
            return make_error_t(fmt::format("path '{}' does not exist", path->get_value()));
        }

        return make_ok_t(std::filesystem::absolute(path->get_value().c_str()).string());
    }

    static obj_result is_dir(const t_vector &args) {
        String *path;
        TRY(parse_args(args, path));

        if (!std::filesystem::exists(path->get_value().c_str())) {
            return make_error_t(fmt::format("path '{}' does not exist", path->get_value()));
        }

        return make_ok(AS_BOOL(std::filesystem::is_directory(path->get_value().c_str())));
    }

    static obj_result is_file(const t_vector &args) {
        String *path;
        TRY(parse_args(args, path));

        if (!std::filesystem::exists(path->get_value().c_str())) {
            return make_error_t(fmt::format("path '{}' does not exist", path->get_value()));
        }
        return make_ok(AS_BOOL(std::filesystem::is_regular_file(path->get_value().c_str())));
    }

    static obj_result is_symlink(const t_vector &args) {
        String *path;
        TRY(parse_args(args, path));

        if (!std::filesystem::exists(path->get_value().c_str())) {
            return make_error_t(fmt::format("path '{}' does not exist", path->get_value()));
        }

        return make_ok(AS_BOOL(std::filesystem::is_symlink(path->get_value().c_str())));
    }

    static obj_result is_empty(const t_vector &args) {
        String *path;
        TRY(parse_args(args, path));

        if (!std::filesystem::exists(path->get_value().c_str())) {
            return make_error_t(fmt::format("path '{}' does not exist", path->get_value()));
        }

        return make_ok(AS_BOOL(std::filesystem::is_empty(path->get_value().c_str())));
    }

    static obj_result is_relative(const t_vector &args) {
        String *path;
        TRY(parse_args(args, path));

        if (!std::filesystem::exists(path->get_value().c_str())) {
            return make_error_t(fmt::format("path '{}' does not exist", path->get_value()));
        }

        return make_ok(AS_BOOL(std::filesystem::path(path->get_value().c_str()).is_relative()));
    }

    static obj_result is_absolute(const t_vector &args) {
        String *path;
        TRY(parse_args(args, path));

        if (!std::filesystem::exists(path->get_value().c_str())) {
            return make_error_t(fmt::format("path '{}' does not exist", path->get_value()));
        }

        return make_ok(AS_BOOL(std::filesystem::path(path->get_value().c_str()).is_absolute()));
    }

    static obj_result cwd(const t_vector &args) {
        TRY(parse_args(args));
        return make_string(std::filesystem::current_path().string());
    }

    static obj_result set_cwd(const t_vector &args) {
        String *path;
        TRY(parse_args(args, path));

        if (!std::filesystem::exists(path->get_value().c_str())) {
            return make_error_t(fmt::format("path '{}' does not exist", path->get_value()));
        }

        std::filesystem::current_path(path->get_value().c_str());
        return make_ok(Runtime::ins()->C_NONE);
    }


    static obj_result join(const t_vector &args) {
        std::vector<std::string> paths;

        size_t i = 0;
        for (auto &arg: args) {
            if (!arg->is<String>()) {
                return ERR(fmt::format("expected String at argument {}, got {}", i, get_type_name(arg)));
            }
            paths.emplace_back(arg->str());
            i++;
        }

        auto path = std::filesystem::path(paths[0]);

        for (size_t k = 1; k < paths.size(); k++) {
            path = path.concat(paths[k]);
        }

        return make_string(path.string());
    }
}


EXPORT void bond_module_init(bond::Context *ctx, bond::Vm* current_vm, bond::Mod &mod) {
    GC_INIT();
    bond::Runtime::ins()->set_runtime(current_vm->runtime());
    bond::set_current_vm(current_vm);

    mod.function("exists", bond::fs::exists, "exists(path: String) -> Bool\n"
                                             "Returns true if the path exists, false otherwise.");
    mod.function("full_path", bond::fs::full_path, "full_path(path: String) -> Result<String, String>\n"
                                                   "Returns the full path of the given path.");
    mod.function("is_dir", bond::fs::is_dir, "is_dir(path: String) -> Result<Bool, String>\n"
                                                "Returns true if the path is a directory, false otherwise.");
    mod.function("is_file", bond::fs::is_file, "is_file(path: String) -> Bool!String>\n"
                                                "Returns true if the path is a file, false otherwise.");
    mod.function("is_symlink", bond::fs::is_symlink, "is_symlink(path: String) -> Bool!String\n"
                                                      "Returns true if the path is a symlink, false otherwise.");
    mod.function("is_empty", bond::fs::is_empty, "is_empty(path: String) -> Bool!String\n"
                                                    "Returns true if the path is empty, false otherwise.");
    mod.function("is_relative", bond::fs::is_relative, "is_relative(path: String) -> Bool!String\n"
                                                        "Returns true if the path is relative, false otherwise.");
    mod.function("is_absolute", bond::fs::is_absolute, "is_absolute(path: String) -> Bool!String\n"
                                                        "Returns true if the path is absolute, false otherwise.");
    mod.function("cwd", bond::fs::cwd, "cwd() -> String\n"
                                        "Returns the current working directory.");
    mod.function("set_cwd", bond::fs::set_cwd, "set_cwd(path: String) -> Result<None, String>\n"
                                                "Sets the current working directory to the given path.");
    mod.function("join", bond::fs::join, "join(...paths: String) -> String\n"
                                            "Joins the given paths together.");
}