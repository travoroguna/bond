//
// Created by Travor Oguna Oneya on 06/04/2023.
//

#include "lsp.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace bond {

void LspClient::start() {
  while (!stop) {
    std::ofstream file(R"(D:\dev\cpp\bond\examples\testing)", std::ios::app);
    while (true) {
      read_message();
    }
    file.close();
  }
}

void LspClient::read_message() {
  std::string buff;
  std::string message;

  while (std::getline(std::cin, buff)) {
    if (buff == "Content-Length: 0") {
      break;
    }

    if (buff.substr(0, 14) == "Content-Length") {
      int length = std::stoi(buff.substr(16));
      std::cout << length << std::endl;

      for (int i = 0; i < length; i++) {
//          message = std::cin.r
        std::cout << buff << std::endl;
      }
    }

  }

  std::cout << message << std::endl;

//    json j = json::parse(message);
//
//    std::string method = j["method"];
//
//    if (method == "initialize") {
//      initialize(j);
//    }



}

void LspClient::initialize(json &init_string) {
  m_root_path = init_string["params"]["rootPath"];
  m_root_uri = init_string["params"]["rootUri"];
  m_client_name = init_string["params"]["clientName"];

  //{
  //  "jsonrpc": "2.0",
  //  "id": 1,
  //  "result": {
  //    "capabilities": {
  //      "textDocumentSync": {
  //        "openClose": true,
  //        "change": 1
  //      },
  //      "definitionProvider": true,
  //      "referencesProvider": true,
  //      "publishDiagnostics": {
  //        "relatedInformation": true
  //      }
  //    }
  //  }
  //}

  json response;
  response["jsonrpc"] = "2.0";
  response["id"] = init_string["id"];
  response["result"]["capabilities"]["textDocumentSync"]["openClose"] = true;
  response["result"]["capabilities"]["textDocumentSync"]["change"] = 1;
  response["result"]["capabilities"]["definitionProvider"] = true;
  response["result"]["capabilities"]["referencesProvider"] = true;
  response["result"]["capabilities"]["publishDiagnostics"]["relatedInformation"] = true;

  std::cout << response.dump() << std::endl;
}

};