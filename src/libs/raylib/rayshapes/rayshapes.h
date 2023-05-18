#include "../../../bond.h"
#include "../raymodule.h"


namespace bond::rayshapes {

    NativeErrorOr set_shapes_texture(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_pixel(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_pixel_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_line(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_line_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_line_ex(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_line_bezier(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_line_bezier_quad(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_line_bezier_cubic(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_line_strip(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_circle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_circle_sector(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_circle_sector_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_circle_gradient(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_circle_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_circle_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_ellipse(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_ellipse_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_ring(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_ring_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_rec(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_pro(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_gradient_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_gradient_h(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_gradient_ex(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_lines_ex(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_rounded(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_rectangle_rounded_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_triangle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_triangle_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_triangle_fan(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_triangle_strip(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_poly(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_poly_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr draw_poly_lines_ex(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr check_collision_recs(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr check_collision_circles(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr check_collision_circle_rec(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr check_collision_point_rec(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr check_collision_point_circle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr check_collision_point_triangle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr check_collision_point_poly(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr check_collision_lines(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr check_collision_point_line(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_collision_rec(const std::vector<GcPtr<Object>> &arguments);
}