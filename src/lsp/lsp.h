#pragma once

#include "jsonrpcpp.hpp"
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

namespace bond {
    std::string read_stdin();

    class Lsp {
    public:
        Lsp() = default;

        void init();

        static void log(const std::string &message);

        [[noreturn]] void run();

    private:
        std::string root_path;


        jsonrpcpp::Parser parser;

        jsonrpcpp::response_ptr initialize(const jsonrpcpp::Id &id, const jsonrpcpp::Parameter &params);

        static void write(const std::string &message);
    };
}