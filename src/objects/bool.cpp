#include "../object.h"
#include "../runtime.h"


namespace bond {
    obj_result Bool_construct(const t_vector &args) {
        if (args.size() != 1) {
            return ERR("Bool constructor takes exactly one argument");
        }

        const auto& arg = args[0];

        if (instanceof<Int>(arg.get())) {
            auto arg_int = arg->as<Int>();
            return OK(AS_BOOL(arg_int->get_value() != 0));
        } else if (instanceof<Float>(arg.get())) {
            auto arg_float = arg->as<Float>();
            return OK(AS_BOOL(arg_float->get_value() != 0));
        } else if (instanceof<Bool>(arg.get())) {
            auto arg_bool = arg->as<Bool>();
            return OK(AS_BOOL(arg_bool->get_value()));
        } else {
            return OK(Runtime::ins()->C_TRUE);
        }
    }

    auto TRUE_CONST = make_immortal<Bool>(true);
    auto FALSE_CONST = make_immortal<Bool>(false);

    obj_result Bool_eq(const GcPtr<Object>& self, const t_vector &args) {
        auto self_bool = self->as<Bool>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (other->is<Bool>()) {
            auto other_bool = other->as<Bool>();
            return OK(AS_BOOL(self_bool->get_value() == other_bool->get_value()));
        } else {
            return OK(FALSE_CONST);
        }

    }

    obj_result Bool_ne(const GcPtr<Object>& self, const t_vector &args) {
        auto self_bool = self->as<Bool>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (other->is<Bool>()) {
            auto other_bool = other->as<Bool>();
            return OK(AS_BOOL(self_bool->get_value() != other_bool->get_value()));
        } else {
            return OK(TRUE_CONST);
        }

    }

    void init_bool() {
        Runtime::ins()->BOOL_STRUCT = make_immortal<NativeStruct>("Bool", "Bool(value)", Bool_construct, method_map{
                                                                              {"__eq__", {Bool_eq, "__eq__()"}},
                                                                              {"__ne__", {Bool_ne, "__ne__()"}},
                                                                      }
        );
        Runtime::ins()->C_TRUE = Runtime::ins()->BOOL_STRUCT->create_immortal<Bool>(true);
        Runtime::ins()->C_FALSE = Runtime::ins()->BOOL_STRUCT->create_immortal<Bool>(false);
    }
}