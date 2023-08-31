//
// Created by travor on 14/07/2023.
//

#pragma once

#include "object.h"
#include "runtime.h"

namespace bond {
    template<typename R, typename = void>
    struct bond_traits {
        static R unwrap(const GcPtr<Object> &object) = delete;

        static GcPtr<Object> wrap(const R &object) = delete;
    };

    template<>
    struct bond_traits<GcPtr<Object>> {
        static GcPtr<Object> unwrap(const GcPtr<Object> &object) { return object; }

        static GcPtr<Object> wrap(const GcPtr<Object> &object) { return object; }
    };

    template<typename Integer>
    struct bond_traits<Integer, std::enable_if_t<std::is_integral_v<Integer> && sizeof(Integer) <= sizeof(int64_t)>> {
        static Integer unwrap(const GcPtr<Object> &object) {
            return object->as<Int>()->get_value();
        }

        static GcPtr<Object> wrap(const Integer &object) {
            return make_int(object);
        }
    };

    template<>
    struct bond_traits<bool> {
        static bool unwrap(const GcPtr<Object> &object) {
            return object->as<Bool>()->get_value();
        }

        static GcPtr<Object> wrap(const bool &object) {
            return AS_BOOL(object);
        }
    };

    template<>
    struct bond_traits<double> {
        static double unwrap(const GcPtr<Object> &object) {
            return object->as<Float>()->get_value();
        }

        static GcPtr<Object> wrap(const double &object) {
            return make_float(object);
        }
    };

    template<>
    struct bond_traits<float> {
        static double unwrap(const GcPtr<Object> &object) {
            return object->as<Float>()->get_value();
        }

        static GcPtr<Object> wrap(const double &object) {
            return make_float(object);
        }
    };

    template<>
    struct bond_traits<t_string> {
        static t_string unwrap(const GcPtr<Object> &object) {
            return object->as<String>()->get_value().c_str();
        }

        static GcPtr<Object> wrap(const t_string &object) {
            return make_string(object);
        }
    };

    template<>
    struct bond_traits<std::string> {
        static std::string unwrap(const GcPtr<Object> &object) {
            return object->as<String>()->get_value().c_str();
        }

        static GcPtr<Object> wrap(const t_string &object) {
            return make_string(object);
        }
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