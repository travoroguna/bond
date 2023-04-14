//
// Created by Travor Oguna Oneya on 09/04/2023.
//

#include "rpc.h"
#include <fstream>
#include <iostream>


namespace bond {
    void Rpc::start() {
        while (!m_stop) {
            read_message();
        }
    }

    //structure
    // Content-Length: 0\r\n
    // Content-Type: application/vscode-jsonrpc; charset=utf8\r\n
    //data
    // {...}


    std::string Rpc::read_until_separator() {
        std::string buff;

        while (char c = getchar()) {
            if (c == '\n') {
                c = getchar();
                break;
            }
            log_ch(std::string(1, c));
        }

        return buff;
    }

    std::string Rpc::read_to_end(size_t length) {
        std::string buff;

        for (size_t i = 0; i < length; i++) {
            char c = getchar();
            log_ch(std::string(1, c));
            buff += c;

        }
        return buff;
    }

    uint32_t Rpc::get_content_length() {
        std::string buff = read_until_separator();
        std::string length = buff.substr(16);
        return std::stoi(length);
    }

    std::string Rpc::get_content_type() {
        std::string buff = read_until_separator();
        return buff.substr(14);
    }

    void Rpc::log_message(const std::string &message) {
        if (message.empty()) {
            return;
        }
        std::ofstream file(R"(D:\dev\cpp\bond\examples\testing)", std::ios::app);
        file << message << std::endl;
        file.close();
    }

    void Rpc::log_ch(const std::string &message) {
        if (message.empty()) {
            return;
        }
        std::ofstream file(R"(D:\dev\cpp\bond\examples\testing2)", std::ios::app);
        file << message;
        file.close();
    }

    void Rpc::read_message() {
        log_message("reading message ...\n");

        auto length = get_content_length();
        auto message = read_to_end(length);

        log_message(message);

        json j = json::parse(message);
        auto request = Request(j);

        if (m_handlers.contains(request.get_method())) {
            auto response = m_handlers[request.get_method()](request);
            std::cout << response.to_json().dump();
            log_message(response.to_json().dump());
            std::cout.flush();
        } else {
            json data;
            data["error"] = "Method not found";
            auto error = Error(ErrorCode::MethodNotFound, "Method not found", data);
            std::cout << "Content-Length: " << error.to_json().dump().length() << "\r\n";
            std::cout << "Content-Type: application/vscode-jsonrpc; charset=utf8\r\n";
            std::cout << error.to_json().dump() << "\n";
            log_message(error.to_json().dump());
            std::cout.flush();
        }
    }
}
