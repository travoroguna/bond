#include "../object.h"


namespace bond {

    auto NONE_CONST = make_immortal<None>();

    obj_result None_construct(const t_vector &args) {
        auto opt = parse_args(args);
        TRY(opt);
        return NONE_CONST;
    }

    obj_result None_eq(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<None>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return AS_BOOL(other->is<None>());
    }

    obj_result None_ne(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<None>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return AS_BOOL(!other->is<None>());
    }

    GcPtr<NativeStruct> NONE_STRUCT = make_immortal<NativeStruct>("None", "None()", None_construct,
      method_map {
              {"__eq__", {None_eq, "eq(other)"}},
              {"__ne__", {None_ne, "ne(other)"}},
         }
       );
    GcPtr<None> C_NONE = NONE_STRUCT->create({}).value()->as<None>();
}