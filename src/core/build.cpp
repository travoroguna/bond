//
// Created by travor on 22/07/2023.
//


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
        TRY(find_deps(main_file));
        if (context.has_error()) return std::unexpected("build failed");

        //build graph
//        std::unordered_map<t_string, std::vector<t_string>> graph;
//
//        for (auto &unit : units) {
//            graph[unit.first] = unit.second->get_dependencies();
//        }
//
//        auto sorted = topologicalSort(graph);
//        order does not matter

        auto output_file = std::filesystem::path(main_file.c_str()).stem().string() + ".bar";
        TRY(write_archive(output_file, units));
        return output_file;
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