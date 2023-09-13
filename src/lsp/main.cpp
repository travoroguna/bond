#include <iostream>
#include "common.hpp"
#include "lsp.hpp"
#include "uri.hh"
#include "../bond.h"
#include "../core/core.h"
#include "../import.h"
#include "resolver.h"
#include <filesystem>
#include <thread>
#include <deque>
#include <cctype>
#include <iomanip>
#include <string>
#include <sstream>


namespace lsp {
#ifdef _WIN32
    #define FILE_URI_PREFIX "file:///"
    #define FILE_URI_PREFIX_LEN 8
#else
    #define FILE_URI_PREFIX "file://"
    #define FILE_URI_PREFIX_LEN 7
#endif

    t_string encode_uri_component(const t_string& input) {
        std::ostringstream oss;
        oss.fill('0');
        oss << std::hex;

        for (char c : input) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                oss << c;
            } else {
                oss << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
            }
        }

        return oss.str();
    }

    t_string file_path_to_uri(const t_string& filePath) {
        t_string encodedPath = encode_uri_component(filePath);
        return "file:///" + encodedPath;
    }

    t_string decode_uri_component(const t_string& input) {
        std::ostringstream decoded;
        std::istringstream iss(input.c_str());
        iss >> std::hex;

        char c;
        while (iss >> c) {
            if (c == '%') {
                char hex[3] = { 0 };
                iss.read(hex, 2);
                int value;
                sscanf(hex, "%02X", &value);
                decoded << static_cast<char>(value);
            } else {
                decoded << c;
            }
        }

        return decoded.str();
    }

    t_string uri_to_file_path(const t_string& uri) {
        if (uri.compare(0, FILE_URI_PREFIX_LEN, FILE_URI_PREFIX) == 0) {
            t_string encodedPath = uri.substr(FILE_URI_PREFIX_LEN);
            t_string filePath = decode_uri_component(encodedPath);
            return filePath;
        }
        return "";
    }

    t_string build_signature(bond::FuncDef *def) {
        t_string sig = "fn " + def->get_name() + "(";

        for (size_t i = 0; i < def->get_params().size(); i++) {
            auto param = def->get_params()[i];

            sig += param->name;

            if (i != def->get_params().size() - 1) {
                sig += ", ";
            }
        }

        return sig + ")";
    }

    std::pair<position, position> span_to_position(bond::Context* ctx, bond::SharedSpan const& span) {
        // TODO: pre calculate
        auto f_name = ctx->get_module_name(span->module_id);
        if (!std::filesystem::exists(f_name))
            return {position(span->line - 1, span->start), position(span->line - 1, span->end)};

        auto contents = bond::Context::read_file(f_name);

        auto line_start = contents.rfind('\n', span->start);
        if (line_start==t_string::npos) line_start = 0;

        auto line_end = contents.find('\n', span->start);
        if (line_end > contents.length() - 1) line_end = contents.length() - 1;


        return {position(span->line - 1, span->start - line_start), position(span->line - 1, span->end - line_start)};
    }

    struct Document{
        t_string uri;
        std::vector<diagnostic> diagnostics;
        std::vector<completion_item> completion_items;
        bool is_dirty = false;
    };


    class Workspace {
    public:
        Workspace(bond::Context *ctx) : m_ctx(ctx) {
            auto globs = bond::Runtime::ins()->MAP_STRUCT->create_instance<bond::StringMap>();
            bond::add_builtins_to_globals(globs);

            for (auto &[k, val]: globs->get_value()) {
                auto c_item = completion_item().label(k);

                if (val->is<bond::NativeFunction>()) {
                    c_item.kind(completion_item_kind::function).detail(val->as<bond::NativeFunction>()->get_doc());
                }
                else if(val->is<bond::NativeStruct>()) {
                    auto the_struct = val->as<bond::NativeStruct>();

                    //this will never fail, right?
                    auto doc = bond::b_help({the_struct}).value()->as<bond::String>()->get_value();
                    c_item.kind(completion_item_kind::struct_).detail(doc);
                }
                c_item.kind(completion_item_kind::variable);

                m_c_items.push_back(c_item);
            }
        }

        void create_document(const t_string &uri_str) {
            if (!m_documents.contains(uri_str)) {
                m_documents[uri_str] = std::make_shared<Document>();
                m_documents[uri_str]->is_dirty = true;
                m_documents[uri_str]->uri = uri_str;
            }
        }

        void set_dirty(const t_string &uri_str) {
            create_document(uri_str);
            m_documents[uri_str]->is_dirty = true;
        }

        void update_document(const t_string &uri_str) {
            std::cerr << "Updating document " << uri_str << std::endl;
            create_document(uri_str);
            auto document = m_documents[uri_str];

            if (!document->is_dirty) return;

            m_ctx->reset_error();
            auto path = uri_to_file_path(uri_str);

            if (!std::filesystem::exists(path.c_str())) {
                std::cerr << "file not found " << path << std::endl;
                return;
            }

            // inline for now
            auto text = bond::Context::read_file(path.c_str());
            auto id = m_ctx->new_module(path.c_str());
            auto lexer = bond::Lexer(text, m_ctx, id);
            lexer.disable_reporting();

            auto parser = bond::Parser(lexer.tokenize(), m_ctx);
            parser.disable_reporting();

            std::vector<std::pair<t_string, bond::SharedSpan>> diags;

//            if (!m_ctx->has_error()) {
                auto nodes = parser.parse();

                for (auto &n : nodes) {
                    if (!bond::instanceof<bond::ImportDef>(n.get())) continue;

                    auto the_import = std::dynamic_pointer_cast<bond::ImportDef>(n);

                    if (the_import->get_name().starts_with("core:")) {
                        auto rest = the_import->get_name().substr(5);
                        auto result = bond::core_module->get_attribute(rest);

                        if (!result) {
                            diags.emplace_back("core module does not have attribute " + rest, the_import->get_span());
                        }
                        continue;
                    }

                    if (the_import->get_name().starts_with("core")) continue;


                    auto res = bond::path_resolver(m_ctx, the_import->get_name());
                    if (!res.has_value()) {
                        diags.emplace_back(res.error(), the_import->get_span());
                        continue;
                    }

                    auto p = res.value();
                    if (p.ends_with(".so") or p.ends_with(".dll")) {
                        continue;
                    }

                    auto u = file_path_to_uri(p);
                    std::cerr << "Importing " << u << std::endl;
                    update_document(u);
                }

                document->completion_items = get_completion_items(nodes);
                auto dot_list = get_dot_completion(nodes);
                document->completion_items.insert(document->completion_items.end(), dot_list.begin(), dot_list.end());
                document->completion_items.insert(document->completion_items.end(), m_c_items.begin(), m_c_items.end());


//            }


            auto resolver = bond::lsp::Resolver(m_ctx, nodes);
            auto resolved = resolver.resolve();

            if (!resolved.has_value()) {
                auto err = resolved.error();
                for (auto &e: err) {
                    diags.emplace_back(e.m_message, e.m_span);
                }
            }


            diags.insert(diags.end(), lexer.get_error_spans().begin(), lexer.get_error_spans().end());
            auto p_err = parser.get_diagnostics();
            diags.insert(diags.end(), p_err.begin(), p_err.end());

            auto report = std::vector<diagnostic>();

            for (auto &[d, span]: diags) {
                auto pos = span_to_position(m_ctx, span);
                auto n_d = diagnostic()
                        .diagnostic_range(
                                range().start(pos.first).end(pos.second))
                        .severity(diagnostic_severity::error)
                        .source("bond language server")
                        .message(d);

                report.push_back(n_d);
            }
            document->diagnostics = report;
            m_changed_documents.push_back(document);
            document->is_dirty = false;
        }

        std::vector<completion_item> get_completion_list(const t_string &uri_link) {
            m_loaded.clear();

            auto path = uri(uri_link.c_str()).get_path();

            int index;
            while ((index = path.find("%3A")) != t_string::npos) {
                path.replace(index, 3, ":");
            }

            auto src = bond::Context::read_file(path);

            std::vector<completion_item> items = {};
            items.insert(items.end(), m_c_items.begin(), m_c_items.end());

            auto idx = m_ctx->new_module(path);
            auto nodes = parse(idx, src);
            auto m = get_completion_items(nodes);

            items.reserve(items.size() + m.size());
            items.insert(items.end(), m.begin(), m.end());

            return items;
        }


        std::vector<completion_item> get_dot_list(const t_string &uri_link) {
            auto path = uri(uri_link.c_str()).get_path();

            int index;
            while ((index = path.find("%3A")) != t_string::npos) {
                path.replace(index, 3, ":");
            }

            auto src = bond::Context::read_file(path);

            auto idx = m_ctx->new_module(path);
            auto nodes = parse(idx, src);
            auto m = get_dot_completion(nodes);

            return m;
        }

        std::vector<completion_item> get_dot_completion(const std::vector<std::shared_ptr<bond::Node>> &nodes) {
            std::vector<completion_item> items;

            for (auto &node: nodes) {

                if (bond::instanceof<bond::StructNode>(node.get())) {
                    auto struct_node = dynamic_cast<bond::StructNode *>(node.get());

                    for (auto &field: struct_node->get_methods()) {
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

        std::vector<completion_item> get_completion_items(const std::vector<std::shared_ptr<bond::Node>> &nodes) {
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
                                               .label(fmt::format("{} {}", import_def->get_alias(),
                                                                  import_def->get_name()))
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


        std::vector<completion_item> get_items_from_import(bond::ImportDef *import_def) {
            auto path = bond::path_resolver(m_ctx, import_def->get_name());

            if (!path.has_value()) {
//                std::cerr << "invalid path " << import->get_name() << std::endl;
                return {};
            }

            if (path->ends_with(".dll") or path->ends_with(".so")) {
                if (m_compiled_library_cache.contains(path.value())) {
                    return m_compiled_library_cache[path.value()];
                }

                auto cmp = get_lib_items(path.value());
                m_compiled_library_cache[path.value()] = cmp;
                return cmp;
            }

            auto id = m_ctx->new_module(path.value().c_str());
            auto mod = parse(id, bond::Context::read_file(path.value().c_str()));
            return get_completion_items(mod);
        }

        std::vector<completion_item> get_lib_items(const t_string &filename) {
            //TODO: use import library
            return {};
        }

        std::vector<std::shared_ptr<bond::Node>> parse(uint32_t id, const t_string &text) {
            auto lexer = bond::Lexer(text.c_str(), m_ctx, id);
            auto parser = bond::Parser(lexer.tokenize(), m_ctx);
            parser.disable_reporting();
            return parser.parse();
        }

        [[nodiscard]] bool is_loading() const {
            return !m_loading.empty();
        }

        std::vector<diagnostic> &get_diagnostics(const t_string &uri) {
            if (!m_documents.contains(uri)) {
                update_document(uri);
            }

            auto doc = m_documents[uri];
            if (doc->is_dirty) {
                update_document(uri);
            }

            return doc->diagnostics;
        }

        std::vector<completion_item> &get_completion_items(const t_string &uri) {
            if (!m_documents.contains(uri)) {
                update_document(uri);
            }

            auto doc = m_documents[uri];
            if (doc->is_dirty) {
                update_document(uri);
            }

            return doc->completion_items;
        }

        std::vector<std::shared_ptr<Document>> &get_changed_documents() {
            return m_changed_documents;
        }

    private:
        bond::Context *m_ctx;
        std::unordered_map<t_string, std::vector<completion_item>> m_compiled_library_cache;
        std::recursive_mutex m_mutex;
        std::vector<bool> m_loading;
        std::unordered_set<t_string> m_loaded;
        std::vector<completion_item> m_c_items;
        std::unordered_map<t_string, std::shared_ptr<Document>> m_documents;
        std::vector<std::shared_ptr<Document>> m_changed_documents;
    };


    class BondLsp : public lsp::langserver {
    public:
        explicit BondLsp(const t_string &path) : m_ctx(path.c_str()) {}

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
//                                    .diagnostic_provider(diagnostic_options())
                                    .text_document_sync(text_document_sync_kind::full)
                    );
        }

        void publish_all_diagnostics() {
            for (auto &doc : m_workspace.get_changed_documents()) {
                auto diags = m_workspace.get_diagnostics(doc->uri);
                publish_diagnostics(doc->uri.c_str(), diags);
            }
            m_workspace.get_changed_documents().clear();
        }

        void on_text_document_opened(did_open_text_document_params const &open_params) override {
            std::cerr << "opened " << open_params.text_document().uri() << std::endl;
            auto u = open_params.text_document().uri();
            m_workspace.set_dirty(u);
            m_workspace.get_diagnostics(u);
            publish_all_diagnostics();
        }

        void on_text_document_changed(did_change_text_document_params const &changed_params) override {
            std::cerr << "changed " << changed_params.text_document().uri() << std::endl;
            m_workspace.set_dirty(changed_params.text_document().uri());
        }

        void on_text_document_saved(did_save_text_document_params const &saved_params) override {
            std::cerr << "saved " << saved_params.text_document().uri() << std::endl;
            auto u = saved_params.text_document().uri();
            m_workspace.set_dirty(u);
            m_workspace.get_diagnostics(u);
            publish_all_diagnostics();
        }

        std::vector<document_highlight> on_text_document_highlight(text_document_position_params const &) override {
            return {};
        }

        std::vector<folding_range> on_folding_range(folding_range_params const &) override {
            return {};
        }

        completion_list on_completion(completion_params const &params) override {
            auto document = params.text_document().uri();
            auto kind = params.doc_context().trigger_kind();

            std::cerr << "completion " << kind << std::endl;

            return completion_list().is_incomplete(false).items(m_workspace.get_completion_items(document));
        }


        document_diagnostic_report on_diagnostic(document_diagnostic_params const &params) override {
            return document_diagnostic_report().items(m_workspace.get_diagnostics(params.text_document().uri()));
        }


    private:
        bond::Context m_ctx;
        Workspace m_workspace{&m_ctx};
        std::vector<completion_list> m_imports;
        t_string m_current;
        std::deque<t_string> m_changed_files;
    };

}


int main(int32_t argc, char **argv) {
    GC_init();
    auto lib_path = std::filesystem::path(argv[0]).parent_path().string() + "/../libraries/";

    bond::Runtime::ins()->init();
    bond::build_core_module();
    bond::lsp::init_symbols();

    auto server = lsp::BondLsp(lib_path);
    lsp::start_langserver(server, std::cin, std::cout);
    return 0;
}