#include "../../../bond.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "../gl/glmodule.h"

using namespace bond;
GarbageCollector *m_gc;
Context *m_ctx;


static void glfw_error_callback(int error, const char *description) {
    fmt::print(stderr, "Glfw Error {}: {}\n", error, description);
}

GLFWwindow *m_window;

NativeErrorOr imgui_init(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(win, GlWindow, 0, arguments);

    if (!glfwInit())
        return Err("Failed to initialize GLFW");

    glfwSetErrorCallback(glfw_error_callback);
    glfwMakeContextCurrent(win->get_window());
    gladLoadGL();

    m_window = win->get_window();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(win->get_window(), true);
    ImGui_ImplOpenGL3_Init(GlWindow::get_glsl_version());

    return Globs::BondNil;
}

NativeErrorOr show_demo_window(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);

    ImGui::ShowDemoWindow();

    return Globs::BondNil;
}

NativeErrorOr imgui_render_loop(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    ImGui::Render();
//    int display_w, display_h;
//    GLFWwindow *window = m_window;
//
//    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//    glfwGetFramebufferSize(window, &display_w, &display_h);
//    glViewport(0, 0, display_w, display_h);
//    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
//    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return Globs::BondNil;
}


NativeErrorOr imgui_begin(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(3, arguments);
    DEFINE(title, String, 0, arguments);
    DEFINE(open, Bool, 1, arguments);
    DEFINE(flags, Integer, 2, arguments);

    auto op = open->get_value();
    ImGui::Begin(title->get_value().c_str(), &op, (int) flags->get_value());

    return BOOL_(op);
}

NativeErrorOr imgui_end(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);

    ImGui::End();

    return Globs::BondNil;
}


NativeErrorOr imgui_new_frame(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    return Globs::BondNil;
}

NativeErrorOr imgui_end_frame(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);

    ImGui::EndFrame();

    return Globs::BondNil;
}

NativeErrorOr imgui_text(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(text, String, 0, arguments);

    ImGui::Text("%s", text->get_value().c_str());

    return Globs::BondNil;
}

NativeErrorOr imgui_button(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(text, String, 0, arguments);

    if (ImGui::Button(text->get_value().c_str())) {
        return Globs::BondTrue;
    }

    return Globs::BondFalse;
}


NativeErrorOr imgui_float_slider(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(4, arguments);
    DEFINE(label, String, 0, arguments);
    DEFINE(value, Float, 1, arguments);
    DEFINE(min, Float, 2, arguments);
    DEFINE(max, Float, 3, arguments);

    float v = value->get_value();
    ImGui::SliderFloat(label->get_value().c_str(), &v, min->get_value(), max->get_value());

    return m_ctx->gc()->make<Float>(v);
}

NativeErrorOr imgui_text_input(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(2, arguments);
    DEFINE(label, String, 0, arguments);
    DEFINE(value, String, 1, arguments);

    std::string text = value->get_value();
    ImGui::InputText(label->get_value().c_str(), &text);

    return m_ctx->gc()->make<String>(text);
}

NativeErrorOr imgui_begin_menu_bar(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    return BOOL_(ImGui::BeginMenuBar());
}

NativeErrorOr imgui_begin_main_menu_bar(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    return BOOL_(ImGui::BeginMainMenuBar());
}

NativeErrorOr imgui_end_main_menu_bar(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);
    ImGui::EndMainMenuBar();
    return Globs::BondNil;
}

NativeErrorOr imgui_end_menu_bar(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);

    ImGui::EndMenuBar();

    return Globs::BondNil;
}

std::expected<std::vector<float>, std::string> get_float_list(ListObj *list) {
    std::vector<float> result;
    for (auto &item: list->get_list()) {
        if (auto float_item = dynamic_cast<Float *>(item.get())) {
            result.push_back(float_item->get_value());
        } else {
            return std::unexpected("Expected float");
        }
    }

    return result;
}


NativeErrorOr imgui_progress_bar(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(3, arguments);
    DEFINE(value, Float, 0, arguments);
    DEFINE(dimentions, ListObj, 1, arguments);
    DEFINE(overlay, String, 2, arguments);

    if (dimentions->get_list().size() != 2) {
        return Err("Expected a list of 2 floats");
    }

    auto dim = get_float_list(dimentions.get());

    if (!dim) {
        return Err(dim.error());
    }

    ImGui::ProgressBar(value->get_value(), ImVec2(dim->at(0), dim->at(1)), overlay->get_value().c_str());

    return Ok(Globs::BondNil);
}

NativeErrorOr imgui_begin_menu(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(1, arguments);
    DEFINE(label, String, 0, arguments);

    if (ImGui::BeginMenu(label->get_value().c_str())) {
        return Globs::BondTrue;
    }

    return Globs::BondFalse;
}


NativeErrorOr imgui_end_menu(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);

    ImGui::EndMenu();

    return Globs::BondNil;
}

NativeErrorOr imgui_same_line(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(0, arguments);

    ImGui::SameLine();

    return Globs::BondNil;
}


NativeErrorOr imgui_menu_item(const std::vector<GcPtr<Object>> &arguments) {
    ASSERT_ARG_COUNT(2, arguments);
    DEFINE(label, String, 0, arguments);
    DEFINE(shortcut, String, 1, arguments);

    if (ImGui::MenuItem(label->get_value().c_str(), shortcut->get_value().c_str())) {
        return Globs::BondTrue;
    }

    return Globs::BondFalse;
}

EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    m_gc = ctx->gc();
    m_ctx = ctx;
    GarbageCollector::instance().set_gc(ctx->gc());

    glfwSetErrorCallback(glfw_error_callback);

    std::unordered_map<std::string, uintmax_t> WINDOW_FLAGS = {
            {"None",                      ImGuiWindowFlags_None},
            {"NoTitleBar",                ImGuiWindowFlags_NoTitleBar},
            {"NoResize",                  ImGuiWindowFlags_NoResize},
            {"NoResize",                  ImGuiWindowFlags_NoResize},
            {"NoScrollbar",               ImGuiWindowFlags_NoScrollbar},
            {"NoScrollWithMouse",         ImGuiWindowFlags_NoScrollWithMouse},
            {"NoCollapse",                ImGuiWindowFlags_NoCollapse},
            {"AlwaysAutoResize",          ImGuiWindowFlags_AlwaysAutoResize},
            {"NoBackground",              ImGuiWindowFlags_NoBackground},
            {"NoSavedSettings",           ImGuiWindowFlags_NoSavedSettings},
            {"NoMouseInputs",             ImGuiWindowFlags_NoMouseInputs},
            {"MenuBar",                   ImGuiWindowFlags_MenuBar},
            {"HorizontalScrollbar",       ImGuiWindowFlags_HorizontalScrollbar},
            {"NoFocusOnAppearing",        ImGuiWindowFlags_NoFocusOnAppearing},
            {"NoBringToFrontOnFocus",     ImGuiWindowFlags_NoBringToFrontOnFocus},
            {"AlwaysVerticalScrollbar",   ImGuiWindowFlags_AlwaysVerticalScrollbar},
            {"AlwaysHorizontalScrollbar", ImGuiWindowFlags_AlwaysHorizontalScrollbar},
            {"AlwaysUseWindowPadding",    ImGuiWindowFlags_AlwaysUseWindowPadding},
            {"NoNavInputs",               ImGuiWindowFlags_NoNavInputs},
            {"NoNavFocus",                ImGuiWindowFlags_NoNavFocus},
            {"UnsavedDocument",           ImGuiWindowFlags_UnsavedDocument},
            {"NoNav",                     ImGuiWindowFlags_NoNav},
            {"NoDecoration",              ImGuiWindowFlags_NoDecoration},
            {"NoInputs",                  ImGuiWindowFlags_NoInputs},
    };


    std::unordered_map<std::string, GcPtr<Object>> imgui_module = {
            {"WINDOW",              m_gc->make<Enum>("WINDOW", WINDOW_FLAGS)},
            {"init",                ctx->gc()->make<NativeFunction>(imgui_init)},
            {"render",              ctx->gc()->make<NativeFunction>(imgui_render_loop)},
            {"begin",               m_gc->make<NativeFunction>(imgui_begin)},
            {"end",                 m_gc->make<NativeFunction>(imgui_end)},
            {"new_frame",           m_gc->make<NativeFunction>(imgui_new_frame)},
            {"end_frame",           m_gc->make<NativeFunction>(imgui_end_frame)},
            {"show_demo_window",    m_gc->make<NativeFunction>(show_demo_window)},
            {"text",                m_gc->make<NativeFunction>(imgui_text)},
            {"button",              m_gc->make<NativeFunction>(imgui_button)},
            {"float_slider",        m_gc->make<NativeFunction>(imgui_float_slider)},
            {"text_input",          m_gc->make<NativeFunction>(imgui_text_input)},
            {"progress_bar",        m_gc->make<NativeFunction>(imgui_progress_bar)},
            {"begin_menu",          m_gc->make<NativeFunction>(imgui_begin_menu)},
            {"end_menu",            m_gc->make<NativeFunction>(imgui_end_menu)},
            {"same_line",           m_gc->make<NativeFunction>(imgui_same_line)},
            {"menu_item",           m_gc->make<NativeFunction>(imgui_menu_item)},
            {"begin_menu_bar",      m_gc->make<NativeFunction>(imgui_begin_menu_bar)},
            {"end_menu_bar",        m_gc->make<NativeFunction>(imgui_end_menu_bar)},
            {"begin_main_menu_bar", m_gc->make<NativeFunction>(imgui_begin_main_menu_bar)},
            {"end_main_menu_bar",   m_gc->make<NativeFunction>(imgui_end_main_menu_bar)},
    };

    auto module = ctx->gc()->make<Module>(path, imgui_module);
    ctx->add_module(path, module);
}


