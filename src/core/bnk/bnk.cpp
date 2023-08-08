//
// Created by travor on 06/08/2023.
//


#include <expected>
#include <utility>

#include "../../bond.h"

#include "bnk.h"
#include "glad/include/glad/glad.h"

#include <GLFW/glfw3.h>
#include <fmt/core.h>

#define STBTT_malloc(x,u)    GC_MALLOC((x))
#define STBTT_free(x,u)      GC_FREE((x))

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER (512 * 1024)
#define MAX_ELEMENT_BUFFER (128 * 1024)


namespace bond::bnk {
    static void error_callback(int e, const char *d) {
        fmt::print("Error {}: {}\n", e, d);
    }

    static struct nk_glfw glfw = {0};
    static GLFWwindow *window;
    static struct nk_context *ctx;
    static struct nk_colorf bg;
    static bool is_initiated = false;

    std::expected<void, std::string> init(int width, int height, const std::string &title) {
        glfwSetErrorCallback(error_callback);
        if (!glfwInit()) {
            return std::unexpected("Failed to init GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!window) {
            return std::unexpected("Failed to create window");
        }

        glfwMakeContextCurrent(window);
        glfwGetWindowSize(window, &width, &height);

        //glad
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            return std::unexpected("Failed to init GLAD");
        }

        glViewport(0, 0, width, height);


        ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
        {
            struct nk_font_atlas *atlas;
            nk_glfw3_font_stash_begin(&glfw, &atlas);
            nk_glfw3_font_stash_end(&glfw);
        }

        is_initiated = true;
        bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
        return {};
    }

    void new_frame() {
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);
    }

    void bnk_end() {
        nk_end(ctx);
    }

    void bnk_render() {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(window);
    }

    void de_init() {
        nk_glfw3_shutdown(&glfw);
        glfwTerminate();
    }

    // API

    obj_result bnk_init(const t_vector &args) {
        Int *width;
        Int *height;
        String *title;

        TRY(parse_args(args, width, height, title));

        auto res = init(width->get_value(), height->get_value(), title->get_value());
        if (res) {
            return make_ok(C_NONE);
        } else {
            return make_error(make_string(res.error()));
        }
    }

    obj_result bnk_new_frame(const t_vector &args) {
        TRY(parse_args(args));
        new_frame();
        return C_NONE;
    }

    obj_result bnk_end_a(const t_vector &args) {
        TRY(parse_args(args));

        bnk_end();
        return C_NONE;
    }

    obj_result bnk_render_a(const t_vector &args) {
        TRY(parse_args(args));

        bnk_render();
        return C_NONE;
    }

    obj_result bnk_de_init_a(const t_vector &args) {
        TRY(parse_args(args));

        de_init();
        return C_NONE;
    }

    obj_result bnk_should_close_a(const t_vector &args) {
        TRY(parse_args(args));
        return AS_BOOL(glfwWindowShouldClose(window));
    }

//    obj_result bnk_set_should_close(const t_vector& args) {
//        Bool* should_close;
//        TRY(parse_args(args, should_close));
//
//        glfwSetWindowShouldClose(window, should_close->get_value());
//        return C_NONE;
//    }

    // "enum"
    struct Enum : public NativeInstance {
        t_map values;

        explicit Enum(t_map values) : values(std::move(values)) {}

        static obj_result get_attr(const GcPtr<Object> &self, const t_vector &args) {
            String *name;
            TRY(parse_args(args, name));

            auto Self = self->as<Enum>();
            auto Name = name->get_value();

            if (Self->values.contains(Name)) {
                return Self->values.at(Name);
            }
            return ERR(fmt::format("Enum has no value named {}", Name));
        }
    };

    auto ENUM_STRUCT = make<NativeStruct>("Enum", "Enum(enum: Enum)", c_Default<Enum>, method_map{
            {"__getattr__", {Enum::get_attr, "__getattr__(attribute: String) -> Any"}},
    });

    GcPtr<Object> make_enum(const t_map &values) {
        return ENUM_STRUCT->create_instance<Enum>(values);
    }



    //structures

#define getter(name, type, field) \
    static obj_result get_##name(const GcPtr<Object> &self) { \
        auto Self = self->as<type>(); \
        return bond_traits<decltype(Self->field.name)>::wrap(Self->field.name); \
    }

#define setter(name, type, field) \
    static obj_result set_##name(const GcPtr<Object> &self, const GcPtr<Object> &value) { \
        auto Self = self->as<type>(); \
        Self->field.name = bond_traits<decltype(Self->field.name)>::unwrap(value); \
        return value; \
    }

#define property(name, type, field) \
    getter(name, type, field)       \
    setter(name, type, field)

    struct BnkRect : public NativeInstance {
        struct nk_rect rect;

        explicit BnkRect(struct nk_rect rect) : rect(rect) {}

        property(x, BnkRect, rect)

        property(y, BnkRect, rect)

        property(w, BnkRect, rect)

        property(h, BnkRect, rect)

        static obj_result constructor(const t_vector &args) {
            Float *x;
            Float *y;
            Float *w;
            Float *h;
            TRY(parse_args(args, x, y, w, h));

            return make<BnkRect>(nk_rect(x->get_value(), y->get_value(), w->get_value(), h->get_value()));
        }

    };

    // widgets

    obj_result bnk_layout_row_dynamic(const t_vector &args) {
        Float *height;
        Int *cols;
        TRY(parse_args(args, height, cols));

        nk_layout_row_dynamic(ctx, height->get_value(), cols->get_value());
        return C_NONE;
    }

    obj_result bnk_layout_row_static(const t_vector &args) {
        Float *height;
        Int *item_width;
        Int *cols;
        TRY(parse_args(args, height, item_width, cols));

        nk_layout_row_static(ctx, height->get_value(), item_width->get_value(), cols->get_value());
        return C_NONE;
    }

    obj_result bnk_begin(const t_vector &args) {
        String *name;
        BnkRect *bounds;
        Int *flags;

        TRY(parse_args(args, name, bounds, flags));

        auto res = nk_begin(ctx, name->get_value().c_str(), bounds->rect, flags->get_value());
        return AS_BOOL(res);
    }

    obj_result bnk_button_label(const t_vector &args) {
        String *label;
        TRY(parse_args(args, label));
        auto res = nk_button_label(ctx, label->get_value().c_str());
        return AS_BOOL(res);
    }

    obj_result bnk_label(const t_vector &args) {
        String *label;
        Int *align;
        TRY(parse_args(args, label, align));
        nk_label(ctx, label->get_value().c_str(), align->get_value());
        return C_NONE;
    }

    GcPtr<Module> build_bnk_module() {
#define ENUM_VALUE(name) {make_string(#name), bond_traits<decltype(name)>::wrap(name)}
#define ENUM_INT(name) {#name, make_int(name)}
#define ENUM_SUB(name, sub) { std::string(#name).substr(sub), make_int(name) }

        t_map nk_enum = {
                // panel flags
                ENUM_SUB(NK_WINDOW_BORDER, 3),
                ENUM_SUB(NK_WINDOW_BORDER, 3),
                ENUM_SUB(NK_WINDOW_MOVABLE, 3),
                ENUM_SUB(NK_WINDOW_SCALABLE, 3),
                ENUM_SUB(NK_WINDOW_CLOSABLE, 3),
                ENUM_SUB(NK_WINDOW_MINIMIZABLE, 3),
                ENUM_SUB(NK_WINDOW_NO_SCROLLBAR, 3),
                ENUM_SUB(NK_WINDOW_TITLE, 3),
                ENUM_SUB(NK_WINDOW_SCROLL_AUTO_HIDE, 3),
                ENUM_SUB(NK_WINDOW_BACKGROUND, 3),
                ENUM_SUB(NK_WINDOW_SCALE_LEFT, 3),
                ENUM_SUB(NK_WINDOW_NO_INPUT, 3),

                //text flags
                // alignment
                ENUM_SUB(NK_TEXT_LEFT, 3),
                ENUM_SUB(NK_TEXT_CENTERED, 3),
                ENUM_SUB(NK_TEXT_RIGHT, 3),
        };

        auto mod = Mod("bnk");

        mod.function("init", bnk_init,
                     "init(width: Int, height: Int, title: String) !<None, String>\ninitializes a window");
        mod.function("new_frame", bnk_new_frame, "new_frame() None\nstarts a new frame");
        mod.function("end", bnk_end_a, "end() None\nends the frame");
        mod.function("render", bnk_render_a, "render() None\nrenders the frame");
        mod.function("de_init", bnk_de_init_a, "de_init() None\ncloses the window and de-initializes the library");
        mod.function("should_close", bnk_should_close_a,
                     "should_close() Bool\nreturns whether the window should close");
//        mod.function("set_should_close", bnk_set_should_close, "set_should_close(should_close: Bool) None");


        mod.function("layout_row_dynamic", bnk_layout_row_dynamic,
                     "layout_row_dynamic(height: Float, cols: Int) None\ncreates a dynamic row layout");
        mod.function("layout_row_static", bnk_layout_row_static, "layout_row_static(height: Float, item_width: Int, cols: Int) None\ncreates a static row layout");

        mod.function("begin", bnk_begin, "begin(name: String, bounds: rect, flags: flags) Bool\nstarts a new widget");
        mod.function("button_label", bnk_button_label,
                     "button_label(label: String) Bool\ncreates a button with a label");
        mod.function("label", bnk_label, "label(label: String, align: flags) None\ncreates a label");


        mod.struct_("rect", "rect(x: Float, y: Float, w: Float, h: Float)")
                .constructor(BnkRect::constructor)
                .field("x", BnkRect::get_x, BnkRect::set_x)
                .field("y", BnkRect::get_y, BnkRect::set_y)
                .field("w", BnkRect::get_w, BnkRect::set_w)
                .field("h", BnkRect::get_h, BnkRect::set_h);

        mod.add("flags", make_enum(nk_enum));

        return mod.build();
    }

}