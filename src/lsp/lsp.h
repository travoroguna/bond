//
// Created by Travor Oguna Oneya on 06/04/2023.
//

#pragma once
#include "../bond.h"
#include "nlohmann/json.hpp"

namespace bond {

class LspClient {
 public:
  LspClient() = default;
  void start();

 private:
  bool stop = false;
  void read_message();

  std::string m_root_path;
  std::string m_root_uri;
  std::string m_client_name;

  void initialize(nlohmann::json_abi_v3_11_2::basic_json<> &init_string);
};

};