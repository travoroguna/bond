#include "../object.h"
#include "../runtime.h"

namespace bond {
    obj_result Instance::bind_method(const t_string &name) {
        auto meth = m_type->get_method(name);
        if (!meth)
            return runtime_error("Method " + name + " not found");
        auto m = Runtime::ins()->make_bound_method(this, *meth);
        return OK(m);
    }

    obj_result Instance::get_method(const t_string &name) {
        if (m_type == nullptr) {
            return runtime_error("Type not set");
        }

        auto meth = m_type->get_method(name);
        if (!meth)
            return runtime_error("Method " + name + " not found");
        return OK(*meth);
    }

    obj_result Instance::get_field(const t_string &name) {
        if (m_fields.contains(name)) {
            return OK(m_fields[name]);
        }
        return runtime_error("Field " + name + " not found");
    }

    obj_result Instance::set_field(const t_string &name, const GcPtr<Object> &value) {
        if (m_fields.contains(name)) {
            m_fields[name] = value;
            return OK(value);
        }
        return runtime_error("Field " + name + " not found");
    }

    obj_result Instance::get_type() {
        if (m_type == nullptr) {
            return runtime_error("Type not set");
        }
        return OK(m_type);
    }

    t_string Instance::str() const {
        std::vector<t_string> fields;
        for (auto const &[key, value]: m_fields) {
            if (value.get() == this)
                fields.push_back(key + ": <self>");
            else
                fields.push_back(key + ": " + value->str());
        }
        return fmt::format("{}({})", m_type->get_name(), fmt::join(fields, ", "));
    }

    obj_result I_get_attribute(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<Instance>();
        String *name;
        auto opt = parse_args(args, name);
        TRY(opt);
        auto res = self->get_field(name->get_value());

        if (!res.has_value()) {
            auto meth = self->bind_method(name->get_value());
            if (meth.has_value()) {
                return OK(meth.value());
            }
            return runtime_error("Field " + name->get_value() + " not found");
        }

        return res;
    }

    obj_result I_set_attribute(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<Instance>();
        String *name;
        Object *value;
        auto opt = parse_args(args, name, value);
        TRY(opt);

        return self->set_field(name->get_value(), value);
    }

    obj_result get_type(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<Instance>();
        TRY(parse_args(args));
        return self->get_type();
    }

    // slot wrappers
    //    NE = 0,
    //    EQ,
    //    LE,
    //    GT,
    //    GE,
    //    LT,
    //
    //    BIN_ADD,
    //    BIN_SUB,
    //    BIN_MUL,
    //    BIN_DIV,
    //    BIN_MOD,
    //    UNARY_SUB,
    //
    //    GET_ATTR,
    //    SET_ATTR,
    //    GET_ITEM,
    //    SET_ITEM,
    //
    //    ITER,
    //    NEXT,
    //    HAS_NEXT,

    auto slot_wrapper(const t_string &slot_name) -> NativeMethodPtr {
        return [slot_name](const GcPtr<Object> &Self, const t_vector &args) -> obj_result {
            auto self = Self->as<Instance>();
            TRY(parse_args(args));
            auto res = self->get_method(slot_name);
            if (!res.has_value()) {
                return runtime_error("Method " + slot_name + " not found");
            }
            return OK(res.value());
        };
    }

    void init_instance_method() {
        auto constructor = c_Default<Instance>;
        Runtime::ins()->INSTANCE_STRUCT = make_immortal<NativeStruct>("Instance",
                                                                          "Instance(type: Type, fields: Map)",
                                                                          constructor, method_map{
                        {"__getattr__",  {I_get_attribute,              "__getattr__(name: String)"}},
                        {"__setattr__",  {I_set_attribute,              "__setattr__(name: String, value: Object)"}},
                        {"get_type",     {get_type,                     "get_type()"}},
                        {"__eq__",       {slot_wrapper("__eq__"),       "__eq__(other: Object)"}},
                        {"__ne__",       {slot_wrapper("__ne__"),       "__ne__(other: Object)"}},
                        {"__lt__",       {slot_wrapper("__lt__"),       "__lt__(other: Object)"}},
                        {"__le__",       {slot_wrapper("__le__"),       "__le__(other: Object)"}},
                        {"__gt__",       {slot_wrapper("__gt__"),       "__gt__(other: Object)"}},
                        {"__ge__",       {slot_wrapper("__ge__"),       "__ge__(other: Object)"}},
                        {"__add__",      {slot_wrapper("__add__"),      "__add__(other: Object)"}},
                        {"__sub__",      {slot_wrapper("__sub__"),      "__sub__(other: Object)"}},
                        {"__mul__",      {slot_wrapper("__mul__"),      "__mul__(other: Object)"}},
                        {"__div__",      {slot_wrapper("__div__"),      "__div__(other: Object)"}},
                        {"__mod__",      {slot_wrapper("__mod__"),      "__mod__(other: Object)"}},
                        {"__neg__",      {slot_wrapper("__neg__"),      "__neg__()"}},
                        {"__getitem__",  {slot_wrapper("__getitem__"),  "__getitem__(key: Object)"}},
                        {"__setitem__",  {slot_wrapper("__setitem__"),  "__setitem__(key: Object, value: Object)"}},
                        {"__iter__",     {slot_wrapper("__iter__"),     "__iter__()"}},
                        {"__next__",     {slot_wrapper("__next__"),     "__next__()"}},
                        {"__has_next__", {slot_wrapper("__has_next__"), "__has_next__()"}},
                        {"__hash__",     {slot_wrapper("__hash__"),     "__hash__()"}},
                });

        Runtime::ins()->BOUND_METHOD_STRUCT = make_immortal<NativeStruct>("BoundMethod",
                                                                              "BoundMethod(instance: Instance, method: Function)",
                                                                              c_Default<BoundMethod>);
    }
};