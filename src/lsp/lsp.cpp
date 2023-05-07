//
// Created by Travor Oguna Oneya on 06/04/2023.
//

#include "lsp.h"
#include <fmt/core.h>
#include <fstream>
#include <iostream>


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


    std::string read_stdin() {
        std::string source;
        while (char c = getchar()) {
            if (c == '\n') {
                break;
            }
            source += c;
//            auto file = std::ofstream(R"(D:\dev\cpp\bond\tools\vscode-bond\bondlsp.log)", std::ios::app);
//            file << c;
//            file.close();
        }
        auto content_length = source.substr(16);
        auto length = std::stoi(content_length);
        getchar();

        std::string read;
        for (int i = 0; i < length; i++) {
            read += getchar();
        }

        return read;
    }

    void Lsp::init() {
        auto file = std::ofstream(R"(D:\dev\cpp\bond\tools\vscode-bond\bondlsp.log)");
        file << "";
        file.close();

        parser.register_request_callback("initialize", [this](auto &&PH1, auto &&PH2) {
            return initialize(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
        });
    }

    jsonrpcpp::response_ptr Lsp::initialize(const jsonrpcpp::Id &id, const jsonrpcpp::Parameter &params) {
        log(fmt::format("Request ->> : {}\n", params.to_json().dump()));

        root_path = params.get("rootPath").get<std::string>();
        log(fmt::format("root path: {}\n", root_path));

        auto initialize_response = R"({"capabilities": {"completionProvider": {"resolveProvider": true,"triggerCharacters": ["."]},"positionEncoding": "utf-8"}})";


        auto response = std::make_shared<jsonrpcpp::Response>(id, initialize_response);
        log(response->to_json().dump());
        auto as_string = response->to_json().dump();
        write(R"({"capabilities": {"completionProvider": {"resolveProvider": true,"triggerCharacters": ["."]},"positionEncoding": "utf-8"}})");
        return response;
    }

    [[noreturn]] void Lsp::run() {
        log("Bond Language Server Protocol started\n");
        while (true) {
            auto read = read_stdin();

            log(fmt::format("source: {}\n", read));
            try {
                auto entity = parser.parse(read);

                if (!entity) {
                    continue;
                }

                if (entity->is_request()) {
                    auto request = dynamic_pointer_cast<jsonrpcpp::Request>(entity);
                    log(fmt::format("Request: {}\n", request->to_json().dump()));
                }
            }
            catch (const std::exception &e) {
                log(fmt::format("source: {}\n, Error: {}\n", read, e.what()));
            }

        }
    }

    void Lsp::log(const std::string &message) {
        auto file = std::ofstream(R"(D:\dev\cpp\bond\tools\vscode-bond\bondlsp.log)", std::ios::app);
        file << message << std::endl;
        file.close();
    }

    void Lsp::write(const std::string &message) {
        auto response = fmt::format(
                "Content-Length: {}\r\nContent-Type: application/vscode-jsonrpc; charset=utf-8\r\n\r\n{}\n",
                message.size(), message);
        log(response);
        std::cout << response << std::flush;
    }
};