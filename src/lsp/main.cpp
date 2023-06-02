#include <iostream>
#include "common.hpp"
#include "lsp.hpp"
#include "uri.hh"
#include "../bond.h"
#include <filesystem>
#include <plibsys.h>
#include <thread>

namespace lsp {
    std::string build_signature(bond::FuncDef *def) {
        std::string sig = "fn " + def->get_name()+ "(";

        for (int i = 0; i < def->get_params().size(); i++) {
            auto param = def->get_params()[i];

            sig += param.first;

            if (i != def->get_params().size() - 1) {
                sig += ", ";
            }
        }

        return sig + ")";
    }


    class Workspace {
    public:
        Workspace(bond::Context *ctx) : m_ctx(ctx) {
            bond::GarbageCollector::instance().add_root(&m_root);
        }

        std::vector<completion_item> get_completion_list(const std::string& uri_link) {
            m_loaded.clear();

            auto path = uri(uri_link).get_path();

            int index;
            while ((index = path.find("%3A")) != std::string::npos) {
                path.replace(index, 3, ":");
            }

            auto src = bond::Context::read_file(path);

            std::vector<completion_item> items = {
                    completion_item().label("println").kind(completion_item_kind::function).detail("builtin"),
                    completion_item().label("Ok").kind(completion_item_kind::function).detail("builtin"),
                    completion_item().label("Err").kind(completion_item_kind::function).detail("builtin"),
                    completion_item().label("int").kind(completion_item_kind::function).detail("builtin"),
                    completion_item().label("float").kind(completion_item_kind::function).detail("builtin"),
                    completion_item().label("str").kind(completion_item_kind::function).detail("builtin"),
                    completion_item().label("range").kind(completion_item_kind::function).detail("builtin"),
                    completion_item().label("is_instance").kind(completion_item_kind::function).detail("builtin"),
                    completion_item().label("map").kind(completion_item_kind::function).detail("builtin"),
            };

            auto idx = m_ctx->new_module(path);
            auto nodes = parse(idx, src);
            auto m = get_completion_items(nodes);

            items.reserve(items.size() + m.size());
            items.insert(items.end(), m.begin(), m.end());

            return items;
        }


        std::vector<completion_item> get_dot_list(const std::string& uri_link) {
            auto path = uri(uri_link).get_path();

            int index;
            while ((index = path.find("%3A")) != std::string::npos) {
                path.replace(index, 3, ":");
            }

            auto src = bond::Context::read_file(path);

            auto idx = m_ctx->new_module(path);
            auto nodes = parse(idx, src);
            auto m = get_dot_completion(nodes);

            return m;
        }

        std::vector<completion_item> get_dot_completion(const std::vector<std::shared_ptr<bond::Node>>& nodes) {
//            std::cerr << "nodes has " << nodes.size() << " items" << std::endl;
            std::vector<completion_item> items;

            for (auto &node: nodes) {
//                std::cerr << "node: " << typeid(*node.get()).name() << std::endl;

                if (bond::instanceof<bond::StructNode>(node.get())) {
//                    std::cerr << "StructNode" << std::endl;
                    auto struct_node = dynamic_cast<bond::StructNode *>(node.get());

                    for (auto &field : struct_node->get_methods()) {
                        auto meth = dynamic_cast<bond::FuncDef *>(field.get());
                        items.push_back(completion_item()
                                                   .label(meth->get_name())
                                                   .kind(completion_item_kind::method)
                                                   .detail(build_signature(meth))
                        );
                    }
                }

                if (bond::instanceof<bond::ImportDef>(node.get())) {
//                    std::cerr << "ImportDef" << std::endl;
                    auto import_def = dynamic_cast<bond::ImportDef *>(node.get());
                    auto import_items = get_items_from_import(import_def);
                    items.insert(items.end(), import_items.begin(), import_items.end());
                }
            }
            return items;
        }

        std::vector<completion_item> get_completion_items(const std::vector<std::shared_ptr<bond::Node>>& nodes) {
            std::vector<completion_item> items;

            for (auto &node: nodes) {
                if (bond::instanceof<bond::FuncDef>(node.get())) {
                    auto func = dynamic_cast<bond::FuncDef *>(node.get());
                    items.emplace_back(completion_item()
                                               .label(func->get_name())
                                               .kind(completion_item_kind::function)
                                               .detail(build_signature(func))
                    );

                    auto fn = get_completion_items(std::vector{func->get_body()});

                } else if (bond::instanceof<bond::StructNode>(node.get())) {
                    auto struct_node = dynamic_cast<bond::StructNode *>(node.get());
                    items.emplace_back(completion_item()
                                               .label(struct_node->get_name())
                                               .kind(completion_item_kind::struct_));

                } else if (bond::instanceof<bond::NewVar>(node.get())) {
                    auto new_var = dynamic_cast<bond::NewVar *>(node.get());
                    items.emplace_back(completion_item().label(new_var->get_name())
                                               .kind(completion_item_kind::variable));

                } else if (bond::instanceof<bond::ImportDef>(node.get())) {
                    auto import_def = dynamic_cast<bond::ImportDef *>(node.get());
                    items.emplace_back(completion_item()
                                               .label(fmt::format("{} {}", import_def->get_alias(), import_def->get_name()))
                                               .insertText(import_def->get_alias())
                                               .sortText(import_def->get_alias())
                                               .kind(completion_item_kind::module));

                } else if (bond::instanceof<bond::Identifier>(node.get())) {
                    auto identifier = dynamic_cast<bond::Identifier *>(node.get());
                    items.emplace_back(completion_item().label(identifier->get_name())
                                               .kind(completion_item_kind::variable));
                } else if (bond::instanceof<bond::Block>(node.get())) {
                    auto block = dynamic_cast<bond::Block *>(node.get());

                    auto block_items = get_completion_items(block->get_nodes());
                    items.insert(items.end(), block_items.begin(), block_items.end());

                } else if (bond::instanceof<bond::If>(node.get())) {
                    auto if_node = dynamic_cast<bond::If *>(node.get());
                    auto if_items = get_completion_items({if_node->get_then()});
                    items.insert(items.end(), if_items.begin(), if_items.end());

                    if (if_node->get_else()) {
                        auto else_items = get_completion_items({if_node->get_else().value()});
                        if_items.insert(if_items.end(), else_items.begin(), else_items.end());
                    }
                } else if (bond::instanceof<bond::While>(node.get())) {
                    auto while_node = dynamic_cast<bond::While *>(node.get());
                    auto while_items = get_completion_items({while_node->get_statement()});
                    items.insert(items.end(), while_items.begin(), while_items.end());
                } else if (bond::instanceof<bond::For>(node.get())) {
                    auto for_node = dynamic_cast<bond::For *>(node.get());
                    auto for_items = get_completion_items({for_node->get_statement()});
                    items.insert(items.end(), for_items.begin(), for_items.end());
                }
            }

            return items;
        }

        std::vector<completion_item> get_items_from_import(bond::ImportDef *import) {
            auto path = path_resolver(import->get_name());
            if (!path.has_value()) {
//                std::cerr << "invalid path " << import->get_name() << std::endl;
                return {};
            }

            if(path->ends_with(".dll") or path->ends_with(".so")){
                if (m_compiled_library_cache.contains(path.value())){
                    return m_compiled_library_cache[path.value()];
                }

                auto cmp = get_lib_items(path.value());
                m_compiled_library_cache[path.value()] = cmp;
                return cmp;
            }

            auto id = m_ctx->new_module(path.value());
            auto mod= parse(id, bond::Context::read_file(path.value()));
            return get_completion_items(mod);
        }

        std::vector<completion_item> get_lib_items(const std::string &filename) {
            auto library = p_library_loader_new(filename.c_str());

            if (library == nullptr) {
                return {};
            }


            auto bond_init = (void (*)(bond::Context *, const std::string &)) p_library_loader_get_symbol(library,
                                                                                                          "bond_module_init");
            if (bond_init == nullptr) {
                fmt::print("Failed to load symbol bond_module_init from {}\n", filename);
                return {};
            }

            auto file = std::filesystem::path(filename);

            auto ctx = bond::Context(file.parent_path().string());

            bond::LockGc lock;
            bond_init(&ctx, file.string());

            auto mod = ctx.get_module(file.string())->as<bond::Module>();

            m_root.push(mod);

            auto items = std::vector<completion_item>();

            for (auto &[name, value]: mod->get_globals()->get_map()) {
//                std::cerr << "found" << name->str() << std::endl;
                if (bond::instanceof<bond::Enum>(value.get())) {
                    auto enum_ = dynamic_cast<bond::Enum *>(value.get());
                    items.emplace_back(completion_item()
                                               .label(name->str())
                                               .kind(completion_item_kind::enum_)
                    );

                    auto attributes = enum_->get_attributes();

                    for (auto &[n, v]: attributes) {
                        items.emplace_back(completion_item().label(n)
                                                   .kind(completion_item_kind::enum_member)
                                                   .detail(v->str()));
                    }

                }

                else if(bond::instanceof<bond::BondObject>(value.get())) {
                    auto bond_object = dynamic_cast<bond::BondObject *>(value.get());
                    items.emplace_back(completion_item()
                                               .label(name->str())
                                               .kind(completion_item_kind::struct_)
                    );
                }

                else if(bond::instanceof<bond::NativeFunction>(value.get())) {
                    auto native_function = dynamic_cast<bond::NativeFunction *>(value.get());
                    items.emplace_back(completion_item()
                                               .label(name->str())
                                               .kind(completion_item_kind::function)
                    );
                }
                else if(bond::instanceof<bond::Module>(value.get())) {
                    auto module = dynamic_cast<bond::Module *>(value.get());
                    items.emplace_back(completion_item()
                                               .label(name->str())
                                               .kind(completion_item_kind::module)
                    );

                    for (auto &[n, v]: module->get_globals()->get_map()) {
                        items.emplace_back(completion_item()
                                                   .label(n->str())
                                                   .kind(completion_item_kind::variable)
                        );
                    }
                }

                else {
                    items.emplace_back(completion_item()
                                               .label(name->str())
                                               .kind(completion_item_kind::variable)

                    );
                }

            }

            return items;
        }

        std::expected<std::string, std::string> path_resolver(const std::string &path) {
#ifdef _WIN32
            auto test_compiled_native = m_ctx->get_lib_path() + path + ".dll";
            auto test_compiled_c_path = path + ".dll";
#else
            auto lib_p = std::string("lib") + path + ".so";
        auto test_compiled_native = m_ctx->get_lib_path() + lib_p + ".so";
        auto test_compiled_c_path = lib_p + ".so";
#endif

            auto test_native = m_ctx->get_lib_path() + path + ".bd";
            auto test_user = path + ".bd";


            std::array<std::string, 4> paths = {test_compiled_native, test_compiled_c_path, test_native, test_user};

            for (auto &p: paths) {
//                std::cerr << "trying " << p << std::endl;
                if (std::filesystem::exists(p)) {
                    return p;
                }
            }

            return std::unexpected(fmt::format("failed to resolve path {}", path));
        }

        std::vector<std::shared_ptr<bond::Node>> parse(uint32_t id, const std::string &text) {
            auto lexer = bond::Lexer(text, m_ctx, id);
            auto parser = bond::Parser(lexer.tokenize(), m_ctx);
            parser.disable_reporting();
            return parser.parse();
        }

        [[nodiscard]] bool is_loading() const {
            return !m_loading.empty();
        }

    private:
        bond::Context *m_ctx;
        std::unordered_map<std::string, std::vector<completion_item>> m_compiled_library_cache;
        std::recursive_mutex m_mutex;
        std::vector<bool> m_loading;
        std::unordered_set<std::string> m_loaded;
        bond::Root m_root;
    };


    class BondLsp : public lsp::langserver {
    public:
        explicit BondLsp(const std::string &path): m_ctx(path) {}

        initialize_result initialize(initialize_params const &params) override {
            auto root_path = params.root_path();

            if (root_path) {
                std::filesystem::current_path(root_path.value());
            }

            return lsp::initialize_result()
                    .capabilities(
                            lsp::server_capabilities()
                                    .completion_provider(
                                            lsp::completion_options()
                                            .resolve_provider(false)
                                            .trigger_characters(std::vector<char>{'.'})
                                            )
                                    .diagnostic_provider(diagnostic_options())
                                    );
        }

        void on_text_document_opened(did_open_text_document_params const & open_params) override {
            std::cerr << "opened " << open_params.text_document().uri() << std::endl;
        }

        void on_text_document_changed(did_change_text_document_params const & changed_params) override {
            std::cerr << "changed " << changed_params.text_document().uri() << std::endl;
        }

        void on_text_document_saved(did_save_text_document_params const &saved_params) override {
            std::cerr << "saved " << saved_params.text_document().uri() << std::endl;
        }

        std::vector<document_highlight> on_text_document_highlight(text_document_position_params const &) override {
            return {};
        }

        std::vector<folding_range> on_folding_range(folding_range_params const &) override {
            return {};
        }

        completion_list on_completion(completion_params const& params) override {
            auto document = params.text_document().uri();
            auto kind = params.doc_context().trigger_kind();

            auto list = m_workspace.get_dot_list(document);
            auto l2 = m_workspace.get_completion_list(document);

            list.insert(list.end(), l2.begin(), l2.end());
//            std::cerr << "completed dot list" << list.size() << " items" << std::endl << std::flush;
            return completion_list().is_incomplete(false).items(list);
        }

        document_diagnostic_report on_diagnostic(document_diagnostic_params const&) {
            return {};
        }


    private:
        bond::Context m_ctx;
        Workspace m_workspace{&m_ctx};
        std::vector<completion_list> m_imports;
        std::string m_current;
    };

}


int main(int32_t argc, char **argv) {
    p_libsys_init();
    bond::GarbageCollector::instance().set_main_thread_id(std::this_thread::get_id());
    bond::GarbageCollector::instance().make_thread_storage();

    auto lib_path = std::filesystem::path(argv[0]).parent_path().string() + "/../libraries/";

    auto server = lsp::BondLsp(lib_path);
    lsp::start_langserver(server, std::cin, std::cout);
    p_libsys_shutdown();
    return 0;
}