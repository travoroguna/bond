#include "raytexture.h"
#include "../raymodule.h"


extern bond::GarbageCollector *m_gc;
extern bond::Context *m_ctx;


namespace bond::raytexture {
    NativeErrorOr load_image(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);
        return GarbageCollector::instance().make<raylib::Image>(LoadImage(fileName->get_value().c_str()));
    }

    NativeErrorOr load_image_raw(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(fileName, String, 0, arguments);
        DEFINE(width, Integer, 1, arguments);
        DEFINE(height, Integer, 2, arguments);
        DEFINE(format, Integer, 3, arguments);
        DEFINE(headerSize, Integer, 4, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                LoadImageRaw(fileName->get_value().c_str(), width->get_value(), height->get_value(),
                             format->get_value(), headerSize->get_value()));
    }

    NativeErrorOr load_image_anim(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(fileName, String, 0, arguments);
        DEFINE(frames, Integer, 1, arguments);

        int framesPtr = frames->get_value();
        return GarbageCollector::instance().make<raylib::Image>(
                LoadImageAnim(fileName->get_value().c_str(), &framesPtr));
    }

    NativeErrorOr load_image_from_memory(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(fileType, String, 0, arguments);
        DEFINE(fileData, String, 1, arguments);
        DEFINE(dataSize, Integer, 2, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                LoadImageFromMemory(fileType->get_value().c_str(), (unsigned char *) fileData->get_value().c_str(),
                                    dataSize->get_value()));
    }

    NativeErrorOr load_image_from_texture(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);

        return GarbageCollector::instance().make<raylib::Image>(LoadImageFromTexture(texture->get_value()));
    }

    NativeErrorOr load_image_from_screen(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);

        return GarbageCollector::instance().make<raylib::Image>(LoadImageFromScreen());
    }

    NativeErrorOr is_image_ready(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        return BOOL_(IsImageReady(image->get_value()));
    }

    NativeErrorOr unload_image(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        UnloadImage(image->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr export_image(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(fileName, String, 1, arguments);

        ExportImage(image->get_value(), fileName->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr export_image_as_code(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(fileName, String, 1, arguments);

        ExportImageAsCode(image->get_value(), fileName->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr gen_image_color(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                GenImageColor(width->get_value(), height->get_value(), color->get_value()));
    }

    NativeErrorOr gen_image_gradient_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(top, raylib::Color, 2, arguments);
        DEFINE(bottom, raylib::Color, 3, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                GenImageGradientV(width->get_value(), height->get_value(), top->get_value(), bottom->get_value()));
    }

    NativeErrorOr gen_image_gradient_h(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(left, raylib::Color, 2, arguments);
        DEFINE(right, raylib::Color, 3, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                GenImageGradientH(width->get_value(), height->get_value(), left->get_value(), right->get_value()));
    }

    NativeErrorOr gen_image_gradient_radial(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(density, Float, 2, arguments);
        DEFINE(inner, raylib::Color, 3, arguments);
        DEFINE(outer, raylib::Color, 4, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                GenImageGradientRadial(width->get_value(), height->get_value(), density->get_value(),
                                       inner->get_value(), outer->get_value()));
    }

    NativeErrorOr gen_image_checked(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(checksX, Integer, 2, arguments);
        DEFINE(checksY, Integer, 3, arguments);
        DEFINE(col1, raylib::Color, 4, arguments);
        DEFINE(col2, raylib::Color, 5, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                GenImageChecked(width->get_value(), height->get_value(), checksX->get_value(), checksY->get_value(),
                                col1->get_value(), col2->get_value()));
    }

    NativeErrorOr gen_image_white_noise(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(factor, Float, 2, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                GenImageWhiteNoise(width->get_value(), height->get_value(), factor->get_value()));
    }

    NativeErrorOr gen_image_perlin_noise(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(offsetX, Integer, 2, arguments);
        DEFINE(offsetY, Integer, 3, arguments);
        DEFINE(scale, Float, 4, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                GenImagePerlinNoise(width->get_value(), height->get_value(), offsetX->get_value(), offsetY->get_value(),
                                    scale->get_value()));
    }

    NativeErrorOr gen_image_cellular(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(tileSize, Integer, 2, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                GenImageCellular(width->get_value(), height->get_value(), tileSize->get_value()));
    }

    NativeErrorOr gen_image_text(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(text, String, 2, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                GenImageText(width->get_value(), height->get_value(), text->get_value().c_str()));
    }

    NativeErrorOr image_copy(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        return GarbageCollector::instance().make<raylib::Image>(ImageCopy(image->get_value()));
    }

    NativeErrorOr image_from_image(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(rec, raylib::Rectangle, 1, arguments);

        return GarbageCollector::instance().make<raylib::Image>(ImageFromImage(image->get_value(), rec->get_value()));
    }

    NativeErrorOr image_text(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(text, String, 0, arguments);
        DEFINE(fontSize, Integer, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                ImageText(text->get_value().c_str(), fontSize->get_value(), color->get_value()));
    }

    NativeErrorOr image_text_ex(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(font, raylib::Font, 0, arguments);
        DEFINE(text, String, 1, arguments);
        DEFINE(fontSize, Float, 2, arguments);
        DEFINE(spacing, Float, 3, arguments);
        DEFINE(tint, raylib::Color, 4, arguments);

        return GarbageCollector::instance().make<raylib::Image>(
                ImageTextEx(font->get_value(), text->get_value().c_str(), fontSize->get_value(), spacing->get_value(),
                            tint->get_value()));
    }

    NativeErrorOr image_format(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(newFormat, Integer, 1, arguments);

        ImageFormat(image->get(), newFormat->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_to_pot(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(fill, raylib::Color, 1, arguments);

        ImageToPOT(image->get(), fill->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_crop(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(crop, raylib::Rectangle, 1, arguments);

        ImageCrop(image->get(), crop->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_alpha_crop(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(threshold, Float, 1, arguments);

        ImageAlphaCrop(image->get(), threshold->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_alpha_clear(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(color, raylib::Color, 1, arguments);
        DEFINE(threshold, Float, 2, arguments);

        ImageAlphaClear(image->get(), color->get_value(), threshold->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_alpha_mask(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(alphaMask, raylib::Image, 1, arguments);

        ImageAlphaMask(image->get(), alphaMask->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_alpha_premultiply(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        ImageAlphaPremultiply(image->get());
        return Globs::BondNil;
    }

    NativeErrorOr image_blur_gaussian(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(blurSize, Integer, 1, arguments);

        ImageBlurGaussian(image->get(), blurSize->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_resize(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(newWidth, Integer, 1, arguments);
        DEFINE(newHeight, Integer, 2, arguments);

        ImageResize(image->get(), newWidth->get_value(), newHeight->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_resize_nn(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(newWidth, Integer, 1, arguments);
        DEFINE(newHeight, Integer, 2, arguments);

        ImageResizeNN(image->get(), newWidth->get_value(), newHeight->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_resize_canvas(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(newWidth, Integer, 1, arguments);
        DEFINE(newHeight, Integer, 2, arguments);
        DEFINE(offsetX, Integer, 3, arguments);
        DEFINE(offsetY, Integer, 4, arguments);
        DEFINE(fill, raylib::Color, 5, arguments);

        ImageResizeCanvas(image->get(), newWidth->get_value(), newHeight->get_value(), offsetX->get_value(),
                          offsetY->get_value(), fill->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_mipmaps(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        ImageMipmaps(image->get());
        return Globs::BondNil;
    }

    NativeErrorOr image_dither(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(rBpp, Integer, 1, arguments);
        DEFINE(gBpp, Integer, 2, arguments);
        DEFINE(bBpp, Integer, 3, arguments);
        DEFINE(aBpp, Integer, 4, arguments);

        ImageDither(image->get(), rBpp->get_value(), gBpp->get_value(), bBpp->get_value(), aBpp->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_flip_vertical(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        ImageFlipVertical(image->get());
        return Globs::BondNil;
    }

    NativeErrorOr image_flip_horizontal(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        ImageFlipHorizontal(image->get());
        return Globs::BondNil;
    }

    NativeErrorOr image_rotate_cw(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        ImageRotateCW(image->get());
        return Globs::BondNil;
    }

    NativeErrorOr image_rotate_ccw(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        ImageRotateCCW(image->get());
        return Globs::BondNil;
    }

    NativeErrorOr image_color_tint(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(color, raylib::Color, 1, arguments);

        ImageColorTint(image->get(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_color_invert(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        ImageColorInvert(image->get());
        return Globs::BondNil;
    }

    NativeErrorOr image_color_grayscale(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        ImageColorGrayscale(image->get());
        return Globs::BondNil;
    }

    NativeErrorOr image_color_contrast(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(contrast, Float, 1, arguments);

        ImageColorContrast(image->get(), contrast->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_color_brightness(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(brightness, Integer, 1, arguments);

        ImageColorBrightness(image->get(), brightness->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_color_replace(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(color, raylib::Color, 1, arguments);
        DEFINE(replace, raylib::Color, 2, arguments);

        ImageColorReplace(image->get(), color->get_value(), replace->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr load_image_colors(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

//        auto colors = LoadImageColors(image->get_value());
        TraceLog(LOG_WARNING, "LoadImageColors not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr load_image_palette(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(maxPaletteSize, Integer, 1, arguments);
        DEFINE(colorCount, Integer, 2, arguments);

        TraceLog(LOG_WARNING, "LoadImagePalette not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr unload_image_colors(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(colors, raylib::Color, 0, arguments);

        TraceLog(LOG_WARNING, "UnloadImageColors not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr unload_image_palette(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(colors, raylib::Color, 0, arguments);

        TraceLog(LOG_WARNING, "UnloadImagePalette not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr get_image_alpha_border(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(threshold, Float, 1, arguments);


        return GarbageCollector::instance().make<raylib::Rectangle>(
                GetImageAlphaBorder(image->get_value(), threshold->get_value()));
    }

    NativeErrorOr get_image_color(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(x, Integer, 1, arguments);
        DEFINE(y, Integer, 2, arguments);

        return GarbageCollector::instance().make<raylib::Color>(
                GetImageColor(image->get_value(), x->get_value(), y->get_value()));
    }

    NativeErrorOr image_clear_background(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(color, raylib::Color, 1, arguments);

        ImageClearBackground(dst->get(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_pixel(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(posX, Integer, 1, arguments);
        DEFINE(posY, Integer, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        ImageDrawPixel(dst->get(), posX->get_value(), posY->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_pixel_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(position, raylib::Vector2, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        ImageDrawPixelV(dst->get(), position->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_line(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(startPosX, Integer, 1, arguments);
        DEFINE(startPosY, Integer, 2, arguments);
        DEFINE(endPosX, Integer, 3, arguments);
        DEFINE(endPosY, Integer, 4, arguments);
        DEFINE(color, raylib::Color, 5, arguments);

        ImageDrawLine(dst->get(), startPosX->get_value(), startPosY->get_value(), endPosX->get_value(),
                      endPosY->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_line_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(start, raylib::Vector2, 1, arguments);
        DEFINE(end, raylib::Vector2, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        ImageDrawLineV(dst->get(), start->get_value(), end->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_circle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(centerX, Integer, 1, arguments);
        DEFINE(centerY, Integer, 2, arguments);
        DEFINE(radius, Integer, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        ImageDrawCircle(dst->get(), centerX->get_value(), centerY->get_value(), radius->get_value(),
                        color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_circle_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(center, raylib::Vector2, 1, arguments);
        DEFINE(radius, Integer, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        ImageDrawCircleV(dst->get(), center->get_value(), radius->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_circle_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(centerX, Integer, 1, arguments);
        DEFINE(centerY, Integer, 2, arguments);
        DEFINE(radius, Integer, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        ImageDrawCircleLines(dst->get(), centerX->get_value(), centerY->get_value(), radius->get_value(),
                             color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_circle_lines_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(center, raylib::Vector2, 1, arguments);
        DEFINE(radius, Integer, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        ImageDrawCircleLinesV(dst->get(), center->get_value(), radius->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_rectangle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(posX, Integer, 1, arguments);
        DEFINE(posY, Integer, 2, arguments);
        DEFINE(width, Integer, 3, arguments);
        DEFINE(height, Integer, 4, arguments);
        DEFINE(color, raylib::Color, 5, arguments);

        ImageDrawRectangle(dst->get(), posX->get_value(), posY->get_value(), width->get_value(), height->get_value(),
                           color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_rectangle_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(position, raylib::Vector2, 1, arguments);
        DEFINE(size, raylib::Vector2, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        ImageDrawRectangleV(dst->get(), position->get_value(), size->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_rectangle_rec(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(rec, raylib::Rectangle, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        ImageDrawRectangleRec(dst->get(), rec->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_rectangle_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(rec, raylib::Rectangle, 1, arguments);
        DEFINE(thick, Integer, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        ImageDrawRectangleLines(dst->get(), rec->get_value(), thick->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(src, raylib::Image, 1, arguments);
        DEFINE(srcRec, raylib::Rectangle, 2, arguments);
        DEFINE(dstRec, raylib::Rectangle, 3, arguments);
        DEFINE(tint, raylib::Color, 4, arguments);

        ImageDraw(dst->get(), src->get_value(), srcRec->get_value(), dstRec->get_value(), tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_text(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(text, String, 1, arguments);
        DEFINE(posX, Integer, 2, arguments);
        DEFINE(posY, Integer, 3, arguments);
        DEFINE(fontSize, Integer, 4, arguments);
        DEFINE(color, raylib::Color, 5, arguments);

        ImageDrawText(dst->get(), text->get_value().c_str(), posX->get_value(), posY->get_value(),
                      fontSize->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr image_draw_text_ex(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(7, arguments);
        DEFINE(dst, raylib::Image, 0, arguments);
        DEFINE(font, raylib::Font, 1, arguments);
        DEFINE(text, String, 2, arguments);
        DEFINE(position, raylib::Vector2, 3, arguments);
        DEFINE(fontSize, Float, 4, arguments);
        DEFINE(spacing, Float, 5, arguments);
        DEFINE(tint, raylib::Color, 6, arguments);

        ImageDrawTextEx(dst->get(), font->get_value(), text->get_value().c_str(), position->get_value(),
                        fontSize->get_value(), spacing->get_value(), tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr load_texture(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);


        return GarbageCollector::instance().make<raylib::Texture2D>(LoadTexture(fileName->get_value().c_str()));
    }

    NativeErrorOr load_texture_from_image(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);

        return GarbageCollector::instance().make<raylib::Texture2D>(LoadTextureFromImage(image->get_value()));
    }

    NativeErrorOr load_texture_cubemap(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        DEFINE(layout, Integer, 1, arguments);

        return GarbageCollector::instance().make<raylib::TextureCubemap>(
                LoadTextureCubemap(image->get_value(), layout->get_value()));
    }

    NativeErrorOr load_render_texture(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);

        return GarbageCollector::instance().make<raylib::RenderTexture2D>(
                LoadRenderTexture(width->get_value(), height->get_value()));
    }

    NativeErrorOr is_texture_ready(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);

        return BOOL_(IsTextureReady(texture->get_value()));
    }

    NativeErrorOr unload_texture(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        TraceLog(LOG_INFO, "Unloading texture not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr is_render_texture_ready(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(target, raylib::RenderTexture2D, 0, arguments);

        return BOOL_(IsRenderTextureReady(target->get_value()));
    }

    NativeErrorOr unload_render_texture(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(target, raylib::RenderTexture2D, 0, arguments);
        TraceLog(LOG_INFO, "Unloading render texture not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr update_texture(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(pixels, Integer, 1, arguments);

        TraceLog(LOG_WARNING, "Update texture not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr update_texture_rec(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(rec, raylib::Rectangle, 1, arguments);
        DEFINE(pixels, Integer, 2, arguments);

        TraceLog(LOG_WARNING, "Update texture rec not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr gen_texture_mipmaps(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        GenTextureMipmaps(texture->get());
        return Globs::BondNil;
    }

    NativeErrorOr set_texture_filter(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(filter, Integer, 1, arguments);
        SetTextureFilter(texture->get_value(), filter->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_texture_wrap(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(wrap, Integer, 1, arguments);
        SetTextureWrap(texture->get_value(), wrap->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_texture(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(posX, Integer, 1, arguments);
        DEFINE(posY, Integer, 2, arguments);
        DEFINE(tint, raylib::Color, 3, arguments);

        DrawTexture(texture->get_value(), posX->get_value(), posY->get_value(), tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_texture_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(position, raylib::Vector2, 1, arguments);
        DEFINE(tint, raylib::Color, 2, arguments);

        DrawTextureV(texture->get_value(), position->get_value(), tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_texture_ex(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(position, raylib::Vector2, 1, arguments);
        DEFINE(rotation, Float, 2, arguments);
        DEFINE(scale, Float, 3, arguments);
        DEFINE(tint, raylib::Color, 4, arguments);

        DrawTextureEx(texture->get_value(), position->get_value(), rotation->get_value(), scale->get_value(),
                      tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_texture_rec(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(source, raylib::Rectangle, 1, arguments);
        DEFINE(position, raylib::Vector2, 2, arguments);
        DEFINE(tint, raylib::Color, 3, arguments);

        DrawTextureRec(texture->get_value(), source->get_value(), position->get_value(), tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_texture_pro(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(source, raylib::Rectangle, 1, arguments);
        DEFINE(dest, raylib::Rectangle, 2, arguments);
        DEFINE(origin, raylib::Vector2, 3, arguments);
        DEFINE(rotation, Float, 4, arguments);
        DEFINE(tint, raylib::Color, 5, arguments);

        DrawTexturePro(texture->get_value(), source->get_value(), dest->get_value(), origin->get_value(),
                       rotation->get_value(), tint->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_texture_n_patch(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(nPatchInfo, Integer, 1, arguments);
        DEFINE(dest, raylib::Rectangle, 2, arguments);
        DEFINE(origin, raylib::Vector2, 3, arguments);
        DEFINE(rotation, Float, 4, arguments);
        DEFINE(tint, raylib::Color, 5, arguments);

        TraceLog(LOG_WARNING, "draw_texture_n_patch not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr fade(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(color, raylib::Color, 0, arguments);
        DEFINE(alpha, Float, 1, arguments);


        return GarbageCollector::instance().make<raylib::Color>(Fade(color->get_value(), alpha->get_value()));
    }

    NativeErrorOr color_to_int(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(color, raylib::Color, 0, arguments);

        return GarbageCollector::instance().make<Integer>(ColorToInt(color->get_value()));
    }

    NativeErrorOr color_normalize(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(color, raylib::Color, 0, arguments);

        return GarbageCollector::instance().make<raylib::Vector4>(ColorNormalize(color->get_value()));
    }

    NativeErrorOr color_from_normalized(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(normalized, raylib::Vector4, 0, arguments);

        return GarbageCollector::instance().make<raylib::Color>(ColorFromNormalized(normalized->get_value()));
    }

    NativeErrorOr color_to_hsv(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(color, raylib::Color, 0, arguments);

        return GarbageCollector::instance().make<raylib::Vector3>(ColorToHSV(color->get_value()));
    }

    NativeErrorOr color_from_hsv(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(hue, Float, 0, arguments);
        DEFINE(saturation, Float, 1, arguments);
        DEFINE(value, Float, 2, arguments);

        return GarbageCollector::instance().make<raylib::Color>(
                ColorFromHSV(hue->get_value(), saturation->get_value(), value->get_value()));
    }

    NativeErrorOr color_tint(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(color, raylib::Color, 0, arguments);
        DEFINE(tint, raylib::Color, 1, arguments);

        return GarbageCollector::instance().make<raylib::Color>(ColorTint(color->get_value(), tint->get_value()));
    }

    NativeErrorOr color_brightness(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(color, raylib::Color, 0, arguments);
        DEFINE(factor, Float, 1, arguments);

        return GarbageCollector::instance().make<raylib::Color>(
                ColorBrightness(color->get_value(), factor->get_value()));
    }

    NativeErrorOr color_contrast(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(color, raylib::Color, 0, arguments);
        DEFINE(contrast, Float, 1, arguments);

        return GarbageCollector::instance().make<raylib::Color>(
                ColorContrast(color->get_value(), contrast->get_value()));
    }

    NativeErrorOr color_alpha(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(color, raylib::Color, 0, arguments);
        DEFINE(alpha, Float, 1, arguments);

        return GarbageCollector::instance().make<raylib::Color>(ColorAlpha(color->get_value(), alpha->get_value()));
    }

    NativeErrorOr color_alpha_blend(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(dst, raylib::Color, 0, arguments);
        DEFINE(src, raylib::Color, 1, arguments);
        DEFINE(tint, raylib::Color, 2, arguments);

        return GarbageCollector::instance().make<raylib::Color>(
                ColorAlphaBlend(dst->get_value(), src->get_value(), tint->get_value()));
    }

    NativeErrorOr get_color(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(hexValue, Integer, 0, arguments);

        return GarbageCollector::instance().make<raylib::Color>(GetColor(hexValue->get_value()));
    }

    NativeErrorOr get_pixel_color(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(srcPtr, Integer, 0, arguments);
        DEFINE(format, Integer, 1, arguments);

        TraceLog(LOG_WARNING, "get_pixel_color not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr set_pixel_color(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(dstPtr, Integer, 0, arguments);
        DEFINE(color, raylib::Color, 1, arguments);
        DEFINE(format, Integer, 2, arguments);

        TraceLog(LOG_WARNING, "set_pixel_color not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr get_pixel_data_size(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(format, Integer, 2, arguments);


        return GarbageCollector::instance().make<Integer>(
                GetPixelDataSize(width->get_value(), height->get_value(), format->get_value()));
    }
}