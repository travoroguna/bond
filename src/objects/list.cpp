#include "../object.h"
#include "../api.h"
#include "../traits.hpp"


namespace bond {
    List::List(const t_vector &elements) {
        m_elements = elements;
    }

    obj_result List::get_item(int64_t index) {
        if (index < 0 or index > (int64_t)m_elements.size() - 1) {
            return runtime_error(fmt::format("Index {} out of range", index));
        }
        return m_elements[index];
    }

    obj_result List::set_item(int64_t index, const GcPtr<Object> &item) {
        if (index < 0 or index > (int64_t)m_elements.size() - 1) {
            return runtime_error(fmt::format("Index {} out of range", index));
        }
        m_elements[index] = item;
        return OK();
    }

    obj_result List::size() const {
        return OK(make_int(m_elements.size()));
    }

    size_t List::get_size() const {
        return m_elements.size();
    }

    obj_result get_item(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<List>();
        Int *index;
        TRY(parse_args(args, index));

        return self->get_item(index->get_value());
    }

    obj_result set_item(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<List>();
        Int *index;
        Object *item;
        TRY(parse_args(args, index, item));

        return self->set_item(index->get_value(), item);
    }

    obj_result size(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<List>();
        TRY(parse_args(args));

        return self->size();
    }

    void List::prepend(const GcPtr<Object> &item) {
        m_elements.insert(m_elements.begin(), item);
    }

    void List::append(const GcPtr<Object> &item) {
        m_elements.push_back(item);
    }

    void List::insert(int64_t index, const GcPtr<Object> &item) {
        m_elements.insert(m_elements.begin() + index, item);
    }

    GcPtr<Object> List::pop() {
        auto last = m_elements.back();
        m_elements.pop_back();
        return last;
    }


    t_string List::str() const {
        std::vector<t_string> strs;
        for (auto &item: m_elements) {
            strs.push_back(item->str());
        }
        return fmt::format("[{}]", fmt::join(strs, ", "));
    }

    obj_result List_append(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<List>();
        Object *item;
        TRY(parse_args(args, item));

        self->append(item);
        return OK();
    }

    obj_result List_insert(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<List>();
        Int *index;
        Object *item;
        TRY(parse_args(args, index, item));

        self->insert(index->get_value(), item);
        return OK();
    }

    obj_result List_pop(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<List>();
        TRY(parse_args(args));

        return self->pop();
    }

    obj_result List_contains(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<List>();
        Object *item;
        TRY(parse_args(args, item));

        auto vm = get_current_vm();

        for (auto &element: self->get_elements()) {
            auto res = vm->call_slot(Slot::EQ, element, {item});
            TRY(res);
            if (TO_BOOL(res.value())->as<Bool>()->get_value()) {
                return OK(Runtime::ins()->C_TRUE);
            }
        }
        return Runtime::ins()->C_FALSE;
    }

    obj_result List::remove(int64_t index) {
        if (index < 0 or index > (int64_t)m_elements.size() - 1) {
            return runtime_error(fmt::format("Index {} out of range", index));
        }
        m_elements.erase(m_elements.begin() + index);
        return OK();
    }

    obj_result List::clear() {
        m_elements.clear();
        return OK();
    }

    obj_result List::index(const GcPtr<Object> &item) {
        auto vm = get_current_vm();
        for (int i = 0; i < m_elements.size(); i++) {
            auto res = vm->call_slot(Slot::EQ, m_elements[i], {item});
            TRY(res);
            if (TO_BOOL(res.value())->as<Bool>()->get_value()) {
                return make_ok(make_int(i));
            }
        }
        return make_error_t("Item not found");
    }

    obj_result List::reverse() {
        std::reverse(m_elements.begin(), m_elements.end());
        return OK();
    }

    obj_result List::concat(const GcPtr<List> &other) {
        auto &other_l = other->get_elements();
        m_elements.insert(m_elements.end(), other_l.begin(), other_l.end());
        return this;
    }

    obj_result list_iterator_next(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<ListIterator>();
        TRY(parse_args(args));
        return self->m_list->get_item(self->m_index++);
    }

    obj_result list_iterator_has_next(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<ListIterator>();
        TRY(parse_args(args));
        return OK(AS_BOOL(self->m_index < (int64_t)self->m_list->get_size()));
    }

    GcPtr<NativeStruct> LIST_ITERATOR_STRUCT = make_immortal<NativeStruct>("ListIterator", "ListIterator(list: List)",
                                                                           c_Default<ListIterator>, method_map{
                    {"__next__",     {list_iterator_next,     "__next__()"}},
                    {"__has_next__", {list_iterator_has_next, "__has_next__()"}},
            });

    obj_result list_iter(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<List>();
        TRY(parse_args(args));

        return OK(LIST_ITERATOR_STRUCT->create_instance<ListIterator>(self));
    }


    void init_list() {
        Runtime::ins()->LIST_STRUCT = make_immortal<NativeStruct>("List", "List(object)", c_Default<List>, method_map{
                {"__iter__",    {list_iter,     "__iter__()"}},
                {"__getitem__", {get_item,      "__getitem__(index: Int)"}},
                {"__setitem__", {set_item,      "__setitem__(index: Int, value: Any)"}},
                {"size",        {size,          "size()\ngets the size of the list\nreturns Int"}},
                {"append",      {List_append,   "append(item: Any)\nappends an item to the end of the list"}},
                {"insert",      {List_insert,   "insert(index: Int, item: Any)\ninserts an item at the given index"}},
                {"pop",         {List_pop,      "pop()\nremoves and returns the last item in the list"}},
                {"contains",    {List_contains, "contains(item: Any)\nreturns true if the list contains the item"}},
                {"remove",      {make_method<&List::remove>(), "remove(index: Int)\nremoves the item at the given index"}},
                {"clear",       {make_method<&List::clear>(),  "clear()\nremoves all items from the list"}},
                {"index",       {make_method<&List::index>(),  "index(item: Any)\nreturns the index of the item in the list"}},
                {"reverse",     {make_method<&List::reverse>(), "reverse()\nreverses the list"}},
                {"concat",      {make_method<&List::concat>(),  "concat(other: List) -> List\nappends the other list to this one"}},
                {"__i_add__", {make_method<&List::concat>(), "__i_add__(other: List) -> List\nappends the other list to this one"}},

        });
    }

}