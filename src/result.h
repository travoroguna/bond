//
// Created by Travor Oguna Oneya on 11/04/2023.
//

#pragma once

#include "object.h"


namespace bond {

    class BondResult : public Object {
    public:
        BondResult(bool is_error, const GcPtr<Object> &obj) {
            m_is_error = is_error;
            m_obj = obj;
        }

        bool is_error() { return m_is_error; }

        bool is_ok() { return !m_is_error; }

        GcPtr<Object> get() {
            if (m_is_error) throw std::runtime_error("Result has no value");
            return m_obj;
        }

        GcPtr<Object> get_error() {
            if (!m_is_error) throw std::runtime_error("Result has no error");
            return m_obj;
        }

        std::string str() override {
            if (m_is_error) return fmt::format("Error({})", m_obj->str());
                return fmt::format("Ok({})", m_obj->str());
        }

        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

        OBJ_RESULT $get_attribute(const GcPtr<Object> &index) override {
            if (!index->is<String>()) return std::unexpected(RuntimeError::AttributeNotFound);
            auto name = index->as<String>()->get_value();
            if (!m_attrs.contains(name)) return std::unexpected(RuntimeError::AttributeNotFound);
            return m_attrs[name];
        }

        void mark() override {
            Object::mark();
            m_obj->mark();

            for (auto &[_, attr]: m_attrs) {
                attr->mark();
            }
        }

        void unmark() override {
            Object::unmark();
            m_obj->unmark();

            for (auto &[_, attr]: m_attrs) {
                attr->unmark();
            }
        }

        OBJ_RESULT $_bool() override {
            return GarbageCollector::instance().make<Bool>(!m_is_error);
        }

    private:
        bool m_is_error;
        GcPtr<Object> m_obj;

        std::unordered_map<std::string, GcPtr<Object>> m_attrs = {
                {"has_value", GarbageCollector::instance()
                                      .make<NativeFunction>(
                                              [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                                                  ASSERT_ARG_COUNT(0, arguments);

                                                  return GarbageCollector::instance().make<Bool>(!m_is_error);
                                              }
                                      )
                },
                {"has_error", GarbageCollector::instance()
                                      .make<NativeFunction>(
                                              [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                                                  ASSERT_ARG_COUNT(0, arguments);
                                                  return GarbageCollector::instance().make<Bool>(m_is_error);
                                              }
                                      )
                },
                {"value",     GarbageCollector::instance()
                                      .make<NativeFunction>(
                                              [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                                                  ASSERT_ARG_COUNT(0, arguments);

                                                  if (m_is_error)
                                                      return std::unexpected(
                                                              FunctionError("Result has no value",
                                                                            RuntimeError::GenericError));
                                                  return m_obj;
                                              }
                                      )
                },
                {"error",     GarbageCollector::instance()
                                      .make<NativeFunction>(
                                              [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                                                  ASSERT_ARG_COUNT(0, arguments);

                                                  if (!m_is_error)
                                                      return std::unexpected(
                                                              FunctionError("Result has a value",
                                                                            RuntimeError::GenericError));
                                                  return m_obj;
                                              }
                                      )
                },

                {"or_else",   GarbageCollector::instance()
                                      .make<NativeFunction>(
                                              [this](const std::vector<GcPtr<Object>> &arguments) -> NativeErrorOr {
                                                  ASSERT_ARG_COUNT(1, arguments);
                                                  if (m_is_error)
                                                      return arguments[0];
                                                  return m_obj;
                                              }
                                      )
                }


        };
    };

} // bond
