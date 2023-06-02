#include "../../../bond.h"
#include "../raymodule.h"

namespace bond::raytext {
    NativeErrorOr get_font_default(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr load_font(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr load_font_ex(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr load_font_from_image(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr load_font_from_memory(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr is_font_ready(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr load_font_data(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_font_atlas(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr unload_font_data(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr unload_font(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr export_font_as_code(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr draw_fps(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr draw_text(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr draw_text_ex(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr draw_text_pro(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr draw_text_codepoint(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr draw_text_codepoints(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr measure_text(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr measure_text_ex(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr get_glyph_index(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr get_glyph_info(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr get_glyph_atlas_rec(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr load_utf8(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr unload_utf8(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr load_codepoints(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr unload_codepoints(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr get_codepoint_count(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr get_codepoint(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr get_codepoint_next(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr get_codepoint_previous(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr codepoint_to_utf8(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_copy(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_is_equal(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_length(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_format(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_subtext(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_replace(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_insert(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_join(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_split(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_append(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_find_index(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_to_upper(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_to_lower(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_to_pascal(const std::vector <GcPtr<Object>> &arguments);

    NativeErrorOr text_to_integer(const std::vector <GcPtr<Object>> &arguments);
}