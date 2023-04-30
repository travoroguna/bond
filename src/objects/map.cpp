#include "../object.h"


namespace bond {
    void Map::set(const GcPtr<Object> &key, const GcPtr<Object> &value) {
        m_internal_map[key] = value;
    }

    bool Map::has(const GcPtr<Object> &key) {
        return m_internal_map.contains(key);
    }

    std::optional<GcPtr<Object>> Map::get(const GcPtr<Object> &key) {
        if (has(key)) {
            return m_internal_map[key];
        }

        return std::nullopt;
    }

    GcPtr<Object> Map::get_unchecked(const GcPtr<Object> &key) {
        return m_internal_map[key];
    }

    void Map::mark() {
        if (m_marked) return;

        Object::mark();

        for (auto &pair: m_internal_map) {
            pair.first->mark();
            pair.second->mark();
        }
    }

    void Map::unmark() {
        if (!m_marked) return;
        Object::unmark();

        for (auto &pair: m_internal_map) {
            pair.first->unmark();
            pair.second->unmark();
        }
    }

    OBJ_RESULT Map::$_bool() {
        return BOOL_(!m_internal_map.empty());
    }
}