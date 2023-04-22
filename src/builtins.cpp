//
// Created by Travor Oguna Oneya on 05/04/2023.
//

#include "builtins.h"
#include "result.h"

#include <fmt/core.h>
#include <unordered_map>

namespace bond {

    auto INT = GarbageCollector::instance().make_immortal<NativeStruct<Integer>>("int", int_);
    auto FLOAT = GarbageCollector::instance().make_immortal<NativeStruct<Float>>("float", float_);
    auto STRING = GarbageCollector::instance().make_immortal<NativeStruct<String>>("str", str);


    std::unordered_map<std::string, GcPtr<Object>> builtins = {
            {"println",     GarbageCollector::instance().make_immortal<NativeFunction>(println, "println")},
            {"range",       GarbageCollector::instance().make_immortal<NativeFunction>(range, "range")},
            {"str",         STRING},
            {"int",         INT},
            {"float",       FLOAT},
            {"is_instance", GarbageCollector::instance().make_immortal<NativeFunction>(is_instance, "is_instance")},
            {"Ok",          GarbageCollector::instance().make_immortal<NativeFunction>(Ok_, "Ok")},
            {"Err",         GarbageCollector::instance().make_immortal<NativeFunction>(Err_, "Err")}
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

    NativeErrorOr is_instance(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);

        auto res = arguments[0]->is_instance(arguments[1]);

        if (res.has_value()) {
            return GarbageCollector::instance().make<Bool>(res.value());
        }

        fmt::print("Error: {}\n", res.error());

        return std::unexpected(
                FunctionError("Expected a type as the first argument to is_instance", RuntimeError::GenericError));
    }


    NativeErrorOr println(const std::vector<GcPtr<Object>> &arguments) {
        for (auto &arg: arguments) {
            fmt::print("{}  ", arg->str());
        }
        fmt::print("\n");
        return GarbageCollector::instance().make<Nil>();
    }

//    NativeErrorOr is_instance(const std::vector<GcPtr<Object>> &arguments) {
//        ASSERT_ARG_COUNT(2, arguments);
//        DEFINE(obj, bond::Object, 0, arguments);
//        DEFINE(class_, bond::Class, 1, arguments);
//
//        return GarbageCollector::instance().make<Bool>(obj->is(class_));
//    }


    void add_builtins_to_globals(GcPtr<Map> &globals) {
        for (auto &[name, function]: builtins) {
            globals->set(GarbageCollector::instance().make_immortal<String>(name), function);
        }
    }

    class RangeIterator : public Object {
    public:
        RangeIterator(const GcPtr<Integer> &start, const GcPtr<Integer> &end) {
            this->start = start->get_value();
            this->end = end->get_value();
        }

        OBJ_RESULT $next() override {
            if (start < end) {
                return GarbageCollector::instance().make<Integer>(start++);
            }
            return GarbageCollector::instance().make<Nil>();
        }

        OBJ_RESULT $has_next() override {
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
        DEFINE(start, bond::Integer, 0, arguments);
        DEFINE(end, bond::Integer, 1, arguments);

        return GarbageCollector::instance().make<RangeIterator>(start, end);
    }

    NativeErrorOr str(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(obj, bond::Object, 0, arguments);
        return GarbageCollector::instance().make<String>(obj->str());
    }

    NativeErrorOr int_(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);

        if (arguments[0]->is<Integer>()) {
            return Ok(arguments[0]);
        } else if (arguments[0]->is<String>()) {
            auto str = arguments[0]->as<String>();
            try {
                return Ok(GarbageCollector::instance().make<Integer>(std::stoi(str->get_value())));
            } catch (...) {
                return Err(fmt::format("Cannot convert {} to integer", arguments[0]->str()));
            }
        } else if (arguments[0]->is<Float>()) {
            auto flt = arguments[0]->as<Float>();
            try {
                return Ok(GarbageCollector::instance().make<Integer>(static_cast<int>(flt->get_value())));
            } catch (...) {
                return Err(fmt::format("Cannot convert {} to integer", arguments[0]->str()));
            }
        }

        return Err(fmt::format("Cannot convert {} to integer", arguments[0]->str()));
    }

    NativeErrorOr float_(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);

        if (arguments[0]->is<Integer>()) {
            auto integer = arguments[0]->as<Integer>();
            return Ok(GarbageCollector::instance().make<Float>(static_cast<double>(integer->get_value())));
        } else if (arguments[0]->is<String>()) {
            auto str = arguments[0]->as<String>();
            try {
                return Ok(GarbageCollector::instance().make<Float>(std::stod(str->get_value())));
            } catch (...) {
                return Err(fmt::format("Cannot convert {} to float", arguments[0]->str()));
            }
        } else if (arguments[0]->is<Float>()) {
            return Ok(arguments[0]);
        }

        return Err(fmt::format("Cannot convert {} to float", arguments[0]->str()));
    }

};