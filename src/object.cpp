#include "object.h"

namespace bond {
    void
    NativeStruct::add_methods(const std::unordered_map<std::string, std::pair<NativeMethodPtr, std::string>> &methods) {
        m_methods = methods;
    }

    std::optional<NativeMethodPtr> NativeStruct::get_method(const std::string &name) const {
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

    const std::unordered_map<std::string, Slot> method_mappings = {
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
            {"__hash__", Slot::HASH},
    };

    std::unordered_map<Slot, std::string> swap_map(const std::unordered_map<std::string, Slot> &map) {
        std::unordered_map<Slot, std::string> res;
        for (auto const &[key, value]: map) {
            res[value] = key;
        }
        return res;
    }

    const std::unordered_map<Slot, std::string> slot_str_map = swap_map(method_mappings);

    std::string Slot_to_string(Slot slot) {
        return slot_str_map.at(slot);
    }

    bool Instance::has_slot(Slot slot) {
        return m_type->has_method(slot_str_map.at(slot));
    }


    void NativeStruct::set_slots() {
        std::fill_n(m_slots.data(), Slot::SIZE, nullptr);

        for (auto &[name, method]: m_methods) {
            if (method_mappings.contains(name))
                m_slots[method_mappings.at(name)] = method.first;
        }
    }

    NativeMethodPtr NativeStruct::get_slot(Slot slot) {
        return m_slots[slot];
    }

    std::optional<getter> NativeStruct::get_getter(const std::string &name) const {
        if (!m_attributes.contains(name))
            return std::nullopt;
        auto attr = m_attributes.at(name);
        if (attr.first == nullptr) return std::nullopt;
        return attr.first;
    }
    std::optional<setter> NativeStruct::get_setter(const std::string &name) const{
        if (!m_attributes.contains(name))
            return std::nullopt;
        auto attr = m_attributes.at(name);
        if (attr.second == nullptr) return std::nullopt;
        return attr.second;
    }

    bool NativeStruct::has_method(const std::string &name) const {
        return m_methods.contains(name);
    }

    [[nodiscard]] obj_result NativeInstance::call_method(const std::string &name, const t_vector &args) {
        auto method = m_native_struct->get_method(name);
        if (!method)
            return ERR("Method " + name + " not found");
        auto res = (*method)(this, args);
        TRY(res);
        return res;
    }

    // is this better than call_method ?
    obj_result NativeInstance::call_slot(Slot slot, const t_vector &args) {
        auto method = m_native_struct->get_slot(slot);
        if (!method)
            return ERR("not implemented");
        assert(method);
        auto res = method(this, args);
        TRY(res);
        return res;
    }

    bool NativeInstance::has_slot(Slot slot) {
        return m_native_struct->get_slot(slot) != nullptr;
    }

    std::optional<obj_result> NativeInstance::get_attr(const std::string &name) {
        auto getter = m_native_struct->get_getter(name);
        if (!getter)
            return std::nullopt;
        return (*getter)(this);
    }

    std::optional<obj_result> NativeInstance::set_attr(const std::string &name, const GcPtr<Object> &value) {
        auto setter = m_native_struct->get_setter(name);
        if (!setter)
            return std::nullopt;
        return (*setter)(this, value);
    }

    bool NativeInstance::has_method(const std::string &name) {
        return m_native_struct->has_method(name);
    }


    obj_result OK() {
        return C_NONE;
    }

    obj_result ERR(std::string error) {
        return std::unexpected(std::move(error));
    }

    //why is this here?
    obj_result OK(const GcPtr<Object> &object) {
        return object;
    }

    GcPtr<Float> make_float(double value) {
        return FLOAT_STRUCT->create_instance<Float>(value);
    }

    GcPtr<String> make_string(std::string value) {
        return STRING_STRUCT->create_instance<String>(std::move(value));
    }



    GcPtr<Int> int_cache[256];

    void init_caches() {
        for (int i = 0; i < 256; i++) {
            int_cache[i] = INT_STRUCT->create_immortal<Int>(i);
        }
    }

    GcPtr<NativeFunction> make_native_function(std::string name, std::string doc, const NativeFunctionPtr& function) {
        return NATIVE_FUNCTION_STRUCT->create_instance<NativeFunction>(std::move(name), std::move(doc), function);
    }

    GcPtr<Module> make_module(std::string path, const GcPtr<StringMap> &globals) {
        return MODULE_STRUCT->create_instance<Module>(std::move(path), globals);
    }

    GcPtr<NativeStruct> make_native_struct(std::string name, std::string doc, NativeFunctionPtr constructor, const method_map& methods) {
        return make<NativeStruct>(std::move(name), std::move(doc), std::move(constructor), std::move(methods));
    }

    std::string get_type_name(const GcPtr<Object> &obj) {
        if (obj->is<Instance>()){
            return obj->as<Instance>()->get_struct()->get_name();
        }
        else if (obj->is<Struct>()) {
            return obj->as<Struct>()->get_name();
        }
        else if (obj->is<NativeInstance>()) {
            return obj->as<NativeInstance>()->get_native_struct()->get_name();
        }
        else if (obj->is<NativeStruct>()) {
            return obj->as<NativeStruct>()->get_name();
        }
        return obj.type_name();
    }

    GcPtr<List> make_list(const t_vector &values) {
        return LIST_STRUCT->create_instance<List>(values);
    }

}

std::string format_(const std::string &format_string, const std::vector<fmt::format_context::format_arg> &args) {
    return format_impl(format_string, args);
}

std::expected<std::string, std::string> bond_format(const std::string &format_string, const bond::t_vector &args) {
    std::vector<fmt::format_context::format_arg> fmt_args;

    for (auto &arg: args) {
        fmt::format_context::format_arg a;

        if (arg->is<bond::Int>()) {
            a = fmt::detail::make_arg<fmt::format_context>(arg->as<bond::Int>()->get_value());
        }
        else if(arg->is<bond::Float>()) {
            a = fmt::detail::make_arg<fmt::format_context>(arg->as<bond::Float>()->get_value());
        }
        else if(arg->is<bond::Bool>()) {
            a = fmt::detail::make_arg<fmt::format_context>(arg->as<bond::Bool>()->get_value());
        }
        else {
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
