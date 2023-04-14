//
// Created by Travor Oguna Oneya on 09/04/2023.
//

#pragma once

#include <nlohmann/json.hpp>
#include <concepts>
#include <utility>
#include <variant>
#include <functional>
#include <unordered_map>

using json = nlohmann::json;


namespace bond {
    class Jsonify {
    public:
        Jsonify() = default;

        virtual ~Jsonify() = default;

        virtual json to_json() = 0;

    };


    class Request {
    public:
        Request(json &request) {
            m_id = request["id"];
            m_method = request["method"];
            m_params = request["params"];
        }

        int get_id() { return m_id; }

        std::string get_method() { return m_method; }

        json get_params() { return m_params; }

    private:
        int m_id;
        std::string m_method;
        json m_params;
    };

    enum class ErrorCode : int32_t {
        ParseError = -32700,
        InvalidRequest = -32600,
        MethodNotFound = -32601,
        InvalidParams = -32602,
        InternalError = -32603,
        ServerErrorStart = -32099,
        ServerErrorEnd = -32000,
        ServerNotInitialized = -32002,
        UnknownErrorCode = -32001,
        RequestCancelled = -32800,
        ContentModified = -32801
    };

    class Error : public Jsonify {
    public:
        Error(ErrorCode code, const std::string &message, json &data) {
            m_code = code;
            m_message = message;
            m_data = data;
        }

        ErrorCode get_code() { return m_code; }

        std::string get_message() { return m_message; }

        json get_data() { return m_data; }

        json to_json() override {
            json j;
            j["code"] = static_cast<int32_t>(m_code);
            j["message"] = m_message;
            j["data"] = m_data;
            return j;
        }

    private:
        ErrorCode m_code;
        std::string m_message;
        json m_data;
    };

    template<class T>
    class Response : public Jsonify {
    public:
        Response(int id, T result) {
            m_id = id;
            m_result = result;
        }

        Response(int id, const Error &error) {
            m_id = id;
            m_result = error;
        }

        ~Response() {
            if (std::holds_alternative<T>(m_result)) {
                delete std::get<T>(m_result);
            }
        }

        int get_id() { return m_id; }

        std::string get_method() { return m_method; }

        json get_params() { return m_params; }

        json to_json() override {
            json j;
            j["id"] = m_id;
            if (std::holds_alternative<Error>(m_result)) {
                j["error"] = std::get<Error>(m_result).to_json();
            } else {
                j["result"] = std::get<T>(m_result)->to_json();
            }
            return j;
        }

    private:
        int m_id;
        std::string m_method;
        json m_params;
        std::variant<T, Error> m_result;
    };


    class Rpc {
    public:
        Rpc() = default;

        void start();

        void register_method(const std::string &method, std::function<Response<Jsonify *>(Request &)> handler) {
            m_handlers[method] = std::move(handler);
        }

        static void log_message(const std::string &message);

    private:
        bool m_stop = false;

        void read_message();

        std::unordered_map<std::string, std::function<Response<Jsonify *>(Request &)>> m_handlers;

        static std::string read_until_separator();

        std::string read_to_end();

        uint32_t get_content_length();

        std::string get_content_type();

        std::string read_to_end(size_t length);

        static void log_ch(const std::string &message);
    };

};
