#include "../object.h"


namespace bond {
    obj_result Float_construct(const t_vector &args) {
        Float *num;

        auto opt = parse_args(args, num);
        TRY(opt);

        return OK(GcPtr<Object>(num));
    }

    obj_result Float_add(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Float *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return make_float(self_num->get_value() + other->get_value());
    }

    obj_result Float_sub(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Float *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return make_float(self_num->get_value() - other->get_value());
    }


    obj_result Float_mul(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Float *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return make_float(self_num->get_value() * other->get_value());
    }


    obj_result Float_div(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Float *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (other->get_value() == 0)
            return ERR("Division by zero error");

        return make_float(self_num->get_value() / other->get_value());
    }

    obj_result Float_lt(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Float *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return AS_BOOL(self_num->get_value() < other->get_value());
    }

    obj_result Float_eq(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Float>())
            return AS_BOOL(false);

        return AS_BOOL(self_num->get_value() == other->as<Float>()->get_value());
    }

    obj_result Float_ne(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Float>())
            return AS_BOOL(true);

        return AS_BOOL(self_num->get_value() != other->as<Float>()->get_value());
    }

    obj_result Float_gt(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Float>())
            return AS_BOOL(false);

        return AS_BOOL(self_num->get_value() > other->as<Float>()->get_value());
    }

    obj_result Float_le(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Float>())
            return AS_BOOL(false);

        return AS_BOOL(self_num->get_value() <= other->as<Float>()->get_value());
    }

    obj_result Float_ge(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Float>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Float>())
            return AS_BOOL(false);

        return AS_BOOL(self_num->get_value() >= other->as<Float>()->get_value());
    }

    obj_result Float_hash(const GcPtr<Object>& self, const t_vector &args) {
        auto self_str = self->as<Float>();
        TRY(parse_args(args));
        return make_int(std::hash<double>{}(self_str->get_value()));
    }


    GcPtr<NativeStruct> FLOAT_STRUCT = make_immortal<NativeStruct>("Float", "Float(value)", Float_construct, method_map{
            {"__add__", {Float_add, "add(other)"}},
            {"__sub__", {Float_sub, "sub(other)"}},
            {"__mul__", {Float_mul, "mul(other)"}},
            {"__div__", {Float_div, "div(other)"}},
            {"__eq__", {Float_eq, "__eq__"}},
            {"__ne__", {Float_ne, "__ne__"}},
            {"__gt__", {Float_gt, "__gt__"}},
            {"__lt__", {Float_lt, "__lt__"}},
            {"__le__", {Float_le, "__le__"}},
            {"__ge__", {Float_ge, "__ge__"}},
            {"__hash__", {Float_hash, "__hash__"}},
    });

}