//
// Created by Travor Oguna Oneya on 28/03/2023.
//
#include "test.h"



int main() {
    auto e = bond::create_engine("bond");
    std::filesystem::current_path("../../tests/bond");
    e->run_file("main.bd");
    fmt::print("working directory {}\n", std::filesystem::current_path().string());
    ASSERT(e->get_context()->has_error() == false && "tests failed")
}