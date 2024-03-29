/**
 * lsp.hpp
 *
 * @author Peter Lenkefi
 * @date 2018-10-23
 * @description Here are the main Language Server Protocol structures and
 * interfaces to communicate with the client.
 */

#ifndef LSP_HPP
#define LSP_HPP

#include <iostream>
#include "rpc.hpp"

namespace lsp {

struct connection;

struct initialize_params;
struct initialize_result;
struct did_open_text_document_params;
struct did_change_text_document_params;
struct document_highlight;
struct text_document_position_params;
struct did_save_text_document_params;
struct folding_range_params;
struct folding_range;
struct publish_diagnostics_params;
struct diagnostic;
struct completion_params;
struct completion_list;
struct diagnostic_options;
struct document_diagnostic_params;
struct document_diagnostic_report;
/**
 * The interface that the language server object has to implement.
 */
struct langserver {
	virtual initialize_result initialize(initialize_params const&) = 0;
	virtual void on_initialized() { }
	virtual void on_text_document_opened(did_open_text_document_params const&) = 0;
	virtual void on_text_document_changed(did_change_text_document_params const&) = 0;
	virtual void on_text_document_saved(did_save_text_document_params const&) = 0;
	virtual std::vector<document_highlight> on_text_document_highlight(text_document_position_params const&) = 0;
	virtual std::vector<folding_range> on_folding_range(folding_range_params const&) = 0;
    virtual completion_list on_completion(completion_params const&) = 0;
    virtual document_diagnostic_report on_diagnostic(document_diagnostic_params const&) = 0;
	void publish_diagnostics(std::string const& uri, std::vector<diagnostic> const& diags);

private:
	void send_notification(char const* method, json&& p);

	friend struct langserver_handler;

	connection* m_Connection;
};

/**
 * A helper object to send and receive LSP messages.
 */
struct connection {
	explicit connection(std::istream& in, std::ostream& out)
		: m_In(&in), m_Out(&out) {
		global_init();
	}

	void write(rpc::message const& msg);
	rpc::message read();

	auto& in() { return *m_In; }
	auto const& in() const { return *m_In; }

	auto& out() { return *m_Out; }
	auto const& out() const { return *m_Out; }

private:
	struct message_header {
		u32 content_length = 0;
		std::string content_type = "";
	};

	static void global_init();

	bool read_message_header_part(message_header& h);
	message_header read_message_header();

	std::istream* m_In;
	std::ostream* m_Out;
};

/**
 * A wrapper for a language server that dispatches method calls.
 */
struct langserver_handler {
	explicit langserver_handler(std::istream& in, std::ostream& out,
		langserver& ls)
		: m_Connection(in, out), m_Langserver(&ls) {
		m_Langserver->m_Connection = &m_Connection;
	}

	auto& in() { return m_Connection.in(); }
	auto const& in() const { return m_Connection.in(); }

	auto& out() { return m_Connection.out(); }
	auto const& out() const { return m_Connection.out(); }

	void next();

private:
	connection m_Connection;
	langserver* m_Langserver;
	bool m_Initialized = false;
};

/**
 * Starts a language server with an infinite message-loop.
 * @param ls The language server object to use.
 * @param in The input stream to read the messages from.
 * @param out The output stream to write the messages to.
 */
void start_langserver(langserver& ls, std::istream& in, std::ostream& out);

#define ctors(x) 					\
x() = default;						\
x(x const&) = default; 				\
x(x&&) = default; 					\
x& operator=(x const&) = default; 	\
x& operator=(x&&) = default

#define named_mem(type, name) 											\
public:																	\
auto& name() { return m_##name; }										\
auto const& name() const { return m_##name; }							\
template <typename T>													\
auto& name(T&& val) { m_##name = std::forward<T>(val); return *this; }	\
private:																\
type m_##name

/**
 * ResourceOperationKind.
 */
enum class resource_operation_kind {
	create, rename, delete_,
};

/**
 * FailureHandlingKind.
 */
enum class failure_handling_kind {
	abort, transactional, undo, text_only_transactional,
};

/**
 * SymbolKind.
 */
enum class symbol_kind {
	file = 1,
	module = 2,
	namespace_ = 3,
	package = 4,
	class_ = 5,
	method = 6,
	property = 7,
	field = 8,
	constructor = 9,
	enum_ = 10,
	interface = 11,
	function = 12,
	variable = 13,
	constant = 14,
	string = 15,
	number = 16,
	boolean = 17,
	array = 18,
	object = 19,
	key = 20,
	null = 21,
	enum_member = 22,
	struct_ = 23,
	event = 24,
	operator_ = 25,
	type_parameter = 26,
};

/**
 * MarkupKind.
 */
enum class markup_kind {
	plaintext, markdown,
};

/**
 * CompletionItemKind.
 */
enum class completion_item_kind {
	text = 1,
	method = 2,
	function = 3,
	constructor = 4,
	field = 5,
	variable = 6,
	class_ = 7,
	interface = 8,
	module = 9,
	property = 10,
	unit = 11,
	value = 12,
	enum_ = 13,
	keyword = 14,
	snippet = 15,
	color = 16,
	file = 17,
	reference = 18,
	folder = 19,
	enum_member = 20,
	constant = 21,
	struct_ = 22,
	event = 23,
	operator_ = 24,
	type_parameter = 25,
};

/**
 * DocumentHighlightKind.
 */
enum class document_highlight_kind {
	text = 1,
	read = 2,
	write = 3,
};

/**
 * TextDocumentSyncKind.
 */
enum class text_document_sync_kind {
	none = 0,
	full = 1,
	incremental = 2,
};

/**
 * FoldingRangeKind.
 */
enum class folding_range_kind {
	comment,
	imports,
	region,
};

/**
 * DiagnosticSeverity.
 */
enum class diagnostic_severity {
	error = 1,
	warning = 2,
	information = 3,
	hint = 4,
};

/**
 * A WorkspaceFolder.
 */
struct workspace_folder {
	ctors(workspace_folder);

	static workspace_folder from_json(json const& js);

	named_mem(std::string, uri);
	named_mem(std::string, name);
};

/**
 * WorkspaceClientCapabilities.
 */
struct workspace_client_capabilities {
	/**
	 * WorkspaceEdit.
	 */
	struct workspace_edit_t {
		ctors(workspace_edit_t);

		static workspace_edit_t from_json(json const& js);

		named_mem(bool, document_changes) = false;
		named_mem(std::vector<resource_operation_kind>, resource_operations);
		named_mem(failure_handling_kind, failure_handling) = failure_handling_kind::abort;
	};

	/**
	 * DidChangeConfiguration.
	 */
	struct did_change_configuration_t {
		ctors(did_change_configuration_t);

		static did_change_configuration_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * DidChangeWatchedFiles.
	 */
	struct did_change_watched_files_t {
		ctors(did_change_watched_files_t);

		static did_change_watched_files_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * Symbol.
	 */
	struct symbol_t {
		/**
		 * SymbolKind.
		 */
		struct symbol_kind_t {
			ctors(symbol_kind_t);

			static symbol_kind_t from_json(json const& js);

			named_mem(std::vector<symbol_kind>, value_set);
		};

		ctors(symbol_t);

		static symbol_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
		named_mem(std::optional<symbol_kind_t>, symbol_kind) = std::nullopt;
	};

	/**
	 * ExecuteCommand.
	 */
	struct execute_command_t {
		ctors(execute_command_t);

		static execute_command_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	ctors(workspace_client_capabilities);

	static workspace_client_capabilities from_json(json const& js);

	named_mem(bool, apply_edit) = false;
	named_mem(std::optional<workspace_edit_t>, workspace_edit) = std::nullopt;
	named_mem(std::optional<did_change_configuration_t>, did_change_configuration) = std::nullopt;
	named_mem(std::optional<did_change_watched_files_t>, did_change_watched_files) = std::nullopt;
	named_mem(std::optional<symbol_t>, symbol) = std::nullopt;
	named_mem(std::optional<execute_command_t>, execute_command) = std::nullopt;
	named_mem(bool, workspace_folders) = false;
	named_mem(bool, configuration) = false;
};

/**
 * TextDocumentClientCapabilities.
 */
struct text_document_client_capabilities {
	/**
	 * Synchronization.
	 */
	struct synchronization_t {
		ctors(synchronization_t);

		static synchronization_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
		named_mem(bool, will_save) = false;
		named_mem(bool, will_save_wait_until) = false;
		named_mem(bool, did_save) = false;
	};

	/**
	 * Completion.
	 */
	struct completion_t {
		/**
	 	* CompletionItem.
		 */
		struct completion_item_t {
			ctors(completion_item_t);

			static completion_item_t from_json(json const& js);

			named_mem(bool, snippet_support) = false;
			named_mem(bool, commit_characters_support) = false;
			named_mem(std::vector<markup_kind>, documentation_format);
			named_mem(bool, deprecated_support) = false;
			named_mem(bool, preselect_support) = false;
		};

		/**
		 * CompletionItemKind.
		 */
		struct completion_item_kind_t {
			ctors(completion_item_kind_t);

			static completion_item_kind_t from_json(json const& js);

			named_mem(std::vector<completion_item_kind>, value_set);
		};

		ctors(completion_t);

		static completion_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
		named_mem(std::optional<completion_item_t>, completion_item) = std::nullopt;
		named_mem(std::optional<completion_item_kind_t>, completion_item_kind) = std::nullopt;
		named_mem(bool, context_support) = false;
	};

	/**
	 * Hover.
	 */
	struct hover_t {
		ctors(hover_t);

		static hover_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
		named_mem(std::vector<markup_kind>, content_format);
	};

	/**
	 * SignatureHelp.
	 */
	struct signature_help_t {
		/**
		 * SignatureInformation.
		 */
		struct signature_information_t {
			ctors(signature_information_t);

			static signature_information_t from_json(json const& js);

			named_mem(std::vector<markup_kind>, documentation_format);
		};

		ctors(signature_help_t);

		static signature_help_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
		named_mem(std::optional<signature_information_t>, signature_information) = std::nullopt;
	};

	/**
	 * References.
	 */
	struct references_t {
		ctors(references_t);

		static references_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * DocumentHighlight.
	 */
	struct document_highlight_t {
		ctors(document_highlight_t);

		static document_highlight_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * DocumentSymbol.
	 */
	struct document_symbol_t {
		/**
		 * symbolKindT.
		 */
		struct symbol_kind_t {
			ctors(symbol_kind_t);

			static symbol_kind_t from_json(json const& js);

			named_mem(std::vector<symbol_kind>, value_set);
		};

		ctors(document_symbol_t);

		static document_symbol_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
		named_mem(std::optional<symbol_kind_t>, symbol_kind) = std::nullopt;
		named_mem(bool, hierarchical_document_symbol_support) = false;
	};

	/**
	 * Formatting.
	 */
	struct formatting_t {
		ctors(formatting_t);

		static formatting_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * RangeFormatting.
	 */
	struct range_formatting_t {
		ctors(range_formatting_t);

		static range_formatting_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * OnTypeFormatting.
	 */
	struct on_type_formatting_t {
		ctors(on_type_formatting_t);

		static on_type_formatting_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * Definition.
	 */
	struct definition_t {
		ctors(definition_t);

		static definition_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * TypeDefinition.
	 */
	struct type_definition_t {
		ctors(type_definition_t);

		static type_definition_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * Implementation.
	 */
	struct implementation_t {
		ctors(implementation_t);

		static implementation_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * CodeAction.
	 */
	struct code_action_t {
		/**
		 * CodeActionLiteralSupport.
		 */
		struct code_action_literal_support_t {
			/**
			 * CcodeActionKind.
			 */
			struct code_action_kind_t {
				ctors(code_action_kind_t);

				static code_action_kind_t from_json(json const& js);

				named_mem(std::vector<std::string>, value_set);
			};

			ctors(code_action_literal_support_t);

			static code_action_literal_support_t from_json(json const& js);

			named_mem(code_action_kind_t, code_action_kind);
		};

		ctors(code_action_t);

		static code_action_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
		named_mem(std::optional<code_action_literal_support_t>, code_action_literal_support) = std::nullopt;
	};

	/**
	 * CodeLens.
	 */
	struct code_lens_t {
		ctors(code_lens_t);

		static code_lens_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * DocumentLink.
	 */
	struct document_link_t {
		ctors(document_link_t);

		static document_link_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * ColorProvider.
	 */
	struct color_provider_t {
		ctors(color_provider_t);

		static color_provider_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
	};

	/**
	 * Rename.
	 */
	struct rename_t {
		ctors(rename_t);

		static rename_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
		named_mem(bool, prepare_support) = false;
	};

	/**
	 * PublishDiagnostics.
	 */
	struct publish_diagnostics_t {
		ctors(publish_diagnostics_t);

		static publish_diagnostics_t from_json(json const& js);

		named_mem(bool, related_information) = false;
	};

	/**
	 * foldingRangeT.
	 */
	struct folding_range_t {
		ctors(folding_range_t);

		static folding_range_t from_json(json const& js);

		named_mem(bool, dynamic_registration) = false;
		named_mem(std::optional<i32>, range_limit) = std::nullopt;
		named_mem(bool, line_folding_only) = true;
	};

	ctors(text_document_client_capabilities);

	static text_document_client_capabilities from_json(json const& js);

	named_mem(std::optional<synchronization_t>, synchronization) = std::nullopt;
	named_mem(std::optional<completion_t>, completion) = std::nullopt;
	named_mem(std::optional<hover_t>, hover) = std::nullopt;
	named_mem(std::optional<signature_help_t>, signature_help) = std::nullopt;
	named_mem(std::optional<references_t>, references) = std::nullopt;
	named_mem(std::optional<document_highlight_t>, document_highlight) = std::nullopt;
	named_mem(std::optional<document_symbol_t>, document_symbol) = std::nullopt;
	named_mem(std::optional<formatting_t>, formatting) = std::nullopt;
	named_mem(std::optional<range_formatting_t>, range_formatting) = std::nullopt;
	named_mem(std::optional<on_type_formatting_t>, on_type_formatting) = std::nullopt;
	named_mem(std::optional<definition_t>, definition) = std::nullopt;
	named_mem(std::optional<type_definition_t>, type_definition) = std::nullopt;
	named_mem(std::optional<implementation_t>, implementation) = std::nullopt;
	named_mem(std::optional<code_action_t>, code_action) = std::nullopt;
	named_mem(std::optional<code_lens_t>, code_lens) = std::nullopt;
	named_mem(std::optional<document_link_t>, document_link) = std::nullopt;
	named_mem(std::optional<color_provider_t>, color_provider) = std::nullopt;
	named_mem(std::optional<rename_t>, rename) = std::nullopt;
	named_mem(std::optional<publish_diagnostics_t>, publish_diagnostics) = std::nullopt;
	named_mem(std::optional<folding_range_t>, folding_range) = std::nullopt;
};

/**
 * ClientCapabilities.
 */
struct client_capabilities {
	ctors(client_capabilities);

	static client_capabilities from_json(json const& js);

	named_mem(std::optional<workspace_client_capabilities>, workspace) = std::nullopt;
	named_mem(std::optional<text_document_client_capabilities>, text_document) = std::nullopt;
	named_mem(std::optional<json>, experimental) = std::nullopt;
};

/**
 * The parameter supplied at the 'initialize' Request, called InitializeParams.
 */
struct initialize_params {
	enum class trace_t {
		off, messages, verbose,
	};

	ctors(initialize_params);

	static initialize_params from_json(json const& js);

	std::optional<std::string> const& root() const;

	named_mem(std::optional<i32>, process_id) = std::nullopt;
	named_mem(std::optional<std::string>, root_path) = std::nullopt;
	named_mem(std::optional<std::string>, root_uri) = std::nullopt;
	named_mem(json, initialization_options) = nullptr;
	named_mem(client_capabilities, capabilities);
	named_mem(trace_t, trace) = trace_t::off;
	named_mem(std::optional<std::vector<workspace_folder>>, workspace_folders) = std::nullopt;
};

/**
 * SaveOptions.
 */
struct save_options {
	ctors(save_options);

	json to_json() const;

	named_mem(bool, include_text) = false;
};

/**
 * TextDocumentSyncOptions.
 */
struct text_document_sync_options {
	ctors(text_document_sync_options);

	json to_json() const;

	named_mem(bool, open_close) = false;
	named_mem(text_document_sync_kind, change) = text_document_sync_kind::none;
	named_mem(bool, will_save) = false;
	named_mem(bool, will_save_wait_until) = false;
	named_mem(save_options, save);
};

/**
 * CompletionOptions.
 */
struct completion_options {
	ctors(completion_options);

	json to_json() const;

	named_mem(bool, resolve_provider) = false;
	named_mem(std::vector<char>, trigger_characters);
};

/**
 * SignatureHelpOptions.
 */
struct signature_help_options {
	ctors(signature_help_options);

	json to_json() const;

	named_mem(std::vector<char>, trigger_characters);
};

/**
 * DocumentFilter.
 */
struct document_filter {
	ctors(document_filter);

	json to_json() const;

	named_mem(std::optional<std::string>, language) = std::nullopt;
	named_mem(std::optional<std::string>, scheme) = std::nullopt;
	named_mem(std::optional<std::string>, pattern) = std::nullopt;
};

/**
 * TextDocumentRegistrationOptions.
 */
struct text_document_registration_options {
	ctors(text_document_registration_options);

	json to_json() const;

	named_mem(std::optional<std::vector<document_filter>>, document_selector) = std::nullopt;
};

/**
 * StaticRegistrationOptions.
 */
struct static_registration_options {
	ctors(static_registration_options);

	json to_json() const;

	named_mem(std::optional<std::string>, id) = std::nullopt;
};

/**
 * CodeActionOptions.
 */
struct code_action_options {
	ctors(code_action_options);

	json to_json() const;

	named_mem(std::vector<std::string>, code_action_kinds);
};

/**
 * CodeLensOptions.
 */
struct code_lens_options {
	ctors(code_lens_options);

	json to_json() const;

	named_mem(bool, resolve_provider) = false;
};

/**
 * DocumentOnTypeFormattingOptions.
 */
struct document_on_type_formatting_options {
	ctors(document_on_type_formatting_options);

	json to_json() const;

	named_mem(char, first_trigger_character);
	named_mem(std::vector<char>, more_trigger_character);
};

/**
 * RenameOptions.
 */
struct rename_options {
	ctors(rename_options);

	json to_json() const;

	named_mem(bool, prepare_provider) = false;
};

/**
 * DocumentLinkOptions.
 */
struct document_link_options {
	ctors(document_link_options);

	json to_json() const;

	named_mem(bool, resolve_provider) = false;
};

/**
 * ColorProviderOptions.
 */
struct color_provider_options {
	ctors(color_provider_options);

	json to_json() const;
};

/**
 * FoldingRangeProviderOptions.
 */
struct folding_range_provider_options {
	ctors(folding_range_provider_options);

	json to_json() const;
};

/**
 * ExecuteCommandOptions.
 */
struct execute_command_options {
	ctors(execute_command_options);

	json to_json() const;

	named_mem(std::vector<std::string>, commands);
};

struct diagnostic_options {
    ctors(diagnostic_options);

    json to_json() const;

    named_mem(std::string, identifier) = "bond language server";
    named_mem(bool, interFileDependencies) = false;
    named_mem(bool, workspaceDiagnostic) = false;
};

/**
 * ServerCapabilities.
 */
struct server_capabilities {
	/**
	 * Workspace.
	 */
	struct workspace_t {
		/**
		 * WorkspaceFolders.
		 */

        struct workspace_folders_t {
			using change_notifications_t = std::variant<bool, std::string>;

			ctors(workspace_folders_t);

			json to_json() const;

			named_mem(bool, supported) = false;
			named_mem(change_notifications_t, change_notifications) = false;
		};

		ctors(workspace_t);

		json to_json() const;

		named_mem(workspace_folders_t, workspace_folders);
	};

	using text_document_sync_t = std::variant<text_document_sync_options, text_document_sync_kind>;
	using type_definition_provider_t = std::variant<bool, std::tuple<text_document_registration_options, static_registration_options>>;
	using implementation_provider_t = std::variant<bool, std::tuple<text_document_registration_options, static_registration_options>>;
	using code_action_provider_t = std::variant<bool, code_action_options>;
	using rename_provider_t = std::variant<bool, rename_options>;
	using color_provider_t = std::variant<
		bool,
		color_provider_options,
		std::tuple<color_provider_options, text_document_registration_options, static_registration_options>
	>;
	using folding_range_provider_t = std::variant<
		bool,
		folding_range_provider_options,
		std::tuple<folding_range_provider_options, text_document_registration_options, static_registration_options>
	>;

	ctors(server_capabilities);

	json to_json() const;

	named_mem(text_document_sync_t, text_document_sync) = text_document_sync_kind::none;
	named_mem(bool, hover_provider) = false;
	named_mem(std::optional<completion_options>, completion_provider) = std::nullopt;
	named_mem(std::optional<signature_help_options>, signature_help_provider) = std::nullopt;
	named_mem(bool, definition_provider) = false;
	named_mem(type_definition_provider_t, type_definition_provider) = false;
	named_mem(implementation_provider_t, implementation_provider) = false;
	named_mem(bool, references_provider) = false;
	named_mem(bool, document_highlight_provider) = false;
	named_mem(bool, document_symbol_provider) = false;
	named_mem(bool, workspace_symbol_provider) = false;
	named_mem(code_action_provider_t, code_action_provider) = false;
	named_mem(std::optional<code_lens_options>, code_lens_provider) = std::nullopt;
	named_mem(bool, document_formatting_provider) = false;
	named_mem(bool, document_range_formatting_provider) = false;
	named_mem(std::optional<document_on_type_formatting_options>, document_on_type_formatting_provider) = std::nullopt;
	named_mem(rename_provider_t, rename_provider) = false;
	named_mem(std::optional<document_link_options>, document_link_provider) = std::nullopt;
	named_mem(color_provider_t, color_provider) = false;
	named_mem(folding_range_provider_t, folding_range_provider) = false;
	named_mem(std::optional<execute_command_options>, execute_command_provider) = std::nullopt;
	named_mem(workspace_t, workspace);
    named_mem(std::optional<diagnostic_options>, diagnostic_provider) = std::nullopt;
	named_mem(std::optional<json>, experimental) = std::nullopt;
};


/**
 * InitializeResult.
 */
struct initialize_result {
	ctors(initialize_result);

	json to_json() const;

	named_mem(server_capabilities, capabilities);
};

/**
 * TextDocumentItem.
 */
struct text_document_item {
	ctors(text_document_item);

	static text_document_item from_json(json const& js);

	named_mem(std::string, uri);
	named_mem(std::string, language_id);
	named_mem(i32, version);
	named_mem(std::string, text);
};

/**
 * DidOpenTextDocumentParams.
 */
struct did_open_text_document_params {
	ctors(did_open_text_document_params);

	static did_open_text_document_params from_json(json const& js);

	named_mem(text_document_item, text_document);
};

/**
 * TextDocumentIdentifier.
 */
struct text_document_identifier {
	ctors(text_document_identifier);

	static text_document_identifier from_json(json const& js);

	named_mem(std::string, uri);
	named_mem(std::optional<i32>, version) = std::nullopt;
};

/**
 * Position.
 */
struct position {
	ctors(position);

	position(i32 ln, i32 ch);

	static position from_json(json const& js);

	json to_json() const;

	named_mem(i32, line);
	named_mem(i32, character);
};

/**
 * Range.
 */
struct range {
	ctors(range);

	range(position const& st, position const& en);

	static range from_json(json const& js);

	json to_json() const;

	named_mem(position, start);
	named_mem(position, end);
};

/**
 * TextDocumentContentChangeEvent.
 */
struct text_document_content_change_event {
	ctors(text_document_content_change_event);

	static text_document_content_change_event from_json(json const& js);

	bool full_content() const;

	named_mem(std::optional<range>, change_range) = std::nullopt;
	named_mem(i32, range_length) = 0;
	named_mem(std::string, text);
};

/**
 * DidChangeTextDocumentParams.
 */
struct did_change_text_document_params {
	ctors(did_change_text_document_params);

	static did_change_text_document_params from_json(json const& js);

	named_mem(text_document_identifier, text_document);
	named_mem(std::vector<text_document_content_change_event>, content_changes);
};

/**
 * TextDocumentPositionParams.
 */
struct text_document_position_params {
	ctors(text_document_position_params);

	static text_document_position_params from_json(json const& js);

	named_mem(text_document_identifier, text_document);
	named_mem(position, document_position);
};

/**
 * DocumentHighlight.
 */
struct document_highlight {
	ctors(document_highlight);

	json to_json() const;

	named_mem(range, highlight_range);
	named_mem(document_highlight_kind, kind) = document_highlight_kind::text;
};

/**
 * DidSaveTextDocumentParams.
 */
struct did_save_text_document_params {
	ctors(did_save_text_document_params);

	static did_save_text_document_params from_json(json const& js);

	named_mem(text_document_identifier, text_document);
	named_mem(std::optional<std::string>, text) = std::nullopt;
};

/**
 * FoldingRangeParams.
 */
struct folding_range_params {
	ctors(folding_range_params);

	static folding_range_params from_json(json const& js);

	named_mem(text_document_identifier, text_document);
};

/**
 * FoldingRange.
 */
struct folding_range {
	ctors(folding_range);

	json to_json() const;

	folding_range& start(position const& pos);
	folding_range& end(position const& pos);

	folding_range& fold_range(range const& r);

	named_mem(i32, start_line);
	named_mem(std::optional<i32>, start_character) = std::nullopt;
	named_mem(i32, end_line);
	named_mem(std::optional<i32>, end_character) = std::nullopt;
	named_mem(std::optional<folding_range_kind>, kind) = std::nullopt;
};

/**
 * Location.
 */
struct location {
	ctors(location);

	json to_json() const;

	named_mem(std::string, uri);
	named_mem(range, location_range);
};

/**
 * DiagnosticRelatedInformation.
 */
struct diagnostic_related_information {
	ctors(diagnostic_related_information);

	json to_json() const;

	named_mem(location, info_location);
	named_mem(std::string, message);
};

/**
 * Diagnostic.
 */
struct diagnostic {
	using code_t = std::variant<i32, std::string>;

	ctors(diagnostic);

	json to_json() const;

	named_mem(range, diagnostic_range);
	named_mem(std::optional<diagnostic_severity>, severity) = std::nullopt;
	named_mem(std::optional<code_t>, code) = std::nullopt;
	named_mem(std::optional<std::string>, source) = std::nullopt;
	named_mem(std::string, message);
	named_mem(std::vector<diagnostic_related_information>, related_information);
};

/**
 * PublishDiagnosticsParams.
 */
struct publish_diagnostics_params {
	ctors(publish_diagnostics_params);

	json to_json() const;

	named_mem(std::string, uri);
	named_mem(std::vector<diagnostic>, diagnostics);
};


struct context {
    ctors(context);

    static context from_json(json const& js);
    named_mem(i32, trigger_kind);
};

struct completion_params {
    ctors(completion_params);

    static completion_params from_json(json const& js);

    named_mem(context, doc_context);
    named_mem(position, doc_position);
    named_mem(text_document_identifier, text_document);

};



struct completion_item {
    ctors(completion_item);

    json to_json() const;
    named_mem(std::string, label);
    named_mem(std::optional<completion_item_kind>, kind) = std::nullopt;
    named_mem(std::optional<std::string>, detail) = std::nullopt;
    named_mem(std::optional<std::string>, documentation) = std::nullopt;
    named_mem(std::optional<std::string>, insertText) = std::nullopt;
    named_mem(std::optional<std::string>, sortText) = std::nullopt;
};


struct completion_list {
    ctors(completion_list);

    json to_json() const;

    named_mem(bool, is_incomplete);
    named_mem(std::vector<completion_item>, items);
};


struct document_diagnostic_params {
    ctors(document_diagnostic_params);

    static document_diagnostic_params from_json(json const& js);

    named_mem(text_document_identifier, text_document);
    named_mem(std::optional<std::string>, identifier) = std::nullopt;
    named_mem(std::optional<std::string>, previous_result_id) = std::nullopt;
};


struct document_diagnostic_report {
    ctors(document_diagnostic_report);

    json to_json() const;

    named_mem(std::vector<diagnostic>, items);
    named_mem(std::string, kind) = "full";
    named_mem(std::optional<std::string>, result_id) = std::nullopt;
};

#undef named_mem
#undef ctors

} /* namespace lsp */

#endif /* LSP_HPP */
