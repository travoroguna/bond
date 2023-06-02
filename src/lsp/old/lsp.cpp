//
// Created by Travor Oguna Oneya on 06/04/2023.
//

#include "lsp.h"
#include "message_buffer.hpp"
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <optional>


#if defined(_WIN32) || defined(_WIN64)
#include <fcntl.h>
#include <io.h>

void platform_init() {
    auto res = _setmode(_fileno(stdin), _O_BINARY);
    assert(res != -1);
    res = _setmode(_fileno(stdout), _O_BINARY);
    assert(res != -1);
}
#else
void platform_init() { }
#endif

namespace bond {

//    Content-Length: ...\r\n
//    \r\n
//{
//    "jsonrpc": "2.0",
//    "id": 1,
//    "method": "textDocument/completion",
//    "params": {
//      ...
//    }
//}

    void Lsp::init() {
        auto file = std::ofstream(R"(D:\dev\cpp\bond\tools\vscode-bond\bondlsp.log)");
        file << "";
        file.close();
        platform_init();
    }

    std::string Lsp::initialize(const Json& body) {
        Json response;
//        response["capabilities"]["completionProvider"]["resolveProvider"] = true;
        response["capabilities"]["completionProvider"]["triggerCharacters"] = { "." };
//        response["capabilities"]["positionEncoding"] = "utf-8";
        return make_response(response);
    }

    std::optional<std::string> Lsp::handle_message(const Json& body) {
        log(fmt::format("Handling message: {}", body["method"]);

        if (body["method"] == "initialize") {
            return initialize(body);
        }
        if (body["method"] == "initialized") {
            return std::nullopt;
        }

        log(fmt::format("Unknown method: {}", body["method"]));

        return std::nullopt;
    }

    [[noreturn]] void Lsp::run() {
        init();
        log("Bond Language Server Protocol started\n");

        char c;
        MessageBuffer buffer;
        bool second_read = false;

        while (true) {
            log("Waiting for input\n");
            bool new_input = false;

            while (std::cin.get(c)) {
                if (!new_input) {
                    log("New input\n");
                    new_input = true;
                }

                buffer.handle_char(c);

                if (buffer.message_completed()) {
                    log(buffer.raw());
                    Json body = buffer.body();
                    log(body.dump(4));

                    auto message = handle_message(body);

                    if (message.has_value()) {
                        write(message.value());
                    }

                    log("Clearing buffer\n");
                    buffer.clear();
                    log("Buffer cleared\n");
                    new_input = false;
                    second_read = true;
                }

                if (second_read) {
                    log(fmt::format("char {}", c));
                }
            }

            log("End of input\n");
        }
    }

    void Lsp::log(const std::string &message) {
        auto file = std::ofstream(R"(D:\dev\cpp\bond\tools\vscode-bond\bondlsp.log)", std::ios::app);
        file << message << std::endl;
        file.close();
    }

    std::string Lsp::make_response(const Json& response)
    {
        Json content = response;
        content["jsonrpc"] = "2.0";

        std::string header;
        header.append("Content-Length: " + std::to_string(content.dump().size()) + "\r\n");
        header.append("Content-Type: application/vscode-jsonrpc;charset=utf-8\r\n");
        header.append("\r\n");
        return header + content.dump();
    }

    void Lsp::write(const std::string &message) {
        log(fmt::format("Writing message: {}", message));
        std::cout << message << std::flush;
    }
};