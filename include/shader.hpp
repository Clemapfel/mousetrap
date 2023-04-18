//
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>
#include <include/gl_common.hpp>
#include <include/gl_transform.hpp>

namespace mousetrap
{
    /// @brief shader type
    enum class ShaderType
    {
        /// @brief shader is a fragment shader
        FRAGMENT = GL_FRAGMENT_SHADER,

        /// @brief shader is a vertex shader
        VERTEX = GL_VERTEX_SHADER
    };

    /// @brief shader, holds the OpenGL shader program, a vertex and a fragment shader
    struct Shader
    {
        public:
            /// @brief construct, holds default shader program for both fragment and vertex
            Shader();

            /// @brief destruct, frees GPU-side memory
            ~Shader();

            /// @brief get the native OpenGL id of the shader program
            /// @return id
            GLNativeHandle get_program_id() const;

            /// @brief get the native OpenGL id of the fragment shader
            /// @return id
            GLNativeHandle get_fragment_shader_id() const;

            /// @brief get the native OpenGL id of the vertex shader
            /// @return id
            GLNativeHandle get_vertex_shader_id() const;

            /// @brief create shader from source code as string
            /// @param code glsl code
            /// @param shader_type
            /// @return true if compiled succesfully, false otherwise
            bool create_from_string(const std::string& code, ShaderType);

            /// @brief create shader from a file, usually .glsl, .frag or .vert
            /// @param path absolute path to file with glsl code
            /// @param shader_type
            /// @return true the file was accessed and compiled succesfully, false otherwise
            bool create_from_file(const std::string& path, ShaderType);

            /// @brief get location of a uniform with given name
            /// @param name exact name of a uniform mentioned in the shader source code
            /// @return location or -1 if no uniform of that name exists
            int get_uniform_location(const std::string&) const;

            /// @brief set uniform float
            /// @param uniform_name exact name of a uniform mentioned in the shader source
            /// @param value
            void set_uniform_float(const std::string& uniform_name, float) const;

            /// @brief set uniform int
            /// @param uniform_name exact name of a uniform mentioned in the shader source
            /// @param value
            void set_uniform_int(const std::string& uniform_name, int) const;

            /// @brief set uniform uint
            /// @param uniform_name exact name of a uniform mentioned in the shader source
            /// @param value
            void set_uniform_uint(const std::string& uniform_name, glm::uint) const;

            /// @brief set uniform vec2
            /// @param uniform_name exact name of a uniform mentioned in the shader source
            /// @param value
            void set_uniform_vec2(const std::string& uniform_name, Vector2f) const;

            /// @brief set uniform vec3
            /// @param uniform_name exact name of a uniform mentioned in the shader source
            /// @param value
            void set_uniform_vec3(const std::string& uniform_name, Vector3f) const;

            /// @brief set uniform vec4
            /// @param uniform_name exact name of a uniform mentioned in the shader source
            /// @param value
            void set_uniform_vec4(const std::string& uniform_name, Vector4f) const;

            /// @brief set uniform mat4x4
            /// @param uniform_name exact name of a uniform mentioned in the shader source
            /// @param value
            void set_uniform_transform(const std::string& uniform_name, GLTransform) const;

            /// @brief get position of the default <tt>_vertex_position</tt> uniform
            /// @returns position
            static int get_vertex_position_location();

            /// @brief get position of the default <tt>_vertex_color</tt> uniform
            /// @returns position
            static int get_vertex_color_location();

            /// @brief get position of the default <tt>_texture_coordinates</tt> uniform
            /// @returns position
            static int get_vertex_texture_coordinate_location();

            /// @brief default fragment shader behavior, render a shape respecting its vertices colors and its optional texture
            static inline const std::string noop_fragment_shader_code = R"(
                #version 130

                in vec4 _vertex_color;
                in vec2 _texture_coordinates;
                in vec3 _vertex_position;

                out vec4 _fragment_color;

                uniform int _texture_set;
                uniform sampler2D _texture;

                void main()
                {
                    if (_texture_set != 1)
                        _fragment_color = _vertex_color;
                    else
                        _fragment_color = texture2D(_texture, _texture_coordinates) * _vertex_color;
                }
            )";

            /// @brief noop vertex shader behavior, render the shapes vertices after applying the set transform
            static inline const std::string noop_vertex_shader_code = R"(
                #version 330

                layout (location = 0) in vec3 _vertex_position_in;
                layout (location = 1) in vec4 _vertex_color_in;
                layout (location = 2) in vec2 _vertex_texture_coordinates_in;

                uniform mat4 _transform;

                out vec4 _vertex_color;
                out vec2 _texture_coordinates;
                out vec3 _vertex_position;

                void main()
                {
                    gl_Position = _transform * vec4(_vertex_position_in, 1.0);
                    _vertex_color = _vertex_color_in;
                    _vertex_position = _vertex_position_in;
                    _texture_coordinates = _vertex_texture_coordinates_in;
                }
            )";

        private:
            [[nodiscard]] GLNativeHandle compile_shader(const std::string&, ShaderType shader_type);
            [[nodiscard]] GLNativeHandle link_program(GLNativeHandle fragment_id, GLNativeHandle vertex_id);

            GLNativeHandle _program_id,
            _fragment_shader_id,
            _vertex_shader_id;

            static inline size_t _noop_program_id,
            _noop_fragment_shader_id,
            _noop_vertex_shader_id;
    };
}


