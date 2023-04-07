//
// Created by Travor Oguna Oneya on 05/04/2023.
//

#include "builtins.h"
#include <fmt/core.h>
#include <unordered_map>

namespace bond {

std::unordered_map<std::string, GcPtr<NativeFunction>> builtins = {
    {"println", GarbageCollector::instance().make_immortal<NativeFunction>(println, "println")},
};

NativeErrorOr println(const std::vector<GcPtr<Object>> &arguments) {
  for (auto &arg : arguments) {
    fmt::print("{} ", arg->str());
  }
  fmt::print("\n");
  return GarbageCollector::instance().make<Nil>();
}

void add_builtins_to_globals(GcPtr<Map> &globals) {
  for (auto &[name, function] : builtins) {
    globals->set(GarbageCollector::instance().make_immortal<String>(name), function);
  }
}

};