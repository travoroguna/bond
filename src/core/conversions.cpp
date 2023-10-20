//
// Created by travor on 15/07/2023.
//

#include "conversions.h"

namespace bond {
    obj_result to_int(const bond::t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));

        if (obj->is<Int>()) return make_ok(obj);
        if (obj->is<Float>()) return make_ok(make_int((int64_t)obj->as<Float>()->get_value()));
        if (obj->is<String>()) {
            auto str = obj->as<String>()->get_value();
            try {
                return make_ok(make_int(std::stoll(str.c_str())));
            } catch ([[maybe_unused]]std::invalid_argument &e) {
                return make_error(make_string("invalid int"));
            } catch ([[maybe_unused]]std::out_of_range &e) {
                return make_error(make_string("int out of range"));
            }
        }
        return make_error(make_string(fmt::format("unable to convert {} to int", obj->str())));
    }

    obj_result to_string(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));
        return OK(make_string(obj->str()));
    }

    obj_result to_float(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));


        if (obj->is<Float>()) return make_ok(obj);

        return make_error(make_string(fmt::format("unable to convert {} to float", obj->str())));
    }
}