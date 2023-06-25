//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/shape.hpp>
#include <mousetrap/shader.hpp>
#include <mousetrap/gl_transform.hpp>
#include <mousetrap/blend_mode.hpp>

#include <map>

namespace mousetrap
{
    #ifndef DOXYGEN
    namespace detail
    {
        struct _RenderTaskInternal
        {
            GObject parent;

            detail::ShapeInternal* _shape = nullptr;
            detail::ShaderInternal* _shader = nullptr;
            GLTransform _transform;
            BlendMode _blend_mode;

            static inline Shader* noop_shader = nullptr;

            std::map<std::string, float>* _floats;
            std::map<std::string, int>* _ints;
            std::map<std::string, glm::uint>* _uints;
            std::map<std::string, Vector2f>* _vec2s;
            std::map<std::string, Vector3f>* _vec3s;
            std::map<std::string, Vector4f>* _vec4s;
            std::map<std::string, GLTransform>* _transforms;
        };
        using RenderTaskInternal = _RenderTaskInternal;
    }
    #endif

    /// @brief bundles a shape in a render task that can be invoked more conveniently during the render cycle
    class RenderTask : public detail::notify_if_gl_uninitialized,
        public SignalEmitter
    {
        public:
            /// @brief construct the render task
            /// @param shape shape to render
            /// @param shader shader to apply to the shape when rendering, if nullptr, the default shader is used
            /// @param transform transform to hand to the vertex shader when rendering, if nullptr, the identity transform will be handed isntead
            /// @param blend_mode blend_mode to set before rendering the shape, normal alpha-blending if unspecified
            explicit RenderTask(const Shape& shape, const Shader* shader = nullptr, const GLTransform& transform = GLTransform(), BlendMode blend_mode = BlendMode::NORMAL);

            /// @brief create from gobject \for_internal_use_only
            RenderTask(detail::RenderTaskInternal*);

            /// @brief destructor
            ~RenderTask();

            /// @brief register a float that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>float</tt>
            /// @param pointer pointer to value, the user is responsible for keeping it in scope
            /// @param value 
            void set_uniform_float(const std::string& uniform_name, float value);

            /// @brief register an int that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>int</tt>
            /// @param value
            void set_uniform_int(const std::string& uniform_name, int value);

            /// @brief register an unsigned integer that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>uint</tt>
            /// @param value
            void set_uniform_uint(const std::string& uniform_name, glm::uint value);

            /// @brief register a Vector2f that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec2</tt>
            /// @param value
            void set_uniform_vec2(const std::string& uniform_name, Vector2f value);

            /// @brief register a Vector3f that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec3</tt>
            /// @param value
            void set_uniform_vec3(const std::string& uniform_name, Vector3f value);

            /// @brief register a Vector4f that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec4</tt>
            /// @param value
            void set_uniform_vec4(const std::string& uniform_name, Vector4f value);

            /// @brief register a transform that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>mat4x4</tt>
            /// @param value
            void set_uniform_transform(const std::string& uniform_name, GLTransform value);

            /// @brief register a color as RGBA that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec4</tt>
            /// @param value
            void set_uniform_rgba(const std::string& uniform_name, RGBA value);

            /// @brief register a color as HSVA that will be handed to a shader uniform of the given name during render
            /// @param uniform_name name of the uniform variable in the glsl shader code, has to be of type <tt>vec4</tt>
            /// @param value
            void set_uniform_hsva(const std::string& uniform_name, HSVA value);

            /// @brief access currently registered float
            /// @param uniform_name
            /// @return float
            float get_uniform_float(const std::string& uniform_name);

            /// @brief access currently registered int
            /// @param uniform_name
            /// @return int32_t
            glm::int32_t get_uniform_int(const std::string& uniform_name);

            /// @brief access currently registered uint
            /// @param uniform_name
            /// @return uint32_t
            glm::uint get_uniform_uint(const std::string& uniform_name);

            /// @brief access currently registered vec2
            /// @param uniform_name
            /// @return Vector2f
            Vector2f get_uniform_vec2(const std::string& uniform_name);

            /// @brief access currently registered vec3
            /// @param uniform_name
            /// @return Vector3f
            Vector3f get_uniform_vec3(const std::string& uniform_name);

            /// @brief access currently registered vec4
            /// @param uniform_name
            /// @return Vector4f
            Vector4f get_uniform_vec4(const std::string& uniform_name);

            /// @brief access currently registered transform (mat4x4)
            /// @param uniform_name
            /// @return GLTransform
            GLTransform get_uniform_transform(const std::string& uniform_name);

            /// @brief access currently registered color (vec4)
            /// @param uniform_name
            /// @return RGBA
            RGBA get_uniform_rgba(const std::string& uniform_name);

            /// @brief access currently registered color (vec4)
            /// @param uniform_name
            /// @return HSVA
            HSVA get_uniform_hsva(const std::string& uniform_name);

            /// @brief perform the render step to the currently bound framebuffer
            void render() const;

            /// @brief expose as GObject \for_internal_use_only
            operator GObject*() const override;

        private:
            detail::RenderTaskInternal* _internal;
    };
}


