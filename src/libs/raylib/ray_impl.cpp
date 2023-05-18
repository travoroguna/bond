//
// Created by Travor Oguna Oneya on 15/05/2023.
//

#include "raymodule.h"


namespace bond::raylib {
    std::expected<GcPtr<Object>, RuntimeError>
    Vector2::$set_attribute(const bond::GcPtr<bond::Object> &index, const bond::GcPtr<bond::Object> &value) {
        auto idx = index->as<String>()->get_value();

        if (!value->is<Float>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        if (idx == "x") {
            m_vector.x = value->as<Float>()->get_value();
        } else if (idx == "y") {
            m_vector.y = value->as<Float>()->get_value();
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError> Vector2::$get_attribute(const GcPtr<bond::Object> &index) {
        auto idx = index->as<String>()->get_value();

        if (idx == "x") {
            return GarbageCollector::instance().make<Float>(m_vector.x);
        } else if (idx == "y") {
            return GarbageCollector::instance().make<Float>(m_vector.y);
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError>
    Vector3::$set_attribute(const bond::GcPtr<bond::Object> &index, const bond::GcPtr<bond::Object> &value) {
        auto idx = index->as<String>()->get_value();

        if (!value->is<Float>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        if (idx == "x") {
            m_vector.x = value->as<Float>()->get_value();
        } else if (idx == "y") {
            m_vector.y = value->as<Float>()->get_value();
        } else if (idx == "z") {
            m_vector.z = value->as<Float>()->get_value();
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError> Vector3::$get_attribute(const GcPtr<bond::Object> &index) {
        auto idx = index->as<String>()->get_value();

        if (idx == "x") {
            return GarbageCollector::instance().make<Float>(m_vector.x);
        } else if (idx == "y") {
            return GarbageCollector::instance().make<Float>(m_vector.y);
        } else if (idx == "z") {
            return GarbageCollector::instance().make<Float>(m_vector.z);
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError> Color::$get_attribute(const GcPtr<bond::Object> &index) {
        auto idx = index->as<String>()->get_value();

        if (idx == "r") {
            return GarbageCollector::instance().make<Integer>(m_color.r);
        } else if (idx == "g") {
            return GarbageCollector::instance().make<Integer>(m_color.g);
        } else if (idx == "b") {
            return GarbageCollector::instance().make<Integer>(m_color.b);
        } else if (idx == "a") {
            return GarbageCollector::instance().make<Integer>(m_color.a);
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError>
    Color::$set_attribute(const GcPtr<bond::Object> &index, const GcPtr<bond::Object> &value) {
        auto idx = index->as<String>()->get_value();

        if (!value->is<Integer>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        if (idx == "r") {
            m_color.r = value->as<Integer>()->get_value();
        } else if (idx == "g") {
            m_color.g = value->as<Integer>()->get_value();
        } else if (idx == "b") {
            m_color.b = value->as<Integer>()->get_value();
        } else if (idx == "a") {
            m_color.a = value->as<Integer>()->get_value();
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    NativeErrorOr c_Color(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(r, Integer, 0, arguments);
        DEFINE(g, Integer, 1, arguments);
        DEFINE(b, Integer, 2, arguments);
        DEFINE(a, Integer, 3, arguments);

        return NativeErrorOr(GarbageCollector::instance().make<Color>(
                ::Color{static_cast<unsigned char>(r->get_value()),
                        static_cast<unsigned char>(g->get_value()),
                        static_cast<unsigned char>(b->get_value()),
                        static_cast<unsigned char>(a->get_value()),
                }));
    }

    NativeErrorOr c_Vector3(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(x, Float, 0, arguments);
        DEFINE(y, Float, 1, arguments);
        DEFINE(z, Float, 2, arguments);

        return NativeErrorOr(
                GarbageCollector::instance().make<Vector3>(::Vector3{x->get_value(), y->get_value(), z->get_value()}));
    }

    NativeErrorOr c_Vector2(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(x, Float, 0, arguments);
        DEFINE(y, Float, 1, arguments);

        return NativeErrorOr(GarbageCollector::instance().make<Vector2>(::Vector2{x->get_value(), y->get_value()}));
    }

    NativeErrorOr c_Rectangle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(x, Float, 0, arguments);
        DEFINE(y, Float, 1, arguments);
        DEFINE(width, Float, 2, arguments);
        DEFINE(height, Float, 3, arguments);

        return NativeErrorOr(GarbageCollector::instance().make<Rectangle>(
                ::Rectangle{x->get_value(), y->get_value(), width->get_value(), height->get_value()}));
    }

}

