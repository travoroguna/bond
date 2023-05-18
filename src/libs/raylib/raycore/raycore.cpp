//
// Created by Travor Oguna Oneya on 15/05/2023.
//

#include "raycore.h"
#include <raylib.h>


namespace bond::raycore {
    using namespace bond;
    using namespace bond::raylib;

    NativeErrorOr init_window(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        DEFINE(title, String, 2, arguments);

        InitWindow(width->get_value(), height->get_value(), title->get_value().c_str());

        return Globs::BondNil;
    }

    NativeErrorOr window_should_close(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(WindowShouldClose());
    }

    NativeErrorOr close_window(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        CloseWindow();
        return Globs::BondNil;
    }

    NativeErrorOr is_window_ready(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsWindowReady());
    }

    NativeErrorOr is_window_fullscreen(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsWindowFullscreen());
    }

    NativeErrorOr is_window_hidden(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsWindowHidden());
    }

    NativeErrorOr is_window_minimized(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsWindowMinimized());
    }

    NativeErrorOr is_window_maximized(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsWindowMaximized());
    }

    NativeErrorOr is_window_focused(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsWindowFocused());
    }

    NativeErrorOr is_window_resized(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsWindowResized());
    }

    NativeErrorOr is_window_state(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(flag, Integer, 0, arguments);
        return BOOL_(IsWindowState(flag->get_value()));
    }

    NativeErrorOr set_window_state(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(flags, Integer, 0, arguments);
        SetWindowState(flags->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr clear_window_state(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(flags, Integer, 0, arguments);
        ClearWindowState(flags->get_value());
        return Globs::BondNil;

    }

    NativeErrorOr toggle_fullscreen(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        ToggleFullscreen();
        return Globs::BondNil;
    }

    NativeErrorOr maximize_window(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        MaximizeWindow();
        return Globs::BondNil;
    }

    NativeErrorOr minimize_window(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        MinimizeWindow();
        return Globs::BondNil;
    }

    NativeErrorOr restore_window(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        RestoreWindow();
        return Globs::BondNil;
    }

    NativeErrorOr set_window_icon(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(image, raylib::Image, 0, arguments);
        SetWindowIcon(image->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_window_icons(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(images, ListObj, 0, arguments);

        std::vector<raylib::Image *> imgs;

        for (auto &img: images->get_list()) {
            if (!img->is<raylib::Image>()) {
                return std::unexpected(FunctionError("expected a list of images", RuntimeError::GenericError));
            }
            imgs.push_back(reinterpret_cast<raylib::Image *const>(img->as<raylib::Image>()->get()));
        }

//        SetWindowIcons(dynamic_cast<Image *>(reinterpret_cast<Image *>(imgs.data())), imgs.size());
        TraceLog(LOG_WARNING, "SetWindowIcons not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr set_window_title(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(title, String, 0, arguments);
        SetWindowTitle(title->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr set_window_position(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(x, Integer, 0, arguments);
        DEFINE(y, Integer, 1, arguments);
        SetWindowPosition(x->get_value(), y->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_window_monitor(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(monitor, Integer, 0, arguments);
        SetWindowMonitor(monitor->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_window_min_size(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        SetWindowMinSize(width->get_value(), height->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_window_size(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(width, Integer, 0, arguments);
        DEFINE(height, Integer, 1, arguments);
        SetWindowSize(width->get_value(), height->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_window_opacity(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(opacity, Float, 0, arguments);
        SetWindowOpacity(opacity->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_window_handle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<raylib::WindowHandle>(GetWindowHandle());
    }

    NativeErrorOr get_screen_width(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetScreenWidth());
    }

    NativeErrorOr get_screen_height(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetScreenHeight());
    }

    NativeErrorOr get_render_width(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetRenderWidth());
    }

    NativeErrorOr get_render_height(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetRenderHeight());
    }

    NativeErrorOr get_monitor_count(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetMonitorCount());
    }

    NativeErrorOr get_current_monitor(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetCurrentMonitor());
    }

    NativeErrorOr get_monitor_position(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(monitor, Integer, 0, arguments);
        return GarbageCollector::instance().make<raylib::Vector2>(GetMonitorPosition(monitor->get_value()));
    }

    NativeErrorOr get_monitor_width(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(monitor, Integer, 0, arguments);
        return GarbageCollector::instance().make<Integer>(GetMonitorWidth(monitor->get_value()));
    }

    NativeErrorOr get_monitor_height(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(monitor, Integer, 0, arguments);
        return GarbageCollector::instance().make<Integer>(GetMonitorHeight(monitor->get_value()));
    }

    NativeErrorOr get_monitor_physical_width(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(monitor, Integer, 0, arguments);
        return GarbageCollector::instance().make<Integer>(GetMonitorPhysicalWidth(monitor->get_value()));
    }

    NativeErrorOr get_monitor_physical_height(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(monitor, Integer, 0, arguments);
        return GarbageCollector::instance().make<Integer>(GetMonitorPhysicalHeight(monitor->get_value()));
    }

    NativeErrorOr get_monitor_refresh_rate(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(monitor, Integer, 0, arguments);
        return GarbageCollector::instance().make<Integer>(GetMonitorRefreshRate(monitor->get_value()));
    }

    NativeErrorOr get_window_position(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<raylib::Vector2>(GetWindowPosition());
    }

    NativeErrorOr get_window_scale_dpi(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<raylib::Vector2>(GetWindowScaleDPI());
    }

    NativeErrorOr get_monitor_name(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(monitor, Integer, 0, arguments);
        return GarbageCollector::instance().make<String>(GetMonitorName(monitor->get_value()));
    }

    NativeErrorOr set_clipboard_text(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, String, 0, arguments);
        SetClipboardText(text->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr get_clipboard_text(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<String>(GetClipboardText());
    }

    NativeErrorOr enable_event_waiting(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EnableEventWaiting();
        return Globs::BondNil;
    }

    NativeErrorOr disable_event_waiting(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        DisableEventWaiting();
        return Globs::BondNil;
    }

    NativeErrorOr swap_screen_buffer(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        SwapScreenBuffer();
        return Globs::BondNil;
    }

    NativeErrorOr poll_input_events(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        PollInputEvents();
        return Globs::BondNil;
    }

    NativeErrorOr wait_time(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(seconds, Float, 0, arguments);
        WaitTime(seconds->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr show_cursor(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        ShowCursor();
        return Globs::BondNil;
    }

    NativeErrorOr hide_cursor(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        HideCursor();
        return Globs::BondNil;
    }

    NativeErrorOr is_cursor_hidden(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsCursorHidden());
    }

    NativeErrorOr enable_cursor(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EnableCursor();
        return Globs::BondNil;
    }

    NativeErrorOr disable_cursor(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        DisableCursor();
        return Globs::BondNil;
    }

    NativeErrorOr is_cursor_on_screen(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsCursorOnScreen());
    }

    NativeErrorOr clear_background(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(color, raylib::Color, 0, arguments);
        ClearBackground(color->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr begin_drawing(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        BeginDrawing();
        return Globs::BondNil;
    }

    NativeErrorOr end_drawing(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EndDrawing();
        return Globs::BondNil;
    }

    NativeErrorOr begin_mode_2d(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(camera, raylib::Camera2D, 0, arguments);
        BeginMode2D(camera->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr end_mode_2d(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EndMode2D();
        return Globs::BondNil;
    }

    NativeErrorOr begin_mode_3d(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(camera, raylib::Camera3D, 0, arguments);
        BeginMode3D(camera->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr end_mode_3d(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EndMode3D();
        return Globs::BondNil;
    }

    NativeErrorOr begin_texture_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(target, raylib::RenderTexture2D, 0, arguments);
        BeginTextureMode(target->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr end_texture_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EndTextureMode();
        return Globs::BondNil;
    }

    NativeErrorOr begin_shader_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(shader, raylib::Shader, 0, arguments);
        BeginShaderMode(shader->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr end_shader_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EndShaderMode();
        return Globs::BondNil;
    }

    NativeErrorOr begin_blend_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(mode, Integer, 0, arguments);
        BeginBlendMode(mode->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr end_blend_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EndBlendMode();
        return Globs::BondNil;
    }

    NativeErrorOr begin_scissor_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(x, Integer, 0, arguments);
        DEFINE(y, Integer, 1, arguments);
        DEFINE(width, Integer, 2, arguments);
        DEFINE(height, Integer, 3, arguments);
        BeginScissorMode(x->get_value(), y->get_value(), width->get_value(), height->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr end_scissor_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EndScissorMode();
        return Globs::BondNil;
    }

    NativeErrorOr begin_vr_stereo_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(config, raylib::VrStereoConfig, 0, arguments);
        BeginVrStereoMode(config->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr end_vr_stereo_mode(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        EndVrStereoMode();
        return Globs::BondNil;
    }

    NativeErrorOr load_vr_stereo_config(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(device, raylib::VrDeviceInfo, 0, arguments);
        LoadVrStereoConfig(device->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr unload_vr_stereo_config(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(config, raylib::VrStereoConfig, 0, arguments);
        UnloadVrStereoConfig(config->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr load_shader(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(vsFileName, String, 0, arguments);
        DEFINE(fsFileName, String, 1, arguments);
        LoadShader(vsFileName->get_value().c_str(), fsFileName->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr load_shader_from_memory(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(vsCode, String, 0, arguments);
        DEFINE(fsCode, String, 1, arguments);
        LoadShaderFromMemory(vsCode->get_value().c_str(), fsCode->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr is_shader_ready(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(shader, raylib::Shader, 0, arguments);
        IsShaderReady(shader->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_shader_location(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(shader, raylib::Shader, 0, arguments);
        DEFINE(uniformName, String, 1, arguments);
        GetShaderLocation(shader->get_value(), uniformName->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr get_shader_location_attrib(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(shader, raylib::Shader, 0, arguments);
        DEFINE(attribName, String, 1, arguments);
        GetShaderLocationAttrib(shader->get_value(), attribName->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr set_shader_value(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(shader, raylib::Shader, 0, arguments);
        DEFINE(locIndex, Integer, 1, arguments);
        DEFINE(value, Integer, 2, arguments);
        DEFINE(uniformType, Integer, 3, arguments);
//        SetShaderValue(shader->get_value(), locIndex->get_value(), value->get_value(), uniformType->get_value());

        TraceLog(LOG_WARNING, "set_shader_value not implemented");
        return Globs::BondNil;
    }


    std::unexpected<FunctionError> make_error(const std::string &str) {
        return std::unexpected(FunctionError(str, RuntimeError::GenericError));
    }

    NativeErrorOr set_shader_value_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(5, arguments);
        DEFINE(shader, raylib::Shader, 0, arguments);
        DEFINE(locIndex, Integer, 1, arguments);
//        DEFINE(value, void, 2, arguments);
        DEFINE(uniformType, Integer, 3, arguments);
        DEFINE(count, Integer, 4, arguments);

        switch (uniformType->get_value()) {
            case SHADER_UNIFORM_FLOAT: {
                if (!arguments[2]->is<Float>()) {
                    return make_error("Expected Float as argument 3");
                }
                auto f = arguments[2]->as<Float>()->get_value();
                SetShaderValueV(shader->get_value(), locIndex->get_value(), &f, uniformType->get_value(),
                                count->get_value());
                break;
            }
            case SHADER_UNIFORM_VEC2: {
                if (!arguments[2]->is<raylib::Vector2>()) {
                    return make_error("Expected Vector2 as argument 3");
                }
                auto v = arguments[2]->as<raylib::Vector2>()->get_value();
                SetShaderValueV(shader->get_value(), locIndex->get_value(), &v, uniformType->get_value(),
                                count->get_value());
                break;
            }

            case SHADER_UNIFORM_VEC3: {
                if (!arguments[2]->is<raylib::Vector3>()) {
                    return make_error("Expected Vector3 as argument 3");
                }
                auto v = arguments[2]->as<raylib::Vector3>()->get_value();
                SetShaderValueV(shader->get_value(), locIndex->get_value(), &v, uniformType->get_value(),
                                count->get_value());
                break;
            }

            case SHADER_UNIFORM_VEC4: {
                if (!arguments[2]->is<raylib::Vector4>()) {
                    return make_error("Expected Vector4 as argument 3");
                }
                auto v = arguments[2]->as<raylib::Vector4>()->get_value();
                SetShaderValueV(shader->get_value(), locIndex->get_value(), &v, uniformType->get_value(),
                                count->get_value());
                break;
            }

            case SHADER_UNIFORM_INT: {
                if (!arguments[2]->is<Integer>()) {
                    return make_error("Expected Integer as argument 3");
                }
                auto i = arguments[2]->as<Integer>()->get_value();
                SetShaderValueV(shader->get_value(), locIndex->get_value(), &i, uniformType->get_value(),
                                count->get_value());
                break;
            }

            case SHADER_UNIFORM_IVEC2: {
//                if (!arguments[2]->is<raylib::IVector2>()) {
//                    return make_error("Expected Vector2 as argument 3");
//                }
//                auto v = arguments[2]->as<raylib::IVector2>()->get_value();
//                SetShaderValueV(shader->get_value(), locIndex->get_value(), &v, uniformType->get_value(), count->get_value());
//                break;
            }

            case SHADER_UNIFORM_IVEC3: {
//                if (!arguments[2]->is<raylib::IVector3>()) {
//                    return make_error("Expected Vector3 as argument 3");
//                }
//                auto v = arguments[2]->as<raylib::IVector3>()->get_value();
//                SetShaderValueV(shader->get_value(), locIndex->get_value(), &v, uniformType->get_value(), count->get_value());
//                break;
            }

            case SHADER_UNIFORM_IVEC4: {
//                if (!arguments[2]->is<raylib::IVector4>()) {
//                    return make_error("Expected Vector4 as argument 3");
//                }
//                auto v = arguments[2]->as<raylib::IVector4>()->get_value();
//                SetShaderValueV(shader->get_value(), locIndex->get_value(), &v, uniformType->get_value(), count->get_value());
//                break;
            }

            case SHADER_UNIFORM_SAMPLER2D: {
//                if (!arguments[2]->is<raylib::Texture2D>()) {
//                    return make_error("Expected Texture2D as argument 3");
//                }
//                auto v = arguments[2]->as<raylib::Texture2D>()->get_value();
//                SetShaderValueV(shader->get_value(), locIndex->get_value(), &v, uniformType->get_value(), count->get_value());
//                break;
                TraceLog(LOG_WARNING, "Type not supported yet");
            }

            default:
                return make_error("Invalid uniform type as argument 4");

        }


        return Globs::BondNil;
    }

    NativeErrorOr set_shader_value_matrix(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(shader, raylib::Shader, 0, arguments);
        DEFINE(locIndex, Integer, 1, arguments);
        DEFINE(mat, raylib::Matrix, 2, arguments);

        SetShaderValueMatrix(shader->get_value(), locIndex->get_value(), mat->get_value());

        return Globs::BondNil;
    }

    NativeErrorOr set_shader_value_texture(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(shader, raylib::Shader, 0, arguments);
        DEFINE(locIndex, Integer, 1, arguments);
        DEFINE(texture, raylib::Texture2D, 2, arguments);
        SetShaderValueTexture(shader->get_value(), locIndex->get_value(), texture->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr unload_shader(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(shader, raylib::Shader, 0, arguments);
        UnloadShader(shader->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_mouse_ray(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(mousePosition, raylib::Vector2, 0, arguments);
        DEFINE(camera, raylib::Camera, 1, arguments);
        auto ray = GetMouseRay(mousePosition->get_value(), camera->get_value());
        return GarbageCollector::instance().make<raylib::Ray>(ray);
    }

    NativeErrorOr get_camera_matrix(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(camera, raylib::Camera, 0, arguments);
        auto mat = GetCameraMatrix(camera->get_value());
        return GarbageCollector::instance().make<raylib::Matrix>(mat);
    }

    NativeErrorOr get_camera_matrix_2d(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(camera, raylib::Camera2D, 0, arguments);
        auto mat = GetCameraMatrix2D(camera->get_value());
        return GarbageCollector::instance().make<raylib::Matrix>(mat);
    }

    NativeErrorOr get_world_to_screen(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(position, raylib::Vector3, 0, arguments);
        DEFINE(camera, raylib::Camera, 1, arguments);

        auto vec = GetWorldToScreen(position->get_value(), camera->get_value());
        return GarbageCollector::instance().make<raylib::Vector2>(vec);
    }

    NativeErrorOr get_screen_to_world_2d(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(position, raylib::Vector2, 0, arguments);
        DEFINE(camera, raylib::Camera2D, 1, arguments);
        auto vec = GetScreenToWorld2D(position->get_value(), camera->get_value());
        return GarbageCollector::instance().make<raylib::Vector2>(vec);
    }

    NativeErrorOr get_world_to_screen_ex(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(position, raylib::Vector3, 0, arguments);
        DEFINE(camera, raylib::Camera, 1, arguments);
        DEFINE(width, Integer, 2, arguments);
        DEFINE(height, Integer, 3, arguments);

        auto vec = GetWorldToScreenEx(position->get_value(), camera->get_value(), width->get_value(),
                                      height->get_value());
        return GarbageCollector::instance().make<raylib::Vector2>(vec);
    }

    NativeErrorOr get_world_to_screen_2d(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(position, raylib::Vector2, 0, arguments);
        DEFINE(camera, raylib::Camera2D, 1, arguments);

        auto vec = GetWorldToScreen2D(position->get_value(), camera->get_value());
        return GarbageCollector::instance().make<raylib::Vector2>(vec);
    }

    NativeErrorOr set_target_fps(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fps, Integer, 0, arguments);
        SetTargetFPS(fps->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_fps(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetFPS());
    }

    NativeErrorOr get_frame_time(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        auto ftime = GetFrameTime();
        return GarbageCollector::instance().make<Float>(ftime);
    }

    NativeErrorOr get_time(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Float>(GetTime());
    }

    NativeErrorOr get_random_value(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(min, Integer, 0, arguments);
        DEFINE(max, Integer, 1, arguments);
        return GarbageCollector::instance().make<Integer>(GetRandomValue(min->get_value(), max->get_value()));
    }

    NativeErrorOr set_random_seed(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(seed, Integer, 0, arguments);
        SetRandomSeed(seed->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr take_screenshot(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);
        TakeScreenshot(fileName->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr set_config_flags(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(flags, Integer, 0, arguments);
        SetConfigFlags(flags->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr trace_log(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(logLevel, Integer, 0, arguments);
        DEFINE(text, String, 1, arguments);
        TraceLog(logLevel->get_value(), text->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr set_trace_log_level(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(logLevel, Integer, 0, arguments);
        SetTraceLogLevel(logLevel->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr mem_alloc(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(size, Integer, 0, arguments);
        fmt::print("what are you doing?\n");
        return Globs::BondNil;
    }

    NativeErrorOr mem_realloc(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(ptr, Integer, 0, arguments);
        DEFINE(size, Integer, 1, arguments);
        fmt::print("what are you doing? The gc will not like this\n");
        return Globs::BondNil;
    }

    NativeErrorOr mem_free(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(ptr, Integer, 0, arguments);
        fmt::print("what are you doing? The gc will not like this\n");
        return Globs::BondNil;
    }

    NativeErrorOr open_url(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(url, String, 0, arguments);
        OpenURL(url->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr set_trace_log_callback(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(callback, Integer, 0, arguments);
        TraceLog(LOG_ERROR, "set_trace_log_callback not implemented, ignoring\n");
        return Globs::BondNil;
    }

    NativeErrorOr set_load_file_data_callback(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(callback, Integer, 0, arguments);
        TraceLog(LOG_ERROR, "set_load_file_data_callback not implemented, ignoring\n");
        return Globs::BondNil;
    }

    NativeErrorOr set_save_file_data_callback(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(callback, Integer, 0, arguments);
        TraceLog(LOG_ERROR, "set_save_file_data_callback not implemented, ignoring\n");
        return Globs::BondNil;
    }

    NativeErrorOr set_load_file_text_callback(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(callback, Integer, 0, arguments);
        TraceLog(LOG_ERROR, "set_load_file_text_callback not implemented, ignoring\n");
        return Globs::BondNil;
    }

    NativeErrorOr set_save_file_text_callback(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(callback, Integer, 0, arguments);
        TraceLog(LOG_ERROR, "set_save_file_text_callback not implemented, ignoring\n");
        return Globs::BondNil;
    }

    NativeErrorOr load_file_data(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);

        unsigned int bytesRead;
        auto data = LoadFileData(fileName->get_value().c_str(), &bytesRead);
        auto str = GarbageCollector::instance().make<Integer>(reinterpret_cast<uintptr_t>(data));
        UnloadFileData(data);
        return str;
    }

    NativeErrorOr unload_file_data(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(data, Integer, 0, arguments);
        TraceLog(LOG_ERROR,
                 "unload_file_data not implemented as memory returned by load_file_data will automatically be freed\n");
        return Globs::BondNil;
    }

    NativeErrorOr save_file_data(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(fileName, String, 0, arguments);
        DEFINE(data, String, 1, arguments);

        auto value = data->get_value();

        return BOOL_(SaveFileData(fileName->get_value().c_str(), (void *) value.c_str(), value.size()));
    }

    NativeErrorOr export_data_as_code(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(data, String, 0, arguments);
        DEFINE(fileName, String, 2, arguments);
        auto value = data->get_value();
        return BOOL_(ExportDataAsCode(reinterpret_cast<const unsigned char *>(value.c_str()), value.size(),
                                      fileName->get_value().c_str()));
    }

    NativeErrorOr load_file_text(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);
        auto txt = LoadFileText(fileName->get_value().c_str());
        auto t = GarbageCollector::instance().make<String>(txt);
        UnloadFileText(txt);
        return t;
    }

    NativeErrorOr unload_file_text(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(text, Integer, 0, arguments);
        TraceLog(LOG_ERROR,
                 "unload_file_text not implemented as memory returned by load_file_text will automatically be freed\n");
        return Globs::BondNil;
    }

    NativeErrorOr save_file_text(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(fileName, String, 0, arguments);
        DEFINE(text, String, 1, arguments);
        auto value = text->get_value();
        return BOOL_(SaveFileText(fileName->get_value().c_str(), const_cast<char *>(value.c_str())));
    }

    NativeErrorOr file_exists(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);
        return BOOL_(FileExists(fileName->get_value().c_str()));
    }

    NativeErrorOr directory_exists(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(dirPath, String, 0, arguments);
        return BOOL_(DirectoryExists(dirPath->get_value().c_str()));
    }

    NativeErrorOr is_file_extension(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(fileName, String, 0, arguments);
        DEFINE(ext, String, 1, arguments);
        return BOOL_(IsFileExtension(fileName->get_value().c_str(), ext->get_value().c_str()));
    }

    NativeErrorOr get_file_length(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);
        return GarbageCollector::instance().make<Integer>(GetFileLength(fileName->get_value().c_str()));
    }

    NativeErrorOr get_file_extension(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);
        return GarbageCollector::instance().make<String>(GetFileExtension(fileName->get_value().c_str()));
    }

    NativeErrorOr get_file_name(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(filePath, String, 0, arguments);
        return GarbageCollector::instance().make<String>(GetFileName(filePath->get_value().c_str()));
    }

    NativeErrorOr get_file_name_without_ext(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(filePath, String, 0, arguments);
        return GarbageCollector::instance().make<String>(GetFileNameWithoutExt(filePath->get_value().c_str()));
    }

    NativeErrorOr get_directory_path(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(filePath, String, 0, arguments);
        return GarbageCollector::instance().make<String>(GetDirectoryPath(filePath->get_value().c_str()));
    }

    NativeErrorOr get_prev_directory_path(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(dirPath, String, 0, arguments);
        return GarbageCollector::instance().make<String>(GetPrevDirectoryPath(dirPath->get_value().c_str()));
    }

    NativeErrorOr get_working_directory(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<String>(GetWorkingDirectory());
    }

    NativeErrorOr get_application_directory(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<String>(GetApplicationDirectory());
    }

    NativeErrorOr change_directory(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(dir, String, 0, arguments);
        return BOOL_(ChangeDirectory(dir->get_value().c_str()));
    }

    NativeErrorOr is_path_file(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(path, String, 0, arguments);
        return BOOL_(IsPathFile(path->get_value().c_str()));
    }

    NativeErrorOr load_directory_files(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(dirPath, String, 0, arguments);
        TraceLog(LOG_ERROR, "load_directory_files not implemented\n");
        return Globs::BondNil;
    }

    NativeErrorOr load_directory_files_ex(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(basePath, String, 0, arguments);
        DEFINE(filter, String, 1, arguments);
        DEFINE(scanSubdirs, Bool, 2, arguments);
        TraceLog(LOG_ERROR, "load_directory_files_ex not implemented\n");
        return Globs::BondNil;
    }

    NativeErrorOr unload_directory_files(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(files, Integer, 0, arguments);
        TraceLog(LOG_ERROR, "unload_directory_files not implemented\n");
        return Globs::BondNil;
    }

    NativeErrorOr is_file_dropped(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsFileDropped());
    }

    NativeErrorOr load_dropped_files(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        TraceLog(LOG_ERROR, "load_dropped_files not implemented\n");
        return Globs::BondNil;
    }

    NativeErrorOr unload_dropped_files(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(files, Integer, 0, arguments);
        TraceLog(LOG_ERROR, "unload_dropped_files not implemented\n");
        return Globs::BondNil;
    }

    NativeErrorOr get_file_mod_time(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);
        return GarbageCollector::instance().make<Integer>(GetFileModTime(fileName->get_value().c_str()));
    }

    NativeErrorOr compress_data(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(data, String, 0, arguments);
        DEFINE(dataSize, Integer, 1, arguments);
        DEFINE(compDataSize, Integer, 2, arguments);

        TraceLog(LOG_ERROR, "compress_data not implemented\n");
        return Globs::BondNil;
    }

    NativeErrorOr decompress_data(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(compData, String, 0, arguments);
        DEFINE(compDataSize, Integer, 1, arguments);
        DEFINE(dataSize, Integer, 2, arguments);

        TraceLog(LOG_ERROR, "decompress_data not implemented\n");
        return Globs::BondNil;
    }

    NativeErrorOr encode_data_base64(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(data, String, 0, arguments);
        DEFINE(dataSize, Integer, 1, arguments);
        DEFINE(outputSize, Integer, 2, arguments);

        TraceLog(LOG_ERROR, "encode_data_base64 not implemented\n");
        return Globs::BondNil;
    }

    NativeErrorOr decode_data_base64(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(data, String, 0, arguments);
        DEFINE(outputSize, Integer, 1, arguments);

        TraceLog(LOG_ERROR, "decode_data_base64 not implemented\n");
        return Globs::BondNil;
    }

    NativeErrorOr is_key_pressed(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(key, Integer, 0, arguments);
        return BOOL_(IsKeyPressed(key->get_value()));
    }

    NativeErrorOr is_key_down(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(key, Integer, 0, arguments);
        return BOOL_(IsKeyDown(key->get_value()));
    }

    NativeErrorOr is_key_released(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(key, Integer, 0, arguments);
        return BOOL_(IsKeyReleased(key->get_value()));
    }

    NativeErrorOr is_key_up(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(key, Integer, 0, arguments);
        return BOOL_(IsKeyUp(key->get_value()));
    }

    NativeErrorOr set_exit_key(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(key, Integer, 0, arguments);
        SetExitKey(key->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_key_pressed(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetKeyPressed());
    }

    NativeErrorOr get_char_pressed(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<String>(std::string() + (char) GetCharPressed());
    }

    NativeErrorOr is_gamepad_available(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(gamepad, Integer, 0, arguments);
        return BOOL_(IsGamepadAvailable(gamepad->get_value()));
    }

    NativeErrorOr get_gamepad_name(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(gamepad, Integer, 0, arguments);
        return GarbageCollector::instance().make<String>(GetGamepadName(gamepad->get_value()));
    }

    NativeErrorOr is_gamepad_button_pressed(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(gamepad, Integer, 0, arguments);
        DEFINE(button, Integer, 1, arguments);

        return BOOL_(IsGamepadButtonPressed(gamepad->get_value(), button->get_value()));
    }

    NativeErrorOr is_gamepad_button_down(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(gamepad, Integer, 0, arguments);
        DEFINE(button, Integer, 1, arguments);

        return BOOL_(IsGamepadButtonDown(gamepad->get_value(), button->get_value()));
    }

    NativeErrorOr is_gamepad_button_released(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(gamepad, Integer, 0, arguments);
        DEFINE(button, Integer, 1, arguments);

        return BOOL_(IsGamepadButtonReleased(gamepad->get_value(), button->get_value()));
    }

    NativeErrorOr is_gamepad_button_up(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(gamepad, Integer, 0, arguments);
        DEFINE(button, Integer, 1, arguments);

        return BOOL_(IsGamepadButtonUp(gamepad->get_value(), button->get_value()));
    }

    NativeErrorOr get_gamepad_button_pressed(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetGamepadButtonPressed());
    }

    NativeErrorOr get_gamepad_axis_count(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(gamepad, Integer, 0, arguments);
        return GarbageCollector::instance().make<Integer>(GetGamepadAxisCount(gamepad->get_value()));
    }

    NativeErrorOr get_gamepad_axis_movement(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(gamepad, Integer, 0, arguments);
        DEFINE(axis, Integer, 1, arguments);
        return GarbageCollector::instance().make<Float>(
                GetGamepadAxisMovement(gamepad->get_value(), axis->get_value()));
    }

    NativeErrorOr set_gamepad_mappings(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(mappings, String, 0, arguments);
        SetGamepadMappings(mappings->get_value().c_str());
        return Globs::BondNil;
    }

    NativeErrorOr is_mouse_button_pressed(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(button, Integer, 0, arguments);
        return BOOL_(IsMouseButtonPressed(button->get_value()));
    }

    NativeErrorOr is_mouse_button_down(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(button, Integer, 0, arguments);

        return BOOL_(IsMouseButtonDown(button->get_value()));
    }

    NativeErrorOr is_mouse_button_released(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(button, Integer, 0, arguments);
        return BOOL_(IsMouseButtonReleased(button->get_value()));
    }

    NativeErrorOr is_mouse_button_up(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(button, Integer, 0, arguments);

        return BOOL_(IsMouseButtonUp(button->get_value()));
    }

    NativeErrorOr get_mouse_x(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetMouseX());
    }

    NativeErrorOr get_mouse_y(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetMouseY());
    }

    NativeErrorOr get_mouse_position(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<raylib::Vector2>(GetMousePosition());
    }

    NativeErrorOr get_mouse_delta(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<raylib::Vector2>(GetMouseDelta());
    }

    NativeErrorOr set_mouse_position(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(x, Integer, 0, arguments);
        DEFINE(y, Integer, 1, arguments);
        SetMousePosition(x->get_value(), y->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_mouse_offset(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(offsetX, Integer, 0, arguments);
        DEFINE(offsetY, Integer, 1, arguments);
        SetMouseOffset(offsetX->get_value(), offsetY->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_mouse_scale(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(scaleX, Float, 0, arguments);
        DEFINE(scaleY, Float, 1, arguments);
        SetMouseScale(scaleX->get_value(), scaleY->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_mouse_wheel_move(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetMouseWheelMove());
    }

    NativeErrorOr get_mouse_wheel_move_v(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<raylib::Vector2>(GetMouseWheelMoveV());
    }

    NativeErrorOr set_mouse_cursor(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(cursor, Integer, 0, arguments);
        SetMouseCursor(cursor->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_touch_x(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetTouchX());
    }

    NativeErrorOr get_touch_y(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetTouchY());
    }

    NativeErrorOr get_touch_position(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(index, Integer, 0, arguments);

        return Globs::BondNil;
    }

    NativeErrorOr get_touch_point_id(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(index, Integer, 0, arguments);
        return GarbageCollector::instance().make<Integer>(GetTouchPointId(index->get_value()));
    }

    NativeErrorOr get_touch_point_count(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetTouchPointCount());
    }

    NativeErrorOr set_gestures_enabled(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(flags, Integer, 0, arguments);
        SetGesturesEnabled(flags->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr is_gesture_detected(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(gesture, Integer, 0, arguments);

        return BOOL_(IsGestureDetected(gesture->get_value()));
    }

    NativeErrorOr get_gesture_detected(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Integer>(GetGestureDetected());
    }

    NativeErrorOr get_gesture_hold_duration(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Float>(GetGestureHoldDuration());
    }

    NativeErrorOr get_gesture_drag_vector(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<raylib::Vector2>(GetGestureDragVector());
    }

    NativeErrorOr get_gesture_drag_angle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Float>(GetGestureDragAngle());
    }

    NativeErrorOr get_gesture_pinch_vector(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<raylib::Vector2>(GetGesturePinchVector());
    }

    NativeErrorOr get_gesture_pinch_angle(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return GarbageCollector::instance().make<Float>(GetGesturePinchAngle());
    }

    NativeErrorOr update_camera(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(camera, raylib::Camera, 0, arguments);
        DEFINE(mode, Integer, 1, arguments);

        UpdateCamera(camera->get(), mode->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr update_camera_pro(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(camera, raylib::Camera, 0, arguments);
        DEFINE(movement, raylib::Vector3, 1, arguments);
        DEFINE(rotation, raylib::Vector3, 2, arguments);
        DEFINE(zoom, Float, 3, arguments);

        UpdateCameraPro(camera->get(), movement->get_value(), rotation->get_value(), zoom->get_value());
        return Globs::BondNil;
    }

}