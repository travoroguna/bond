#pragma once

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
        std::string initialize(const Json &body);
        static void write(const std::string &message);
        std::string make_response(const Json &response);

        std::optional<std::string> handle_message(const Json &body);
    };
}