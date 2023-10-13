//
// Created by travor on 14/09/2023.
//

#include "../../bond.h"
#include <httplib.h>

#include <utility>

namespace bond::http {

    // Exceptions why
    class InternalServerError final : public std::exception {
    public:
        InternalServerError() = default;

        [[nodiscard]] const char *what() const noexcept override {
            return "Internal Server Error";
        }
    };

    void server_error() {
        get_current_vm()->reset_error();
        throw InternalServerError();
    }

    using Callback = std::function<void(Vm *)>;
    constexpr bool use_pool = false;

    class HttpRuntime {
    public:
        explicit HttpRuntime(Context *ctx) : m_ctx(ctx) {}

        void init() {
            m_vm_pool.reserve(30);

            for (int i = 0; i < 30; i++) {
                m_vm_pool.emplace_back(new(GC) Vm(m_ctx));
            }
        }


        void exec(Callback callback) {
            if constexpr (use_pool) {
                std::lock_guard<std::mutex> lock(m_mutex);

                m_callbacks.emplace_back([&callback, this](Vm *vm) {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    callback(vm);
                    if (vm->had_error()) {
                        server_error();
                    }
                    m_vm_pool.emplace_back(vm);
                });
            } else {
                auto vm = get_current_vm();
                callback(vm);
                if (vm->had_error()) {
                    server_error();
                }
            }
        }

        [[noreturn]] void run() {
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_callbacks.empty()) continue;
                if (!m_vm_pool.empty()) continue;

                auto callback = m_callbacks.back();
                m_callbacks.pop_back();

                auto vm = m_vm_pool.back();
                m_vm_pool.pop_back();
                callback(vm);
            }
        }

    public:
        Context *m_ctx;
        std::vector<Callback> m_callbacks;
        std::vector<Vm *, gc_allocator<Vm *>> m_vm_pool;
        size_t pool_size = 30;
        size_t pool = 0;
        std::mutex m_mutex;
    };

    HttpRuntime *http_runtime = nullptr;


    class Request final : public NativeInstance {
    public:
        INSTANCE(Request)

        explicit Request(httplib::Request req)
                : request(std::move(req)) {}

        obj_result get_path_param(const t_string &name) {
            if (!request.path_params.contains(name.c_str())) {
                return make_error_t(fmt::format("path param '{}' not found", name));
            }
            return make_ok(make_string(request.path_params.at(name.c_str())));
        }

        obj_result get_query_param(const t_string &name) {
            if (!request.params.contains(name.c_str())) {
                return make_error_t(fmt::format("query param '{}' not found", name));
            }
            return make_ok(make_string(request.get_param_value(name.c_str())));
        }

        obj_result has_query_param(const t_string &name) {
            return AS_BOOL(request.has_param(name.c_str()));
        }

        obj_result has_path_param(const t_string &name) {
            return AS_BOOL(request.path_params.contains(name.c_str()));
        }

        obj_result get_header(const t_string &name) {
            if (!request.headers.contains(name.c_str())) {
                return make_error_t(fmt::format("header '{}' not found", name));
            }
            return make_ok(make_string(request.get_header_value(name.c_str())));
        }

        obj_result has_header(const t_string &name) {
            return AS_BOOL(request.has_header(name.c_str()));
        }

    private:
        const httplib::Request request;
    };


    class Response final : public NativeInstance {
    public:
        INSTANCE(Response)

        explicit Response(httplib::Response &res)
                : response(res) {}

        void set_content(const t_string &content, const t_string &type) {
            response.set_content(content.c_str(), content.size(), type.c_str());
        }

        obj_result status_getter() {
            return make_int(res.status);
        }

        obj_result body_getter() {
            return make_string(res.body);
        }

        void set_result(httplib::Response the_res) {
            res = the_res;
            response = res;
        }

    private:
        httplib::Response& response;
        httplib::Response res;
    };

    class Server final : public NativeInstance {
    public:
        INSTANCE(Server)

        void get(const t_string &path, const GcPtr<Object> &callback) {
            server.Get(path.c_str(), [callback](const httplib::Request &req, httplib::Response &res) {
                auto args = t_vector{
                        Runtime::ins()->construct<Request>("http", "Request", req),
                        Runtime::ins()->construct<Response>("http", "Response", res)
                };

                http_runtime->exec([callback, args](Vm *vm) {
                    vm->call_function_ex(callback, args);
                });
            });
        }

        bool listen(const t_string &host, int port) {
            fmt::print("server will be started at {}:{}\npress CTRL c to quit\n", host, port);
            return server.listen(host.c_str(), port);
        }

        void stop() {
            server.stop();
        }

        void set_defaults() {
            server.set_logger([](const auto &req, const auto &res) {
                fmt::print("{} - '{} {} {}' [{}]\n", req.local_addr, req.method, req.version, req.path, res.status);
            });

//            server.set_error_handler([](const auto& req, auto& res) {
//                auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
//                char buf[BUFSIZ];
//                snprintf(buf, sizeof(buf), fmt, res.status);
//                res.set_content(buf, "text/html");
//            });

            server.set_exception_handler([](const auto &req, auto &res, std::exception_ptr ep) {
                auto fmt = "<h1>Error 500</h1><p>%s</p>";
                char buf[BUFSIZ];
                try {
                    std::rethrow_exception(ep);
                } catch (std::exception &e) {
                    snprintf(buf, sizeof(buf), fmt, e.what());
                } catch (...) {
                    snprintf(buf, sizeof(buf), fmt, "Unknown Exception");
                }
                res.set_content(buf, "text/html");
                res.status = 500;
            });
        }

    private:
        httplib::Server server;
    };

    return_type Server_new(const t_vector &args) {
        TRY(parse_args(args));
        auto server = Runtime::ins()->construct<Server>("http", "Server");
        server->as<Server>()->set_defaults();
        return server;
    }

    class Client : public NativeInstance {
    public:
        INSTANCE(Client);

        Client(const t_string &url) : client(url.c_str()) {}

        auto err_to_string(httplib::Error err) -> const char * {
            switch (err) {
                case httplib::Error::Success:
                    return "Success";
                case httplib::Error::Connection:
                    return "Connection";
                case httplib::Error::BindIPAddress:
                    return "BindIPAddress";
                case httplib::Error::Read:
                    return "Read";
                case httplib::Error::Write:
                    return "Write";
                case httplib::Error::ExceedRedirectCount:
                    return "ExceedRedirectCount";
                case httplib::Error::Canceled:
                    return "Canceled";
                case httplib::Error::SSLConnection:
                    return "SSLConnection";
                case httplib::Error::SSLLoadingCerts:
                    return "SSLLoadingCerts";
                case httplib::Error::SSLServerVerification:
                    return "SSLServerVerification";
                case httplib::Error::UnsupportedMultipartBoundaryChars:
                    return "UnsupportedMultipartBoundaryChars";
                case httplib::Error::Compression:
                    return "Compression";
                case httplib::Error::Unknown:
                    return "Unknown";
                default:
                    return "Unknown";
                    break;
            }
        }

        auto get(const t_string &path) -> obj_result {
            auto res = client.Get(path.c_str());
            if (!res) {
                auto err = err_to_string(res.error());
                return make_error_t(err);
            }

            auto r = Runtime::ins()->construct<Response>("http", "Response", res.value())->as<Response>();
            r->set_result(res.value());
            return make_ok(r);
        }

    private:
        httplib::Client client;
    };

    obj_result Client_new(const t_vector &args) {
        String *url {nullptr};
        TRY(parse_args(args, url));
        return Runtime::ins()->construct<Client>("http", "Client", url->get_value());
    }

}


EXPORT void bond_module_init(bond::Context *ctx, bond::Vm *current_vm, bond::Mod &mod) {
    GC_INIT();
    bond::Runtime::ins()->set_runtime(current_vm->runtime());
    bond::set_current_vm(current_vm);


    bond::http::http_runtime = new(GC) bond::http::HttpRuntime(ctx);
    bond::http::http_runtime->init();
    std::thread(&bond::http::HttpRuntime::run, bond::http::http_runtime).detach();


    // server
    auto server = mod.struct_("Server", "Server()")
            .constructor(bond::http::Server_new)
            .method<&bond::http::Server::get>("get", "get(path: String, callback: Function)")
            .method<&bond::http::Server::listen>("listen", "listen(host: String, port: Int)")
            .method<&bond::http::Server::stop>("stop", "stop()");

    auto ser_type = server.build();
    bond::Runtime::ins()->register_type("http", "Server", ser_type);


    // request
    auto request = mod.struct_("Request", "Request(req: Request)")
            .constructor(bond::c_Default<bond::http::Request>)
            .method<&bond::http::Request::get_path_param>("get_path_param", "get_path_param(name: String) -> Result")
            .method<&bond::http::Request::has_path_param>("has_path_param", "has_path_param(name: String) -> Bool")
            .method<&bond::http::Request::get_query_param>("get_query_param", "get_query_param(name: String) -> Result")
            .method<&bond::http::Request::has_query_param>("has_query_param", "has_query_param(name: String) -> Bool")
            .method<&bond::http::Request::get_header>("get_header", "get_header(name: String) -> Result")
            .method<&bond::http::Request::has_header>("has_header", "has_header(name: String) -> Bool");


    auto req_type = request.build();
    bond::Runtime::ins()->register_type("http", "Request", req_type);


    // response
    auto response = mod.struct_("Response", "Response(res: Response)")
            .constructor(bond::c_Default<bond::http::Response>)
            .method<&bond::http::Response::set_content>("set_content", "set_content(content: String, type: String)")
            .getter<&bond::http::Response::status_getter>("status")
            .getter<&bond::http::Response::body_getter>("body");
    bond::Runtime::ins()->register_type("http", "Response", response.build());


    // client
    auto client = mod.struct_("Client", "Client(url: String)")
            .constructor(bond::http::Client_new)
            .method<&bond::http::Client::get>("get", "get(path: String) -> Result");

    bond::Runtime::ins()->register_type("http", "Client", client.build());
}
