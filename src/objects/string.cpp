#include "../object.h"
#include "../runtime.h"


namespace bond {
    obj_result String_construct(const t_vector &args) {
        String *num;

        auto opt = parse_args(args, num);
        TRY(opt);

        return GcPtr<Object>(num);
    }

    obj_result String_add(const GcPtr<Object> &self, const t_vector &args) {
        auto self_num = self->as<String>();
        String *other;

        auto opt = parse_args(args, other);
        TRY(opt);

        return make_string(fmt::format("{}{}", self_num->get_value().c_str(), other->get_value().c_str()));
    }

    obj_result String_join(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();

        std::vector<t_string> strings;

        for (auto &item: args) {
            strings.push_back(item->str());
        }

        return make_string(fmt::format("{}", fmt::join(strings, self_str->get_value())));
    }

    obj_result String_size(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        TRY(parse_args(args));
        return make_int(self_str->get_value().size());
    }

    obj_result String_get_item(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        Int *index;
        TRY(parse_args(args, index));

        if (index->get_value() < 0 or index->get_value() > self_str->get_value().size() - 1) {
            return runtime_error(fmt::format("Index {} out of range", index->get_value()));
        }

        return make_string(fmt::format("{}", self_str->get_value()[index->get_value()]));
    }

    obj_result String_sub_string(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        Int *start;
        Int *end;
        TRY(parse_args(args, start, end));

        if (start->get_value() < 0 or start->get_value() > self_str->get_value().size() - 1) {
            return runtime_error(fmt::format("Index {} out of range", start->get_value()));
        }

        if (end->get_value() < 0 or end->get_value() > self_str->get_value().size() - 1) {
            return runtime_error(fmt::format("Index {} out of range", end->get_value()));
        }

        t_string sub = self_str->get_value_ref().substr(start->get_value(), end->get_value());
        return make_string(sub);
    }

    obj_result String_it_next(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<StringIterator>();
        TRY(parse_args(args));
        return make_string(fmt::format("{}", self_str->m_value[self_str->m_index++]));
    }

    obj_result String_it_has_next(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<StringIterator>();
        TRY(parse_args(args));
        return AS_BOOL(self_str->m_index < self_str->m_value.size());
    }

    auto STRING_ITER_STRUCT = make_immortal<NativeStruct>("StringIter", "StringIterator(value)",
                                                          c_Default<StringIterator>, method_map{
                    {"__next__",     {String_it_next,     "__next__() -> String"}},
                    {"__has_next__", {String_it_has_next, "__has_next__() -> Bool"}},
            });

    obj_result String_iter(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        TRY(parse_args(args));
        return OK(STRING_ITER_STRUCT->create_instance<StringIterator>(self_str->get_value()));
    }

    obj_result String_eq(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        String *other;
        TRY(parse_args(args, other));
        return AS_BOOL(self_str->get_value() == other->get_value());
    }

    obj_result String_neq(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        String *other;
        TRY(parse_args(args, other));
        return AS_BOOL(self_str->get_value() != other->get_value());
    }

    obj_result String_hash(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        TRY(parse_args(args));
        return make_int(std::hash<t_string>{}(self_str->get_value()));
    }

    obj_result String_is_digit(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        TRY(parse_args(args));
        auto &val = self_str->get_value_ref();
        return AS_BOOL(std::all_of(val.begin(), val.end(), ::isdigit));
    }

    obj_result String_is_alpha(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        TRY(parse_args(args));
        auto &val = self_str->get_value_ref();
        return AS_BOOL(std::all_of(val.begin(), val.end(), ::isalpha));
    }

    obj_result String_encode(const GcPtr<Object> &self, const t_vector &args) {
        auto self_str = self->as<String>();
        TRY(parse_args(args));
        auto &val = self_str->get_value_ref();
        return Runtime::ins()->make_bytes(std::vector<uint8_t>(val.begin(), val.end()));
    }

    void init_string() {
        auto methods = method_map{
                method_map{
                        {"__add__",     {String_add,        "__add__(other: String) -> String"}},
                        {"__iter__",    {String_iter,       "__iter__() -> StringIterator"}},
                        {"__getitem__", {String_get_item,   "__get_item__(index: Int) -> String"}},
                        {"__eq__",      {String_eq,         "__eq__(other: String) -> Bool"}},
                        {"__ne__",      {String_neq,        "__neq__(other: String) -> Bool"}},
                        {"size",        {String_size,       "size() -> Int"}},
                        {"__hash__",    {String_hash,       "__hash__() -> Int"}},
                        {"join",        {String_join,       "join(*args: List<Any>) -> String"}},
                        {"sub_string",  {String_sub_string, "sub_string(start: Int, end: Int) -> String"}},
                        {"encode",      {String_encode,     "encode() -> Bytes"}},
                        {"is_digit",    {String_is_digit,   "is_digit() -> Bool"}},
                        {"is_alpha",    {String_is_alpha,   "is_alpha() -> Bool"}},
                }};

        Runtime::ins()->STRING_STRUCT = make_immortal<NativeStruct>("String", "String(value)", String_construct,
                                                                    methods);
    }

};