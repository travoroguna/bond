//
// Created by Travor Oguna Oneya on 24/05/2023.
//

#pragma once

#include <cstdint>
#include <fstream>
#include <type_traits>
#include <vector>
#include "object.h"


namespace bond{
    template <typename T>
    auto write_val(std::ofstream &stream, T value) -> void{
        stream.write(reinterpret_cast<const char *>(&value), sizeof(T));
    }

    template <typename T>
    auto read_val(std::ifstream &stream) -> T {
        char buffer[sizeof(T)];
        stream.read(buffer, sizeof(T));
        return *((T*)buffer);
    }


    auto write_code_file(const std::string &path, const GcPtr<Code> &code) -> void;
    auto read_code_file(const std::string &path) -> std::expected<GcPtr<Code>, std::string>;
}
