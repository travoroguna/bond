//
// Created by Travor Oguna Oneya on 05/04/2023.
//

#include "builtins.h"
#include "debug.h"
#include <fmt/core.h>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include "conv.hpp"

namespace bond {
    obj_result b_println(const t_vector &args) {
        for (auto &arg: args) {
            fmt::print("{} ", arg->str());
        }
        fmt::print("\n");
        return OK();
    }


    obj_result b_print(const t_vector &args) {
        for (auto &arg: args) {
            fmt::print("{} ", arg->str());
        }
        fmt::print("\n");
        return OK();
    }

    obj_result b_dump(const t_vector &args) {
//        GC_dump();
//        GC_generate_random_backtrace();
        fmt::print("heap size: {} bytes\n", GC_get_heap_size());
        return OK();
    }

    obj_result b_exit(const t_vector &args) {
        Int* code;
        TRY(parse_args(args, code));
        exit(code->get_value());
        return OK();
    }

    obj_result build_help_for_native_struct(const GcPtr<NativeStruct> &native_struct) {
        std::stringstream help;
        help << fmt::format("native struct {}:\n\t{}\n", native_struct->get_name(), native_struct->get_doc());

        for (auto const &[name, doc]: native_struct->get_methods()) {
            auto meth = doc.second;
            help << fmt::format("\n{}\n", meth);
        }

        return OK(make_string(help.str()));
    }

    std::string build_doc(const GcPtr<Function> &function) {
        std::stringstream help;
        help << fmt::format("fn {}(", function->get_name());

        std::vector<std::string> params;

        for (auto const &[name, _]: function->get_arguments()) {
            params.push_back(name);
        }

        help << fmt::format("{}", fmt::join(params, ", ")) << ")\n";
        return help.str();
    }

    obj_result build_help_for_struct(const GcPtr<Struct> &struct_) {
        std::stringstream help;

        help << fmt::format("struct {}:\n\tattributes\n", struct_->get_name());

        for (auto const &field_name: struct_->get_fields()) {
            help << fmt::format("\t\t{}\n", field_name);
        }

        help << "\n\tmethods\n";
        for (auto const &[name, function]: struct_->get_methods()) {
            help << fmt::format("\t\t{}: {}\n", name, build_doc(function));
        }

        return OK(make_string(help.str()));
    }

    obj_result b_help(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));

        if (obj->is<NativeStruct>()) {
            return build_help_for_native_struct(obj->as<NativeStruct>());
        }
        else if (obj->is<Struct>()) {
            return build_help_for_struct(obj->as<Struct>());
        }
        else if (obj->is<Instance>()) {
            return build_help_for_struct(obj->as<Instance>()->get_struct());
        }
        else if (obj->is<NativeInstance>()) {
            return build_help_for_native_struct(obj->as<NativeInstance>()->get_native_struct());
        }
        return ERR("help() only works on structs and native structs");
    }

    obj_result b_type_of(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));
        if (obj->is<NativeStruct>() or obj->is<Struct>()) {
            return obj;
        }
        else if (obj->is<Instance>()) {
            return obj->as<Instance>()->get_struct();
        }
        else if (obj->is<NativeInstance>()) {
            return obj->as<NativeInstance>()->get_native_struct();
        }
        return ERR(fmt::format("unable to get type of {}", obj->str()));
    }

    obj_result b_instance_of(const t_vector &args) {
        Object *obj;
        Object *struct_;
        TRY(parse_args(args, obj, struct_));

        if (obj->is<Instance>()) {
            if (struct_->is<Struct>()) {
                return AS_BOOL(obj->as<Instance>()->get_struct() == struct_->as<Struct>().get());
            }
            if (struct_->is<NativeStruct>()) {
                return AS_BOOL(false);
            }
            return ERR("expected a Type as the second argument");
        }

        else if (obj->is<NativeInstance>()) {
            if (struct_->is<NativeStruct>()) {
                return AS_BOOL(obj->as<NativeInstance>()->get_native_struct() == struct_->as<NativeStruct>().get());
            }

            if (struct_->is<Struct>()) {
                return AS_BOOL(false);
            }

            return ERR("expected a Type as the second argument");
        }

        return ERR("expected an instance as the first argument");
    }

    obj_result b_input(const t_vector &args) {
        String* prompt;
        TRY(parse_args(args, prompt));
        fmt::print("{}", prompt->get_value());
        std::string input;
        std::getline(std::cin, input);
        return OK(make_string(input));
    }

    obj_result b_debug_break(const t_vector &args) {
        debug_break();
        return OK();
    }


    obj_result b_to_string(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));
        return OK(make_string(obj->str()));
    }



    class Point: public NativeInstance {
    public:
        int64_t x;
        int64_t y;
        Point(int x, int y): x(x), y(y) {}

        static obj_result constructor(const t_vector &args) {
            Int *x;
            Int *y;
            TRY(parse_args(args, x, y));
            return make<Point>(x->get_value(), y->get_value());
        }

        static obj_result get_x(const GcPtr<Object>& Self) {
            auto self = Self->as<Point>();
            return OK(make_int(self->x));
        }

        static obj_result get_y(const GcPtr<Object>& Self) {
            auto self = Self->as<Point>();
            return OK(make_int(self->y));
        }

        static obj_result set_x(const GcPtr<Object>& Self, const GcPtr<Object>& value) {
            auto self = Self->as<Point>();

            if (!value->is<Int>()) {
                return ERR("expected an int");
            }

            self->x = value->as<Int>()->get_value();
            return OK(self);
        }

        static obj_result set_y(const GcPtr<Object>& Self,  const GcPtr<Object>& value) {
            auto self = Self->as<Point>();

            if (!value->is<Int>()) {
                return ERR("expected an int");
            }

            self->y = value->as<Int>()->get_value();
            return OK(self);
        }
    };

    bool built = false;
    std::unordered_map<std::string, GcPtr<Object>> builtins;

    void add_builtins_to_globals(const GcPtr<Map> &globals) {
        if (!built) {
            auto future = Mod("future");
            future.function("to_string", b_to_string, "to_string(obj)");
            future.struct_("Point", "Point(x, y)")
                .constructor(Point::constructor)
                .field("x", Point::get_x, Point::set_x)
                .field("y", Point::get_y, Point::set_y);



            builtins = {
                    {"println", NATIVE_FUNCTION_STRUCT->create_immortal<NativeFunction>("println", "println(...)",
                                                                                        b_println)},
                    {"print",   NATIVE_FUNCTION_STRUCT->create_immortal<NativeFunction>("print", "print(...)", b_print)},
                    {"dump",    NATIVE_FUNCTION_STRUCT->create_immortal<NativeFunction>("dump", "dump()", b_dump)},
                    {"exit",    NATIVE_FUNCTION_STRUCT->create_immortal<NativeFunction>("exit", "exit(code)", b_exit)},
                    {"help",    NATIVE_FUNCTION_STRUCT->create_immortal<NativeFunction>("help", "help(obj)", b_help)},
                    {"type_of", NATIVE_FUNCTION_STRUCT->create_immortal<NativeFunction>("type_of", "type_of(obj)", b_type_of)},
                    {"instance_of", NATIVE_FUNCTION_STRUCT->create_immortal<NativeFunction>("instance_of", "instance_of(obj, type)", b_instance_of)},
                    {"input",   NATIVE_FUNCTION_STRUCT->create_immortal<NativeFunction>("input", "input(prompt)", b_input)},
                    {"Int", INT_STRUCT},
                    {"Float", FLOAT_STRUCT},
                    {"String", STRING_STRUCT},
                    {"List", LIST_STRUCT},
                    {"Bool", BOOL_STRUCT},
                    {"Nil", NONE_STRUCT},
                    {"debug_break", NATIVE_FUNCTION_STRUCT->create_immortal<NativeFunction>("debug_break", "debug_break()", b_debug_break)},
                    {"__future__", future.build()},
            };

            built = true;
        }

        for (auto const &[name, value]: builtins) {
            globals->set(name, value);
        }
    }


};