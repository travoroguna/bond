#include "../object.h"


namespace bond {
    void Map::set(const std::string& key, const GcPtr<Object>& obj) {
        m_value[key] = obj;
    }
    std::optional<GcPtr<Object>> Map::get(const std::string& key) {
        if (m_value.contains(key)) return m_value[key];
        return std::nullopt;
    }
    GcPtr<Object> Map::get_unchecked(const std::string& key) {
        return m_value[key];
    }

    obj_result c_Map(const t_vector &args) {
        Map* map;
        auto res = parse_args(args, map);
        TRY(res);
        return OK(args[0]);
    }

    GcPtr<NativeStruct> MAP_STRUCT = make_immortal<NativeStruct>("Map", "Map(value: Map)", c_Map);
}