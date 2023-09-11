#include "../object.h"
#include "../runtime.h"

namespace bond {
    obj_result Int_construct(const t_vector &args) {
        Int *num;

        auto opt = parse_args(args, num);
        TRY(opt);

        return GcPtr<Object>(num);
    }

    obj_result check_args(const t_vector &args) {
        if (args.size() != 1) {
            return ERR("Expected 1 argument");
        }

        if (!args[0]->is_one_of<Int, Float>()) {
            return ERR(fmt::format("Expected Int, got {}", get_type_name(args[0])));
        }

        return OK();
    }

#define apply_int_op(op) \
if (args[0]->is<Int>()) { \
        auto other = args[0]->as<Int>(); \
        return make_int(self_num->get_value() op other->get_value()); \
    } else { \
        auto other = args[0]->as<Float>(); \
        return make_float(self_num->get_value() op other->get_value()); \
    }

#define int_op(op) \
    auto self_num = self->as<Int>(); \
    TRY(check_args(args));           \
    apply_int_op(op)

#define compare_int_op(op) \
    auto self_num = self->as<Int>(); \
    TRY(check_args(args)); \
    if (args[0]->is<Int>()) { \
        auto other = args[0]->as<Int>(); \
        return AS_BOOL(self_num->get_value() op other->get_value()); \
    } else { \
        auto other = args[0]->as<Float>(); \
        return AS_BOOL(self_num->get_value() op other->get_value()); \
    }

#define equality_int_op(op, def) \
    auto self_num = self->as<Int>(); \
    if (!args[0]->is_one_of<Int, Float>()) return AS_BOOL(def); \
    if (args[0]->is<Int>()) { \
        auto other = args[0]->as<Int>(); \
        return AS_BOOL(self_num->get_value() op other->get_value()); \
    } else { \
        auto other = args[0]->as<Float>(); \
        return AS_BOOL(self_num->get_value() op other->get_value()); \
    }

    obj_result Int_add(const GcPtr<Object> &self, const t_vector &args) {
        int_op(+)
    }

    obj_result Int_sub(const GcPtr<Object> &self, const t_vector &args) {
        int_op(-)
    }

    obj_result Int_mul(const GcPtr<Object> &self, const t_vector &args) {
        int_op(*)
    }



    obj_result Int_div(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<Int>();
        TRY(check_args(args));

        if (args[0]->is<Int>()) {
            auto other = args[0]->as<Int>();
            if (other->get_value() == 0)
                return ERR("Division by zero");

            return make_int(self_num->get_value() / other->get_value());
        } else {
            auto other = args[0]->as<Float>();
            if (other->get_value() == 0)
                return ERR("Division by zero");

            return make_float((double)self_num->get_value() / other->get_value());
        }
    }

    obj_result Int_mod(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<Int>();
        Int *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        if (other->get_value() == 0)
            return ERR("Division by zero");

        return make_int(self_num->get_value() % other->get_value());
    }

    obj_result Int_lt(const GcPtr<Object> &self, const t_vector &args) {
        compare_int_op(<)
    }

    obj_result Int_eq(const GcPtr<Object> &self, const t_vector &args) {
        equality_int_op(==, false)
    }

    obj_result Int_ne(const GcPtr<Object> &self, const t_vector &args) {
        equality_int_op(!=, true)
    }

    obj_result Int_gt(const GcPtr<Object> &self, const t_vector &args) {
        compare_int_op(>)
    }

    obj_result Int_le(const GcPtr<Object> &self, const t_vector &args) {
        compare_int_op(<=)
    }

    obj_result Int_ge(const GcPtr<Object> &self, const t_vector &args) {
        compare_int_op(>=)
    }

    obj_result Int_hash(const GcPtr<Object> &self, const t_vector &args) {
        TRY(parse_args(args));
        auto self_num = self->as<Int>();
        return make_int((int64_t)std::hash<int64_t>{}(self_num->get_value()));
    }


    void init_int() {
        Runtime::ins()->INT_STRUCT = make_immortal<NativeStruct>("Int", "Int(value)", Int_construct, method_map{
                {"__add__",  {Int_add,  "add(other)"}},
                {"__sub__",  {Int_sub,  "sub(other)"}},
                {"__mul__",  {Int_mul,  "mul(other)"}},
                {"__div__",  {Int_div,  "div(other)"}},
                {"__mod__",  {Int_mod,  "mod(other)"}},
                {"__lt__",   {Int_lt,   "__lt__(other)"}},
                {"__eq__",   {Int_eq,   "__eq__(other)"}},
                {"__ne__",   {Int_ne,   "__ne__(other)"}},
                {"__gt__",   {Int_gt,   "__gt__(other)"}},
                {"__le__",   {Int_le,   "__le__(other)"}},
                {"__ge__",   {Int_ge,   "__ge__(other)"}},
                {"__hash__", {Int_hash, "__hash__()"}}
        });
    }
}