#include "../object.h"
#include "../runtime.h"


namespace bond {

    auto NONE_CONST = make_immortal<None>();

    obj_result None_construct(const t_vector &args) {
        if (!args.empty()) {
            return runtime_error(fmt::format("None() takes 0 arguments, {} given", args.size()));
        }
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

    void init_none() {
        Runtime::ins()->NONE_STRUCT = make_immortal<NativeStruct>("None", "None()", None_construct,
                                                                      method_map{
                                                                              {"__eq__", {None_eq, "eq(other)"}},
                                                                              {"__ne__", {None_ne, "ne(other)"}},
                                                                      }
        );
        Runtime::ins()->C_NONE = Runtime::ins()->NONE_STRUCT->create({}).value()->as<None>();
    }
}