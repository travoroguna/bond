#include "object.h"
#include "runtime.h"

namespace bond {
    void
    NativeStruct::add_methods(const std::unordered_map<t_string, std::pair<NativeMethodPtr, t_string>> &methods) {
        m_methods = methods;
    }

    std::optional<NativeMethodPtr> NativeStruct::get_method(const t_string &name) const {
        if (!m_methods.contains(name))
            return std::nullopt;
        return m_methods.at(name).first;
    }

    obj_result NativeStruct::create(const t_vector &args) const {
        auto res = m_constructor(args);
        TRY(res);
        auto as_ins = res.value()->as<NativeInstance>();
        assert(as_ins);
        res.value()->as<NativeInstance>()->set_native_struct(const_cast<NativeStruct *>(this));
        return res;
    }


    std::unordered_map<Slot, t_string> swap_map(const std::unordered_map<t_string, Slot> &map) {
        std::unordered_map<Slot, t_string> res;
        for (auto const &[key, value]: map) {
            res[value] = key;
        }
        return res;
    }


    t_string Slot_to_string(Slot slot) {
        const std::unordered_map<t_string, Slot> method_mappings = {
                {"__ne__",       Slot::NE},
                {"__eq__",       Slot::EQ},
                {"__lt__",       Slot::LT},
                {"__gt__",       Slot::GT},
                {"__le__",       Slot::LE},
                {"__ge__",       Slot::GE},
                {"__add__",      Slot::BIN_ADD},
                {"__sub__",      Slot::BIN_SUB},
                {"__mul__",      Slot::BIN_MUL},
                {"__div__",      Slot::BIN_DIV},
                {"__mod__",      Slot::BIN_MOD},
                {"__getattr__",  Slot::GET_ATTR},
                {"__setattr__",  Slot::SET_ATTR},
                {"__getitem__",  Slot::GET_ITEM},
                {"__setitem__",  Slot::SET_ITEM},
                {"__iter__",     Slot::ITER},
                {"__next__",     Slot::NEXT},
                {"__has_next__", Slot::HAS_NEXT},
                {"__hash__",     Slot::HASH},
        };

        const std::unordered_map<Slot, t_string> slot_str_map = swap_map(method_mappings);
        return slot_str_map.at(slot);
    }

    bool Instance::has_slot(Slot slot) {
        const std::unordered_map<t_string, Slot> method_mappings = {
                {"__ne__",       Slot::NE},
                {"__eq__",       Slot::EQ},
                {"__lt__",       Slot::LT},
                {"__gt__",       Slot::GT},
                {"__le__",       Slot::LE},
                {"__ge__",       Slot::GE},
                {"__add__",      Slot::BIN_ADD},
                {"__sub__",      Slot::BIN_SUB},
                {"__mul__",      Slot::BIN_MUL},
                {"__div__",      Slot::BIN_DIV},
                {"__mod__",      Slot::BIN_MOD},
                {"__getattr__",  Slot::GET_ATTR},
                {"__setattr__",  Slot::SET_ATTR},
                {"__getitem__",  Slot::GET_ITEM},
                {"__setitem__",  Slot::SET_ITEM},
                {"__iter__",     Slot::ITER},
                {"__next__",     Slot::NEXT},
                {"__has_next__", Slot::HAS_NEXT},
                {"__hash__",     Slot::HASH},
        };

        const std::unordered_map<Slot, t_string> slot_str_map = swap_map(method_mappings);
        return m_type->has_method(slot_str_map.at(slot));
    }


    void NativeStruct::set_slots() {
        const std::unordered_map<t_string, Slot> method_mappings = {
                {"__ne__",       Slot::NE},
                {"__eq__",       Slot::EQ},
                {"__lt__",       Slot::LT},
                {"__gt__",       Slot::GT},
                {"__le__",       Slot::LE},
                {"__ge__",       Slot::GE},
                {"__add__",      Slot::BIN_ADD},
                {"__sub__",      Slot::BIN_SUB},
                {"__mul__",      Slot::BIN_MUL},
                {"__div__",      Slot::BIN_DIV},
                {"__mod__",      Slot::BIN_MOD},
                {"__getattr__",  Slot::GET_ATTR},
                {"__setattr__",  Slot::SET_ATTR},
                {"__getitem__",  Slot::GET_ITEM},
                {"__setitem__",  Slot::SET_ITEM},
                {"__iter__",     Slot::ITER},
                {"__next__",     Slot::NEXT},
                {"__has_next__", Slot::HAS_NEXT},
                {"__hash__",     Slot::HASH},
        };
        std::fill_n(m_slots.data(), Slot::SIZE, nullptr);

        for (auto &[name, method]: m_methods) {
            if (method_mappings.contains(name))
                m_slots[method_mappings.at(name)] = method.first;
        }
    }

    NativeMethodPtr NativeStruct::get_slot(Slot slot) {
        return m_slots[slot];
    }

    std::optional<getter> NativeStruct::get_getter(const t_string &name) const {
        if (!m_attributes.contains(name))
            return std::nullopt;
        auto attr = m_attributes.at(name);
        if (attr.first == nullptr) return std::nullopt;
        return attr.first;
    }

    std::optional<setter> NativeStruct::get_setter(const t_string &name) const {
        if (!m_attributes.contains(name))
            return std::nullopt;
        auto attr = m_attributes.at(name);
        if (attr.second == nullptr) return std::nullopt;
        return attr.second;
    }

    bool NativeStruct::has_method(const t_string &name) const {
        return m_methods.contains(name);
    }

    void NativeStruct::add_static_method(const t_string& name, const NativeFunctionPtr &s_meth, const t_string &doc) {
        m_static_methods[name] = {s_meth, doc};
    }

    std::expected<NativeFunctionPtr, t_string> NativeStruct::get_static_method(const t_string& name) {
        if (!m_static_methods.contains(name))
            return std::unexpected(fmt::format("static method {} not found", name));
        return m_static_methods.at(name).first;
    }

    [[nodiscard]] obj_result NativeInstance::call_method(const t_string &name, const t_vector &args) {
        auto method = m_native_struct->get_method(name);
        if (!method)
            return runtime_error(fmt::format("method {} not found", name));
        auto res = (*method)(this, args);
        TRY(res);
        return res;
    }

    // is this better than call_method ?
    obj_result NativeInstance::call_slot(Slot slot, const t_vector &args) {
        auto method = m_native_struct->get_slot(slot);
        if (!method)
            return runtime_error("not implemented");
        assert(method);
        auto res = method(this, args);
        TRY(res);
        return res;
    }

    bool NativeInstance::has_slot(Slot slot) {
        return m_native_struct->get_slot(slot) != nullptr;
    }

    std::optional<obj_result> NativeInstance::get_attr(const t_string &name) {
        auto getter = m_native_struct->get_getter(name);
        if (!getter)
            return std::nullopt;
        return (*getter)(this);
    }

    std::optional<obj_result> NativeInstance::set_attr(const t_string &name, const GcPtr<Object> &value) {
        auto setter = m_native_struct->get_setter(name);
        if (!setter)
            return std::nullopt;
        return (*setter)(this, value);
    }

    bool NativeInstance::has_method(const t_string &name) {
        return m_native_struct->has_method(name);
    }


    obj_result OK() {
        return Runtime::ins()->C_NONE;
    }

    obj_result runtime_error(t_string error) {
        return std::unexpected(std::move(error));
    }

    obj_result ERR(const std::string &error) {
        return std::unexpected(error);
    }

    //why is this here?
    obj_result OK(const GcPtr<Object> &object) {
        return object;
    }


    t_string get_type_name(const GcPtr<Object> &obj) {
        if (obj->is<Instance>()) {
            return obj->as<Instance>()->get_struct()->get_name();
        } else if (obj->is<Struct>()) {
            return obj->as<Struct>()->get_name();
        } else if (obj->is<NativeInstance>()) {
            return obj->as<NativeInstance>()->get_native_struct()->get_name();
        } else if (obj->is<NativeStruct>()) {
            return obj->as<NativeStruct>()->get_name();
        }
        return obj.type_name();
    }

}

t_string format_(const t_string &format_string, const std::vector<fmt::format_context::format_arg> &args) {
    return format_impl(format_string, args);
}

std::expected<t_string, t_string> bond_format(const t_string &format_string, const bond::t_vector &args) {
    std::vector<fmt::format_context::format_arg> fmt_args;

    for (auto &arg: args) {
        fmt::format_context::format_arg a;

        if (arg->is<bond::Int>()) {
            a = fmt::detail::make_arg<fmt::format_context>(arg->as<bond::Int>()->get_value());
        } else if (arg->is<bond::Float>()) {
            a = fmt::detail::make_arg<fmt::format_context>(arg->as<bond::Float>()->get_value());
        } else if (arg->is<bond::Bool>()) {
            a = fmt::detail::make_arg<fmt::format_context>(arg->as<bond::Bool>()->get_value());
        } else {
            a = fmt::detail::make_arg<fmt::format_context>(arg);
        }

        fmt_args.push_back(a);
    }

    try {
        return format_(format_string, fmt_args);
    }
    catch (fmt::format_error &e) {
        return std::unexpected(e.what());
    }
}
