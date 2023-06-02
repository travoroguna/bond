//
// Created by Travor Oguna Oneya on 15/05/2023.
//

#pragma once

#include "../../bond.h"
#include <raylib.h>


namespace bond::raylib {

    class Image : public Object {
    public:
        explicit Image(::Image image) : m_image(image) {}

        ::Image get_value() { return m_image; }

        ::Image *get() { return &m_image; }

        std::string str() override { return ::fmt::format("Image({})", (void *) &m_image); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }


    private:
        ::Image m_image;
    };

//    NativeErrorOr c_Image(const std::vector<GcPtr<Object>> &arguments);

    class WindowHandle : public Object {
    public:
        explicit WindowHandle(void *window) : m_window(window) {}

        void *get_value() { return m_window; }

        std::string str() override { return ::fmt::format("WindowHandle({})", m_window); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        void *m_window;
    };

//    NativeErrorOr c_WindowHandle(const std::vector<GcPtr<Object>> &arguments);


    class Vector2 : public Object {
    public:
        explicit Vector2(::Vector2 vector) : m_vector(vector) {}

        ::Vector2 get_value() { return m_vector; }

        ::Vector2 *get() { return &m_vector; }

        std::string str() override { return ::fmt::format("Vector2({})", (void *) &m_vector); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

        std::expected<GcPtr<Object>, RuntimeError> $get_attribute(const GcPtr<bond::Object> &index) override;

        std::expected<GcPtr<Object>, RuntimeError>
        $set_attribute(const GcPtr<bond::Object> &index, const GcPtr<bond::Object> &value) override;

    private:
        ::Vector2 m_vector;
    };

    NativeErrorOr c_Vector2(const std::vector<GcPtr<Object>> &arguments);


    class Vector3 : public Object {
    public:
        explicit Vector3(::Vector3 vector) : m_vector(vector) {}

        ::Vector3 get_value() { return m_vector; }

        std::string str() override { return ::fmt::format("Vector3({})", (void *) &m_vector); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

        std::expected<GcPtr<Object>, RuntimeError> $get_attribute(const GcPtr<bond::Object> &index) override;

        std::expected<GcPtr<Object>, RuntimeError>
        $set_attribute(const GcPtr<bond::Object> &index, const GcPtr<bond::Object> &value) override;

    private:
        ::Vector3 m_vector;
    };

    NativeErrorOr c_Vector3(const std::vector<GcPtr<Object>> &arguments);

    class Color : public Object {
    public:
        explicit Color(::Color color) : m_color(color) {}

        ::Color get_value() { return m_color; }

        std::string str() override { return ::fmt::format("Color({})", (void *) &m_color); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

        std::expected<GcPtr<Object>, RuntimeError> $get_attribute(const GcPtr<bond::Object> &index) override;

        std::expected<GcPtr<Object>, RuntimeError>
        $set_attribute(const GcPtr<bond::Object> &index, const GcPtr<bond::Object> &value) override;

    private:
        ::Color m_color;
    };

    NativeErrorOr c_Color(const std::vector<GcPtr<Object>> &arguments);


    class Camera2D : public BondObject {
    public:
        explicit Camera2D(::Camera2D camera) : m_camera(camera) {
            m_attributes["target"] = GarbageCollector::instance().make<Vector2>(camera.target);
            m_attributes["offset"] = GarbageCollector::instance().make<Vector2>(camera.offset);
            m_attributes["rotation"] = GarbageCollector::instance().make<Float>(camera.rotation);
            m_attributes["zoom"] = GarbageCollector::instance().make<Float>(camera.zoom);
        }

        Camera2D(const GcPtr<Vector2> &target, const GcPtr<Vector2> &offset, const GcPtr<Float> &rotation,
                 const GcPtr<Float> &zoom) : m_camera({0}) {
            m_attributes["target"] = target;
            m_attributes["offset"] = offset;
            m_attributes["rotation"] = rotation;
            m_attributes["zoom"] = zoom;
        }

        ::Camera2D get_value() {
            m_camera = {
                    m_attributes["target"]->as<Vector2>()->get_value(),
                    m_attributes["offset"]->as<Vector2>()->get_value(),
                    m_attributes["rotation"]->as<Float>()->get_value(),
                    m_attributes["zoom"]->as<Float>()->get_value()
            };
            return m_camera;
        }

        std::string str() override { return ::fmt::format("Camera2D({})", (void *) &m_camera); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Camera2D m_camera;
    };

    NativeErrorOr c_Camera2D(const std::vector<GcPtr<Object>> &arguments);


    class Camera3D : public Object {
    public:
        explicit Camera3D(::Camera3D camera) : m_camera(camera) {}

        ::Camera3D get_value() { return m_camera; }

        std::string str() override { return ::fmt::format("Camera3D({})", (void *) &m_camera); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Camera3D m_camera;
    };


    class Matrix : public Object {
    public:
        explicit Matrix(::Matrix matrix) : m_matrix(matrix) {}

        ::Matrix get_value() { return m_matrix; }

        std::string str() override { return ::fmt::format("Matrix({})", (void *) &m_matrix); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Matrix m_matrix;
    };

    NativeErrorOr c_Matrix(const std::vector<GcPtr<Object>> &arguments);


    class RenderTexture2D : public Object {
    public:
        explicit RenderTexture2D(::RenderTexture2D renderTexture) : m_renderTexture(renderTexture) {}

        ::RenderTexture2D get_value() { return m_renderTexture; }

        std::string str() override { return ::fmt::format("RenderTexture2D({})", (void *) &m_renderTexture); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::RenderTexture2D m_renderTexture;
    };

    NativeErrorOr c_RenderTexture2D(const std::vector<GcPtr<Object>> &arguments);


    class Shader : public Object {
    public:
        explicit Shader(::Shader shader) : m_shader(shader) {}

        ::Shader get_value() { return m_shader; }

        std::string str() override { return ::fmt::format("Shader({})", (void *) &m_shader); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Shader m_shader;
    };

    NativeErrorOr c_Shader(const std::vector<GcPtr<Object>> &arguments);


    class VrStereoConfig : public Object {
    public:
        explicit VrStereoConfig(::VrStereoConfig vrStereoConfig) : m_vrStereoConfig(vrStereoConfig) {}

        ::VrStereoConfig get_value() { return m_vrStereoConfig; }

        std::string str() override { return ::fmt::format("VrStereoConfig({})", (void *) &m_vrStereoConfig); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::VrStereoConfig m_vrStereoConfig;
    };

    NativeErrorOr c_VrStereoConfig(const std::vector<GcPtr<Object>> &arguments);


    class VrDeviceInfo : public Object {
    public:
        explicit VrDeviceInfo(::VrDeviceInfo vrDeviceInfo) : m_vrDeviceInfo(vrDeviceInfo) {}

        ::VrDeviceInfo get_value() { return m_vrDeviceInfo; }

        std::string str() override { return ::fmt::format("VrDeviceInfo({})", (void *) &m_vrDeviceInfo); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::VrDeviceInfo m_vrDeviceInfo;
    };

    NativeErrorOr c_VrDeviceInfo(const std::vector<GcPtr<Object>> &arguments);

    class Camera : public Object {
    public:
        explicit Camera(::Camera camera) : m_camera(camera) {}

        ::Camera get_value() { return m_camera; }

        ::Camera *get() { return &m_camera; }

        std::string str() override { return ::fmt::format("Camera({})", (void *) &m_camera); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Camera m_camera;
    };

    NativeErrorOr c_Camera(const std::vector<GcPtr<Object>> &arguments);


    class Ray : public Object {
    public:
        explicit Ray(::Ray ray) : m_ray(ray) {}

        ::Ray get_value() { return m_ray; }

        ::Ray *get() { return &m_ray; }

        std::string str() override { return ::fmt::format("Ray({})", (void *) &m_ray); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Ray m_ray;
    };

    NativeErrorOr c_Ray(const std::vector<GcPtr<Object>> &arguments);

    class Vector4 : public Object {
    public:
        explicit Vector4(::Vector4 vector4) : m_vector4(vector4) {}

        ::Vector4 get_value() { return m_vector4; }

        ::Vector4 *get() { return &m_vector4; }

        std::string str() override { return ::fmt::format("Vector4({})", (void *) &m_vector4); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Vector4 m_vector4;
    };

    NativeErrorOr c_Vector4(const std::vector<GcPtr<Object>> &arguments);

    class Texture2D : public Object {
    public:
        explicit Texture2D(::Texture2D texture2D) : m_texture2D(texture2D) {}

        ::Texture2D get_value() { return m_texture2D; }

        ::Texture2D *get() { return &m_texture2D; }

        std::string str() override { return ::fmt::format("Texture2D({})", (void *) &m_texture2D); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Texture2D m_texture2D;
    };

    NativeErrorOr c_Texture2D(const std::vector<GcPtr<Object>> &arguments);


    class Rectangle : public BondObject {
    public:
        explicit Rectangle(::Rectangle rectangle) : m_rectangle(rectangle) {
            m_attributes["x"] = GarbageCollector::instance().make<Float>(rectangle.x);
            m_attributes["y"] = GarbageCollector::instance().make<Float>(rectangle.y);
            m_attributes["width"] = GarbageCollector::instance().make<Float>(rectangle.width);
            m_attributes["height"] = GarbageCollector::instance().make<Float>(rectangle.height);
        }

        Rectangle(const GcPtr<Float> &x, const GcPtr<Float> &y, const GcPtr<Float> &width, const GcPtr<Float> &height) {
            m_attributes["x"] = x;
            m_attributes["y"] = y;
            m_attributes["width"] = width;
            m_attributes["height"] = height;

            m_rectangle = {
                    x->get_value(),
                    y->get_value(),
                    width->get_value(),
                    height->get_value()
            };
        }

        ::Rectangle get_value() {
            m_rectangle.x = m_attributes["x"]->as<Float>()->get_value();
            m_rectangle.y = m_attributes["y"]->as<Float>()->get_value();
            m_rectangle.width = m_attributes["width"]->as<Float>()->get_value();
            m_rectangle.height = m_attributes["height"]->as<Float>()->get_value();
            return m_rectangle;
        }

        ::Rectangle *get() {
            get_value();
            return &m_rectangle;
        }

        std::string str() override { return ::fmt::format("Rectangle({})", (void *) &m_rectangle); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Rectangle m_rectangle{};

    };

    NativeErrorOr c_Rectangle(const std::vector<GcPtr<Object>> &arguments);

    class Font : public Object {
    public:
        explicit Font(::Font font) : m_font(font) {}
        ~Font() override {
            if (m_font.texture.id != 0) {
                UnloadFont(m_font);
            }
        }

        ::Font get_value() { return m_font; }

        ::Font *get() { return &m_font; }

        std::string str() override { return ::fmt::format("Font({})", (void *) &m_font); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::Font m_font;
    };

    NativeErrorOr c_Font(const std::vector<GcPtr<Object>> &arguments);

    class TextureCubemap : public Object {
    public:
        explicit TextureCubemap(::TextureCubemap textureCubemap) : m_textureCubemap(textureCubemap) {}

        ::TextureCubemap get_value() { return m_textureCubemap; }

        ::TextureCubemap *get() { return &m_textureCubemap; }

        std::string str() override { return ::fmt::format("TextureCubemap({})", (void *) &m_textureCubemap); }

        bool equal(const GcPtr<Object> &other) override { return this == other.get(); };

        size_t hash() override { return reinterpret_cast<size_t>(this); }

    private:
        ::TextureCubemap m_textureCubemap;
    };

    NativeErrorOr c_TextureCubemap(const std::vector<GcPtr<Object>> &arguments);


    struct Wave : public BondObject {
    public:
        Wave(::Wave wave) { m_wave= wave; }
        ~Wave() { ::UnloadWave(m_wave); }
        ::Wave get_value() { return m_wave; }
        ::Wave *get() { return &m_wave; }

    private:
        ::Wave m_wave;
    };

    struct AudioStream : public BondObject {
    public:
        AudioStream(::AudioStream audioStream) { m_audioStream= audioStream; }
        ~AudioStream() { ::UnloadAudioStream(m_audioStream); }
        ::AudioStream get_value() { return m_audioStream; }
        ::AudioStream *get() { return &m_audioStream; }

    private:
        ::AudioStream m_audioStream;
    };

    struct Sound : public BondObject {
    public:
        Sound(::Sound sound) { m_sound= sound; }
        ~Sound() { ::UnloadSound(m_sound); }
        ::Sound get_value() { return m_sound; }
        ::Sound *get() { return &m_sound; }

    private:
        ::Sound m_sound;
    };

    struct Music : public BondObject {
    public:
        Music(::Music music) { m_music = music; m_methods["set_loop"] = BIND(set_loop);}
        ~Music() { ::UnloadMusicStream(m_music); }
        ::Music get_value() { return m_music; }
        ::Music *get() { return &m_music; }


        NativeErrorOr set_loop(const std::vector<GcPtr<Object>>& arguments) {
            ASSERT_ARG_COUNT(1, arguments);
            DEFINE(cond, Bool, 0, arguments);

            m_music.looping = cond->get_value();
            return Globs::BondNil;
        }
    private:
        ::Music m_music;
    };
}