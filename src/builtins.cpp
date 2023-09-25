//
// Created by Travor Oguna Oneya on 05/04/2023.
//

#include "builtins.h"
#include "debug.h"
#include <fmt/core.h>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include "traits.hpp"
#include "api.h"


#ifdef DEBUG
#include <gc/gc_backptr.h>
#endif

namespace bond {
    obj_result b_println(const t_vector &args) {
        for (auto &arg: args) {
            fmt::print("{} ", arg->str());
        }
        fmt::print("\n");
        return OK();
    }


    obj_result b_print(const t_vector &args) {
        for (auto &arg: args) {
            fmt::print("{} ", arg->str());
        }
        return OK();
    }

    obj_result b_dump(const t_vector &args) {
#ifdef DEBUG
        //        GC_dump();
        //        GC_generate_random_backtrace();
#endif
//        fmt::print("heap size: {} bytes\n", GC_get_heap_size());
        return OK();
    }

    obj_result b_exit(const t_vector &args) {
        Int *code;
        TRY(parse_args(args, code));
        exit(code->get_value());
        return OK();
    }


    obj_result build_help_for_native_struct(const GcPtr<NativeStruct> &native_struct) {
#define HAS(X) (X) != nullptr ? "✅" : "❎"
        std::stringstream help;
        help << fmt::format("struct {}\n", native_struct->get_doc());

        auto &attributes = native_struct->get_attributes();

        if (!attributes.empty()) {
            help << "\n attributes:\n";

            std::vector<t_string> names;
            for (auto &[k, _]: attributes) names.push_back(k);
            std::sort(names.begin(), names.end());

            for (auto &name: names) {
                auto ass = attributes[name];
                help << fmt::format("  {}, getter {}, setter {}\n", name, HAS(ass.first), HAS(ass.second));
            }
        }

        auto &methods = native_struct->get_methods();

        if (!methods.empty()) {
            help << "\n methods:\n";

            std::vector<t_string> names;
            for (auto &[k, _]: methods) names.push_back(k);
            std::sort(names.begin(), names.end());

            fmt::print("size {}, {}\n", methods.size(), names.size());

            for (auto &name: names) {
                auto meth = methods[name].second;
                help << "\n  ";

                for (auto c: meth) {
                    if (c == '\n') {
                        help << "\n    ";
                    } else {
                        help << c;
                    }
                }

                help << "\n";
            }
        }

        return OK(make_string(help.str()));
    }

    t_string build_doc(const GcPtr<Function> &function) {
        std::stringstream help;
        help << fmt::format("fn {}(", function->get_name());

        std::vector<t_string> params;

        for (auto const &param: function->get_arguments()) {
            params.push_back(param->name);
        }

        help << fmt::format("{}", fmt::join(params, ", ")) << ")\n";
        return help.str();
    }

    obj_result build_help_for_struct(const GcPtr<Struct> &struct_) {
        std::stringstream help;

        help << fmt::format("struct {}:\n\tattributes\n", struct_->get_name());

        for (auto const &field_name: struct_->get_fields()) {
            help << fmt::format("\t\t{}\n", field_name);
        }

        help << "\n\tmethods\n";
        for (auto const &[name, function]: struct_->get_methods()) {
            help << fmt::format("\t\t{}: {}\n", name, build_doc(function));
        }

        return OK(make_string(help.str()));
    }

    obj_result b_help(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));

        if (obj->is<NativeStruct>()) {
            return build_help_for_native_struct(obj->as<NativeStruct>());
        } else if (obj->is<NativeFunction>()) {
            auto doc = make_string(obj->as<NativeFunction>()->get_doc());
            return OK(make_string(fmt::format("fn {}\n", doc->get_value())));
        } else if (obj->is<Struct>()) {
            return build_help_for_struct(obj->as<Struct>());
        } else if (obj->is<Instance>()) {
            return build_help_for_struct(obj->as<Instance>()->get_struct());
        } else if (obj->is<NativeInstance>()) {
            return build_help_for_native_struct(obj->as<NativeInstance>()->get_native_struct());
        }
        return runtime_error("help() only works on structs and native structs");
    }

    obj_result b_type_of(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));
        if (obj->is<NativeStruct>() or obj->is<Struct>()) {
            return obj;
        } else if (obj->is<Instance>()) {
            return obj->as<Instance>()->get_struct();
        } else if (obj->is<NativeInstance>()) {
            return obj->as<NativeInstance>()->get_native_struct();
        }
        return runtime_error(fmt::format("unable to get type of {}", obj->str()));
    }

    obj_result b_instance_of(const t_vector &args) {
        Object *obj;
        Object *struct_;
        TRY(parse_args(args, obj, struct_));

        if (obj->is<Instance>()) {
            if (struct_->is<Struct>()) {
                return AS_BOOL(obj->as<Instance>()->get_struct() == struct_->as<Struct>().get());
            }
            if (struct_->is<NativeStruct>()) {
                return AS_BOOL(false);
            }
            return runtime_error("expected a Type as the second argument");
        } else if (obj->is<NativeInstance>()) {
            if (struct_->is<NativeStruct>()) {
                return AS_BOOL(obj->as<NativeInstance>()->get_native_struct() == struct_->as<NativeStruct>().get());
            }

            if (struct_->is<Struct>()) {
                return AS_BOOL(false);
            }

            return runtime_error("expected a Type as the second argument");
        }

        return runtime_error("expected an instance as the first argument");
    }

    obj_result b_input(const t_vector &args) {
        String *prompt;
        TRY(parse_args(args, prompt));
        fmt::print("{}", prompt->get_value());
        t_string input;
        std::getline(std::cin, input);
        return OK(make_string(input));
    }

    obj_result b_debug_break(const t_vector &args) {
        debug_break();
        return OK();
    }


    obj_result b_to_string(const t_vector &args) {
        Object *obj;
        TRY(parse_args(args, obj));
        return OK(make_string(obj->str()));
    }


    obj_result b_format(const t_vector &args) {
        if (args.empty()) {
            return runtime_error("expected at least one argument");
        }

        if (!args[0]->is<String>()) {
            return runtime_error("expected a string as the first argument");
        }

        auto rest = t_vector(args.begin() + 1, args.end());

        auto formatted = bond_format(args[0]->as<String>()->get_value(), rest);

        if (!formatted) {
            return runtime_error(formatted.error());
        }

        return OK(make_string(*formatted));
    }


    GcPtr<NativeStruct> ITER_STRUCT = make<NativeStruct>("Iter", "Iter(iter: Iter)", nullptr);


    struct Iterator : public NativeInstance {
        virtual obj_result next() = 0;

        virtual std::expected<bool, t_string> has_next() = 0;

        virtual obj_result to_list() {
            t_vector items;

            while (true) {
                auto next = this->has_next();
                TRY(next);

                if (!next.value()) {
                    break;
                }

                auto item = this->next();
                TRY(item);

                items.push_back(item.value());
            }

            return make_list(items);
        }

        virtual obj_result verify_iterable() {
            if (!it->is<Iterator>()) {
                return std::unexpected(fmt::format("instance of type {} is not iterable", get_type_name(it)));
            }
            return {};
        }

    protected:
        GcPtr<Object> it;
        bool has_error = false;
    };


    struct BasicIterator : public Iterator {
        GcPtr<Object> iterable;

        explicit BasicIterator(const GcPtr<Object> &iterable) : iterable(iterable) {}

        obj_result verify_iterable() override {
            auto vm = get_current_vm();
            if (!iterable->is<NativeInstance>()) {
                return std::unexpected("Types are not iterable");
            }

            auto iter = iterable->as<NativeInstance>();

            if (!iter->has_slot(Slot::ITER)) {
                return std::unexpected(fmt::format("instance of type {} is not iterable, __iter__ is not defined",
                                                   get_type_name(iter)));
            }

            auto iterator = vm->call_slot(Slot::ITER, iter, {});
            if (!iterator) {
                return std::unexpected(iterator.error());
            }

            if (!iterator.value()->is<NativeInstance>()) {
                return std::unexpected(
                        fmt::format("instance of type {} is not iterable, __iter__ did not return an iterator",
                                    get_type_name(iter)));
            }

            auto val = iterator.value()->as<NativeInstance>();

            if (!val->has_slot(Slot::NEXT)) {
                return std::unexpected(fmt::format(
                        "instance of type {} is not iterable, iterator returned by __iter__ does not implement __next__",
                        get_type_name(iter)));
            }

            if (!val->has_slot(Slot::HAS_NEXT)) {
                return std::unexpected(fmt::format(
                        "instance of type {} is not iterable, iterator returned by __iter__ does not implement __has_next__",
                        get_type_name(iter)));
            }

            it = val;
            return OK();
        }

        obj_result next() override {
            auto vm = get_current_vm();
            auto next = vm->call_slot(Slot::NEXT, it, {});

            if (!next) {
                return std::unexpected(next.error());
            }

            return next.value();
        }

        std::expected<bool, t_string> has_next() override {
            auto vm = get_current_vm();
            auto next = vm->call_slot(Slot::HAS_NEXT, it, {});

            if (!next) {
                return std::unexpected(next.error());
            }

            return TO_BOOL(next.value())->get_value();
        }

        obj_result to_list() override {
            t_vector items;

            while (true) {
                auto next = this->has_next();
                TRY(next);

                if (!next.value()) {
                    break;
                }

                auto item = this->next();
                TRY(item);

                items.push_back(item.value());
            }

            return make_list(items);
        }
    };

    auto BASIC_ITER = make<NativeStruct>("BasicIterator", "BasicIterator(iterable: Iterable)",
                                         c_Default<BasicIterator>);


    struct MapIterator : public Iterator {
        GcPtr<Object> func;

        MapIterator(const GcPtr<Object> &iterable, const GcPtr<Object> &func) : func(func) {
            it = iterable;
        }

        obj_result next() override {
            auto vm = get_current_vm();
            auto next = it->as<Iterator>()->next();
            TRY(next);
            t_vector args = {next.value()};

            if (vm->call_object_ex(func, args))
                return std::unexpected("error calling function in map iterator");
            return vm->pop();
        }

        std::expected<bool, t_string> has_next() override {
            auto next = it->as<Iterator>()->has_next();
            TRY(next);
            return next.value();
        }
    };

    auto MAP_ITER = make<NativeStruct>("MapIterator", "MapIterator(iterable: Iterable, func: Function)",
                                       c_Default<MapIterator>);

    struct TakeIterator : public Iterator {
        int64_t count;

        TakeIterator(const GcPtr<Object> &iterable, int64_t count) : count(count) {
            it = iterable;
        }

        obj_result next() override {
            auto vm = get_current_vm();
            auto next = it->as<Iterator>()->next();
            TRY(next);
            count--;
            return next.value();
        }

        std::expected<bool, t_string> has_next() override {
            auto next = it->as<Iterator>()->has_next();
            TRY(next);
            return next.value() && count > 0;
        }
    };

    auto TAKE_ITER = make<NativeStruct>("TakeIterator", "TakeIterator(iterable: Iterable, count: Int)",
                                        c_Default<TakeIterator>);

    struct FilterIterator : public Iterator {
        GcPtr<Object> func;

        FilterIterator(const GcPtr<Object> &iterable, const GcPtr<Object> &func) : func(func) {
            it = iterable;
        }

        obj_result next() override {
            auto vm = get_current_vm();
            while (true) {
                auto next = it->as<Iterator>()->next();
                TRY(next);
                t_vector args = {next.value()};

                if (vm->call_object_ex(func, args))
                    return std::unexpected("error calling function in filter iterator");

                auto res = vm->pop();
                if (TO_BOOL(res)->get_value())
                    return next.value();
            }
        }

        std::expected<bool, t_string> has_next() override {
            auto next = it->as<Iterator>()->has_next();
            TRY(next);
            return next.value();
        }
    };

    auto FILTER_ITER = make<NativeStruct>("FilterIterator", "FilterIterator(iterable: Iterable, func: Function)",
                                          c_Default<FilterIterator>);


    struct SkipIterator : public Iterator {
        int64_t count;

        SkipIterator(const GcPtr<Object> &iterable, int64_t count) : count(count) {
            it = iterable;
        }

        obj_result next() override {
            auto vm = get_current_vm();
            while (count > 0 && it->as<Iterator>()->has_next().value()) {
                auto next = it->as<Iterator>()->next();
                TRY(next);
                count--;
            }

            return it->as<Iterator>()->next();
        }

        std::expected<bool, t_string> has_next() override {
            auto next = it->as<Iterator>()->has_next();
            TRY(next);
            return next.value();
        }
    };

    auto SKIP_ITER = make<NativeStruct>("SkipIterator", "SkipIterator(iterable: Iterable, count: Int)",
                                        c_Default<SkipIterator>);

    struct StepByIterator : public Iterator {
        int64_t step;
        GcPtr<Object> next_value;

        StepByIterator(const GcPtr<Object> &iterable, int64_t step) : step(step) {
            it = iterable;
        }

        obj_result verify_iterable() override {
            TRY(Iterator::verify_iterable());
            if (step <= 0)
                return std::unexpected("step must be greater than 0");
            return OK();
        }

        obj_result next() override {
            return next_value;
        }

        std::expected<bool, t_string> has_next() override {
            auto h_next = it->as<Iterator>()->has_next();
            TRY(h_next);

            auto next = it->as<Iterator>()->next();
            TRY(next);

            for (int64_t i = 0; i < step - 1; i++) {
                if (!it->as<Iterator>()->has_next().value())
                    return false;

                // skip
                it->as<Iterator>()->next();
            }

            next_value = next.value();
            return true;
        }
    };

    auto STEP_BY_ITER = make<NativeStruct>("StepByIterator", "StepByIterator(iterable: Iterable, step: Int)",
                                           c_Default<StepByIterator>);

    struct ChainIterator : public Iterator {
        GcPtr<Iterator> next_iter;

        ChainIterator(const GcPtr<Object> &iterable, const GcPtr<Iterator> &next_iter) : next_iter(next_iter) {
            it = iterable;
        }

        obj_result next() override {
            auto vm = get_current_vm();
            if (it->as<Iterator>()->has_next().value())
                return it->as<Iterator>()->next();

            auto it_ = next_iter->as<Iterator>()->next();
            TRY(it_);
            return it_.value();
        }

        std::expected<bool, t_string> has_next() override {
            auto next = it->as<Iterator>()->has_next();
            TRY(next);
            if (next.value())
                return true;

            return next_iter->as<Iterator>()->has_next();
        }
    };

    auto CHAIN_ITER = make<NativeStruct>("ChainIterator", "ChainIterator(iterable: Iterable, next_iter: Iterable)",
                                         c_Default<ChainIterator>);


    struct EnumerateIterator : public Iterator {
        int64_t index;

        EnumerateIterator(const GcPtr<Object> &iterable) : index(0) {
            it = iterable;
        }

        obj_result next() override {
            auto next = it->as<Iterator>()->next();
            TRY(next);
            auto res = make_list({make_int(index), next.value()});
            index++;
            return res;
        }

        std::expected<bool, t_string> has_next() override {
            auto next = it->as<Iterator>()->has_next();
            TRY(next);
            return next.value();
        }
    };

    auto ENUMERATE_ITER = make<NativeStruct>("EnumerateIterator", "EnumerateIterator(iterable: Iterable)",
                                             c_Default<EnumerateIterator>);


    struct TakeWhileIterator : public Iterator {
        GcPtr<Object> func;
        GcPtr<Object> next_value;

        TakeWhileIterator(const GcPtr<Object> &iterable, const GcPtr<Object> &func) : func(func) {
            it = iterable;
        }

        obj_result next() override {
            return next_value;
        }

        std::expected<bool, t_string> has_next() override {
            auto h_next = it->as<Iterator>()->has_next();
            TRY(h_next);
            if (!h_next.value())
                return false;

            auto next = it->as<Iterator>()->next();
            TRY(next);

            t_vector args = {next.value()};
            auto vm = get_current_vm();

            if (vm->call_object_ex(func, args))
                return std::unexpected("error calling function in take_while iterator");

            auto res = vm->pop();

            if (TO_BOOL(res)->get_value()) {
                next_value = next.value();
                return true;
            }

            return false;
        }
    };


    auto TAKE_WHILE_ITER = make<NativeStruct>("TakeWhileIterator",
                                              "TakeWhileIterator(iterable: Iterable, func: Function)",
                                              c_Default<TakeWhileIterator>);


    class Iter : public NativeInstance {
    public:
        GcPtr<Iterator> iterable;

        Iter() = default;

        obj_result build_object(const GcPtr<Object> &object) {
            iterable = BASIC_ITER->create_instance<BasicIterator>(object);
            TRY(iterable->verify_iterable());
            return {};
        }

        static obj_result map(const GcPtr<Object> &self, const t_vector &args) {
            Object *func;
            TRY(parse_args(args, func));
            auto it = self->as<Iter>();

            it->iterable = MAP_ITER->create_instance<MapIterator>(it->iterable, func);
            TRY(it->iterable->verify_iterable());
            return self;
        }

        static obj_result take(const GcPtr<Object> &self, const t_vector &args) {
            Int *count;
            TRY(parse_args(args, count));
            auto iter = self->as<Iter>();

            iter->iterable = TAKE_ITER->create_instance<TakeIterator>(iter->iterable, count->get_value());
            TRY(iter->iterable->verify_iterable());
            return self;
        }

        static obj_result filter(const GcPtr<Object> &self, const t_vector &args) {
            Object *func;
            TRY(parse_args(args, func));
            auto iter = self->as<Iter>();

            iter->iterable = FILTER_ITER->create_instance<FilterIterator>(iter->iterable, func);
            TRY(iter->iterable->verify_iterable());
            return self;
        }

        static obj_result to_list(const GcPtr<Object> &self, const t_vector &args) {
            auto iter = self->as<Iter>();
            auto value = iter->iterable->to_list();
            TRY(value);
            return OK(value.value());
        }

        static obj_result reduce(const GcPtr<Object> &self, const t_vector &args) {
            Object *func;
            TRY(parse_args(args, func));
            auto iter = self->as<Iter>();

            auto vm = get_current_vm();

            if (!iter->iterable->has_next().value())
                return runtime_error("cannot reduce empty iterator");

            auto first = iter->iterable->next();
            TRY(first);

            while (iter->iterable->has_next().value()) {
                auto next = iter->iterable->next();
                TRY(next);
                auto a = t_vector{first.value(), next.value()};
                vm->call_object_ex(func, a);

                if (vm->had_error())
                    return runtime_error("error while reducing iterator");
                first = vm->pop();
            }

            return first;
        }

        static obj_result skip(const GcPtr<Object> &self, const t_vector &args) {
            Int *count;
            TRY(parse_args(args, count));
            auto iter = self->as<Iter>();

            iter->iterable = SKIP_ITER->create_instance<SkipIterator>(iter->iterable, count->get_value());
            TRY(iter->iterable->verify_iterable());
            return self;
        }

        static obj_result step_by(const GcPtr<Object> &self, const t_vector &args) {
            Int *count;
            TRY(parse_args(args, count));
            auto iter = self->as<Iter>();

            iter->iterable = STEP_BY_ITER->create_instance<StepByIterator>(iter->iterable, count->get_value());
            TRY(iter->iterable->verify_iterable());
            return self;
        }

        static obj_result enumerate(const GcPtr<Object> &self, const t_vector &args) {
            TRY(parse_args(args));
            auto iter = self->as<Iter>();

            iter->iterable = ENUMERATE_ITER->create_instance<EnumerateIterator>(iter->iterable);
            TRY(iter->iterable->verify_iterable());
            return self;
        }

        static obj_result chain(const GcPtr<Object> &self, const t_vector &args) {
            Iter *next_iter;
            TRY(parse_args(args, next_iter));
            auto iter = self->as<Iter>();

            iter->iterable = CHAIN_ITER->create_instance<ChainIterator>(iter->iterable, next_iter->iterable);
            TRY(iter->iterable->verify_iterable());
            return self;
        }

        static obj_result take_while(const GcPtr<Object> &self, const t_vector &args) {
            Object *func;
            TRY(parse_args(args, func));
            auto iter = self->as<Iter>();

            iter->iterable = TAKE_WHILE_ITER->create_instance<TakeWhileIterator>(iter->iterable, func);
            TRY(iter->iterable->verify_iterable());
            return self;
        }

        static obj_result iter(const GcPtr<Object> &self, const t_vector &args) {
            TRY(parse_args(args));
            return self;
        }

        static obj_result next(const GcPtr<Object> &self, const t_vector &args) {
            auto iter = self->as<Iter>();
            auto next = iter->iterable->next();
            TRY(next);
            return next.value();
        }

        static obj_result has_next(const GcPtr<Object> &self, const t_vector &args) {
            auto iter = self->as<Iter>();
            auto next = iter->iterable->has_next();
            TRY(next);
            return AS_BOOL(next.value());
        }
    };


    auto iter_methods = method_map{
            {"take",         {Iter::take,       "take(count: Int) -> Iter\nAn iterator that iterates over he first `count` elements"}},
            {"map",          {Iter::map,        "map(func: Function) -> Iter\nAn iterator that applies `func` to each element"}},
            {"filter",       {Iter::filter,     "filter(func: Function) -> Iter\nAn iterator that only yields elements for which `func` returns `true`"}},
            {"reduce",       {Iter::reduce,     "reduce(func: Function) -> Object\nApplies `func` to each element and returns the result"}},
            {"step_by",      {Iter::step_by,    "step_by(count: Int) -> Iter\nAn iterator that skips `count` elements between each yield"}},
            {"skip",         {Iter::skip,       "skip(count: Int) -> Iter\nAn iterator that skips the first `count` elements"}},
            {"enumerate",    {Iter::enumerate,  "enumerate() -> Iter\nAn iterator that yields tuples of the form `(index, element)`"}},
            {"chain",        {Iter::chain,      "chain(iter: Iter) -> Iter\nAn iterator that yields elements from `iter` after `self` is exhausted"}},
            {"to_list",      {Iter::to_list,    "to_list() -> List\nReturns a list of all elements in the iterator"}},
            {"take_while",   {Iter::take_while, "take_while(func: Function) -> Iter\nAn iterator that yields elements until `func` returns `false`"}},

            {"__iter__",     {Iter::iter,       "__iter__() -> Iter\nReturns the iterator itself"}},
            {"__next__",     {Iter::next,       "__next__() -> Object\nReturns the next element in the iterator"}},
            {"__has_next__", {Iter::has_next,   "__has_next__i() -> Bool\nReturns `true` if the iterator has more elements"}},
    };


    obj_result b_iter(const t_vector &args) {
        Object *iterable;
        TRY(parse_args(args, iterable));

        auto it = ITER_STRUCT->create_instance<Iter>();
        TRY(it->build_object(iterable));
        return it;
    }

    bool built = false;
    t_map builtins;

    void add_builtins_to_globals(const GcPtr<StringMap> &globals) {
        if (!built) {
            ITER_STRUCT->set_constructor(b_iter);
            ITER_STRUCT->set_methods(iter_methods);

            auto future = Mod("future");
            future.function("to_string", b_to_string, "to_string(obj)");


            builtins = {
                    {"println",     Runtime::ins()->make_native_function("println", "println(...)",
                                                                         b_println)},
                    {"print",       Runtime::ins()->make_native_function("print", "print(...)", b_print)},
                    {"dump",        Runtime::ins()->make_native_function("dump", "dump()", b_dump)},
                    {"exit",        Runtime::ins()->make_native_function("exit", "exit(code)", b_exit)},
                    {"help",        Runtime::ins()->make_native_function("help", "help(obj)", b_help)},
                    {"type_of",     Runtime::ins()->make_native_function("type_of", "type_of(obj)", b_type_of)},
                    {"instance_of", Runtime::ins()->make_native_function("instance_of", "instance_of(obj, type)",
                                                                         b_instance_of)},
                    {"input",       Runtime::ins()->make_native_function("input", "input(prompt)", b_input)},
                    {"Int",         Runtime::ins()->INT_STRUCT},
                    {"Float",       Runtime::ins()->FLOAT_STRUCT},
                    {"String",      Runtime::ins()->STRING_STRUCT},
                    {"List",        Runtime::ins()->LIST_STRUCT},
                    {"Bool",        Runtime::ins()->BOOL_STRUCT},
                    {"Nil",         Runtime::ins()->NONE_STRUCT},
                    {"Bytes",       Runtime::ins()->BYTES_STRUCT},
                    {"Future",      Runtime::ins()->FUTURE_STRUCT},
                    {"iter",        Runtime::ins()->make_native_function("iter", "iter(iterable: Any) -> Iter",
                                                                         b_iter)},
                    {"debug_break", Runtime::ins()->make_native_function("debug_break", "debug_break()",
                                                                         b_debug_break)},
                    {"__future__",  future.build()},
                    {"format",      Runtime::ins()->make_native_function("format", "format(str, ...)", b_format)},
            };

            built = true;
        }

        for (auto const &[name, value]: builtins) {
            globals->set(name, value);
        }
    }


};