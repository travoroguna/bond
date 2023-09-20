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

#include "../md5.h"
#include "../runtime.h"

#include <unordered_set>


#define BOND_MAGIC_NUMBER 0x424F4E44
#define BOND_BAR_VERSION 0x00000001


namespace bond {
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

        std::unordered_set<t_string> &get_compiled() { return compiled_deps; }


    private:
        t_string path;
        std::vector<std::shared_ptr<Node>> nodes;
        std::vector<t_string> dependencies;
        size_t unit_id{0};
        std::shared_ptr<Scopes> scopes;
        Context *context;
        std::unordered_set<t_string> compiled_deps;
    };

    template<typename S, typename T, typename=std::is_trivially_copyable<T>>
    auto write_val(S &stream, T value) -> void {
        stream.write(reinterpret_cast<const char *>(&value), sizeof(T));
    }

    template<typename S, typename T>
    auto read_val(S &stream) -> T {
        T value;
        stream.read(reinterpret_cast<char *>(&value), sizeof(T));
        return value;
    }

    template<typename S>
    auto write_val(S &stream, t_string value) -> void {
        stream.write(value.c_str(), value.size() + 1);
    }

    template<typename S>
    auto write_val(S &stream, std::string value) -> void {
        stream.write(value.c_str(), value.size() + 1);
    }

    template<typename S>
    auto read_val(S &stream) -> t_string {
        std::vector<char> buff;
        char mini_buf[1];

        while (true) {
            stream.read(&mini_buf[0], 1);

            if (!stream || *mini_buf == '\0') {
                break;
            }

            buff.push_back(*mini_buf);
        }

        return std::string(buff.begin(), buff.end());
    }

    template<typename S>
    auto read_string(S &stream) -> t_string {
        std::vector<char> buff;
        char mini_buf[1];

        while (true) {
            stream.read(&mini_buf[0], 1);

            if (!stream || *mini_buf == '\0') {
                break;
            }

            buff.push_back(*mini_buf);
        }

        return std::string(buff.begin(), buff.end());
    }

    template <typename S>
    auto write_string(S &stream, t_string value) -> void {
        stream.write(value.c_str(), value.size() + 1);
    }


    template<typename S>
    auto read_val(S &stream) -> std::string {
        std::vector<char> buff;
        char mini_buf[1];

        while (true) {
            stream.read(&mini_buf[0], 1);

            if (!stream || *mini_buf == '\0') {
                break;
            }

            buff.push_back(*mini_buf);
        }

        return std::string(buff.begin(), buff.end());
    }

    template<typename S>
    auto write_span(S &stream, const SharedSpan &span) -> void {
        write_val<S, uint32_t>(stream, span->module_id);
        write_val<S, uint32_t>(stream, span->start);
        write_val<S, uint32_t>(stream, span->end);
        write_val<S, uint32_t>(stream, span->line);
    }

    template<typename S>
    auto read_span(S &stream) -> SharedSpan {
        auto module_id = read_val<S, uint32_t>(stream);
        auto start = read_val<S, uint32_t>(stream);
        auto end = read_val<S, uint32_t>(stream);
        auto line = read_val<S, uint32_t>(stream);

        return std::make_shared<Span>(module_id, start, end, line);
    }

    template<typename S>
    auto write_function(S &stream, Function *function) -> void {
        // 1. name
        // 2. arg count
        // 3. code

        auto arguments = function->get_arguments();
        auto code = function->get_code();

        write_string(stream, function->get_name());
        write_val<S, uint32_t>(stream, (uint32_t)arguments.size());
        write_code_impl(stream, code);

        for (auto &param: arguments) {
            write_string(stream, param->name);
            write_span(stream, param->span);
        }
    }

    template<typename S>
    auto read_code_impl(S &stream) -> GcPtr<Code>;

    template<typename S>
    auto read_function(S &stream) -> GcPtr<Function> {
        auto name = read_string<S>(stream);
        auto arg_count = read_val<S, uint32_t>(stream);
        auto code = read_code_impl(stream);

        auto params = std::vector<std::shared_ptr<Param>>();

        for (uint32_t i = 0; i < arg_count; i++) {
            auto arg_name = read_string<S>(stream);
            auto arg_span = read_span(stream);

            params.emplace_back(std::make_shared<Param>(arg_name.c_str(), std::nullopt, arg_span));
        }

        return Runtime::ins()->make_function(name, params, code);
    }

    template<typename S>
    auto write_struct(S &stream, Struct *s) -> void {
        // 1. name
        // 2. field_count
        // 3. method_count
        // 5. fields
        // 6. methods -> [name, function], ....

        write_string(stream, s->get_name());

        auto fields = s->get_fields();
        auto methods = s->get_methods();

        write_val<S, uint32_t>(stream, (uint32_t)fields.size());
        write_val<S, uint32_t>(stream, (uint32_t)methods.size());

        for (auto &field: fields) {
            write_string(stream, field);
        }

        for (auto &[name, method]: methods) {
            write_string(stream, name);
            write_function(stream, method->as<Function>().get());
        }

    }

    template<typename S>
    auto read_struct(S &stream) -> GcPtr<Struct> {
        auto name = read_string<S>(stream);

        auto field_count = read_val<S, uint32_t>(stream);
        auto method_count = read_val<S, uint32_t>(stream);

        auto fields = std::vector<t_string>();

        for (uint32_t i = 0; i < field_count; i++) {
            auto field_name = read_string<S>(stream);
            fields.push_back(field_name);
        }

        auto s = Runtime::ins()->STRUCT_STRUCT->create_instance<Struct>(name, fields);

        for (uint32_t i = 0; i < method_count; i++) {
            t_string method_name = read_string<S>(stream);
            auto method = read_function(stream);
            s->add_method(method_name, method);
        }

        return s;
    }

    template<typename S>
    inline auto write_code_impl(S &stream, const GcPtr<Code> &code) -> void {
        auto constants = code->get_constants();
        auto instructions = code->get_instructions();
        auto spans = code->get_spans();

        write_val<S, uint32_t>(stream, (uint32_t)constants.size());
        write_val<S, uint32_t>(stream, (uint32_t)instructions.size());
        write_val<S, uint32_t>(stream, (uint32_t)spans.size());

        for (auto &constant: constants) {
            if (instanceof<Int>(constant.get())) {
                write_val<S, uint8_t>(stream, 0);
                write_val<S, int64_t>(stream, constant->as<Int>()->get_value());
            } else if (instanceof<String>(constant.get())) {
                write_val<S, uint8_t>(stream, 1);
                write_string<S>(stream, t_string(constant->as<String>()->get_value()));
            } else if (instanceof<Float>(constant.get())) {
                write_val<S, uint8_t>(stream, 2);
                write_val<S, double>(stream, constant->as<Float>()->get_value());
            } else if (instanceof<Function>(constant.get())) {
                auto function = constant->as<Function>();
                write_val<S, uint8_t>(stream, 3);
                write_function(stream, function.get());
            } else if (instanceof<Struct>(constant.get())) {
                write_val<S, uint8_t>(stream, 4);
                write_struct(stream, constant->as<Struct>().get());
            } else {
                throw std::runtime_error("Unknown constant type");
            }
        }

        for (auto &instruction: instructions) {
            write_val<S, uint32_t>(stream, instruction);
        }

        for (auto &span: spans) {
            write_span(stream, span);
        }

    }

    template<typename S>
    auto write_archive(S &stream,
                       const std::unordered_map<t_string, std::shared_ptr<Unit>> &units) -> std::expected<void, t_string> {
        write_val<S, uint32_t>(stream, BOND_MAGIC_NUMBER);
        write_val<S, uint32_t>(stream, BOND_BAR_VERSION);
        write_val<S, uint32_t>(stream, (uint32_t)units.size());

        size_t i = 1;


        for (auto &[_, unit]: units) {
            fmt::print("[{}/{}] unit {}, {}\n", i, units.size(), unit->get_unit_id(), _);
            write_val<S, uint32_t>(stream, (uint32_t)unit->get_unit_id());
            auto code = unit->compile();
            TRY(code);
            write_code_impl(stream, code.value());
            i++;
        }

        return {};
    }

    inline auto write_archive_file(const t_string &path,
                            const std::unordered_map<t_string, std::shared_ptr<Unit>> &units) -> std::expected<void, t_string> {
        if (std::filesystem::exists(path.c_str())) {
            std::filesystem::remove(path.c_str());
            fmt::print("Removed existing archive {}\n", path);
        }

        fmt::print("Writing archive to {}\n", path);

        std::ofstream stream(path.c_str(), std::ios::binary);

        if (!stream) {
            return std::unexpected("Could not open file");
        }

        return write_archive(stream, units);
    }

    template<typename S>
    auto read_code_impl(S &stream) -> GcPtr<Code> {
        auto constants_count = read_val<S, uint32_t>(stream);
        auto code_size = read_val<S, uint32_t>(stream);
        auto spans_count = read_val<S, uint32_t>(stream);

        t_vector constants;

        for (uint32_t i = 0; i < constants_count; i++) {
            if (!stream) {
                throw std::runtime_error("Unexpected end of file");
            }

            auto type = read_val<S, uint8_t>(stream);
            if (type == 0) {
                auto value = read_val<S, int64_t>(stream);
                constants.emplace_back(make_int(value));
            } else if (type == 1) {
                t_string value = read_string<S>(stream);
                constants.emplace_back(make_string(value));
            }
            else if (type == 2) {
                auto value = read_val<S, double>(stream);
                constants.emplace_back(make_float(value));
            } else if (type == 3) {
                auto function = read_function(stream);
                constants.emplace_back(function);
            } else if (type == 4) {
                auto s = read_struct(stream);
                constants.emplace_back(s);
            } else {
                throw std::runtime_error("Unknown constant type");
            }
        }

        auto instructions = std::vector<uint32_t>();

        for (uint32_t i = 0; i < code_size; i++) {
            auto instruction = read_val<S, uint32_t>(stream);
            instructions.emplace_back(instruction);
        }

        auto spans = std::vector<SharedSpan>();

        for (uint32_t i = 0; i < spans_count; i++) {
            spans.emplace_back(read_span(stream));
        }

        return Runtime::ins()->CODE_STRUCT->create_instance<Code>(instructions, spans, constants);
    }


    template<typename S>
    auto read_archive(S &stream) -> std::expected<std::unordered_map<uint32_t, GcPtr<Code>>, t_string> {
        auto magic_number = read_val<S, uint32_t>(stream);
        if (magic_number != BOND_MAGIC_NUMBER) {
            return std::unexpected(
                    fmt::format("Invalid magic number, expected {} got {}", BOND_MAGIC_NUMBER, magic_number));
        }

        auto version = read_val<S, uint32_t>(stream);
        if (version != BOND_BAR_VERSION) {
            return std::unexpected(fmt::format("Invalid version, expected {}, got {}", BOND_BAR_VERSION, version));
        }

        auto module_count = read_val<S, uint32_t>(stream);

        std::unordered_map<uint32_t, GcPtr<Code>> modules;

        for (uint32_t i = 0; i < module_count; i++) {
            auto module_id = read_val<S, uint32_t>(stream);
            auto code = read_code_impl(stream);
            modules.emplace(module_id, code);
        }

        return modules;
    }

    inline auto read_archive_file(const t_string &path) -> std::expected<std::unordered_map<uint32_t, GcPtr<Code>>, t_string> {
        std::ifstream stream(path.c_str(), std::ios::binary);
        if (stream.bad()) {
            return std::unexpected("Could not open file");
        }

        return read_archive(stream);
    }

    inline auto may_be_bar_file(const t_string &path) -> bool {
        std::ifstream stream(path.c_str(), std::ios::binary);
        if (!stream) {
            return false;
        }

        auto magic_number = read_val<std::ifstream, uint32_t>(stream);
        auto version = read_val<std::ifstream, uint32_t>(stream);

        return magic_number == BOND_MAGIC_NUMBER and version == BOND_BAR_VERSION;
    }
}
