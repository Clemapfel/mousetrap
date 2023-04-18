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
    /// @brief bundles a shape in a render task that can be invoked more conveniently during the render cycle
    class RenderTask
    {
        public:
            /// @brief construct the render task
            /// @param shape shape to render
            /// @param shader shader to apply to the shape when rendering, if nullptr, the default shader is used
            /// @param transform transform to hand to the vertex shader when rendering, if nullptr, the identity transform will be handed isntead
            /// @param blend_mode blend_mode to set before rendering the shape, normal alpha-blending if unspecified
            explicit RenderTask(const Shape*, const Shader* = nullptr, const GLTransform* = nullptr, BlendMode blend_mode = BlendMode::NORMAL);

            /// @brief register a float that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>float</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_float(const std::string& uniform_name, float*);

            /// @brief register an int that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>int</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_int(const std::string& uniform_name, int*);

            /// @brief register an unsigned integer that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>uint</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_uint(const std::string& uniform_name, glm::uint*);

            /// @brief register a Vector2f that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec2</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_vec2(const std::string& uniform_name, Vector2f*);

            /// @brief register a Vector3f that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec3</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_vec3(const std::string& uniform_name, Vector3f*);

            /// @brief register a Vector4f that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec4</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_vec4(const std::string& uniform_name, Vector4f*);

            /// @brief register a transform that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>mat4x4</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_transform(const std::string& uniform_name, GLTransform*);

            /// @brief register a color as RGBA that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec4</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_color(const std::string& uniform_name, RGBA*);

            /// @brief register a color as HSVA that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec4</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_color(const std::string& uniform_name, HSVA* will_not_be_converted);

            /// @brief register a const float that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>float>/tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_float(const std::string& uniform_name, const float*);

            /// @brief register a const int that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>int</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_int(const std::string& uniform_name, const int*);

            /// @brief register a const unsigned int that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>uint</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_uint(const std::string& uniform_name, const glm::uint*);

            /// @brief register a const Vector2f that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec2</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_vec2(const std::string& uniform_name, const Vector2f*);

            /// @brief register a const Vector3f that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec3</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_vec3(const std::string& uniform_name, const Vector3f*);

            /// @brief register a const Vector4f that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec4</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_vec4(const std::string& uniform_name, const Vector4f*);

            /// @brief register a const transfor that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>mat4x4</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_transform(const std::string& uniform_name, const GLTransform*);

            /// @brief register a const color in RGBA that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec4</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_color(const std::string& uniform_name, const RGBA*);

            /// @brief register a const color in HSVA that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec4</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            void register_color(const std::string& uniform_name, const HSVA* will_not_be_converted);

            /// @brief perform the render step to the currently bound framebuffer
            void render() const;

            /// @brief get the shape used during render
            /// @return pointer to shape, const
            const Shape* get_shape() const;

            /// @brief get the shader used during render. If no shader was specified, this will return the default shader
            /// @return pointer to shader, const
            const Shader* get_shader() const;

            /// @brief get the transform used during render. If no transform was specified, this will return the identity transform
            /// @return pointer to transform, const
            const GLTransform* get_transform() const;

        private:
            const Shape* _shape = nullptr;
            const Shader* _shader = nullptr;
            const GLTransform* _transform = nullptr;
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


