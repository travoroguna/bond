#include "raytext.h"


namespace bond::raytext {
    NativeErrorOr get_font_default(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<raylib::Font>(GetFontDefault());
    }

    NativeErrorOr load_font(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);

        return GarbageCollector::instance().make<raylib::Font>(LoadFont(fileName->get_value().c_str()));
    }

    NativeErrorOr load_font_ex(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(fileName, String, 0, arguments);
        DEFINE(fontSize, Integer, 1, arguments);
        DEFINE(glyphCount, Integer, 2, arguments);

        return GarbageCollector::instance().make<raylib::Font>(
                LoadFontEx(fileName->get_value().c_str(), fontSize->get_value(), nullptr,
                           glyphCount->get_value()));
    }

    NativeErrorOr load_font_from_image(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(key, raylib::Color, 1, arguments);
        DEFINE(firstChar, Integer, 2, arguments);

        return GarbageCollector::instance().make<raylib::Font>(
                LoadFontFromImage(image->get_value(), key->get_value(), firstChar->get_value()));
    }

    NativeErrorOr load_font_from_memory(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(fileType, String, 0, arguments);
        DEFINE(fileData, Integer, 1, arguments);
        DEFINE(dataSize, Integer, 2, arguments);
        DEFINE(fontSize, Integer, 3, arguments);
        DEFINE(fontChars, Integer, 4, arguments);
        DEFINE(glyphCount, Integer, 5, arguments);

        TraceLog(LOG_WARNING, "load_font_from_memory is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr is_font_ready(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(font, raylib::Font, 0, arguments);

        return BOOL_(IsFontReady(font->get_value()));
    }

    NativeErrorOr load_font_data(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(fileData, Integer, 0, arguments);
        DEFINE(dataSize, Integer, 1, arguments);
        DEFINE(fontSize, Integer, 2, arguments);
        DEFINE(fontChars, Integer, 3, arguments);
        DEFINE(glyphCount, Integer, 4, arguments);
        DEFINE(type, Integer, 5, arguments);

        TraceLog(LOG_WARNING, "load_font_data is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr gen_image_font_atlas(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
//        DEFINE(chars, raylib::GlyphInfo, 0, arguments);
        DEFINE(recs, raylib::Rectangle, 1, arguments);
        DEFINE(glyphCount, Integer, 2, arguments);
        DEFINE(fontSize, Integer, 3, arguments);
        DEFINE(padding, Integer, 4, arguments);
        DEFINE(packMethod, Integer, 5, arguments);

        TraceLog(LOG_WARNING, "gen_image_font_atlas is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr unload_font_data(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
//        DEFINE(chars, GlyphInfo, 0, arguments);
        DEFINE(glyphCount, Integer, 1, arguments);
        TraceLog(LOG_WARNING, "unload_font_data is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr unload_font(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        TraceLog(LOG_WARNING, "unload_font is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr export_font_as_code(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(fileName, String, 1, arguments);

        ExportFontAsCode(font->get_value(), fileName->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr draw_fps(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(posX, Integer, 0, arguments);
        DEFINE(posY, Integer, 1, arguments);
        DrawFPS(posX->get_value(), posY->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_text(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(posX, Integer, 1, arguments);
        DEFINE(posY, Integer, 2, arguments);
        DEFINE(fontSize, Integer, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        DrawText(text->get_value().c_str(), posX->get_value(), posY->get_value(), fontSize->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_text_ex(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(text, String, 1, arguments);
        DEFINE(position, raylib::Vector2, 2, arguments);
        DEFINE(fontSize, Float, 3, arguments);
        DEFINE(spacing, Float, 4, arguments);
        DEFINE(tint, raylib::Color, 5, arguments);

        DrawTextEx(font->get_value(), text->get_value().c_str(), position->get_value(), fontSize->get_value(),
                   spacing->get_value(), tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_text_pro(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(8, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(text, String, 1, arguments);
        DEFINE(position, raylib::Vector2, 2, arguments);
        DEFINE(origin, raylib::Vector2, 3, arguments);
        DEFINE(rotation, Float, 4, arguments);
        DEFINE(fontSize, Float, 5, arguments);
        DEFINE(spacing, Float, 6, arguments);
        DEFINE(tint, raylib::Color, 7, arguments);

        DrawTextPro(font->get_value(), text->get_value().c_str(), position->get_value(), origin->get_value(),
                    rotation->get_value(), fontSize->get_value(), spacing->get_value(), tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_text_codepoint(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(codepoint, Integer, 1, arguments);
        DEFINE(position, raylib::Vector2, 2, arguments);
        DEFINE(fontSize, Float, 3, arguments);
        DEFINE(tint, raylib::Color, 4, arguments);

        DrawTextCodepoint(font->get_value(), codepoint->get_value(), position->get_value(), fontSize->get_value(),
                          tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_text_codepoints(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(7, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(codepoints, Integer, 1, arguments);
        DEFINE(count, Integer, 2, arguments);
        DEFINE(position, raylib::Vector2, 3, arguments);
        DEFINE(fontSize, Float, 4, arguments);
        DEFINE(spacing, Float, 5, arguments);
        DEFINE(tint, raylib::Color, 6, arguments);

//        DrawTextCodepoints(font->get_value(), codepoints->get_value(), count->get_value(), position->get_value(),
//                           fontSize->get_value(), spacing->get_value(), tint->get_value());
        TraceLog(LOG_WARNING, "draw_text_codepoints is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr measure_text(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(fontSize, Integer, 1, arguments);

        MeasureText(text->get_value().c_str(), fontSize->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr measure_text_ex(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(text, String, 1, arguments);
        DEFINE(fontSize, Float, 2, arguments);
        DEFINE(spacing, Float, 3, arguments);

        MeasureTextEx(font->get_value(), text->get_value().c_str(), fontSize->get_value(), spacing->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_glyph_index(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(codepoint, Integer, 1, arguments);

        GetGlyphIndex(font->get_value(), codepoint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_glyph_info(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(codepoint, Integer, 1, arguments);

        GetGlyphInfo(font->get_value(), codepoint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_glyph_atlas_rec(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(codepoint, Integer, 1, arguments);

        GetGlyphAtlasRec(font->get_value(), codepoint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr load_utf8(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(codepoints, Integer, 0, arguments);
        DEFINE(length, Integer, 1, arguments);

//        LoadUTF8(codepoints->get_value(), length->get_value());
        TraceLog(LOG_WARNING, "load_utf8 is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr unload_utf8(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, Integer, 0, arguments);
        TraceLog(LOG_WARNING, "unload_utf8 is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr load_codepoints(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(count, Integer, 1, arguments);
        TraceLog(LOG_WARNING, "load_codepoints is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr unload_codepoints(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(codepoints, Integer, 0, arguments);
        TraceLog(LOG_WARNING, "unload_codepoints is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr get_codepoint_count(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, String, 0, arguments);
        TraceLog(LOG_WARNING, "get_codepoint_count is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr get_codepoint(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(codepointSize, Integer, 1, arguments);
        TraceLog(LOG_WARNING, "get_codepoint is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr get_codepoint_next(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(codepointSize, Integer, 1, arguments);
        TraceLog(LOG_WARNING, "get_codepoint_next is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr get_codepoint_previous(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(codepointSize, Integer, 1, arguments);
        TraceLog(LOG_WARNING, "get_codepoint_previous is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr codepoint_to_utf8(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(codepoint, Integer, 0, arguments);
        DEFINE(utf8Size, Integer, 1, arguments);
        TraceLog(LOG_WARNING, "codepoint_to_utf8 is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr text_copy(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(dst, Integer, 0, arguments);
        DEFINE(src, String, 1, arguments);
        TraceLog(LOG_WARNING, "text_copy is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr text_is_equal(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(text1, String, 0, arguments);
        DEFINE(text2, String, 1, arguments);
        TraceLog(LOG_WARNING, "text_is_equal is not implemented use == instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_length(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, String, 0, arguments);
        TraceLog(LOG_WARNING, "text_length is not implemented use .length instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_format(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, String, 0, arguments);
        TraceLog(LOG_WARNING, "text_format is not implemented use fmt.format instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_subtext(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(position, Integer, 1, arguments);
        DEFINE(length, Integer, 2, arguments);
        TraceLog(LOG_WARNING, "text_subtext is not implemented use .substr instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_replace(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(text, Integer, 0, arguments);
        DEFINE(replace, String, 1, arguments);
        DEFINE(by, String, 2, arguments);

        TraceLog(LOG_WARNING, "text_replace is not implemented use .replace instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_insert(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(insert, String, 1, arguments);
        DEFINE(position, Integer, 2, arguments);

        TraceLog(LOG_WARNING, "text_insert is not implemented use .insert instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_join(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(textList, Integer, 0, arguments);
        DEFINE(count, Integer, 1, arguments);
        DEFINE(delimiter, String, 2, arguments);

        TraceLog(LOG_WARNING, "text_join is not implemented use .join instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_split(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(delimiter, Integer, 1, arguments);
        DEFINE(count, Integer, 2, arguments);

        TraceLog(LOG_WARNING, "text_split is not implemented use .split instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_append(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(text, Integer, 0, arguments);
        DEFINE(append, String, 1, arguments);
        DEFINE(position, Integer, 2, arguments);

        TraceLog(LOG_WARNING, "text_append is not implemented use .append instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_find_index(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(find, String, 1, arguments);

        TraceLog(LOG_WARNING, "text_find_index is not implemented use .find instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_to_upper(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, String, 0, arguments);

        TraceLog(LOG_WARNING, "text_to_upper is not implemented use .upper instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_to_lower(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, String, 0, arguments);

        TraceLog(LOG_WARNING, "text_to_lower is not implemented use .lower instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_to_pascal(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, String, 0, arguments);

        TraceLog(LOG_WARNING, "text_to_pascal is not implemented use .capitalize instead");
        return Globs::BondNil;
    }

    NativeErrorOr text_to_integer(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, String, 0, arguments);

        TraceLog(LOG_WARNING, "text_to_integer is not implemented use int() instead");
        return Globs::BondNil;
    }
}