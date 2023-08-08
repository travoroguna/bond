//
// Created by travor on 22/07/2023.
//

#ifndef BOND_BUILD_H
#define BOND_BUILD_H

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include "../compiler/context.h"
#include "../compiler/lexer.h"
#include "../compiler/parser.h"
#include "../compiler/codegen.h"
#include "../compiler/bfmt.h"



namespace bond {
    class Build {
    public:
        Build(const std::string &lib_path, const std::string &main_file);
        Context *get_context() { return &context; }
        std::expected<std::string, std::string> build();

    private:
        std::string main_file;
        Context context;
        std::unordered_map<std::string, std::shared_ptr<Unit>> units;
        std::expected<void, std::string> find_deps(const std::string &path);
    };

}



#endif //BOND_BUILD_H
