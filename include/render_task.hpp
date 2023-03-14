// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/shape.hpp>
#include <include/shader.hpp>
#include <include/gl_transform.hpp>
#include <include/blend_mode.hpp>

#include <map>

namespace mousetrap
{
    class RenderTask
    {
        public:
            RenderTask(Shape*, Shader* = nullptr, GLTransform* = nullptr, BlendMode blend_mode = BlendMode::NORMAL);

            void register_float(const std::string& uniform_name, float*);
            void register_int(const std::string& uniform_name, int*);
            void register_uint(const std::string& uniform_name, glm::uint*);
            void register_vec2(const std::string& uniform_name, Vector2f*);
            void register_vec3(const std::string& uniform_name, Vector3f*);
            void register_vec4(const std::string& uniform_name, Vector4f*);
            void register_transform(const std::string& uniform_name, GLTransform*);
            void register_color(const std::string& uniform_name, RGBA*);
            void register_color(const std::string& uniform_name, HSVA* will_not_be_converted);

            void register_float(const std::string& uniform_name, const float*);
            void register_int(const std::string& uniform_name, const int*);
            void register_uint(const std::string& uniform_name, const glm::uint*);
            void register_vec2(const std::string& uniform_name, const Vector2f*);
            void register_vec3(const std::string& uniform_name, const Vector3f*);
            void register_vec4(const std::string& uniform_name, const Vector4f*);
            void register_transform(const std::string& uniform_name, const GLTransform*);
            void register_color(const std::string& uniform_name, const RGBA*);
            void register_color(const std::string& uniform_name, const HSVA* will_not_be_converted);

            void render();

            Shape* get_shape();
            Shader* get_shader();
            GLTransform* get_transform();
            
        private:
            Shape* _shape = nullptr;
            Shader* _shader = nullptr;
            GLTransform* _transform = nullptr;
            BlendMode _blend_mode;
            
            static inline Shader* noop_shader = nullptr;
            static inline GLTransform* noop_transform = nullptr;
            
            std::map<std::string, const float*> _floats;
            std::map<std::string, const int*> _ints;
            std::map<std::string, const glm::uint*> _uints;
            std::map<std::string, const Vector2f*> _vec2s;
            std::map<std::string, const Vector3f*> _vec3s;
            std::map<std::string, const Vector4f*> _vec4s;
            std::map<std::string, const GLTransform*> _transforms;
            std::map<std::string, const RGBA*> _colors_rgba;
            std::map<std::string, const HSVA*> _colors_hsva;
    };
}

#include <src/render_task.inl>
