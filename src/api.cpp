//
// Created by Travor Oguna Oneya on 03/04/2023.
//


#include "api.h"
#include "bond.h"

namespace bond {
    NativeErrorOr native_add(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);

        DEFINE(left, bond::Integer, 0, arguments);
        DEFINE(right, bond::Integer, 1, arguments);

        return GarbageCollector::instance().make<bond::Integer>(left->get_value() + right->get_value());
    }

    NativeErrorOr Ok(const GcPtr<Object> &ok) {
        return GarbageCollector::instance().make<BondResult>(false, ok);
    }

    NativeErrorOr Err(const GcPtr<Object> &err) {
        return GarbageCollector::instance().make<BondResult>(true, err);
    }

    NativeErrorOr Err(const std::string &err) {
        return GarbageCollector::instance().make<BondResult>(true, GarbageCollector::instance().make<String>(err));
    }

    Enum::Enum(std::string name, const std::unordered_map<std::string, uintmax_t> &attributes) {
        m_name = std::move(name);
        for (auto &[key, value]: attributes) {
            m_attributes[key] = bond::GarbageCollector::instance().make_immortal<bond::Integer>(value);
        }
    }

    std::expected<GcPtr<Object>, RuntimeError> Enum::$get_attribute(const GcPtr<Object> &index) {
        if (!index->is<String>()) return std::unexpected(RuntimeError::AttributeNotFound);
        auto key = index->as<String>()->get_value();
        if (m_attributes.find(key) != m_attributes.end()) return m_attributes[key];
        return std::unexpected(RuntimeError::AttributeNotFound);
    }


    BondObject::BondObject(const std::string &name) {
        m_name = name;
    }

    GcPtr<NativeStruct<BondObject>>
    BondObject::make_native_struct(const std::string &name, const NativeFunctionPtr &constructor) {
        return GarbageCollector::instance().make<NativeStruct<BondObject>>(name, constructor);
    }

    std::expected<GcPtr<Object>, RuntimeError> BondObject::$get_attribute(const GcPtr<Object> &index) {
        if (!index->is<String>()) return std::unexpected(RuntimeError::AttributeNotFound);
        auto key = index->as<String>()->get_value();
        if (m_attributes.find(key) != m_attributes.end()) return m_attributes[key];
        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError>
    BondObject::$set_attribute(const GcPtr<Object> &index, const GcPtr<Object> &value) {
        if (!index->is<String>()) return std::unexpected(RuntimeError::AttributeNotFound);
        auto key = index->as<String>()->get_value();
        m_attributes[key] = value;
        return value;
    }

    void BondObject::mark() {
        if (m_marked) return;
        m_marked = true;
        for (auto &[key, value]: m_attributes) {
            value->mark();
        }
    }

    void BondObject::unmark() {
        if (!m_marked) return;
        m_marked = false;
        for (auto &[key, value]: m_attributes) {
            value->unmark();
        }
    }

    bool BondObject::equal(const GcPtr<Object> &other) {
        if (!other->is<BondObject>()) return false;

        auto other_obj = other->as<BondObject>();
        if (m_attributes.size() != other_obj->m_attributes.size()) return false;

        for (auto &[key, value]: m_attributes) {
            if (other_obj->m_attributes.find(key) == other_obj->m_attributes.end()) return false;
            if (!value->equal(other_obj->m_attributes[key])) return false;
        }

        return true;
    }

    size_t BondObject::hash() {
        size_t hash = 0;
        for (auto &[key, value]: m_attributes) {
            hash ^= std::hash<std::string>()(key);
            hash ^= value->hash();
        }
        return hash;
    }

    BondObject::BondObject() = default;


};