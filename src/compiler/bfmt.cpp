//
// Created by Travor Oguna Oneya on 24/05/2023.
//

#include "bfmt.h"
#include "../md5.h"


#define BOND_MAGIC_NUMBER 0x424F4E44
#define BOND_VERSION 0x00000001


namespace bond{
    auto write_code_impl(std::ofstream& stream, const GcPtr<Code>& code) -> void;
    auto read_code_impl(std::ifstream &stream) -> GcPtr<Code>;

    auto write_val(std::ofstream &stream, std::string value) -> void{
        stream.write(value.c_str(), value.size() + 1);
    }

    auto read_val(std::ifstream& stream) -> std::string {
        std::vector<char> buff;
        char mini_buf[1];

        while (true) {
            stream.read(&mini_buf[0], 1);

            if (!stream || *mini_buf == '\0') {
                break;
            }

            buff.push_back(*mini_buf);
        }

        return {buff.begin(), buff.end()};
    }


    auto write_span(std::ofstream& stream, const SharedSpan& span) -> void{
        write_val<uint32_t>(stream, span->module_id);
        write_val<uint32_t>(stream, span->start);
        write_val<uint32_t>(stream, span->end);
        write_val<uint32_t>(stream, span->line);
    }

    auto read_span(std::ifstream& stream) -> SharedSpan{
        auto module_id = read_val<uint32_t>(stream);
        auto start = read_val<uint32_t>(stream);
        auto end = read_val<uint32_t>(stream);
        auto line = read_val<uint32_t>(stream);

        return std::make_shared<Span>(module_id, start, end, line);
    }

    auto write_function(std::ofstream& stream, Function *function) -> void {
        // 1. name
        // 2. arg count
        // 3. code

        auto arguments = function->get_arguments();
        auto code = function->get_code();

        write_val(stream, function->get_name());
        write_val<uint32_t>(stream, arguments.size());
        write_code_impl(stream, code);

        for (auto& [name, spn] : arguments) {
            write_val(stream, name);
            write_span(stream, spn);
        }
    }

    auto read_function(std::ifstream& stream) -> GcPtr<Function> {
        std::string name = read_val(stream);
        auto arg_count = read_val<uint32_t>(stream);
        auto code = read_code_impl(stream);


        auto params = std::vector<std::pair<std::string, SharedSpan>>();

        for (int i = 0; i < arg_count; i++) {
            std::string arg_name = read_val(stream);
            auto arg_span = read_span(stream);

            params.emplace_back(arg_name, arg_span);
        }

        return FUNCTION_STRUCT->create_immortal<Function>(name, params, code);
    }


    auto write_struct(std::ofstream& stream, Struct *s) -> void {
        // 1. name
        // 2. field_count
        // 3. method_count
        // 5. fields
        // 6. methods -> [name, function], ....

        write_val(stream, s->get_name());

        auto fields = s->get_fields();
        auto methods = s->get_methods();

        write_val<uint32_t>(stream, fields.size());
        write_val<uint32_t>(stream, methods.size());

        for (auto &field: fields){
            write_val(stream, field->get_value());
        }

        for (auto& [name, method] : methods){
            write_val(stream, name->as<String>()->get_value());
            write_function(stream, method->as<Function>().get());
        }

    }

    auto read_struct(std::ifstream& stream) -> GcPtr<Struct> {
        std::string name = read_val(stream);

        auto field_count = read_val<uint32_t>(stream);
        auto method_count = read_val<uint32_t>(stream);

        auto fields = std::vector<GcPtr<String>>();

        auto lock = LockGc();

        for (int i = 0; i < field_count; i++){
            std::string field_name = read_val(stream);
            fields.push_back(GarbageCollector::instance().make_immortal<String>(field_name));
        }

        auto s= GarbageCollector::instance().make_immortal<Struct>(GarbageCollector::instance().make_immortal<String>(name), fields);

        for (int i = 0; i < method_count; i++){
            std::string method_name = read_val(stream);
            auto method = read_function(stream);
            s->add_method(GarbageCollector::instance().make_immortal<String>(method_name), method);
        }

        return s;
    }

    auto write_code_impl(std::ofstream& stream, const GcPtr<Code>& code) -> void{
        auto constants = code->get_constants();
        auto instructions = code->get_instructions();
        auto spans = code->get_spans();

        write_val<uint32_t>(stream, constants.size());
        write_val<uint32_t>(stream, instructions.size());
        write_val<uint32_t>(stream, spans.size());

        for(auto& constant : constants){
            if (instanceof<Integer>(constant.get())) {
                write_val<uint8_t>(stream, 0);
                write_val<int64_t>(stream, constant->as<Integer>()->get_value());
            }
            else if(instanceof<String>(constant.get())) {
                write_val<uint8_t>(stream, 1);
                write_val(stream, std::string(constant->as<String>()->get_value()));
            }
            else if(instanceof<Float>(constant.get())) {
                write_val<uint8_t>(stream, 2);
                write_val<float>(stream, constant->as<Float>()->get_value());
            }
            else if(instanceof<Function>(constant.get())){
                auto function = constant->as<Function>();
                write_val<uint8_t>(stream, 3);
                write_function(stream, function.get());
            }
            else if(instanceof<Struct>(constant.get())) {
                write_val<uint8_t>(stream, 4);
                write_struct(stream, constant->as<Struct>().get());
            }
            else{
                throw std::runtime_error("Unknown constant type");
            }
        }

        for(auto& instruction : instructions){
            write_val<uint32_t>(stream, instruction);
        }

        for (auto& span : spans) {
            write_span(stream, span);
        }

    }

    auto write_code_file(const std::string &path, const GcPtr<Code> &code) -> void{
        //format
        // 1. magic number int32_t 0x424F4E44
        // 2. version int32_t 0x00000001
        // 3. md5 hash
        // 4. constants count uint32_t
        // 5. code size uint32_t
        // 6. span count uint32_t
        // 7. constants
        // 8. code
        // 9. spans

        std::ofstream stream(path, std::ios::binary);
        write_val<int32_t>(stream, BOND_MAGIC_NUMBER);
        write_val<int32_t>(stream, BOND_VERSION);
        write_code_impl(stream, code);
    }


    auto read_code_impl(std::ifstream &stream) -> GcPtr<Code> {
        auto constants_count = read_val<uint32_t>(stream);
        auto code_size = read_val<uint32_t>(stream);
        auto spans_count = read_val<uint32_t>(stream);

        auto constants = std::vector<GcPtr<GcObject>>();
        auto lock = LockGc();

        for (auto i = 0; i < constants_count; i++) {
            auto type = read_val<uint8_t>(stream);
            if (type == 0) {
                auto value = read_val<int64_t>(stream);
                constants.emplace_back(GarbageCollector::instance().make_immortal<Integer>(value));
            }
            else if (type == 1) {
                std::string value = read_val(stream);
                constants.emplace_back(GarbageCollector::instance().make_immortal<String>(value));
            }
            else if (type == 2) {
                auto value = read_val<float>(stream);
                constants.emplace_back(GarbageCollector::instance().make_immortal<Float>(value));
            }
            else if (type == 3) {
                auto function = read_function(stream);
                constants.emplace_back(function);
            }
            else if (type == 4) {
                auto s = read_struct(stream);
                constants.emplace_back(s);
            }
            else {
                throw std::runtime_error("Unknown constant type");
            }
        }

        auto instructions = std::vector<uint32_t>();

        for (auto i = 0; i < code_size; i++) {
            auto instruction = read_val<uint32_t>(stream);
            instructions.emplace_back(instruction);
        }

        auto spans = std::vector<SharedSpan>();

        for (auto i = 0; i < spans_count; i++) {
            spans.emplace_back(read_span(stream));
        }

        return GarbageCollector::instance().make_immortal<Code>(instructions, constants, spans);
    }

    auto read_code_file(const std::string &path) -> std::expected<GcPtr<Code>, std::string>{
        std::ifstream stream(path, std::ios::binary);
        if (!stream) {
            return std::unexpected("Could not open file");
        }

        auto magic_number = read_val<int32_t>(stream);
        if (magic_number != BOND_MAGIC_NUMBER) {
            return std::unexpected(fmt::format("Invalid magic number, expected {} got {}", BOND_MAGIC_NUMBER, magic_number));
        }

        auto version = read_val<int32_t>(stream);
        if (version != BOND_VERSION) {
            return std::unexpected(fmt::format("Invalid version, expected {}, got {}", BOND_VERSION, version));
        }

        return read_code_impl(stream);
    }
}