//
// Created by Travor Oguna Oneya on 24/05/2023.
//

#pragma once

#include <cstdint>
#include <fstream>
#include <type_traits>
#include <utility>
#include <vector>
#include "../object.h"
#include "../compiler/parser.h"


namespace bond{
    // bar format is a binary format for storing bond code

    // MAGIC NUMBER i32
    // VERSION      i32
    // module count u32

    // for each module (code object)
    // module id u32
    // constant count u32
    // instructions count u32
    // span count u32
    // constants
    // instructions
    // spans

    // for each constant
    // type u8
    // value

    // instructions array<u32>

    // for each span
    // module id u32
    // start u32
    // end u32
    // line u32


    class Unit {
    public:
        Unit(t_string path, Context *context) : path(std::move(path)), context(context) {};
        std::expected<void, t_string> find_dependencies();
        std::vector<t_string> &get_dependencies();
        std::shared_ptr<Scopes> get_scopes() { return scopes; }
        std::vector<std::shared_ptr<Node>> &get_nodes() { return nodes; }
        size_t get_unit_id() const { return unit_id; }
        std::expected<GcPtr<Code>, t_string> compile();


    private:
        t_string path;
        std::vector<std::shared_ptr<Node>> nodes;
        std::vector<t_string> dependencies;
        size_t unit_id {0};
        std::shared_ptr<Scopes> scopes;
        Context *context;
    };

    template <typename T, typename=std::is_trivially_copyable<T>>
    auto write_val(std::ofstream &stream, T value) -> void{
        stream.write(reinterpret_cast<const char *>(&value), sizeof(T));
    }

    template <typename T>
    auto read_val(std::ifstream &stream) -> T {
        T value;
        stream.read(reinterpret_cast<char *>(&value), sizeof(T));
        return value;
    }

    auto write_archive(const t_string &path, const std::unordered_map<t_string, std::shared_ptr<Unit>>&) -> std::expected<void, t_string>;
    auto read_archive(const t_string &path) -> std::expected<std::unordered_map<uint32_t, GcPtr<Code>>, t_string>;

    auto may_be_bar_file(const t_string &path) -> bool;
}
