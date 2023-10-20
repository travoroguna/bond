#include "../object.h"
#include "../runtime.h"


namespace bond {
    obj_result Float_construct(const t_vector &args) {
        Float *num;

        auto opt = parse_args(args, num);
        TRY(opt);

        return OK(GcPtr<Object>(num));
    }

    obj_result check_args_f(const t_vector &args) {
        if (args.size() != 1) {
            return runtime_error("Expected 1 argument");
        }

        if (!args[0]->is_one_of<Int, Float>()) {
            return runtime_error(fmt::format("Expected Float, got {}", get_type_name(args[0])));
        }

        return OK();
    }

#define apply_float_op(op) \
if (args[0]->is<Float>()) { \
        auto other = args[0]->as<Float>(); \
        return make_float(self_num->get_value() op other->get_value()); \
    } else { \
        auto other = args[0]->as<Int>(); \
        return make_float(self_num->get_value() op other->get_value()); \
    }

#define float_op(op) \
    auto self_num = self->as<Float>(); \
    TRY(check_args_f(args));           \
    apply_float_op(op)

#define compare_float_op(op) \
    auto self_num = self->as<Float>(); \
    TRY(check_args_f(args)); \
    if (args[0]->is<Float>()) { \
        auto other = args[0]->as<Float>(); \
        return AS_BOOL(self_num->get_value() op other->get_value()); \
    } else { \
        auto other = args[0]->as<Int>(); \
        return AS_BOOL(self_num->get_value() op other->get_value()); \
    }

#define equality_float_op(op, def) \
    auto self_num = self->as<Float>(); \
    if (!args[0]->is_one_of<Int, Float>()) return AS_BOOL(def); \
    if (args[0]->is<Float>()) { \
        auto other = args[0]->as<Float>(); \
        return AS_BOOL(self_num->get_value() op other->get_value()); \
    } else { \
        auto other = args[0]->as<Int>(); \
        return AS_BOOL(self_num->get_value() op other->get_value()); \
    }

    obj_result Float_add(const GcPtr<Object> &self, const t_vector &args) {
        float_op(+)
    }

    obj_result Float_sub(const GcPtr<Object> &self, const t_vector &args) {
        float_op(-)
    }


    obj_result Float_mul(const GcPtr<Object> &self, const t_vector &args) {
       float_op(*)
    }


    obj_result Float_div(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<Float>();
        TRY(check_args_f(args));

        if (args[0]->is<Float>()) {
            auto other = args[0]->as<Float>();
            if (other->get_value() == 0) {
                return runtime_error("Attempted to divide by zero");
            }

            return make_float(self_num->get_value() / other->get_value());
        } else {
            auto other = args[0]->as<Int>();
            if (other->get_value() == 0) {
                return runtime_error("Attempted to divide by zero");
            }

            return make_float(self_num->get_value() / (double)other->get_value());
        }
    }

    obj_result Float_lt(const GcPtr<Object>& self, const t_vector &args) {
        compare_float_op(<)
    }

    obj_result Float_eq(const GcPtr<Object>& self, const t_vector &args) {
        equality_float_op(==, false)
    }

    obj_result Float_ne(const GcPtr<Object>& self, const t_vector &args) {
        equality_float_op(!=, true)
    }

    obj_result Float_gt(const GcPtr<Object>& self, const t_vector &args) {
        compare_float_op(>)
    }

    obj_result Float_le(const GcPtr<Object>& self, const t_vector &args) {
        compare_float_op(<=)
    }

    obj_result Float_ge(const GcPtr<Object>& self, const t_vector &args) {
        compare_float_op(>=)
    }

    obj_result Float_hash(const GcPtr<Object>& self, const t_vector &args) {
        auto self_str = self->as<Float>();
        TRY(parse_args(args));
        return make_int((int64_t )std::hash<double>{}(self_str->get_value()));
    }


    void init_float() {
        auto methods = method_map{
                {"__add__",  {Float_add,  "add(other)"}},
                {"__sub__",  {Float_sub,  "sub(other)"}},
                {"__mul__",  {Float_mul,  "mul(other)"}},
                {"__div__",  {Float_div,  "div(other)"}},

                {"__i_add__", {Float_add, "iadd(other)"}},
                {"__i_sub__", {Float_sub, "isub(other)"}},
                {"__i_mul__", {Float_mul, "imul(other)"}},
                {"__i_div__", {Float_div, "idiv(other)"}},

                {"__eq__",   {Float_eq,   "__eq__"}},
                {"__ne__",   {Float_ne,   "__ne__"}},
                {"__gt__",   {Float_gt,   "__gt__"}},
                {"__lt__",   {Float_lt,   "__lt__"}},
                {"__le__",   {Float_le,   "__le__"}},
                {"__ge__",   {Float_ge,   "__ge__"}},
                {"__hash__", {Float_hash, "__hash__"}},
        };

        Runtime::ins()->FLOAT_STRUCT = make_immortal<NativeStruct>("Float", "Float(value)", Float_construct, methods);
    }

}