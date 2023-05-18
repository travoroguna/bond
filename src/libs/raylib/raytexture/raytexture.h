#include "../../../bond.h"
#include "../raymodule.h"
#include <raylib.h>


namespace bond::raytexture {
    NativeErrorOr load_image(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_image_raw(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_image_anim(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_image_from_memory(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_image_from_texture(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_image_from_screen(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_image_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_image(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr export_image(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr export_image_as_code(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_color(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_gradient_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_gradient_h(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_gradient_radial(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_checked(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_white_noise(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_perlin_noise(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_cellular(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_image_text(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_copy(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_from_image(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_text(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_text_ex(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_format(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_to_pot(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_crop(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_alpha_crop(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_alpha_clear(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_alpha_mask(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_alpha_premultiply(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_blur_gaussian(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_resize(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_resize_nn(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_resize_canvas(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_mipmaps(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_dither(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_flip_vertical(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_flip_horizontal(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_rotate_cw(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_rotate_ccw(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_color_tint(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_color_invert(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_color_grayscale(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_color_contrast(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_color_brightness(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_color_replace(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_image_colors(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_image_palette(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_image_colors(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_image_palette(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_image_alpha_border(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_image_color(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_clear_background(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_pixel(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_pixel_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_line(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_line_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_circle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_circle_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_circle_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_circle_lines_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_rectangle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_rectangle_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_rectangle_rec(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_rectangle_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_text(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr image_draw_text_ex(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_texture(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_texture_from_image(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_texture_cubemap(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_render_texture(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_texture_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_texture(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_render_texture_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_render_texture(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr update_texture(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr update_texture_rec(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gen_texture_mipmaps(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_texture_filter(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_texture_wrap(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_texture(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_texture_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_texture_ex(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_texture_rec(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_texture_pro(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_texture_n_patch(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr fade(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_to_int(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_normalize(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_from_normalized(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_to_hsv(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_from_hsv(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_tint(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_brightness(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_contrast(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_alpha(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr color_alpha_blend(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_color(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_pixel_color(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_pixel_color(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_pixel_data_size(const std::vector<GcPtr<Object>> &arguments);
}