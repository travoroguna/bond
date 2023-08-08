const vscode = require("vscode");
const vscode_languageclient = require("vscode-languageclient");

class FailHandler {
	error(_error, _message, count) {
		return ErrorAction.Shutdown;
	}

	closed() {
		vscode.window.showErrorMessage("The Bond language server has crashed");
		return CloseAction.DoNotRestart;
	}
}

/**
 * @param {vscode.ExtensionContext} context
 */
function activate(context) {
	console.log('Congratulations, your extension "bond-lsp" is now active!');

	var config = vscode.workspace.getConfiguration("bond");

	var binaryPath = config.get("bondExecutablePath");
	if (!binaryPath) {
		vscode.window.showErrorMessage(
			"Could not start bond language server due to missing setting: bond.bondExecutablePath"
		);
		return;
	}

	var failFast = !!config.get("failFast") || false;

	var serverOptions = {
		command: binaryPath,
		options: { shell: true },
		transport: vscode_languageclient.TransportKind.stdio,
	};

	var clientOptions = {
		documentSelector: [{ scheme: "file", language: "bond" }],
		errorHandler: failFast ? new FailFastErrorHandler() : null,
	};

	var client = new vscode_languageclient.LanguageClient(
		"bondLanguageServer",
		"bond language server",
		serverOptions,
		clientOptions
	);
	client.trace = vscode_languageclient.Trace.Verbose;
	// context.subscriptions.push(client.start());
	client.start();
}

function deactivate() {}

module.exports = {
	activate,
	deactivate,
};
