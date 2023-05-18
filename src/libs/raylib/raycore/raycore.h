//
// Created by Travor Oguna Oneya on 15/05/2023.
//

#pragma once

#include "../../../bond.h"
#include "../raymodule.h"

namespace bond::raycore {
    NativeErrorOr init_window(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr window_should_close(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr close_window(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_window_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_window_fullscreen(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_window_hidden(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_window_minimized(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_window_maximized(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_window_focused(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_window_resized(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_window_state(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_window_state(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr clear_window_state(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr toggle_fullscreen(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr maximize_window(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr minimize_window(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr restore_window(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_window_icon(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_window_icons(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_window_title(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_window_position(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_window_monitor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_window_min_size(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_window_size(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_window_opacity(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_window_handle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_screen_width(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_screen_height(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_render_width(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_render_height(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_monitor_count(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_current_monitor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_monitor_position(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_monitor_width(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_monitor_height(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_monitor_physical_width(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_monitor_physical_height(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_monitor_refresh_rate(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_window_position(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_window_scale_dpi(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_monitor_name(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_clipboard_text(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_clipboard_text(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr enable_event_waiting(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr disable_event_waiting(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr swap_screen_buffer(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr poll_input_events(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr wait_time(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr show_cursor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr hide_cursor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_cursor_hidden(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr enable_cursor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr disable_cursor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_cursor_on_screen(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr clear_background(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr begin_drawing(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr end_drawing(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr begin_mode_2d(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr end_mode_2d(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr begin_mode_3d(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr end_mode_3d(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr begin_texture_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr end_texture_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr begin_shader_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr end_shader_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr begin_blend_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr end_blend_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr begin_scissor_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr end_scissor_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr begin_vr_stereo_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr end_vr_stereo_mode(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_vr_stereo_config(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_vr_stereo_config(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_shader(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_shader_from_memory(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_shader_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_shader_location(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_shader_location_attrib(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_shader_value(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_shader_value_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_shader_value_matrix(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_shader_value_texture(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_shader(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_mouse_ray(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_camera_matrix(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_camera_matrix_2d(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_world_to_screen(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_screen_to_world_2d(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_world_to_screen_ex(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_world_to_screen_2d(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_target_fps(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_fps(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_frame_time(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_time(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_random_value(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_random_seed(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr take_screenshot(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_config_flags(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr trace_log(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_trace_log_level(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr mem_alloc(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr mem_realloc(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr mem_free(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr open_url(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_trace_log_callback(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_load_file_data_callback(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_save_file_data_callback(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_load_file_text_callback(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_save_file_text_callback(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_file_data(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_file_data(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr save_file_data(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr export_data_as_code(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_file_text(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_file_text(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr save_file_text(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr file_exists(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr directory_exists(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_file_extension(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_file_length(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_file_extension(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_file_name(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_file_name_without_ext(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_directory_path(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_prev_directory_path(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_working_directory(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_application_directory(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr change_directory(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_path_file(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_directory_files(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_directory_files_ex(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_directory_files(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_file_dropped(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_dropped_files(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_dropped_files(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_file_mod_time(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr compress_data(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr decompress_data(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr encode_data_base64(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr decode_data_base64(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_key_pressed(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_key_down(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_key_released(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_key_up(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_exit_key(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_key_pressed(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_char_pressed(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_gamepad_available(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gamepad_name(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_gamepad_button_pressed(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_gamepad_button_down(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_gamepad_button_released(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_gamepad_button_up(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gamepad_button_pressed(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gamepad_axis_count(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gamepad_axis_movement(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_gamepad_mappings(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_mouse_button_pressed(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_mouse_button_down(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_mouse_button_released(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_mouse_button_up(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_mouse_x(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_mouse_y(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_mouse_position(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_mouse_delta(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_mouse_position(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_mouse_offset(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_mouse_scale(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_mouse_wheel_move(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_mouse_wheel_move_v(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_mouse_cursor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_touch_x(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_touch_y(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_touch_position(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_touch_point_id(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_touch_point_count(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_gestures_enabled(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_gesture_detected(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gesture_detected(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gesture_hold_duration(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gesture_drag_vector(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gesture_drag_angle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gesture_pinch_vector(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_gesture_pinch_angle(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr update_camera(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr update_camera_pro(const std::vector<GcPtr<Object>> &arguments);
}