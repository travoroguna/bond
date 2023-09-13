//
// Created by travor on 22/07/2023.
//


#include <iosfwd>
#include "build.h"
#include "../import.h"

namespace bond{
    std::expected<void, t_string> Unit::find_dependencies() {
        if (std::filesystem::is_directory(path.c_str())) {
            return std::unexpected(fmt::format("can not import directory {} as a module", path));
        }

        auto src = Context::read_file(path.c_str());
        unit_id = context->new_module(path.c_str());
        auto lexer = Lexer(src, context, unit_id);
        auto tokens = lexer.tokenize();

        auto parser = Parser(tokens, context);
        nodes = parser.parse();
        scopes = std::make_shared<Scopes>(*parser.get_scopes());

        auto pre_cwd = std::filesystem::current_path();
        std::filesystem::current_path(std::filesystem::path(path.c_str()).parent_path());

        for (auto &node : nodes) {
            if (instanceof<ImportDef>(node.get())) {
                auto import_def = std::dynamic_pointer_cast<ImportDef>(node);
                auto name = import_def->get_name();

                //do not add core to dependencies
                if (name.starts_with("core:")) {
                    auto p = path.substr(5);
                    auto res = resolve_core(p);

                    if (!res) {
                        return std::unexpected(res.error());
                    }
                    continue;
                }
                if (name.starts_with("core")) continue;

                auto res = path_resolver(context, import_def->get_name());

                if (!res) {
                    return std::unexpected(res.error());
                }

                import_def->set_actual_path(res.value().c_str());

                dependencies.push_back(res.value());
            }
        }
        std::filesystem::current_path(pre_cwd);
        return {};
    }

    std::vector<t_string> &Unit::get_dependencies() {
        return dependencies;
    }


    Build::Build(const t_string &lib_path, const t_string &main_file)
        :context(lib_path.c_str()) {
        this->main_file = main_file;
    }


    void topologicalSortDFS(const t_string& module, std::unordered_map<t_string, std::vector<t_string>>& graph,
                            std::unordered_set<t_string>& visited, std::vector<t_string>& result) {

        visited.insert(module);

        for (auto& dep : graph[module]) {
            if (visited.contains(dep)) continue;
            topologicalSortDFS(dep, graph, visited, result);
        }

        result.push_back(module);
    }

    std::vector<t_string> topologicalSort(std::unordered_map<t_string, std::vector<t_string>>& graph) {
        std::unordered_set<t_string> visited;
        std::vector<t_string> result;

        for (auto& [module, _] : graph) {
            if (visited.contains(module)) continue;
            topologicalSortDFS(module, graph, visited, result);
        }
//        std::reverse(result.begin(), result.end());
        return result;
    }


    std::expected<GcPtr<Code>, t_string> Unit::compile() {
        auto codegen = CodeGenerator(context, scopes.get());
        auto code = codegen.generate_code(nodes);

        if (context->has_error()) {
            return std::unexpected("compilation failed");
        }
        return code;
    }


    std::expected<t_string, t_string> Build::build() {
        try {
            TRY(find_deps(main_file));
            if (context.has_error()) return std::unexpected("build failed");

            auto output_file = std::filesystem::path(main_file.c_str()).stem().string() + ".bar";
            TRY(write_archive_file(output_file, units));
            TRY(pack_to_dist_dir(output_file));
            return output_file;
        }
        catch (std::exception& e) {
            return std::unexpected(fmt::format("build failed: {}", e.what()));
        }
    }


    std::vector<t_string> required_files = {
            "gc.dll", "gccpp.dll", "gctba.dll"
    };

    std::vector<t_string> optional_debug = {
            "gc.pdb", "gccpp.pdb", "gctba.pdb"
    };

    std::expected<void, t_string> Build::pack_to_dist_dir(const t_string& arch_path) {
        fmt::print("Packing to dist directory\n");
        std::string dist_dir = "dist/";

        auto exe_path = std::filesystem::path(get_exe_path()).parent_path().string() + "/";

        if (!std::filesystem::exists(dist_dir)) {
            std::filesystem::create_directory(dist_dir);
        }
        else {
            if (!std::filesystem::is_directory(dist_dir)) {
                return std::unexpected(fmt::format("dist is not a directory"));
            }
            std::filesystem::remove_all(dist_dir);
            std::filesystem::create_directory(dist_dir);
        }

        std::filesystem::create_directory(dist_dir + "/libraries");

        auto b_path = exe_path + "bootstrap.exe";
        // copy bootstrap and rename to dist dir
        if (!std::filesystem::exists(b_path)) {
            return std::unexpected(fmt::format("bootstrap not found: {}", b_path));
        }

        auto arch_name = std::filesystem::path(arch_path.c_str()).filename().stem().string();

        //optional copy bootstrap debug files
//        if (std::filesystem::exists(context.get_lib_path() + "bootstrap.pdb")) {
//            std::filesystem::copy_file(context.get_lib_path() + "bootstrap.pdb", dist_dir + arch_name + ".pdb", std::filesystem::copy_options::overwrite_existing);
//        }

        std::filesystem::copy_file(b_path, dist_dir + arch_name + ".exe", std::filesystem::copy_options::overwrite_existing);

        // copy required files
        for (auto& file : required_files) {
            auto the_lib = exe_path + file.c_str();
            if (!std::filesystem::exists(the_lib)) {
                return std::unexpected(fmt::format("required file not found: {}", the_lib));
            }
            std::filesystem::copy_file(the_lib, dist_dir + file.c_str(), std::filesystem::copy_options::overwrite_existing);
        }

        // copy optional debug files
//        for (auto& file : optional_debug) {
//            if (!std::filesystem::exists(context.get_lib_path() + file.c_str())) {
//                continue;
//            }
//            std::filesystem::copy_file(context.get_lib_path() + file.c_str(), dist_dir + file.c_str(), std::filesystem::copy_options::overwrite_existing);
//        }

        //copy archive
        if (!std::filesystem::exists(arch_path.c_str())) {
            return std::unexpected(fmt::format("archive not found: {}", arch_path));
        }

        std::filesystem::copy_file(arch_path.c_str(), dist_dir + arch_name + ".bar", std::filesystem::copy_options::overwrite_existing);


        // TODO: copy libraries

        return {};
    }


    std::expected<void, t_string> Build::find_deps(const t_string& path) {
        //do not add core to dependencies
        if (path.starts_with("core:")) {
            auto p = path.substr(5);
            auto res = resolve_core(p);

            if (!res) {
                return std::unexpected(res.error());
            }
            return {};
        }
        if (path.starts_with("core")) return {};

        auto unit = std::make_shared<Unit>(path, &context);
        TRY(unit->find_dependencies());
        units[std::filesystem::absolute(path.c_str()).string()] = unit;

        for (auto &dep : unit->get_dependencies()) {
            if (units.contains(std::filesystem::absolute(dep.c_str()).string())) {
                continue;
            }
            TRY(find_deps(dep));
        }

        return {};
    }
}