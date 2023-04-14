//
// Created by Travor Oguna Oneya on 05/04/2023.
//

#include "builtins.h"
#include "result.h"

#include <fmt/core.h>
#include <unordered_map>

namespace bond {

    std::unordered_map<std::string, GcPtr<NativeFunction>> builtins = {
            {"println", GarbageCollector::instance().make_immortal<NativeFunction>(println, "println")},
            {"range",   GarbageCollector::instance().make_immortal<NativeFunction>(range, "range")},
            {"str",     GarbageCollector::instance().make_immortal<NativeFunction>(str, "str")},
            {"Ok",      GarbageCollector::instance().make_immortal<NativeFunction>(Ok_, "Ok")},
            {"Err",     GarbageCollector::instance().make_immortal<NativeFunction>(Err_, "Err")}
    };

    auto BondOk__(const GcPtr<Object> &obj) -> NativeErrorOr {
        return GarbageCollector::instance().make<BondResult>(false, obj);
    }

    auto BondErr__(const GcPtr<Object> &obj) -> NativeErrorOr {
        return GarbageCollector::instance().make<BondResult>(true, obj);
    }

    NativeErrorOr Ok_(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        return BondOk__(arguments[0]);
    }

    NativeErrorOr Err_(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        return BondErr__(arguments[0]);
    }

    NativeErrorOr println(const std::vector<GcPtr<Object>> &arguments) {
        for (auto &arg: arguments) {
            fmt::print("{} ", arg->str());
        }
        fmt::print("\n");
        return GarbageCollector::instance().make<Nil>();
    }

    void add_builtins_to_globals(GcPtr<Map> &globals) {
        for (auto &[name, function]: builtins) {
            globals->set(GarbageCollector::instance().make_immortal<String>(name), function);
        }
    }

    class RangeIterator : public Object {
    public:
        RangeIterator(const GcPtr<Number> &start, const GcPtr<Number> &end) {
            this->start = start->get_value();
            this->end = end->get_value();
        }

        OBJ_RESULT $next() override {
            if (start < end) {
                return GarbageCollector::instance().make<Number>(start++);
            }
            return GarbageCollector::instance().make<Nil>();
        }

        OBJ_RESULT $has_next() {
            return GarbageCollector::instance().make<Bool>(start < end);
        }

        bool equal(const GcPtr<Object> &other) override {
            if (!other->is<RangeIterator>()) {
                return false;
            }

            auto other_range = other->as<RangeIterator>();
            return start == other_range->start && end == other_range->end;
        }

        OBJ_RESULT $iter(const GcPtr<Object> &self) override { return self; }


        size_t hash() override {
            return std::hash<std::string>()(str());
        };


        std::string str() override { return fmt::format("RangeIterator({}, {})", start, end); }

    private:
        size_t start;
        size_t end;
    };

    NativeErrorOr range(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(start, bond::Number, 0, arguments);
        DEFINE(end, bond::Number, 1, arguments);

        return GarbageCollector::instance().make<RangeIterator>(start, end);
    }

    NativeErrorOr str(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(obj, bond::Object, 0, arguments);
        return GarbageCollector::instance().make<String>(obj->str());
    }

};