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
        Build(const t_string &lib_path, const t_string &main_file);
        Context *get_context() { return &context; }
        std::expected<t_string, t_string> build();

    private:
        t_string main_file;
        Context context;
        std::unordered_map<t_string, std::shared_ptr<Unit>> units;
        std::expected<void, t_string> find_deps(const t_string &path);
    };

}



#endif //BOND_BUILD_H
