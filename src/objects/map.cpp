#include "../object.h"
#include "../result.h"


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

    std::expected<GcPtr<Object>, RuntimeError> Map::$get_item(const GcPtr<bond::Object> &index) {
        if (has(index)) {
            return GarbageCollector::instance().make<BondResult>(false, get_unchecked(index));
        }

        auto err = GarbageCollector::instance().make<String>(
                fmt::format("Key error: key {} not found in map", index->str()));
        return GarbageCollector::instance().make<BondResult>(true, err);
    }

    std::expected<GcPtr<Object>, RuntimeError>
    Map::$set_item(const GcPtr<bond::Object> &index, const GcPtr<bond::Object> &value) {
        set(index, value);
        return Globs::BondNil;
    }

    std::string Map::str() {
        std::string result = "{";
        for (auto &[key, value]: m_internal_map) {
            result += fmt::format("{}: {}, ", key->str(), value->str());
        }
        result += "}";
        return result;
    }
}