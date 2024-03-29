//
// Created by travor on 14/07/2023.
//

#pragma once

#include "object.h"
#include "runtime.h"
#include <type_traits>

namespace bond {
    template<typename R, typename = void>
    struct bond_traits {
        static R unwrap(const GcPtr<Object> &object) = delete;
        static GcPtr<Object> wrap(const R &object) = delete;
        static bool can_unwrap(const GcPtr<Object> &object) = delete;
    };

    template<>
    struct bond_traits<GcPtr<Object>> {
        static GcPtr<Object> unwrap(const GcPtr<Object> &object) { return object; }

        static GcPtr<Object> wrap(const GcPtr<Object> &object) { return object; }

        static bool can_unwrap(const GcPtr<Object> &object) { return true; }

        using type = GcPtr<Object>;
    };

    template<typename Integer>
    struct bond_traits<Integer, std::enable_if_t<std::is_integral_v<Integer> && sizeof(Integer) <= sizeof(int64_t)>> {
        static Integer unwrap(const GcPtr<Object> &object) {
            return object->as<Int>()->get_value();
        }

        static GcPtr<Int> wrap(const Integer &object) {
            return make_int(object);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<Int>(); }

        using type = Int;
    };


    template<>
    struct bond_traits<bool> {
        static bool unwrap(const GcPtr<Object> &object) {
            return object->as<Bool>()->get_value();
        }

        static GcPtr<Object> wrap(const bool &object) {
            return AS_BOOL(object);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<Bool>(); }

        using type = bool;
    };

    template<>
    struct bond_traits<double> {
        static double unwrap(const GcPtr<Object> &object) {
            return object->as<Float>()->get_value();
        }

        static GcPtr<Float> wrap(const double &object) {
            return make_float(object);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<Float>(); }

        using type = Float;
    };

    template<>
    struct bond_traits<float> {
        static double unwrap(const GcPtr<Object> &object) {
            return object->as<Float>()->get_value();
        }

        static GcPtr<Object> wrap(const double &object) {
            return make_float(object);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<Float>(); }

        using type = Float;
    };

    template<>
    struct bond_traits<t_string> {
        static t_string unwrap(const GcPtr<Object> &object) {
            return object->as<String>()->get_value().c_str();
        }

        static GcPtr<Object> wrap(const t_string &object) {
            return make_string(object);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<String>(); }
    };

    template<>
    struct bond_traits<std::string> {
        static std::string unwrap(const GcPtr<Object> &object) {
            return object->as<String>()->get_value().c_str();
        }

        static GcPtr<Object> wrap(const t_string &object) {
            return make_string(object);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<String>(); }
    };

    template<>
    struct bond_traits<std::optional<GcPtr<Object>>> {
        static std::optional<GcPtr<Object>> unwrap(const GcPtr<Object> &object) {
            if (object->is<None>()) {
                return std::nullopt;
            }
            return object;
        }

        static GcPtr<Object> wrap(const std::optional<GcPtr<Object>> &object) {
            if (object.has_value()) {
                return object.value();
            }
            return Runtime::ins()->C_NONE;
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return true; }
    };

    template<>
    struct bond_traits<t_map> {
        static t_map unwrap(const GcPtr<Object> &object) {
            auto obj = object->as<StringMap>();
            return obj->get_value();
        }

        static GcPtr<Object> wrap(const t_map &object) {
            return Runtime::ins()->make_string_map(object);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<StringMap>(); }
    };

    template<>
    struct bond_traits<t_vector> {
        static t_vector unwrap(const GcPtr<Object> &object) {
            auto obj = object->as<List>();
            return obj->get_elements();
        }

        static GcPtr<Object> wrap(const t_vector &object) {
            return Runtime::ins()->make_list(object);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<List>(); }
    };

    template<>
    struct bond_traits<const char *> {
        static t_string unwrap(const GcPtr<Object> &object) {
            auto obj = object->as<String>();
            return obj->get_value();
        }

        static GcPtr<Object> wrap(const char *object) {
            return make_string(object);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<String>(); }
    };

    template<>
    struct bond_traits<void> {
        static GcPtr<None> wrap() {
            return Runtime::ins()->C_NONE;
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<None>(); }
    };

    template<typename T>
    struct bond_traits<std::expected<GcPtr<T>, t_string>> {
        static GcPtr<T> unwrap(const GcPtr<Object> &t) {
            return t->as<T>();
        }

        static std::expected<GcPtr<Object>, t_string>

        wrap(const GcPtr<T> &t) {
            return t;
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return true; }

    };

    template<typename R, typename... Args>
    struct bond_traits<std::function<R(Args...)>> {
        static std::function<R(Args...)> unwrap(const GcPtr<Object> &t) {
            return t->as<NativeFunction>()->get_function();
        }

        static GcPtr<NativeFunction> wrap(const t_string& name, const t_string& doc, const std::function<R(Args...)> &t) {
            return make<NativeFunction>(name, doc, t);
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<NativeFunction>(); }
    };

    template<auto F, bool PassThis = false>
    struct f_wrapper {
       t_string name;
       t_string doc;
    };


    using arg_list = std::vector<GcPtr<Object>>;
    using return_type = std::expected<GcPtr<Object>, t_string>;

    template<typename T>
    auto do_unwrap(const GcPtr<Object>& obj, size_t& i){
        if (!bond_traits<T>::can_unwrap(obj)) {
            throw fmt::format("Expected argument {} to be of type {} but got {}", i, bond_traits<T>::type::name(), get_type_name(obj));
        }
        i--;
        return bond_traits<T>::unwrap(obj);
    }


    template<typename R, typename... Args, R (*F)(Args...)>
    struct bond_traits<f_wrapper<F, false>> {
        static GcPtr<NativeFunction> wrap(const f_wrapper<F, false> &f_w) {
            return make<NativeFunction>(f_w.name, f_w.doc, wrap_impl);
        }

        static NativeFunctionPtr wrap_to_fptr(const f_wrapper<F, false> &f_w) {
            return wrap_impl;
        }

        static return_type wrap_impl(const t_vector &args) {
            if (args.size() != sizeof...(Args)) {
                return ERR("Expected " + std::to_string(sizeof...(Args)) + " arguments, got " +
                           std::to_string(args.size()));
            }

            if constexpr (std::is_same_v<R, void>) {
                return invoke_and_wrap_void<Args...>(args);
            } else {
                return invoke_and_wrap<R, Args...>(args);
            }
        }

        template <typename... InvokeArgs>
        static return_type invoke_and_wrap_void(const t_vector &args) {
            size_t i = sizeof ...(InvokeArgs) - 1;
            try {
                F(do_unwrap<InvokeArgs>(args[i], i)...);
                return bond_traits<void>::wrap();
            }
            catch (const std::string &e) {
                return std::unexpected(e);
            }

            return bond_traits<void>::wrap();
        }

        template <typename Ret, typename... InvokeArgs>
        static return_type invoke_and_wrap(const t_vector &args) {
            size_t i = sizeof ...(InvokeArgs) - 1;
            try {
                return bond_traits<Ret>::wrap(F(do_unwrap<InvokeArgs>(args[i], i)...));
            }
            catch (const std::string &e) {
                return std::unexpected(e);
            }
        }

        static bool can_unwrap(const GcPtr<Object> &object) { return object->is<NativeFunction>(); }
    };



    template<const char *t_name, typename T>
    struct AnyOpaque final: public bond::NativeInstance {
        static const char *name() {
            return t_name;
        }

        T value;

        static auto register_type(const t_string& mod_name) -> bond::GcPtr<bond::NativeStruct> {
            auto doc = fmt::format("{}({}: any) -> {}", t_name, t_name, t_name);
            auto the_struct = bond::make_immortal<bond::NativeStruct>(t_name, doc, bond::c_Default<AnyOpaque<t_name, T>>);
            bond::Runtime::ins()->register_type(mod_name, t_name, the_struct);
            return the_struct;
        }

        static auto register_type(const t_string& mod_name, const t_string& doc, const bond::NativeFunctionPtr& constructor) -> bond::GcPtr<bond::NativeStruct> {
            auto the_struct = bond::make_immortal<bond::NativeStruct>(t_name, doc, constructor);
            bond::Runtime::ins()->register_type(mod_name, t_name, the_struct);
            return the_struct;
        }

        template <auto Constructor>
        static auto register_type(const t_string& mod_name, const t_string& doc) -> bond::GcPtr<bond::NativeStruct> {
            using Type = bond::f_wrapper<Constructor, false>;
            auto the_constructor = bond::bond_traits<Type>::wrap_to_fptr(Type{t_name, doc});
            auto the_struct = bond::make_immortal<bond::NativeStruct>(t_name, doc, the_constructor);
            bond::Runtime::ins()->register_type(mod_name, t_name, the_struct);
            return the_struct;
        }

        AnyOpaque() = default;
        AnyOpaque(T value) : value(value) {}
    };


    template <const char *t_name, typename Any>
    struct bond::bond_traits<const AnyOpaque<t_name, Any>&> {
        static auto wrap(const AnyOpaque<t_name, Any>& any) -> bond::GcPtr<AnyOpaque<t_name, Any>> {
            auto p_struct= bond::Runtime::ins()->get_type(any.module, any.name)->template as<bond::NativeStruct>()->template create_instance<AnyOpaque<t_name, Any>>();
            p_struct->value = any.value;
            return p_struct;
        }

        static auto unwrap(const bond::GcPtr<Object>& any) -> Any {
            return any->template as<AnyOpaque<t_name, Any>>()->value;
        }

        static auto can_unwrap(const bond::GcPtr<Object>& any) -> bool {
            return any->is<AnyOpaque<t_name, Any>>();
        }

        using type = AnyOpaque<t_name, Any>;
    };



    class Mod {
        class StructBuilder {
        public:
            method_map m_methods;
            std::unordered_map<t_string, std::pair<getter, setter>> m_fields;
            t_string m_name;
            t_string m_doc;
            NativeFunctionPtr m_constructor;

            StructBuilder(const t_string &name, const t_string &doc) : m_name(name), m_doc(doc) {}

            StructBuilder &method(const t_string &name, const NativeMethodPtr &value, const t_string &doc) {
                m_methods[name] = {value, doc};
                return *this;
            }

            StructBuilder &constructor(const NativeFunctionPtr &value) {
                m_constructor = value;
                return *this;
            }

            StructBuilder &field(const t_string &name, const getter &get, const setter &set) {
                m_fields[name] = {get, set};
                return *this;
            }
        };

        GcPtr<StringMap> m_exports = Runtime::ins()->make_string_map();
        std::vector<std::shared_ptr<StructBuilder>> m_structs;
        t_string m_path;

    public:
        explicit Mod(t_string path) : m_path(std::move(path)) {}

        GcPtr<Module> build() {
            for (auto &builder: m_structs) {
                auto struct_ = make<NativeStruct>(builder->m_name, builder->m_doc, builder->m_constructor,
                                                  builder->m_methods, builder->m_fields);
                auto constructor = builder->m_constructor;
                auto wrapper = [constructor, struct_](const t_vector &args) -> obj_result {
                    auto res = constructor(args);
                    if (!res.has_value()) {
                        return res;
                    }

                    auto val = res.value();

                    if (val->is<Result>()) {
                        if (val->as<Result>()->has_value()) {
                            val->as<Result>()->get_value()->as<NativeInstance>()->set_native_struct(struct_.get());
                        }
                        return val;
                    }

                    val->as<NativeInstance>()->set_native_struct(struct_.get());
                    return res;
                };

                struct_->set_constructor(wrapper);

                m_exports->set(builder->m_name, struct_);
            }
            return Runtime::ins()->make_module(m_path, m_exports);
        }

        /**
 * Constructor for the struct_ class.
 *
 * @param name The name of the struct_.
 * @param doc The documentation for the struct_.
 */

        StructBuilder &struct_(const t_string &name, const t_string &doc) {
            auto builder = std::make_shared<StructBuilder>(name, doc);
            m_structs.push_back(builder);
            return *builder.get();
        }

        template<typename T>
        Mod &add(const t_string &name, const T &value) {
            m_exports->set(name, bond_traits<T>::wrap(value));
            return *this;
        }

        Mod &add(const t_string &name, const GcPtr<Object> &value) {
            m_exports->set(name, value);
            return *this;
        }

        Mod &function(const t_string &name, const NativeFunctionPtr &value, const t_string &doc) {
            auto fn = Runtime::ins()->make_native_function(name, doc, value);
            m_exports->set(name, fn);
            return *this;
        }

        template <auto K>
        Mod &function(const t_string &name, const t_string& doc) {
            auto fn = bond_traits<f_wrapper<K, false>>::wrap(f_wrapper<K, false>{name, doc});
            m_exports->set(name, fn);
            return *this;
        }

    };

    template<typename T>
    inline GcPtr<Result> make_ok_t(T object) {
        return make_ok(bond_traits<T>::wrap(object));
    }

    template<typename T>
    inline GcPtr<Result> make_error_t(T object) {
        return make_error(bond_traits<T>::wrap(object));
    }
}