//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/shader.hpp>
#include <include/log.hpp>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

namespace mousetrap
{
    Shader::Shader()
    {
        if (_noop_program_id == 0)
        {
            _noop_fragment_shader_id = compile_shader(noop_fragment_shader_code, ShaderType::FRAGMENT);
            _noop_vertex_shader_id = compile_shader(noop_vertex_shader_code, ShaderType::VERTEX);
            _noop_program_id = link_program(_noop_fragment_shader_id, _noop_vertex_shader_id);
        }

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

    bool Shader::create_from_string(const std::string& code, ShaderType type)
    {
        if (type == ShaderType::FRAGMENT)
            _fragment_shader_id = compile_shader(code, type);
        else
            _vertex_shader_id = compile_shader(code, type);

        _program_id = link_program(_fragment_shader_id, _vertex_shader_id);

        if (
            (type == ShaderType::FRAGMENT and _fragment_shader_id == 0) or
            (type == ShaderType::VERTEX and _vertex_shader_id == 0) or
            _program_id == 0
        )
            return false;
        else
            return true;
    }

    bool Shader::create_from_file(const std::string& path, ShaderType type)
    {
        auto file = std::ifstream();

        file.open(path);
        if (not file.is_open())
        {
            log::critical("[WARNING] In Shader::create_from_file: Unable to open file at `" + path + "`", MOUSETRAP_DOMAIN);
            return false;
        }
        auto str = std::stringstream();
        str << file.rdbuf();

        return create_from_string(str.str(), type);
        file.close();
        return true;
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
            std::stringstream str;
            str << "In Shader::compile_shader: compilation failed:\n"
                      << source << "\n\n";

            int info_length = 0;
            int max_length = info_length;

            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);

            auto log = std::vector<char>();
            log.resize(max_length);

            glGetShaderInfoLog(id, max_length, &info_length, log.data());

            for (auto c: log)
                str << c;

            log::critical(str.str(), MOUSETRAP_DOMAIN);

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
            std::stringstream str;
            str << "In Shader::link_program: linking failed:" << std::endl;

            int info_length = 0;
            int max_length = info_length;

            glGetProgramiv(_program_id, GL_INFO_LOG_LENGTH, &max_length);

            auto log = std::vector<char>();
            log.resize(max_length);

            glGetProgramInfoLog(_program_id, max_length, &info_length, log.data());

            for (auto c: log)
                str << c;

            log::critical(str.str(), MOUSETRAP_DOMAIN);

            glDeleteProgram(id);
            id = 0;
        }

        return id;
    }

    void Shader::set_uniform_float(const std::string& uniform_name, float value) const
    {
        glUniform1f(get_uniform_location(uniform_name), value);
    }

    void Shader::set_uniform_int(const std::string& uniform_name, int value) const
    {
        glUniform1i(get_uniform_location(uniform_name), value);
    }

    void Shader::set_uniform_uint(const std::string& uniform_name, glm::uint value) const
    {
        glUniform1ui(get_uniform_location(uniform_name), value);
    }

    void Shader::set_uniform_vec2(const std::string& uniform_name, Vector2f value) const
    {
        glUniform2f(get_uniform_location(uniform_name), value.x, value.y);
    }

    void Shader::set_uniform_vec3(const std::string& uniform_name, Vector3f value) const
    {
        glUniform3f(get_uniform_location(uniform_name), value.x, value.y, value.z);
    }

    void Shader::set_uniform_vec4(const std::string& uniform_name, Vector4f value) const
    {
        glUniform4f(get_uniform_location(uniform_name), value.x, value.y, value.z, value.w);
    }

    void Shader::set_uniform_transform(const std::string& uniform_name, GLTransform value) const
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