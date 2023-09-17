//
// Created by travor on 17/09/2023.
//
#include "../object.h"
#include "../runtime.h"


namespace bond {
    obj_result decode(const GcPtr<Object>& obj, const t_vector& args) {
        auto self = obj->as<Bytes>();
        TRY(parse_args(args));
        auto the_bytes = self->get_value();
        return make_string(std::string(the_bytes.begin(), the_bytes.end()));
    }

    void init_bytes() {
        auto methods = method_map {
            {"decode", {decode, "decode() -> String"}},
        };

        Runtime::ins()->BYTES_STRUCT = make_immortal<NativeStruct>("Bytes", "Bytes(bytes)", c_Default<Bytes>, methods);
    }
}