#include "../object.h"

namespace bond {
    obj_result Int_construct(const t_vector &args) {
        Int *num;

        auto opt = parse_args(args, num);
        TRY(opt);

        return GcPtr<Object>(num);
    }

    obj_result Int_add(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Int *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return make_int(self_num->get_value() + other->get_value());
    }

    obj_result Int_sub(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Int *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return make_int(self_num->get_value() - other->get_value());
    }


    obj_result Int_mul(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Int *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return make_int(self_num->get_value() * other->get_value());
    }


    obj_result Int_div(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Int *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (other->get_value() == 0)
            return ERR("Division by zero");

        return make_int(self_num->get_value() / other->get_value());
    }

    obj_result Int_mod(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Int *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (other->get_value() == 0)
            return ERR("Division by zero");

        return make_int(self_num->get_value() % other->get_value());
    }

    obj_result Int_lt(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Int *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return AS_BOOL(self_num->get_value() < other->get_value());
    }

    obj_result Int_eq(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Int>())
            return AS_BOOL(false);

        return AS_BOOL(self_num->get_value() == other->as<Int>()->get_value());
    }

    obj_result Int_ne(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Int>())
            return AS_BOOL(true);

        return AS_BOOL(self_num->get_value() != other->as<Int>()->get_value());
    }

    obj_result Int_gt(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Int>())
            return AS_BOOL(false);

        return AS_BOOL(self_num->get_value() > other->as<Int>()->get_value());
    }

    obj_result Int_le(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Int>())
            return AS_BOOL(false);

        return AS_BOOL(self_num->get_value() <= other->as<Int>()->get_value());
    }

    obj_result Int_ge(const GcPtr<Object>& self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Object *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (!other->is<Int>())
            return AS_BOOL(false);

        return AS_BOOL(self_num->get_value() >= other->as<Int>()->get_value());
    }

    obj_result Int_hash(const GcPtr<Object>& self, const t_vector &args) {
        TRY(parse_args(args));
        auto self_num = self->as<Int>();
        return make_int(std::hash<int64_t>{}(self_num->get_value()));
    }


    GcPtr<NativeStruct> INT_STRUCT = make_immortal<NativeStruct>("Int", "Int(value)", Int_construct, method_map {
            {"__add__", {Int_add, "add(other)"}},
            {"__sub__", {Int_sub, "sub(other)"}},
            {"__mul__", {Int_mul, "mul(other)"}},
            {"__div__", {Int_div, "div(other)"}},
            {"__mod__", {Int_mod, "mod(other)"}},
            {"__lt__", {Int_lt, "__lt__(other)"}},
            {"__eq__", {Int_eq, "__eq__(other)"}},
            {"__ne__", {Int_ne, "__ne__(other)"}},
            {"__gt__", {Int_gt, "__gt__(other)"}},
            {"__le__", {Int_le, "__le__(other)"}},
            {"__ge__", {Int_ge, "__ge__(other)"}},
            {"__hash__", {Int_hash, "__hash__()"}}
    });



    GcPtr<Int> make_int(int64_t value) {
        if (value > -1 and value < 256) {
            auto val = int_cache[value];
            return val;
        }

        return INT_STRUCT->create_instance<Int>(value);
    }
}