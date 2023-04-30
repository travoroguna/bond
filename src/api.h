//
// Created by Travor Oguna Oneya on 03/04/2023.
//

#ifndef BOND_SRC_API_H_
#define BOND_SRC_API_H_

#include "gc.h"
#include "object.h"

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif

namespace bond {
    NativeErrorOr native_add(const std::vector<GcPtr<Object>> &arguments);

    class Enum : public Object {
    public:
        explicit Enum(std::string name, const std::unordered_map<std::string, uintmax_t> &attributes);

        bool equal([[maybe_unused]] const GcPtr<Object> &other) override { return false; }

        size_t hash() override { return 0; }

        std::string str() override { return fmt::format("{}", m_name); }

        OBJ_RESULT $get_attribute(const GcPtr<Object> &index) override;

        void mark() override {
            if (m_marked) return;
            m_marked = true;
            for (auto &[key, value]: m_attributes) {
                value->mark();
            }
        }

        void unmark() override {
            if (!m_marked) return;
            m_marked = false;
            for (auto &[key, value]: m_attributes) {
                value->unmark();
            }
        }

    private:
        std::string m_name;
        std::unordered_map<std::string, GcPtr<Object>> m_attributes;
    };


    class BondObject : public Object {
    public:
        explicit BondObject(const std::string &name);

        GcPtr<NativeStruct<BondObject>> make_native_struct(const NativeFunctionPtr &constructor);

        virtual void init() = 0;

        std::expected<GcPtr<Object>, RuntimeError> $get_attribute(const GcPtr<bond::Object> &index) override;

        std::expected<GcPtr<Object>, RuntimeError>
        $set_attribute(const GcPtr<bond::Object> &index, const GcPtr<bond::Object> &value) override;

        void mark() override;

        void unmark() override;

        bool equal(const GcPtr<bond::Object> &other) override;

        size_t hash() override;


    private:
        std::unordered_map<std::string, GcPtr<Object>> m_attributes;
        std::string m_name = "Object";

    };
}


#endif //BOND_SRC_API_H_
