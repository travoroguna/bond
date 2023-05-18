#include "rayshapes.h"


extern bond::GarbageCollector *m_gc;
extern bond::Context *m_ctx;

namespace bond::rayshapes {

    NativeErrorOr set_shapes_texture(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(texture, raylib::Texture2D, 0, arguments);
        DEFINE(source, raylib::Rectangle, 1, arguments);

        SetShapesTexture(texture->get_value(), source->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_pixel(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(posX, Integer, 0, arguments);
        DEFINE(posY, Integer, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        DrawPixel(posX->get_value(), posY->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_pixel_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(position, raylib::Vector2, 0, arguments);
        DEFINE(color, raylib::Color, 1, arguments);

        DrawPixelV(position->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_line(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(startPosX, Integer, 0, arguments);
        DEFINE(startPosY, Integer, 1, arguments);
        DEFINE(endPosX, Integer, 2, arguments);
        DEFINE(endPosY, Integer, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        DrawLine(startPosX->get_value(), startPosY->get_value(), endPosX->get_value(), endPosY->get_value(),
                 color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_line_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(startPos, raylib::Vector2, 0, arguments);
        DEFINE(endPos, raylib::Vector2, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        DrawLineV(startPos->get_value(), endPos->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_line_ex(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(startPos, raylib::Vector2, 0, arguments);
        DEFINE(endPos, raylib::Vector2, 1, arguments);
        DEFINE(thick, Float, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        DrawLineEx(startPos->get_value(), endPos->get_value(), thick->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_line_bezier(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(startPos, raylib::Vector2, 0, arguments);
        DEFINE(endPos, raylib::Vector2, 1, arguments);
        DEFINE(thick, Float, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        DrawLineBezier(startPos->get_value(), endPos->get_value(), thick->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_line_bezier_quad(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(startPos, raylib::Vector2, 0, arguments);
        DEFINE(endPos, raylib::Vector2, 1, arguments);
        DEFINE(controlPos, raylib::Vector2, 2, arguments);
        DEFINE(thick, Float, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        DrawLineBezierQuad(startPos->get_value(), endPos->get_value(), controlPos->get_value(), thick->get_value(),
                           color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_line_bezier_cubic(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(startPos, raylib::Vector2, 0, arguments);
        DEFINE(endPos, raylib::Vector2, 1, arguments);
        DEFINE(startControlPos, raylib::Vector2, 2, arguments);
        DEFINE(endControlPos, raylib::Vector2, 3, arguments);
        DEFINE(thick, Float, 4, arguments);
        DEFINE(color, raylib::Color, 5, arguments);

        DrawLineBezierCubic(startPos->get_value(), endPos->get_value(), startControlPos->get_value(),
                            endControlPos->get_value(), thick->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_line_strip(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(points, raylib::Vector2, 0, arguments);
        DEFINE(pointCount, Integer, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        TraceLog(LOG_WARNING, "draw_line_strin not implemented");

//        DrawLineStrip(points->get(), pointCount->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_circle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(centerX, Integer, 0, arguments);
        DEFINE(centerY, Integer, 1, arguments);
        DEFINE(radius, Float, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        DrawCircle(centerX->get_value(), centerY->get_value(), radius->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_circle_sector(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(center, raylib::Vector2, 0, arguments);
        DEFINE(radius, Float, 1, arguments);
        DEFINE(startAngle, Float, 2, arguments);
        DEFINE(endAngle, Float, 3, arguments);
        DEFINE(segments, Integer, 4, arguments);
        DEFINE(color, raylib::Color, 5, arguments);

        DrawCircleSector(center->get_value(), radius->get_value(), startAngle->get_value(), endAngle->get_value(),
                         segments->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_circle_sector_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(center, raylib::Vector2, 0, arguments);
        DEFINE(radius, Float, 1, arguments);
        DEFINE(startAngle, Float, 2, arguments);
        DEFINE(endAngle, Float, 3, arguments);
        DEFINE(segments, Integer, 4, arguments);
        DEFINE(color, raylib::Color, 5, arguments);

        DrawCircleSectorLines(center->get_value(), radius->get_value(), startAngle->get_value(), endAngle->get_value(),
                              segments->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_circle_gradient(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(centerX, Integer, 0, arguments);
        DEFINE(centerY, Integer, 1, arguments);
        DEFINE(radius, Float, 2, arguments);
        DEFINE(color1, raylib::Color, 3, arguments);
        DEFINE(color2, raylib::Color, 4, arguments);

        DrawCircleGradient(centerX->get_value(), centerY->get_value(), radius->get_value(), color1->get_value(),
                           color2->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_circle_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(center, raylib::Vector2, 0, arguments);
        DEFINE(radius, Float, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        DrawCircleV(center->get_value(), radius->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_circle_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(centerX, Integer, 0, arguments);
        DEFINE(centerY, Integer, 1, arguments);
        DEFINE(radius, Float, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        DrawCircleLines(centerX->get_value(), centerY->get_value(), radius->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_ellipse(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(centerX, Integer, 0, arguments);
        DEFINE(centerY, Integer, 1, arguments);
        DEFINE(radiusH, Float, 2, arguments);
        DEFINE(radiusV, Float, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        DrawEllipse(centerX->get_value(), centerY->get_value(), radiusH->get_value(), radiusV->get_value(),
                    color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_ellipse_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(centerX, Integer, 0, arguments);
        DEFINE(centerY, Integer, 1, arguments);
        DEFINE(radiusH, Float, 2, arguments);
        DEFINE(radiusV, Float, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        DrawEllipseLines(centerX->get_value(), centerY->get_value(), radiusH->get_value(), radiusV->get_value(),
                         color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_ring(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(7, arguments);
        DEFINE(center, raylib::Vector2, 0, arguments);
        DEFINE(innerRadius, Float, 1, arguments);
        DEFINE(outerRadius, Float, 2, arguments);
        DEFINE(startAngle, Float, 3, arguments);
        DEFINE(endAngle, Float, 4, arguments);
        DEFINE(segments, Integer, 5, arguments);
        DEFINE(color, raylib::Color, 6, arguments);

        DrawRing(center->get_value(), innerRadius->get_value(), outerRadius->get_value(), startAngle->get_value(),
                 endAngle->get_value(), segments->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_ring_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(7, arguments);
        DEFINE(center, raylib::Vector2, 0, arguments);
        DEFINE(innerRadius, Float, 1, arguments);
        DEFINE(outerRadius, Float, 2, arguments);
        DEFINE(startAngle, Float, 3, arguments);
        DEFINE(endAngle, Float, 4, arguments);
        DEFINE(segments, Integer, 5, arguments);
        DEFINE(color, raylib::Color, 6, arguments);

        DrawRingLines(center->get_value(), innerRadius->get_value(), outerRadius->get_value(), startAngle->get_value(),
                      endAngle->get_value(), segments->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(posX, Integer, 0, arguments);
        DEFINE(posY, Integer, 1, arguments);
        DEFINE(width, Integer, 2, arguments);
        DEFINE(height, Integer, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        DrawRectangle(posX->get_value(), posY->get_value(), width->get_value(), height->get_value(),
                      color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(position, raylib::Vector2, 0, arguments);
        DEFINE(size, raylib::Vector2, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        DrawRectangleV(position->get_value(), size->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_rec(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(rec, raylib::Rectangle, 0, arguments);
        DEFINE(color, raylib::Color, 1, arguments);

        DrawRectangleRec(rec->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_pro(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(rec, raylib::Rectangle, 0, arguments);
        DEFINE(origin, raylib::Vector2, 1, arguments);
        DEFINE(rotation, Float, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        DrawRectanglePro(rec->get_value(), origin->get_value(), rotation->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_gradient_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(posX, Integer, 0, arguments);
        DEFINE(posY, Integer, 1, arguments);
        DEFINE(width, Integer, 2, arguments);
        DEFINE(height, Integer, 3, arguments);
        DEFINE(color1, raylib::Color, 4, arguments);
        DEFINE(color2, raylib::Color, 5, arguments);

        DrawRectangleGradientV(posX->get_value(), posY->get_value(), width->get_value(), height->get_value(),
                               color1->get_value(), color2->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_gradient_h(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(posX, Integer, 0, arguments);
        DEFINE(posY, Integer, 1, arguments);
        DEFINE(width, Integer, 2, arguments);
        DEFINE(height, Integer, 3, arguments);
        DEFINE(color1, raylib::Color, 4, arguments);
        DEFINE(color2, raylib::Color, 5, arguments);

        DrawRectangleGradientH(posX->get_value(), posY->get_value(), width->get_value(), height->get_value(),
                               color1->get_value(), color2->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_gradient_ex(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(rec, raylib::Rectangle, 0, arguments);
        DEFINE(col1, raylib::Color, 1, arguments);
        DEFINE(col2, raylib::Color, 2, arguments);
        DEFINE(col3, raylib::Color, 3, arguments);
        DEFINE(col4, raylib::Color, 4, arguments);

        DrawRectangleGradientEx(rec->get_value(), col1->get_value(), col2->get_value(), col3->get_value(),
                                col4->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(posX, Integer, 0, arguments);
        DEFINE(posY, Integer, 1, arguments);
        DEFINE(width, Integer, 2, arguments);
        DEFINE(height, Integer, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        DrawRectangleLines(posX->get_value(), posY->get_value(), width->get_value(), height->get_value(),
                           color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_lines_ex(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(rec, raylib::Rectangle, 0, arguments);
        DEFINE(lineThick, Float, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        DrawRectangleLinesEx(rec->get_value(), lineThick->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_rounded(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(rec, raylib::Rectangle, 0, arguments);
        DEFINE(roundness, Float, 1, arguments);
        DEFINE(segments, Integer, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        DrawRectangleRounded(rec->get_value(), roundness->get_value(), segments->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_rectangle_rounded_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(rec, raylib::Rectangle, 0, arguments);
        DEFINE(roundness, Float, 1, arguments);
        DEFINE(segments, Integer, 2, arguments);
        DEFINE(lineThick, Float, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        DrawRectangleRoundedLines(rec->get_value(), roundness->get_value(), segments->get_value(),
                                  lineThick->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_triangle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(v1, raylib::Vector2, 0, arguments);
        DEFINE(v2, raylib::Vector2, 1, arguments);
        DEFINE(v3, raylib::Vector2, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        DrawTriangle(v1->get_value(), v2->get_value(), v3->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_triangle_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(v1, raylib::Vector2, 0, arguments);
        DEFINE(v2, raylib::Vector2, 1, arguments);
        DEFINE(v3, raylib::Vector2, 2, arguments);
        DEFINE(color, raylib::Color, 3, arguments);

        DrawTriangleLines(v1->get_value(), v2->get_value(), v3->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_triangle_fan(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(points, raylib::Vector2, 0, arguments);
        DEFINE(pointCount, Integer, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        TraceLog(LOG_WARNING, "draw_poly not implemented");

        //DrawTriangleFan(points->get(), pointCount->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_triangle_strip(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(points, raylib::Vector2, 0, arguments);
        DEFINE(pointCount, Integer, 1, arguments);
        DEFINE(color, raylib::Color, 2, arguments);

        TraceLog(LOG_WARNING, "draw_triangle_strip not implemented");
//        DrawTriangleStrip(points->get(), pointCount->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_poly(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(center, raylib::Vector2, 0, arguments);
        DEFINE(sides, Integer, 1, arguments);
        DEFINE(radius, Float, 2, arguments);
        DEFINE(rotation, Float, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        TraceLog(LOG_WARNING, "draw_poly not implemented");
        //DrawPoly(center->get(), sides->get_value(), radius->get_value(), rotation->get_value(), color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr draw_poly_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(center, raylib::Vector2, 0, arguments);
        DEFINE(sides, Integer, 1, arguments);
        DEFINE(radius, Float, 2, arguments);
        DEFINE(rotation, Float, 3, arguments);
        DEFINE(color, raylib::Color, 4, arguments);

        TraceLog(LOG_WARNING, "draw_poly_lines not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr draw_poly_lines_ex(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(center, raylib::Vector2, 0, arguments);
        DEFINE(sides, Integer, 1, arguments);
        DEFINE(radius, Float, 2, arguments);
        DEFINE(rotation, Float, 3, arguments);
        DEFINE(lineThick, Float, 4, arguments);
        DEFINE(color, raylib::Color, 5, arguments);

        TraceLog(LOG_WARNING, "draw_poly_lines_ex not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr check_collision_recs(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(rec1, raylib::Rectangle, 0, arguments);
        DEFINE(rec2, raylib::Rectangle, 1, arguments);

        return BOOL_(CheckCollisionRecs(rec1->get_value(), rec2->get_value()));
    }

    NativeErrorOr check_collision_circles(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(center1, raylib::Vector2, 0, arguments);
        DEFINE(radius1, Float, 1, arguments);
        DEFINE(center2, raylib::Vector2, 2, arguments);
        DEFINE(radius2, Float, 3, arguments);

        return BOOL_(CheckCollisionCircles(center1->get_value(), radius1->get_value(), center2->get_value(),
                                           radius2->get_value()));
    }

    NativeErrorOr check_collision_circle_rec(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(center, raylib::Vector2, 0, arguments);
        DEFINE(radius, Float, 1, arguments);
        DEFINE(rec, raylib::Rectangle, 2, arguments);

        return BOOL_(CheckCollisionCircleRec(center->get_value(), radius->get_value(), rec->get_value()));
    }

    NativeErrorOr check_collision_point_rec(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(point, raylib::Vector2, 0, arguments);
        DEFINE(rec, raylib::Rectangle, 1, arguments);

        return BOOL_(CheckCollisionPointRec(point->get_value(), rec->get_value()));
    }

    NativeErrorOr check_collision_point_circle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(point, raylib::Vector2, 0, arguments);
        DEFINE(center, raylib::Vector2, 1, arguments);
        DEFINE(radius, Float, 2, arguments);

        return BOOL_(CheckCollisionPointCircle(point->get_value(), center->get_value(), radius->get_value()));
    }

    NativeErrorOr check_collision_point_triangle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(point, raylib::Vector2, 0, arguments);
        DEFINE(p1, raylib::Vector2, 1, arguments);
        DEFINE(p2, raylib::Vector2, 2, arguments);
        DEFINE(p3, raylib::Vector2, 3, arguments);

        return BOOL_(CheckCollisionPointTriangle(point->get_value(), p1->get_value(), p2->get_value(),
                                                 p3->get_value()));
    }

    NativeErrorOr check_collision_point_poly(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(point, raylib::Vector2, 0, arguments);
        DEFINE(points, raylib::Vector2, 1, arguments);
        DEFINE(pointCount, Integer, 2, arguments);

        TraceLog(LOG_WARNING, "check_collision_point_poly not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr check_collision_lines(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(startPos1, raylib::Vector2, 0, arguments);
        DEFINE(endPos1, raylib::Vector2, 1, arguments);
        DEFINE(startPos2, raylib::Vector2, 2, arguments);
        DEFINE(endPos2, raylib::Vector2, 3, arguments);
        DEFINE(collisionPoint, raylib::Vector2, 4, arguments);

        TraceLog(LOG_WARNING, "check_collision_lines not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr check_collision_point_line(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(point, raylib::Vector2, 0, arguments);
        DEFINE(p1, raylib::Vector2, 1, arguments);
        DEFINE(p2, raylib::Vector2, 2, arguments);
        DEFINE(threshold, Integer, 3, arguments);

        return BOOL_(CheckCollisionPointLine(point->get_value(), p1->get_value(), p2->get_value(),
                                             threshold->get_value()));
    }

    NativeErrorOr get_collision_rec(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(rec1, raylib::Rectangle, 0, arguments);
        DEFINE(rec2, raylib::Rectangle, 1, arguments);

        return GarbageCollector::instance().make<raylib::Rectangle>(
                GetCollisionRec(rec1->get_value(), rec2->get_value()));
    }
}