#include "../object.h"


namespace bond {
    OBJ_RESULT ListObj::$set_item(const GcPtr<Object> &index, const GcPtr<Object> &value) {
        if (!is<Integer>(index)) return std::unexpected(RuntimeError::ExpectedNumberIndex);
        auto index_value = as<Integer>(index)->get_value();

        if (std::floor(index_value) != index_value) {
            return std::unexpected(RuntimeError::ExpectedNumberIndex);
        }

        auto i = static_cast<size_t>(index_value);

        if (i < 0 || i >= m_internal_list.size()) {
            return std::unexpected(RuntimeError::IndexOutOfBounds);
        }

        m_internal_list[i] = value;
        return value;
    }

    OBJ_RESULT ListObj::$get_item(const GcPtr<Object> &index) {
        if (!is<Integer>(index)) return std::unexpected(RuntimeError::ExpectedNumberIndex);
        auto index_value = as<Integer>(index)->get_value();

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

    OBJ_RESULT ListObj::$_bool() {
        return BOOL_(!m_internal_list.empty());
    }

    OBJ_RESULT ListObj::$iter() {
        return GarbageCollector::instance().make<ListIterator>(this);
    }

    OBJ_RESULT ListObj::$get_attribute(const GcPtr<Object> &index) {
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

        } else if (attr == "clear") {
            return GarbageCollector::instance().make<NativeFunction>(
                    [this](const arg_t &args) -> NativeErrorOr {
                        if (!args.empty())
                            return std::unexpected(
                                    FunctionError("clear takes no arguments", RuntimeError::InvalidArgument));

                        this->m_internal_list.clear();
                        return GarbageCollector::instance().make<Nil>();
                    }, "clear");
        } else if (attr == "size") {
            return GarbageCollector::instance().make<NativeFunction>(
                    [this](const arg_t &args) -> NativeErrorOr {
                        if (!args.empty())
                            return std::unexpected(
                                    FunctionError("size takes no arguments", RuntimeError::InvalidArgument));
                        return GarbageCollector::instance().make<Integer>(m_internal_list.size());
                    }, "size");
        }


        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    void ListObj::append(const GcPtr<Object> &value) {
        m_internal_list.push_back(value);
    }
};