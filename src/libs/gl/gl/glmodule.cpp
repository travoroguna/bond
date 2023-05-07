//
// Created by Travor Oguna Oneya on 01/05/2023.
//

#include "glmodule.h"


using namespace bond;
GarbageCollector *m_gc;
Context *m_ctx;


static void glfw_error_callback(int error, const char *description) {
    fmt::print(stderr, "Glfw Error {}: {}\n", error, description);
}


namespace bond {

    NativeErrorOr c_GlWindow(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);

        DEFINE(title, String, 0, arguments);
        DEFINE(width, Integer, 1, arguments);
        DEFINE(height, Integer, 2, arguments);

        if (!glfwInit())
            return Err("Failed to initialize GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        GLFWwindow *window = glfwCreateWindow(width->get_value(), height->get_value(), title->get_value().c_str(),
                                              nullptr, nullptr);
        if (window == nullptr)
            return Err("Failed to create window");

        glfwMakeContextCurrent(window);
        gladLoadGL();


        return Ok(m_gc->make<GlWindow>(window));
    }

    NativeErrorOr gl_gen_buffers(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);

        DEFINE(count, Integer, 0, arguments);

        GLuint buffer;
        glGenBuffers(count->get_value(), &buffer);

        return m_gc->make<Integer>(buffer);
    }

    NativeErrorOr gl_bind_buffer(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);

        DEFINE(target, Integer, 0, arguments);
        DEFINE(buffer, Integer, 1, arguments);

        glBindBuffer(target->get_value(), buffer->get_value());

        return Globs::BondNil;
    }

    NativeErrorOr gl_buffer_data(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(target, Integer, 0, arguments);
        DEFINE(vertices, ListObj, 1, arguments);
        DEFINE(usage, Integer, 2, arguments);

        std::vector<float> data;

        for (auto &item: vertices->get_list()) {
            if (!item->is<Float>()) return Err("Expected float");
            data.push_back(item->as<Float>()->get_value());
        }

        glBufferData(target->get_value(), data.size() * sizeof(float), data.data(), usage->get_value());
        return Ok(Globs::BondNil);
    }

    NativeErrorOr gl_clear(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(target, Integer, 0, arguments);
        glClear(target->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr gl_poll_events(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        glfwPollEvents();
        return Globs::BondNil;
    }

    NativeErrorOr gl_create_shader(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(type, Integer, 0, arguments);

        GLuint shader = glCreateShader(type->get_value());
        return m_gc->make<Integer>(shader);
    }

    NativeErrorOr gl_shader_source(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(shader, Integer, 0, arguments);
        DEFINE(source, String, 1, arguments);

        auto src = source->get_value();
        auto c_src = src.c_str();
        glShaderSource(shader->get_value(), 1, &c_src, nullptr);

        return Globs::BondNil;
    }

    NativeErrorOr gl_compile_shader(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(shader, Integer, 0, arguments);

        glCompileShader(shader->get_value());

        int success;
        char infoLog[512];

        glGetShaderiv(shader->get_value(), GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shader->get_value(), 512, nullptr, infoLog);
            return Err(infoLog);
        }

        return Ok(Globs::BondNil);
    }

    NativeErrorOr gl_create_program(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return m_gc->make<Integer>(glCreateProgram());
    }

    NativeErrorOr gl_attach_shader(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(program, Integer, 0, arguments);
        DEFINE(shader, Integer, 1, arguments);

        glAttachShader(program->get_value(), shader->get_value());

        return Globs::BondNil;
    }

    NativeErrorOr gl_link_program(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(program, Integer, 0, arguments);

        glLinkProgram(program->get_value());

        int success;
        glGetProgramiv(program->get_value(), GL_LINK_STATUS, &success);

        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program->get_value(), 512, nullptr, infoLog);
            return Err(infoLog);
        }

        return Ok(Globs::BondNil);
    }

    NativeErrorOr gl_use_program(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(program, Integer, 0, arguments);

        glUseProgram(program->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr gl_delete_shader(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(shader, Integer, 0, arguments);

        glDeleteShader(shader->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr gl_vertex_attrib_pointer(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(6, arguments);
        DEFINE(index, Integer, 0, arguments);
        DEFINE(size, Integer, 1, arguments);
        DEFINE(type, Integer, 2, arguments);
        DEFINE(normalized, Integer, 3, arguments);
        DEFINE(stride, Integer, 4, arguments);
        DEFINE(offset, Integer, 5, arguments);

        if (type->get_value() != GL_FLOAT)
            return std::unexpected(FunctionError("Only GL_FLOAT is supported", RuntimeError::InvalidArgument));

        glVertexAttribPointer(index->get_value(), size->get_value(), type->get_value(), normalized->get_value(),
                              stride->get_value() * sizeof(float), (GLvoid *) (offset->get_value() * sizeof(float)));
        return Globs::BondNil;
    }

    NativeErrorOr gl_enable_vertex_attrib_array(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(index, Integer, 0, arguments);
        glEnableVertexAttribArray(index->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr gl_gen_vertex_arrays(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(n, Integer, 0, arguments);
        GLuint vao;
        glGenVertexArrays(n->get_value(), &vao);
        return m_gc->make<Integer>(vao);
    }

    NativeErrorOr gl_bind_vertex_array(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(vao, Integer, 0, arguments);
        glBindVertexArray(vao->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr gl_draw_arrays(const std::vector<GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(mode, Integer, 0, arguments);
        DEFINE(first, Integer, 1, arguments);
        DEFINE(count, Integer, 2, arguments);

        glDrawArrays(mode->get_value(), first->get_value(), count->get_value());
        return Globs::BondNil;
    }

    std::expected<GcPtr<Object>, RuntimeError> GlWindow::$get_attribute(const GcPtr<bond::Object> &index) {
        if (!index->is<String>()) return std::unexpected(RuntimeError::AttributeNotFound);

        auto item = index->as<String>()->get_value();
        if (m_methods.contains(item)) {
            return m_gc->make<NativeFunction>(m_methods[item]);
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    NativeErrorOr GlWindow::swap_buffers(const std::vector<GcPtr<Object>> &arguments) {
        glfwSwapBuffers(m_window);
        return Globs::BondNil;
    }

    NativeErrorOr GlWindow::should_close(const std::vector<GcPtr<Object>> &arguments) {
        return BOOL_(glfwWindowShouldClose(m_window));
    }
}

EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    m_gc = ctx->gc();
    m_ctx = ctx;
    GarbageCollector::instance().set_gc(ctx->gc());

    glfwSetErrorCallback(glfw_error_callback);

    auto gl_enum = m_gc->make<Enum>("GL", bond::gl_constants);

    std::unordered_map<std::string, GcPtr<Object>> imgui_module = {
            {"init_gl",                    m_gc->make<NativeStruct<GlWindow>>("init_gl", c_GlWindow)},
            {"GL",                         gl_enum},
            {"gen_buffers",                m_gc->make<NativeFunction>(gl_gen_buffers)},
            {"bind_buffer",                m_gc->make<NativeFunction>(gl_bind_buffer)},
            {"clear",                      m_gc->make<NativeFunction>(gl_clear)},
            {"poll_events",                m_gc->make<NativeFunction>(gl_poll_events)},
            {"buffer_data",                m_gc->make<NativeFunction>(gl_buffer_data)},

            {"create_shader",              m_gc->make<NativeFunction>(gl_create_shader)},
            {"shader_source",              m_gc->make<NativeFunction>(gl_shader_source)},
            {"compile_shader",             m_gc->make<NativeFunction>(gl_compile_shader)},

            {"create_program",             m_gc->make<NativeFunction>(gl_create_program)},
            {"attach_shader",              m_gc->make<NativeFunction>(gl_attach_shader)},
            {"link_program",               m_gc->make<NativeFunction>(gl_link_program)},
            {"use_program",                m_gc->make<NativeFunction>(gl_use_program)},
            {"delete_shader",              m_gc->make<NativeFunction>(gl_delete_shader)},
            {"vertex_attrib_pointer",      m_gc->make<NativeFunction>(gl_vertex_attrib_pointer)},
            {"enable_vertex_attrib_array", m_gc->make<NativeFunction>(gl_enable_vertex_attrib_array)},
            {"gen_vertex_arrays",          m_gc->make<NativeFunction>(gl_gen_vertex_arrays)},
            {"bind_vertex_array",          m_gc->make<NativeFunction>(gl_bind_vertex_array)},
            {"draw_arrays",                m_gc->make<NativeFunction>(gl_draw_arrays)},

//            {"swap_buffers", m_gc->make<NativeFunction>(GlWindow::swap_buffers)},
//            {"should_close", m_gc->make<NativeFunction>(GlWindow::should_close)


    };

    auto module = ctx->gc()->make<Module>(path, imgui_module);
    ctx->add_module(path, module);
}


