// 
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>
#include <include/gl_common.hpp>
#include <include/transform.hpp>

namespace rat
{
    enum class ShaderType
    {
        FRAGMENT = GL_FRAGMENT_SHADER,
        VERTEX = GL_VERTEX_SHADER
    };

    struct Shader
    {
        public:
            Shader();
            ~Shader();

            GLNativeHandle get_program_id() const;
            GLNativeHandle get_fragment_shader_id() const;
            GLNativeHandle get_vertex_shader_id() const;

            //
            void create_from_string(const std::string& code, ShaderType);
            void create_from_file(const std::string& path, ShaderType);

            //
            int get_uniform_location(const std::string&) const;

            //
            void set_uniform_float(const std::string& uniform_name, float);
            void set_uniform_int(const std::string& uniform_name, int);
            void set_uniform_vec2(const std::string& uniform_name, Vector2f);
            void set_uniform_vec3(const std::string& uniform_name, Vector3f);
            void set_uniform_vec4(const std::string& uniform_name, Vector4f);
            void set_uniform_transform(const std::string& uniform_name, Transform);

            //
            static int get_vertex_position_location();
            static int get_vertex_color_location();
            static int get_vertex_texture_coordinate_location();

        private:
            [[nodiscard]] GLNativeHandle compile_shader(const std::string&, ShaderType shader_type);
            [[nodiscard]] GLNativeHandle link_program(GLNativeHandle fragment_id, GLNativeHandle vertex_id);

            // local
            GLNativeHandle _program_id,
                    _fragment_shader_id,
                    _vertex_shader_id;

            // default noop
            static inline size_t _noop_program_id,
                    _noop_fragment_shader_id,
                    _noop_vertex_shader_id;

            static inline const std::string _noop_fragment_shader_source = R"(
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

            static inline const std::string _noop_vertex_shader_source = R"(
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
    };
}

// #############################################################################

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

namespace rat
{
    Shader::Shader()
    {
        if (GL_INITIALIZED and _noop_program_id == 0)
        {
            _noop_fragment_shader_id = compile_shader(_noop_fragment_shader_source, ShaderType::FRAGMENT);
            _noop_vertex_shader_id = compile_shader(_noop_vertex_shader_source, ShaderType::VERTEX);
            _noop_program_id = link_program(_noop_fragment_shader_id, _noop_vertex_shader_id);
        }
        else if (not GL_INITIALIZED)
            std::cerr << "[WARNING] In Shader::Shader: Trying to initialize the noop shaders, but GL_INITIALIZE is still false." << std::endl;

        _program_id = _noop_program_id;
        _fragment_shader_id = _noop_fragment_shader_id;
        _vertex_shader_id = _noop_vertex_shader_id;
    }

    Shader::~Shader()
    {
        if (_fragment_shader_id != 0 and _fragment_shader_id != _noop_fragment_shader_id)
            glDeleteShader(_fragment_shader_id);

        if (_vertex_shader_id != 0 and _vertex_shader_id != _noop_vertex_shader_id)
            glDeleteShader(_vertex_shader_id);

        if (_program_id != 0 and _program_id != _noop_program_id)
            glDeleteProgram(_program_id);
    }

    void Shader::create_from_string(const std::string &code, ShaderType type)
    {
        if (type == ShaderType::FRAGMENT)
            _fragment_shader_id = compile_shader(code, type);
        else
            _vertex_shader_id = compile_shader(code, type);

        _program_id = link_program(_fragment_shader_id, _vertex_shader_id);
    }

    void Shader::create_from_file(const std::string& path, ShaderType type)
    {
        auto file = std::ifstream();

        file.open(path);
        if (not file.is_open())
        {
            std::cerr << "[WARNING] In Shader::create_from_file: Unable to open file at `" << path << "`" << std::endl;
            return;
        }
        auto str = std::stringstream();
        str << file.rdbuf();

        create_from_string(str.str(), type);
        file.close();
    }

    GLNativeHandle Shader::get_program_id() const
    {
        return _program_id;
    }

    GLNativeHandle Shader::get_vertex_shader_id() const
    {
        return _vertex_shader_id;
    }

    GLNativeHandle Shader::get_fragment_shader_id() const
    {
        return _fragment_shader_id;
    }

    GLNativeHandle Shader::compile_shader(const std::string& source, ShaderType shader_type)
    {
        GLNativeHandle id = glCreateShader(static_cast<GLenum>(shader_type));

        const char* source_ptr = source.c_str();
        glShaderSource(id, 1, &source_ptr, nullptr);
        glCompileShader(id);

        GLint compilation_success = GL_FALSE;
        glGetShaderiv(id, GL_COMPILE_STATUS, &compilation_success);
        if (compilation_success != GL_TRUE)
        {
            std::cerr << "In Shader::compile_shader: compilation failed:\n"
                      << source << "\n\n";

            int info_length = 0;
            int max_length = info_length;

            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);

            auto log = std::vector<char>();
            log.resize(max_length);

            glGetShaderInfoLog(id, max_length, &info_length, log.data());

            for (auto c : log)
                std::cerr << c;
            std::cerr << std::endl;

            glDeleteShader(id);
            id = 0;
        }

        return id;
    }

    GLNativeHandle Shader::link_program(GLNativeHandle fragment_id, GLNativeHandle vertex_id)
    {
        GLNativeHandle id = glCreateProgram();
        glAttachShader(id, fragment_id);
        glAttachShader(id, vertex_id);
        glLinkProgram(id);

        GLint link_success = GL_FALSE;
        glGetProgramiv(id, GL_LINK_STATUS, &link_success);
        if (link_success != GL_TRUE)
        {
            std::cerr << "In Shader::link_program: linking failed:" << std::endl;

            int info_length = 0;
            int max_length = info_length;

            glGetProgramiv(_program_id, GL_INFO_LOG_LENGTH, &max_length);

            auto log = std::vector<char>();
            log.resize(max_length);

            glGetProgramInfoLog(_program_id, max_length, &info_length, log.data());

            for (auto c: log)
                std::cerr << c;
            std::cerr << std::endl;

            glDeleteProgram(id);
            id = 0;
        }

        return id;
    }

    void Shader::set_uniform_float(const std::string& uniform_name, float value)
    {
        glUniform1f(get_uniform_location(uniform_name), value);
    }

    void Shader::set_uniform_int(const std::string& uniform_name, int value)
    {
        glUniform1i(get_uniform_location(uniform_name), value);
    }

    void Shader::set_uniform_vec2(const std::string& uniform_name, Vector2f value)
    {
        glUniform2f(get_uniform_location(uniform_name), value.x, value.y);
    }

    void Shader::set_uniform_vec3(const std::string& uniform_name, Vector3f value)
    {
        glUniform3f(get_uniform_location(uniform_name), value.x, value.y, value.z);
    }

    void Shader::set_uniform_vec4(const std::string& uniform_name, Vector4f value)
    {
        glUniform4f(get_uniform_location(uniform_name), value.x, value.y, value.z, value.w);
    }

    void Shader::set_uniform_transform(const std::string& uniform_name, Transform value)
    {
        glUniformMatrix4fv(get_uniform_location(uniform_name), 1, false, &value.transform[0][0]);
    }

    int Shader::get_uniform_location(const std::string& str) const
    {
        return glGetUniformLocation(_program_id, str.c_str());
    }

    int Shader::get_vertex_position_location()
    {
        return 0;
    }

    int Shader::get_vertex_color_location()
    {
        return 1;
    }

    int Shader::get_vertex_texture_coordinate_location()
    {
        return 2;
    }
}