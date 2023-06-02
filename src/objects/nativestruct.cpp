//
// Created by Travor Oguna Oneya on 29/05/2023.
//

#include "../object.h"


namespace bond {
    template<typename T>
    NativeStruct<T>::NativeStruct(std::string name, const NativeFunctionPtr &constructor) {
        m_name = std::move(name);
        m_constructor = constructor;
    }

    template<typename T>
    NativeStruct<T>::NativeStruct(std::string name, const NativeFunctionPtr &constructor,
                                  const std::unordered_map<std::string, NativeMethodPtr> &attributes) {
        m_name = std::move(name);
        m_constructor = constructor;
        m_attributes = attributes;
    }

    template<typename T>
    std::string NativeStruct<T>::str() {
        return fmt::format("<native struct {} at {}>", m_name, (void *) this);
    }

    template<typename T>
    bool NativeStruct<T>::equal(const GcPtr<Object> &other) {
        return this == other.get();
    }

    template<typename T>
    NativeErrorOr NativeStruct<T>::call(const std::vector<GcPtr<Object>> &args) {
        return m_constructor(args);
    }

    template<typename T>
    std::expected<bool, std::string> NativeStruct<T>::is_instance(const GcPtr<Object> &obj)  {
        return instanceof<T>(obj.get());
    }

    template<typename T>
    std::expected<GcPtr<Object>, std::string> NativeStruct<T>::call__(std::vector<GcPtr<Object>> &arguments)  {
        auto result = m_constructor(arguments);

        if (result.has_value()) {
            return result.value();
        } else {
            return std::unexpected(result.error().message);
        }
    }

    template<typename T>
    std::expected<NativeMethodPtr, std::string> NativeStruct<T>::get_struct_attribute(const std::string &name)  {
        if (m_attributes.contains(name)) {
            return m_attributes[name];
        } else {
            return std::unexpected(fmt::format("attribute {} not found", name));
        }
    }

    NativeInstance::NativeInstance(const GcPtr<NativeStruct<NativeInstance>>& parent) {
        m_parent = parent;
    }


    std::string NativeInstance::str() {
        return fmt::format("<instance of {} at {}>", m_parent->get_name(), (void *) this);
    }

    bool NativeInstance::equal(const GcPtr<bond::Object> &other) {
        return this == other.get();
    }

    size_t NativeInstance::hash() {
        return 0;
    }

}