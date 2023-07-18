#include "../../bond.h"
#include "plibsys.h"

using namespace bond;
GarbageCollector *m_gc;

class SocketAddress : public GcObject {
public:
    explicit SocketAddress(PSocketAddress *socket_address) : m_socket_address(socket_address) {}
    // FIXME: This is a memory leak but program crashes on delete
    //~SocketAddress() override { p_socket_address_free(m_socket_address); }

    std::string str() override { return "SocketAddress"; }

    bool equal(const GcPtr<bond::GcObject> &other) override { return false; };

    size_t hash() override { return 0; };

    PSocketAddress *get_socket_address() { return m_socket_address; }

    NativeErrorOr get_address(const std::vector<GcPtr<GcObject>> &obj) {
        ASSERT_ARG_COUNT(0, obj);
        char *address = p_socket_address_get_address(m_socket_address);
        if (address == nullptr) {
            return Err("Failed to get address");
        }
        auto res = Ok(m_gc->make<String>(address));
        std::free(address);
        return res;
    }

    NativeErrorOr get_port(const std::vector<GcPtr<GcObject>> &obj) {
        ASSERT_ARG_COUNT(0, obj);

        auto port = p_socket_address_get_port(m_socket_address);
        if (port == 0) {
            return Err("Failed to get port");
        }
        return Ok(m_gc->make<Integer>(port));
    }

    std::expected<GcPtr<GcObject>, RuntimeError> $get_attribute(const GcPtr<bond::GcObject> &index) override {
        auto name = index->as<String>()->get_value();
        if (name == "get_address") {
            return MAKE_METHOD(get_address);
        } else if (name == "get_port") {
            return MAKE_METHOD(get_port);
        }
        return std::unexpected(RuntimeError::AttributeNotFound);
    }

private:
    PSocketAddress *m_socket_address;
};


NativeErrorOr c_SocketAddress(const std::vector<GcPtr<GcObject>> &arguments) {
    ASSERT_ARG_COUNT(2, arguments);
    DEFINE(host, bond::String, 0, arguments);
    DEFINE(port, bond::Integer, 1, arguments);

    auto addr = p_socket_address_new(host->get_value().c_str(), port->get_value());
    if (addr == nullptr) {
        return Err("Failed to create socket address");
    }
    return Ok(m_gc->make<SocketAddress>(addr));
}


class Socket : public GcObject {
public:
    explicit Socket(PSocket *socket) : m_socket(socket) {}
    //TODO: program crashes on delete
//    ~Socket() override { p_socket_free(m_socket); }

    bool equal(const GcPtr<bond::GcObject> &other) override { return false; };

    size_t hash() override { return 0; };

    std::string str() override { return "Socket"; }

    PSocket *get_socket() { return m_socket; }


    //methods

    NativeErrorOr bind(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(address, SocketAddress, 0, arguments);
        DEFINE(reuse, bond::Bool, 1, arguments);

        auto error = p_error_new();
        auto res = p_socket_bind(m_socket, address->get_socket_address(), reuse->get_value(), &error);

        if (!res) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        return Ok(m_gc->make<Bool>(res));
    }

    NativeErrorOr close(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        auto error = p_error_new();
        auto res = p_socket_close(m_socket, &error);

        if (!res) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        return Ok(m_gc->make<Bool>(res));
    }

    NativeErrorOr shutdown(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(shutdown_read, Bool, 0, arguments);
        DEFINE(shutdown_write, Bool, 1, arguments);

        auto error = p_error_new();

        auto res = p_socket_shutdown(m_socket, shutdown_read->get_value(), shutdown_write->get_value(), &error);

        if (!res) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        return Ok(m_gc->make<Bool>(res));
    }

    NativeErrorOr listen(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(backlog, bond::Integer, 0, arguments);

        auto error = p_error_new();

        p_socket_set_listen_backlog(m_socket, backlog->get_value());
        auto res = p_socket_listen(m_socket, &error);

        if (!res) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }


        p_error_free(error);
        return Ok(m_gc->make<Bool>(res));
    }

    NativeErrorOr accept(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);

        auto error = p_error_new();
        auto socket = p_socket_accept(m_socket, &error);

        if (socket == nullptr) {
            auto e_m = p_error_get_message(error);
            auto e = Err(e_m);
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        return Ok(m_gc->make<Socket>(socket));
    }

    NativeErrorOr connect(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(address, SocketAddress, 0, arguments);

        auto error = p_error_new();
        auto res = p_socket_connect(m_socket, address->get_socket_address(), &error);

        if (!res) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        return Ok(m_gc->make<Bool>(res));
    }

    NativeErrorOr send(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(data, bond::String, 0, arguments);

        auto error = p_error_new();
        auto res = p_socket_send(m_socket, data->get_value().c_str(), data->get_value().size(), &error);

        if (res == -1) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        return Ok(m_gc->make<Integer>(res));
    }

    NativeErrorOr receive(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(size, bond::Integer, 0, arguments);

        auto error = p_error_new();
        auto buffer = (pchar *) p_malloc(size->get_value());
        auto res = p_socket_receive(m_socket, buffer, size->get_value(), &error);

        if (res == -1) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        auto r = Ok(m_gc->make<String>(std::string(static_cast<char *>(buffer), res)));
        p_free(buffer);
        return r;
    }

    NativeErrorOr send_to(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(data, bond::String, 0, arguments);
        DEFINE(address, SocketAddress, 1, arguments);

        auto error = p_error_new();
        auto res = p_socket_send_to(m_socket, address->get_socket_address(), data->get_value().c_str(),
                                    data->get_value().size(), &error);

        if (res == -1) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        return Ok(m_gc->make<Integer>(res));
    }

    NativeErrorOr receive_from(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(address, SocketAddress, 0, arguments);
        DEFINE(size, bond::Integer, 1, arguments);

        auto error = p_error_new();
        auto buffer = (pchar *) p_malloc(size->get_value());
        auto addr = address->get_socket_address();
        auto res = p_socket_receive_from(m_socket, &addr, buffer, size->get_value(), &error);

        if (res == -1) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        auto r = Ok(m_gc->make<String>(std::string(static_cast<char *>(buffer), res)));
        p_free(buffer);
        return r;
    }

    NativeErrorOr get_local_address(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);

        auto error = p_error_new();
        auto addr = p_socket_get_local_address(m_socket, &error);

        if (addr == nullptr) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        return Ok(m_gc->make<SocketAddress>(addr));
    }

    NativeErrorOr get_remote_address(const std::vector<GcPtr<GcObject>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);

        auto error = p_error_new();
        auto addr = p_socket_get_remote_address(m_socket, &error);

        if (addr == nullptr) {
            auto e = Err(p_error_get_message(error));
            p_error_free(error);
            return e;
        }

        p_error_free(error);
        return Ok(m_gc->make<SocketAddress>(addr));
    }

    std::expected<GcPtr<GcObject>, RuntimeError> $get_attribute(const GcPtr<bond::GcObject> &index) override {
        if (!index->is<String>()) {
            return std::unexpected(RuntimeError::AttributeNotFound);
        }

        auto name = index->as<String>()->get_value();
        if (!m_methods.contains(name)) {
            return std::unexpected(RuntimeError::AttributeNotFound);
        }

        return m_gc->make<NativeFunction>(m_methods[name], name);
    }


private:
    PSocket *m_socket;
#define METHOD(name) {#name, BIND(name)}
    std::unordered_map<std::string, std::function<NativeErrorOr(const std::vector<GcPtr<GcObject>> &)>> m_methods = {
            METHOD(close),
            METHOD(shutdown),
            METHOD(bind),
            METHOD(listen),
            METHOD(accept),
            METHOD(connect),
            METHOD(send),
            METHOD(receive),
            METHOD(send_to),
            METHOD(receive_from),
            METHOD(get_local_address),
            METHOD(get_remote_address)
    };
};


NativeErrorOr c_Socket(const std::vector<GcPtr<GcObject>> &arguments) {
    ASSERT_ARG_COUNT(3, arguments);
    DEFINE(family, bond::Integer, 0, arguments);
    DEFINE(type, bond::Integer, 1, arguments);
    DEFINE(protocol, bond::Integer, 2, arguments);

    auto error = p_error_new();
    auto socket = p_socket_new(static_cast<PSocketFamily>(family->get_value()),
                               static_cast<PSocketType>(type->get_value()),
                               static_cast<PSocketProtocol>(protocol->get_value()), &error);
    if (socket == nullptr) {
        auto e = Err(p_error_get_message(error));
        p_error_free(error);
        return e;
    }

    p_error_free(error);
    return Ok(m_gc->make<Socket>(socket));
}


EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    m_gc = ctx->gc();
    GarbageCollector::instance().set_gc(ctx->gc());

    std::unordered_map<std::string, uintmax_t> family = {
            {"UNKNOWN", P_SOCKET_FAMILY_UNKNOWN},
            {"INET",    P_SOCKET_FAMILY_INET},
            {"INET6",   P_SOCKET_FAMILY_INET6}
    };

    std::unordered_map<std::string, uintmax_t> type = {
            {"UNKNOWN",   P_SOCKET_TYPE_UNKNOWN},
            {"STREAM",    P_SOCKET_TYPE_STREAM},
            {"DATAGRAM",  P_SOCKET_TYPE_DATAGRAM},
            {"SEQPACKET", P_SOCKET_TYPE_SEQPACKET},
    };

    std::unordered_map<std::string, uintmax_t> protocol = {
            {"UNKNOWN", P_SOCKET_PROTOCOL_UNKNOWN},
            {"DEFAULT", P_SOCKET_PROTOCOL_DEFAULT},
            {"TCP",     P_SOCKET_PROTOCOL_TCP},
            {"UDP",     P_SOCKET_PROTOCOL_UDP},
            {"SCTP",    P_SOCKET_PROTOCOL_SCTP},
    };

    auto f_enum = ctx->gc()->make<Enum>("family", family);
    auto t_enum = ctx->gc()->make<Enum>("type", type);
    auto p_enum = ctx->gc()->make<Enum>("protocol", protocol);


    std::unordered_map<std::string, GcPtr<GcObject>> io_module = {
            {"family",        f_enum},
            {"type",          t_enum},
            {"protocol",      p_enum},
            {"SocketAddress", m_gc->make<NativeStruct<SocketAddress>>("SocketAddress", c_SocketAddress)},
            {"Socket",        m_gc->make<NativeStruct<Socket>>("Socket", c_Socket)},
    };

    auto module = ctx->gc()->make<Module>(path, io_module);
    ctx->add_module(path, module);
}