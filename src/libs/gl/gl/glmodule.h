//
// Created by Travor Oguna Oneya on 01/05/2023.
//

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../../../bond.h"


namespace bond {
    class GlWindow : public Object {
    public:
        explicit GlWindow(GLFWwindow *window) : m_window(window) {}

        ~GlWindow() override {
            glfwDestroyWindow(m_window);
            glfwTerminate();
        }

        GLFWwindow *get_window() { return m_window; }

        static const char *get_glsl_version() { return "#version 460"; }

        bool equal(const GcPtr<bond::Object> &other) override { return false; }

        size_t hash() override { return 0; }

        std::expected<GcPtr<Object>, RuntimeError> $get_attribute(const GcPtr<bond::Object> &index) override;

        NativeErrorOr swap_buffers(const std::vector<GcPtr<Object>> &arguments);

        NativeErrorOr should_close(const std::vector<GcPtr<Object>> &arguments);

        std::string str() override { return fmt::format("<Gl_Window object at {}>", (void *) this); }


    private:
        GLFWwindow *m_window;
        std::unordered_map<std::string, std::function<NativeErrorOr(const std::vector<GcPtr<Object>> &)>> m_methods = {
                {"swap_buffers", BIND(swap_buffers)},
                {"should_close", BIND(should_close)}
        };

    };

    NativeErrorOr c_GlWindow(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_gen_buffers(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_bind_buffer(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_buffer_data(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_create_shader(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_shader_source(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_compile_shader(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_create_program(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_attach_shader(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_link_program(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_use_program(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_delete_shader(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_vertex_attrib_pointer(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_enable_vertex_attrib_array(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_gen_vertex_arrays(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_bind_vertex_array(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr gl_draw_arrays(const std::vector<GcPtr<Object>> &arguments);

    const auto gl_constants = std::unordered_map<std::string, uintmax_t>{
            {"GLFW_CONTEXT_VERSION_MAJOR", 4},
            {"GLFW_CONTEXT_VERSION_MINOR", 6},
            {"ARRAY_BUFFER",         GL_ARRAY_BUFFER},
            {"COLOR_BUFFER_BIT",     GL_COLOR_BUFFER_BIT},
            {"ARRAY_BUFFER",         GL_ARRAY_BUFFER},
            {"STATIC_DRAW",          GL_STATIC_DRAW},
            {"ELEMENT_ARRAY_BUFFER", GL_ELEMENT_ARRAY_BUFFER},
            {"VERTEX_SHADER",        GL_VERTEX_SHADER},
            {"FRAGMENT_SHADER",      GL_FRAGMENT_SHADER},
            {"COMPILE_STATUS",       GL_COMPILE_STATUS},
            {"LINK_STATUS",          GL_LINK_STATUS},
            {"FALSE",                GL_FALSE},
            {"TRUE",                 GL_TRUE},
            {"FLOAT",                GL_FLOAT},
            {"TRIANGLES",            GL_TRIANGLES},
            {"UNSIGNED_INT",         GL_UNSIGNED_INT},
            {"TEXTURE_2D",           GL_TEXTURE_2D},
            {"TEXTURE0",             GL_TEXTURE0},
            {"TEXTURE1",             GL_TEXTURE1},
            {"TEXTURE2",             GL_TEXTURE2},
            {"TEXTURE3",             GL_TEXTURE3},
            {"TEXTURE4",             GL_TEXTURE4},
            {"TEXTURE5",             GL_TEXTURE5},
            {"TEXTURE6",             GL_TEXTURE6},
            {"TEXTURE7",             GL_TEXTURE7},
            {"TEXTURE8",             GL_TEXTURE8},
            {"TEXTURE9",             GL_TEXTURE9},
            {"TEXTURE10",            GL_TEXTURE10},
            {"TEXTURE11",            GL_TEXTURE11},
            {"TEXTURE12",            GL_TEXTURE12},
            {"TEXTURE13",            GL_TEXTURE13},
            {"TEXTURE14",            GL_TEXTURE14},
            {"TEXTURE15",            GL_TEXTURE15},
            {"TEXTURE16",            GL_TEXTURE16},
            {"TEXTURE17",            GL_TEXTURE17},
            {"TEXTURE18",            GL_TEXTURE18},
            {"TEXTURE19",            GL_TEXTURE19},
            {"TEXTURE20",            GL_TEXTURE20},
            {"TEXTURE21",            GL_TEXTURE21},
            {"TEXTURE22",            GL_TEXTURE22},
            {"TEXTURE23",            GL_TEXTURE23},
            {"TEXTURE24",            GL_TEXTURE24},
            {"TEXTURE25",            GL_TEXTURE25},
            {"TEXTURE26",            GL_TEXTURE26},
            {"TEXTURE27",            GL_TEXTURE27},
            {"TEXTURE28",            GL_TEXTURE28},
            {"TEXTURE29",            GL_TEXTURE29},
            {"TEXTURE30",            GL_TEXTURE30},
            {"TEXTURE31",            GL_TEXTURE31},
            {"TEXTURE_WRAP_S",       GL_TEXTURE_WRAP_S},
            {"TEXTURE_WRAP_T",       GL_TEXTURE_WRAP_T},
            {"TEXTURE_MIN_FILTER",   GL_TEXTURE_MIN_FILTER},
            {"TEXTURE_MAG_FILTER",   GL_TEXTURE_MAG_FILTER},
            {"NEAREST",              GL_NEAREST},
            {"LINEAR",               GL_LINEAR},
    };

}

