#include "object.h"
#include <functional>
#include <cmath>

namespace bond {
    OBJ_RESULT Number::$add(const GcPtr<Object> &other) {
        if (!other->is<Number>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        return GarbageCollector::instance().make<Number>(m_value + other->as<Number>()->get_value());
    }

    OBJ_RESULT Number::$sub(const GcPtr<Object> &other) {
        if (!other->is<Number>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        return GarbageCollector::instance().make<Number>(m_value - other->as<Number>()->get_value());
    }

    OBJ_RESULT Number::$mul(const GcPtr<Object> &other) {
        if (!other->is<Number>()) {
            return std::unexpected(RuntimeError::TypeError);
        }

        return GarbageCollector::instance().make<Number>(m_value * other->as<Number>()->get_value());
    }

    OBJ_RESULT Number::$div(const GcPtr<Object> &other) {
        auto res = const_cast<GcPtr<Object> &>(other)
                .use_if<Number, std::optional<GcPtr<Number>>>(
                        [&](Number &other_number) -> std::optional<GcPtr<Number>> {
                            if (other_number.get_value() == 0) {
                                return std::nullopt;
                            }
                            return GarbageCollector::instance().make<Number>(m_value / other_number.get_value());

                        });

        if (res.has_value()) {
            if (res.value().has_value()) {
                return res.value().value();
            }
            return std::unexpected(RuntimeError::DivisionByZero);
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    std::string Number::str() {
        return fmt::format("{:.30g}", m_value);
    }

    bool Number::equal(const GcPtr<Object> &other) {
        if (!is<Number>(other)) return false;
        return m_value == as<Number>(other)->get_value();
    }

    size_t Number::hash() {
        return std::hash<float>{}(m_value);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Number::$eq(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Number::$ne(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(!this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Number::$lt(const GcPtr<Object> &other) {
        if (!is<Number>(other)) return std::unexpected(RuntimeError::TypeError);
        return GarbageCollector::instance().make<Bool>(m_value < as<Number>(other)->get_value());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Number::$le(const GcPtr<Object> &other) {
        if (!is<Number>(other)) return std::unexpected(RuntimeError::TypeError);
        return GarbageCollector::instance().make<Bool>(m_value <= as<Number>(other)->get_value());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Number::$gt(const GcPtr<Object> &other) {
        if (!is<Number>(other)) return std::unexpected(RuntimeError::TypeError);
        return GarbageCollector::instance().make<Bool>(m_value > as<Number>(other)->get_value());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Number::$ge(const GcPtr<Object> &other) {
        if (!is<Number>(other)) return std::unexpected(RuntimeError::TypeError);
        return GarbageCollector::instance().make<Bool>(m_value >= as<Number>(other)->get_value());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Number::$_bool() {
        return GarbageCollector::instance().make<Bool>(m_value != 0);
    }

    OBJ_RESULT

    String::$add(const GcPtr<Object> &other) {
        auto
                res = const_cast<GcPtr<Object> &>(other)
                .use_if<String, GcPtr<String >>([&](String &other_string) {
                    return GarbageCollector::instance().make<String>(m_value + other_string.get_value());
                });

        if (res.has_value()) {
            return res.value();
        }
        return std::unexpected(RuntimeError::TypeError);
    }

    bool String::equal(const GcPtr<Object> &other) {
        if (!is<String>(other)) return false;
        return m_value == as<String>(other)->get_value();
    }

    std::expected<GcPtr<Object>, RuntimeError>

    String::$eq(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    String::$ne(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(!this->equal(other));
    }

    size_t String::hash() {
        return std::hash<std::string>{}(m_value);
    }

    std::string String::str() {
        return fmt::format("\"{}\"", m_value);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    String::$_bool() {
        return GarbageCollector::instance().make<Bool>(!m_value.empty());
    }

    std::string Bool::str() {
        return m_value ? "true" : "false";
    }

    bool Bool::equal(const GcPtr<Object> &other) {
        if (!is<Bool>(other)) return false;
        return m_value == as<Bool>(other)->get_value();
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Bool::$eq(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Bool::$ne(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(!this->equal(other));
    }

    size_t Bool::hash() {
        return std::hash<bool>{}(m_value);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Bool::$_bool() {
        return GarbageCollector::instance().make<Bool>(m_value);
    }

    std::string Nil::str() {
        return "nil";
    }

    bool Nil::equal(const GcPtr<Object> &other) {
        return is<Nil>(other);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Nil::$eq(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Nil::$ne(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(!this->equal(other));
    }

    size_t Nil::hash() {
        //Todo: find a good way to hash as all nill
        //      values are the same
        return 7654345678900987654;
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Nil::$_bool() {
        return GarbageCollector::instance().make<Bool>(false);
    }

    void Map::set(const GcPtr<Object> &key, const GcPtr<Object> &value) {
        m_internal_map[key] = value;
    }

    bool Map::has(const GcPtr<Object> &key) {
        return m_internal_map.contains(key);
    }

    std::optional<GcPtr<Object>>

    Map::get(const GcPtr<Object> &key) {
//        for (auto &[n, method] : m_internal_map){
//            fmt::print("({} == {}) -> {}, is_same: {}\n", n->str(), key->str(), method->str(), n == key);
//        }

        if (has(key)) {
            return m_internal_map[key];
        }

        return std::nullopt;
    }

    GcPtr<Object> Map::get_unchecked(const GcPtr<Object> &key) {
        return m_internal_map[key];
    }

    void Map::mark() {
        Object::mark();

        for (auto &pair: m_internal_map) {
            pair.first->mark();
            pair.second->mark();
        }
    }

    void Map::unmark() {
        Object::unmark();

        for (auto &pair: m_internal_map) {
            pair.first->unmark();
            pair.second->unmark();
        }
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Map::$_bool() {
        return GarbageCollector::instance().make<Bool>(!m_internal_map.empty());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    ListObj::$set_item(const GcPtr<Object> &index, const GcPtr<Object> &value) {
        if (!is<Number>(index)) return std::unexpected(RuntimeError::ExpectedNumberIndex);
        auto index_value = as<Number>(index)->get_value();

        if (std::floor(index_value) != index_value) {
            return std::unexpected(RuntimeError::ExpectedWholeNumberIndex);
        }

        auto i = static_cast<size_t>(index_value);

        if (i < 0 || i >= m_internal_list.size()) {
            return std::unexpected(RuntimeError::IndexOutOfBounds);
        }

        m_internal_list[i] = value;
        return value;
    }

    std::expected<GcPtr<Object>, RuntimeError>

    ListObj::$get_item(const GcPtr<Object> &index) {
        if (!is<Number>(index)) return std::unexpected(RuntimeError::ExpectedNumberIndex);
        auto index_value = as<Number>(index)->get_value();

        if (std::floor(index_value) != index_value) {
            return std::unexpected(RuntimeError::ExpectedWholeNumberIndex);
        }

        auto i = static_cast<size_t>(index_value);

        if (i < 0 || i >= m_internal_list.size()) {
            return std::unexpected(RuntimeError::IndexOutOfBounds);
        }

        return m_internal_list[i];
    }

    void ListObj::mark() {
        Object::mark();

        for (auto &item: m_internal_list) {
            item->mark();
        }
    }

    void ListObj::unmark() {
        Object::unmark();

        for (auto &item: m_internal_list) {
            item->unmark();
        }
    }

    void ListObj::prepend(const GcPtr<Object> &value) {
        m_internal_list.insert(m_internal_list.begin(), value);
    }

    std::string ListObj::str() {
        std::string result = "[";

        for (auto &item: m_internal_list) {
            result += item->str() + ", ";
        }

        if (result.size() > 1) {
            result.pop_back();
            result.pop_back();
        }

        result += "]";

        return result;
    }

    std::expected<GcPtr<Object>, RuntimeError>

    ListObj::$_bool() {
        return GarbageCollector::instance().make<Bool>(!m_internal_list.empty());
    }

    std::expected<GcPtr<Object>, RuntimeError>

    ListObj::$iter(const GcPtr<Object> &self) {
        return GarbageCollector::instance().make<ListIterator>(self);
    }

    std::expected<GcPtr<Object>, RuntimeError> ListObj::$get_attribute(const GcPtr<Object> &index) {
        if (!is<String>(index)) return std::unexpected(RuntimeError::ExpectedStringIndex);

        auto attr = as<String>(index)->get_value();
        using arg_t = std::vector<GcPtr<Object>>;

        if (attr == "append") {
            return GarbageCollector::instance().make<NativeFunction>(
                    [this](const arg_t &args) -> NativeErrorOr {
                        if (args.size() != 1)
                            return std::unexpected(
                                    FunctionError("append takes 1 argument", RuntimeError::InvalidArgument));

                        this->m_internal_list.push_back(args[0]);
                        return GarbageCollector::instance().make<Nil>();
                    }, "append");
        } else if (attr == "prepend") {
            return GarbageCollector::instance().make<NativeFunction>(
                    [this](const arg_t &args) -> NativeErrorOr {
                        if (args.size() != 1)
                            return std::unexpected(
                                    FunctionError("prepend takes 1 argument", RuntimeError::InvalidArgument));

                        this->prepend(args[0]);
                        return GarbageCollector::instance().make<Nil>();
                    }, "prepend");
        } else if (attr == "pop") {
            return GarbageCollector::instance().make<NativeFunction>(
                    [this](const arg_t &args) -> NativeErrorOr {
                        if (!args.empty())
                            return std::unexpected(
                                    FunctionError("pop takes 0 arguments", RuntimeError::InvalidArgument));

                        auto result = m_internal_list.back();
                        m_internal_list.pop_back();
                        return result;
                    }, "pop");
        } else if (attr == "remove") {
            return GarbageCollector::instance().make<NativeFunction>(
                    [this](const arg_t &args) -> NativeErrorOr {
                        if (args.size() != 1)
                            return std::unexpected(
                                    FunctionError("remove takes 1 argument", RuntimeError::InvalidArgument));

                        auto it = std::find(m_internal_list.begin(), m_internal_list.end(), args[0]);
                        if (it == m_internal_list.end())
                            return std::unexpected(
                                    FunctionError(fmt::format("unable to remove element"), RuntimeError::GenericError));
                        m_internal_list.erase(it);
                        return GarbageCollector::instance().make<Nil>();
                    }, "remove");
        }


        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    Struct::Struct(const GcPtr<String> &name, const std::vector<GcPtr<String>>

    &instance_variables) {
        m_name = name;
        m_instance_variables = instance_variables;
        m_methods = GarbageCollector::instance().make_immortal<Map>();
    }


    OBJ_RESULT

    Struct::$get_attribute(const GcPtr<Object> &index) {
        auto attr = get_method(index);
        if (attr.has_value()) return attr.value();
        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    void Struct::mark() {
        Object::mark();

        for (auto &name: m_instance_variables) name.mark();
        m_methods.mark();
    }

    void Struct::unmark() {
        Object::unmark();

        for (auto &name: m_instance_variables) name.unmark();
        m_methods.unmark();
    }

    void Struct::set_globals(const GcPtr<Map> &globals) {
        m_globals = globals;

        for (auto &[_, meth]: m_methods->get_map()) {
            auto func = meth->as<Function>();
            func->set_globals(m_globals);
        }
    }

    StructInstance::StructInstance(const GcPtr<Struct> &struct_type) {
        m_struct_type = struct_type;
        m_attributes = GarbageCollector::instance().make_immortal<Map>();
    }

    void StructInstance::mark() {
        Object::mark();
        m_struct_type.mark();
        m_attributes.mark();
    }

    void StructInstance::unmark() {
        Object::unmark();
        m_struct_type.unmark();
        m_attributes.unmark();
    }


    std::expected<GcPtr<Object>, RuntimeError>

    StructInstance::$set_attribute(const GcPtr<Object> &index, const GcPtr<Object> &value) {
        if (!is<String>(index)) return std::unexpected(RuntimeError::ExpectedStringIndex);
        auto name = as<String>(index)->get_value();

        if (m_attributes->has(index)) {
            m_attributes->set(index, value);
            return value;
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    StructInstance::$get_attribute(const GcPtr<Object> &index) {
        if (!is<String>(index)) return std::unexpected(RuntimeError::ExpectedStringIndex);
        auto name = as<String>(index)->get_value();

        if (m_attributes->has(index)) {
            return m_attributes->get_unchecked(index);
        }

        if (auto method = m_struct_type->get_method(index)) {
            return GarbageCollector::instance().make<BoundMethod>(this, method.value());
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    StructInstance::$set_item(const GcPtr<Object> &index, const GcPtr<Object> &value) {
        auto name = String("__set_item__");
        GcPtr<String> name_ptr = &name;

        if (auto method = m_struct_type->get_method(name_ptr)) {
            return GarbageCollector::instance().make<BoundMethod>(this, method.value());
        }

        return std::unexpected(RuntimeError::AttributeNotFound);;
    }

    std::expected<GcPtr<Object>, RuntimeError>

    StructInstance::$get_item(const GcPtr<Object> &index) {
        auto name = String("__get_item__");
        GcPtr<String> name_ptr = &name;


        if (auto method = m_struct_type->get_method(name_ptr)) {
            return GarbageCollector::instance().make<BoundMethod>(this, method.value());
        }

        fmt::print("No method found for {}[{}], {}\n", m_struct_type->get_name(), index->str(), name_ptr->get_value());

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    StructInstance::$eq(const GcPtr<Object> &other) {
        if (!other->is<StructInstance>()) return GarbageCollector::instance().make<Bool>(false);

        auto other_instance = other->as<StructInstance>();
        if (other_instance->get_struct_type() != m_struct_type) return GarbageCollector::instance().make<Bool>(false);

        for (auto &[name, value]: m_attributes->get_map()) {
            if (auto v = other_instance->get_attr(name)) {

                if (v == value) continue;
                return GarbageCollector::instance().make<Bool>(false);
            }
        }

        return GarbageCollector::instance().make<Bool>(true);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    StructInstance::$ne(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(!$eq(other).value()->as<Bool>()->get_value());
    }

    void BoundMethod::mark() {
        Object::mark();
        m_receiver.mark();
        m_method.mark();
    }

    void BoundMethod::unmark() {
        Object::unmark();
        m_receiver.unmark();
        m_method.unmark();
    }

}