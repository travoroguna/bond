//
// Created by travor on 15/07/2023.
//


#include "../object.h"
#include "../conv.hpp"
#include "conversions.h"


namespace bond {
    GcPtr<Module> core_module;

    class Range : public NativeInstance {
    public:
        uint64_t m_start;
        uint64_t m_end;
        uint64_t m_step;

        Range(Int *start, Int *end, Int *step) {
            m_start = start->get_value();
            m_end = end->get_value();
            m_step = step->get_value();
        }

        static obj_result c_Range(const t_vector &args) {
            Int *start;
            Int *end;
            Int *step;

            auto res = parse_args(args, start, end, step);
            TRY(res);

            return OK(make<Range>(start, end, step));
        }

        static obj_result iter(const GcPtr<Object> &Self, const t_vector &args) {
            auto self = Self->as<Range>();
            return self;
        }

        static obj_result has_next(const GcPtr<Object> &Self, const t_vector &args) {
            auto self = Self->as<Range>();
            return OK(AS_BOOL(self->m_start < self->m_end));
        }

        static obj_result next(const GcPtr<Object> &Self, const t_vector &args) {
            auto self = Self->as<Range>();
            auto res = self->m_start;
            self->m_start += self->m_step;
            return OK(make<Int>(res));
        }
    };

    obj_result is_callable(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));

        return OK(AS_BOOL(obj->is<Function>() || obj->is<NativeFunction>() || obj->is<NativeStruct>() ||
                          obj->is<Struct>() || obj->is<BoundMethod>()));
    }

    obj_result is_function(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));

        return OK(AS_BOOL(obj->is<Function>()));
    }

    void build_core_module() {
        auto mod = Mod("core");

        mod.function("to_string", to_string,
                     "to_string(object: Any) -> Sting\nconverts any type to its string representation");
        mod.function("to_int", to_int, "to_int(object: Any) -> String!Int\ntrys to convert any type to an integer");
        mod.function("to_float", to_int, "to_int(object: Any) -> String!Float\ntrys to convert any type to an float");
        mod.function("is_callable", is_callable,
                     "is_callable(object: Any) -> Bool\nreturns true if the object is callable");
        mod.function("is_function", is_function, "is_function(object: Any) -> Bool\nreturns true if the object is a function");

        mod.struct_("Range", "Range(start: Int, end: Int, step: Int)")
                .constructor(Range::c_Range)
                .method("__iter__", Range::iter, "__iter__() -> Range\nreturns an iterator for the range")
                .method("__has_next__", Range::has_next,
                        "__has_next__() -> Bool\nreturns true if the range has a next element")
                .method("__next__", Range::next, "__next__() -> Int\nreturns the next element of the range");


        core_module = mod.build();
    }

}