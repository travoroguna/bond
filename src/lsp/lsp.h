//
// Created by Travor Oguna Oneya on 06/04/2023.
//

#pragma once

#include "../bond.h"
#include "rpc.h"
#include "nlohmann/json.hpp"

namespace bond {

    class InitializeParams : public Jsonify {
    public:
        InitializeParams(int32_t id) { m_id = id; }

        json to_json() override {
            json response;

            response["jsonrpc"] = "2.0";
            response["id"] = m_id;
            response["result"]["capabilities"]["textDocumentSync"]["openClose"] = true;
            response["result"]["capabilities"]["textDocumentSync"]["change"] = 1;
            response["result"]["capabilities"]["definitionProvider"] = true;
            response["result"]["capabilities"]["referencesProvider"] = true;
            response["result"]["capabilities"]["publishDiagnostics"]["relatedInformation"] = true;


            return response;
        }

    private:
        int32_t m_id;
    };

    class LspClient {
    public:
        LspClient() = default;

        void start() {
            bond::Rpc::log_message("LSP...\n");
            m_rpc.register_method("initialize",
                                  [this](auto &&PH1) { return initialize(std::forward<decltype(PH1)>(PH1)); });
            m_rpc.start();
        }

        Response<Jsonify *> initialize(Request &request) {
            return {request.get_id(), new InitializeParams(request.get_id())};
        }

    private:
        Rpc m_rpc;

    };

};