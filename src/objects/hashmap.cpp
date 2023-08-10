//
// Created by travor on 31/07/2023.
//


#include "../object.h"
#include "../api.h"
#include "../vm.h"


#define INITIAL_HASH_CAPACITY 16


namespace bond {
    std::expected<void, std::string> check_key_hashable(const GcPtr<Object> &key) {
        if (!key->is<NativeInstance>()) {
            return std::unexpected("key is not hashable");
        }

        auto the_key = key->as<NativeInstance>();

        if (!the_key->has_slot(Slot::HASH)) {
            return std::unexpected(fmt::format("key {} is not hashable __hash__ not found", key->str()));
        }

        if (!the_key->has_slot(Slot::EQ)) {
            return std::unexpected(fmt::format("key {} is not hashable __eq__ not found", key->str()));
        }

        return {};
    }

    std::expected<GcPtr<Object>, std::string> HashMap::get(const GcPtr<Object> &key) {
        auto h_res = hash_key(key);
        TRY(h_res);
        auto h = h_res.value();

        auto index = (size_t) (h & (uint64_t) (m_entries.size() - 1));
        auto vm = get_current_vm();
        assert(vm != nullptr && "vm is null");


        while (m_entries[index]->key.get() != nullptr) {
            auto eq = vm->call_slot(Slot::EQ, m_entries[index]->key, {key});
            if (!eq) {
                return std::unexpected(fmt::format("failed to call __eq__ {}", eq.error()));
            }

            auto the_cond = TO_BOOL(eq.value())->get_value();

            if (the_cond) {
                return m_entries[index]->value;
            }

            index++;

            if (index >= m_entries.size()) {
                index = 0;
            }
        }

        return std::unexpected("key not found");
    }


    std::expected<void, std::string> HashMap::set(const GcPtr<Object> &key, const GcPtr<Object> &value) {
        auto cap = m_entries.size();

        if (m_size >= cap / 2) {
            expand();
        }

        return set_entry(key, value);
    }

    std::expected<void, std::string> HashMap::remove(const GcPtr<Object> &key) {
        auto h_res = hash_key(key);
        TRY(h_res);

        auto h = h_res.value();
        auto index = (size_t) (h & (uint64_t) (m_entries.size() - 1));
        auto vm = get_current_vm();
        assert(vm != nullptr && "vm is null");

        while (m_entries[index]->key.get() != nullptr) {
            auto eq = vm->call_slot(Slot::EQ, m_entries[index]->key, {key});
            if (!eq) {
                return std::unexpected(fmt::format("failed to call __eq__ {}", eq.error()));
            }

            auto the_cond = TO_BOOL(eq.value())->get_value();

            if (the_cond) {
                m_entries[index]->key = GcPtr<Object>(nullptr);
                m_entries[index]->value = GcPtr<Object>(nullptr);
                m_size--;
                return {};
            }

            index++;

            if (index >= m_entries.size()) {
                index = 0;
            }
        }

        return std::unexpected(fmt::format("key {} not found", key->str()));
    }

    std::expected<bool, std::string> HashMap::has(const GcPtr<Object> &key) {
        auto h_res = hash_key(key);
        TRY(h_res);

        auto h = h_res.value();
        auto index = (size_t) (h & (uint64_t) (m_entries.size() - 1));
        auto vm = get_current_vm();
        assert(vm != nullptr && "vm is null");

        while (m_entries[index]->key.get() != nullptr) {
            auto eq = vm->call_slot(Slot::EQ, m_entries[index]->key, {key});
            if (!eq) {
                return std::unexpected(fmt::format("failed to call __eq__ {}", eq.error()));
            }

            auto the_cond = TO_BOOL(eq.value())->get_value();

            if (the_cond) {
                return true;
            }

            index++;

            if (index >= m_entries.size()) {
                index = 0;
            }
        }

        return false;
    }

    std::expected<size_t, std::string> HashMap::hash_key(const GcPtr<bond::Object> &key) {
        TRY(check_key_hashable(key));
        auto the_key = key->as<NativeInstance>();

        auto vm = get_current_vm();

        assert(vm != nullptr && "vm is null");

        auto hash_res = vm->call_slot(Slot::HASH, the_key, {});

        if (!hash_res) {
            return std::unexpected(fmt::format("failed to call __hash__ {}", hash_res.error()));

        }

        auto hash = hash_res.value();

        if (!hash->is<Int>()) {
            return std::unexpected(fmt::format("hash for key {} is not an integer", key->str()));
        }

        auto h = hash->as<Int>()->get_value();

        //FIXME: implement big int that can be used as hash
        if (h < 0) {
            h = -h;
        }

        return h;
    }


    std::expected<void, std::string> HashMap::set_entry(const GcPtr<Object> &key, const GcPtr<Object> &value) {
        auto h_res = hash_key(key);
        TRY(h_res);
        auto h = h_res.value();

        auto index = (size_t) (h & (uint64_t) (m_entries.size() - 1));
        auto vm = get_current_vm();
        assert(vm != nullptr && "vm is null");

        while (m_entries[index]->key.get() != nullptr) {
            auto eq = vm->call_slot(Slot::EQ, m_entries[index]->key, {key});
            if (!eq) {
                return std::unexpected(fmt::format("failed to call __hash__ {}", eq.error()));
            }

            auto the_cond = TO_BOOL(eq.value())->get_value();

            if (the_cond) {
                m_entries[index]->value = value;
                m_entries[index]->hash = h;
                m_size++;
                return {};
            }

            index++;

            if (index >= m_entries.size()) {
                index = 0;
            }
        }

        m_entries[index]->key = key;
        m_entries[index]->value = value;
        m_entries[index]->hash = h;
        m_size++;
        return {};
    }

    size_t HashMap::size() const {
        return m_size;
    }

    void HashMap::expand() {
        auto new_cap = m_entries.size() * 2;

        if (m_entries.empty()) {
            new_cap = INITIAL_HASH_CAPACITY;
        }

        std::vector<std::shared_ptr<ht_entry>> new_entries;
        new_entries.resize(new_cap);

        for (auto &entry: new_entries) {
            entry = std::make_shared<ht_entry>(nullptr, nullptr, 0);
        }

        assert(new_entries.size() == new_cap);

        auto old_entries = m_entries;
        m_entries = new_entries;
        m_size = 0;

        for (auto &entry: old_entries) {
            if (entry->key.get() == nullptr) {
                continue;
            }
            auto res = set_entry(entry->key, entry->value);
            assert(res.has_value() && "failed to set entry");
        }

    }

    std::string HashMap::str() const {
        std::string res = "{ ";
        for (auto &entry: m_entries) {
            if (entry->key.get() == nullptr) {
                continue;
            }
            res += fmt::format("{}: {}, ", entry->key->str(), entry->value->str());
        }
        res += "}";
        return res;
    }

    obj_result HashMap_getitem(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<HashMap>();
        Object *key;
        auto opt = parse_args(args, key);
        TRY(opt);
        auto res = self->get(key);
        if (!res.has_value()) {
            return ERR(res.error());
        }
        return OK(res.value());
    }

    obj_result HashMap_setitem(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<HashMap>();
        Object *key;
        Object *value;
        auto opt = parse_args(args, key, value);
        TRY(opt);
        auto res = self->set(key, value);
        if (!res.has_value()) {
            return ERR(res.error());
        }
        return OK();
    }

    obj_result HashMap_len(const GcPtr<Object> &Self, [[maybe_unused]] const t_vector &args) {
        auto self = Self->as<HashMap>();
        return OK(make_int(self->size()));
    }


    obj_result HashMap_get(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<HashMap>();
        Object *key;

        TRY(parse_args(args, key));

        auto res = self->get(key);

        if (!res.has_value()) {
            return make_error(make_string(res.error()));
        }
        return make_ok(res.value());
    }

    obj_result HashMap_set(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<HashMap>();
        Object *key;
        Object *value;
        TRY(parse_args(args, key, value));

        auto res = self->set(key, value);
        if (!res.has_value()) {
            return make_error(make_string(res.error()));
        }
        return make_ok(value);
    }

    obj_result HashMap_contains(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<HashMap>();
        Object *key;
        TRY(parse_args(args, key));

        auto res = self->has(key);
        if (!res.has_value()) {
            return ERR(res.error());
        }
        return AS_BOOL(res.value());
    }

    obj_result HashMap_remove(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<HashMap>();
        Object *key;
        TRY(parse_args(args, key));

        auto res = self->remove(key);
        if (!res.has_value()) {
            return ERR(res.error());
        }
        return OK();
    }

    class HashMapIterator : public NativeInstance {
    public:
        explicit HashMapIterator(const GcPtr<HashMap> &map) {
            m_index = 0;
            m_map = map;
            get_next();
        };

        bool has_next() const {
            return !at_end;
        }

        GcPtr<Object> next() {
            auto pre = next_item;
            get_next();
            return pre;
        }

        void get_next() {
            while (m_index < m_map->capacity()) {
                auto entry = m_map->get_entries()[m_index];
                m_index++;
                if (entry->key.get() != nullptr) {
                    next_item = LIST_STRUCT->create_instance<List>(t_vector{entry->key, entry->value});
                    return;
                }
            }
            at_end = true;
        }

        GcPtr<Object> next_item;
        bool at_end{false};
        size_t m_index;
        GcPtr<HashMap> m_map;
    };

    obj_result Hash_it_next(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<HashMapIterator>();
        TRY(parse_args(args));
        return self->next();
    }

    obj_result Hash_it_has_next(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<HashMapIterator>();
        TRY(parse_args(args));
        return AS_BOOL(self->has_next());
    }

    auto HASH_ITER_STRUCT = make_immortal<NativeStruct>("HashMapIter", "HashMapIterator(value)",
                                                        c_Default<HashMapIterator>, method_map{
                    {"__next__",     {Hash_it_next,     "__next__() -> String"}},
                    {"__has_next__", {Hash_it_has_next, "__has_next__() -> Bool"}},
            });

    obj_result HashMap_iter(const GcPtr<Object> &Self, const t_vector &args) {
        auto self = Self->as<HashMap>();
        TRY(parse_args(args));
        return HASH_ITER_STRUCT->create_instance<HashMapIterator>(self);
    }


    GcPtr<NativeStruct> HASHMAP_STRUCT = make<NativeStruct>("HashMap", "HashMap(val: HashMap)", c_Default<HashMap>,
                                                            method_map{
                                                                    {"__getitem__", {HashMap_getitem, "__getitem__(key: Any) -> Any"}},
                                                                    {"__setitem__", {HashMap_setitem, "__setitem__(key: Any, value: Any) -> None"}},
                                                                    {"size",        {HashMap_len,     "size() -> Int"}},
                                                                    {"get",         {HashMap_get,     "get(key: Any) -> String!Any"}},
                                                                    {"set",         {HashMap_set,     "set(key: Any, value: Any) -> String!None"}},
                                                                    {"contains",    {HashMap_contains, "contains(key: Any) -> Bool"}},
                                                                    {"remove",      {HashMap_remove,   "remove(key: Any) -> None"}},
                                                                    {"__iter__",    {HashMap_iter,    "__iter__() -> Iterator"}},
                                                            });
}