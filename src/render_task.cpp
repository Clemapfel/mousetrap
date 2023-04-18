//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <include/render_task.hpp>
#include <iostream>

namespace mousetrap
{
    RenderTask::RenderTask(const Shape* shape, const Shader* shader, const GLTransform* transform, BlendMode blend_mode)
    {
        if (noop_shader == nullptr)
            noop_shader = new Shader();

        if (noop_transform == nullptr)
            noop_transform = new GLTransform();

        _shape = shape;
        _shader = shader;
        _transform = transform;
        _blend_mode = blend_mode;
    }

    void RenderTask::render() const
    {
        if (_shape == nullptr)
            return;

        auto* shader = _shader == nullptr ? noop_shader : _shader;
        auto* transform = _transform == nullptr ? noop_transform : _transform;

        glUseProgram(shader->get_program_id());

        for (auto& pair : _floats)
            if (pair.second != nullptr)
                shader->set_uniform_float(pair.first, *pair.second);

        for (auto& pair : _ints)
            if (pair.second != nullptr)
                shader->set_uniform_int(pair.first, *pair.second);

        for (auto& pair : _vec2s)
            if (pair.second != nullptr)
                shader->set_uniform_vec2(pair.first, *pair.second);

        for (auto& pair : _vec3s)
            if (pair.second != nullptr)
                shader->set_uniform_vec3(pair.first, *pair.second);

        for (auto& pair : _vec4s)
            if (pair.second != nullptr)
                shader->set_uniform_vec4(pair.first, *pair.second);

        for (auto& pair : _transforms)
            if (pair.second != nullptr)
                shader->set_uniform_transform(pair.first, *pair.second);

        for (auto& pair : _colors_rgba)
            if (pair.second != nullptr)
                shader->set_uniform_vec4(pair.first, pair.second->operator glm::vec4());

        for (auto& pair : _colors_hsva)
            if (pair.second != nullptr)
                shader->set_uniform_vec4(pair.first, pair.second->operator glm::vec4());

        glEnable(GL_BLEND);
        set_current_blend_mode(_blend_mode);
        _shape->render(*shader, *transform);
        set_current_blend_mode(BlendMode::NORMAL);
    }

    void RenderTask::register_float(const std::string& uniform_name, float* value)
    {
        _floats.insert({uniform_name, value});
    }

    void RenderTask::register_int(const std::string& uniform_name, int* value)
    {
        _ints.insert({uniform_name, value});
    }

    void RenderTask::register_uint(const std::string& uniform_name, glm::uint* value)
    {
        _uints.insert({uniform_name, value});
    }

    void RenderTask::register_vec2(const std::string& uniform_name, Vector2f* value)
    {
        _vec2s.insert({uniform_name, value});
    }

    void RenderTask::register_vec3(const std::string& uniform_name, Vector3f* value)
    {
        _vec3s.insert({uniform_name, value});
    }

    void RenderTask::register_vec4(const std::string& uniform_name, Vector4f* value)
    {
        _vec4s.insert({uniform_name, value});
    }

    void RenderTask::register_transform(const std::string& uniform_name, GLTransform* value)
    {
        _transforms.insert({uniform_name, value});
    }

    void RenderTask::register_color(const std::string& uniform_name, RGBA* value)
    {
        _colors_rgba.insert({uniform_name, value});
    }

    void RenderTask::register_color(const std::string& uniform_name, HSVA* value)
    {
        _colors_hsva.insert({uniform_name, value});
    }

    void RenderTask::register_float(const std::string& uniform_name, const float* value)
    {
        _floats.insert({uniform_name, value});
    }

    void RenderTask::register_int(const std::string& uniform_name, const int* value)
    {
        _ints.insert({uniform_name, value});
    }

    void RenderTask::register_uint(const std::string& uniform_name, const glm::uint* value)
    {
        _uints.insert({uniform_name, value});
    }

    void RenderTask::register_vec2(const std::string& uniform_name, const Vector2f* value)
    {
        _vec2s.insert({uniform_name, value});
    }

    void RenderTask::register_vec3(const std::string& uniform_name, const Vector3f* value)
    {
        _vec3s.insert({uniform_name, value});
    }

    void RenderTask::register_vec4(const std::string& uniform_name, const Vector4f* value)
    {
        _vec4s.insert({uniform_name, value});
    }

    void RenderTask::register_transform(const std::string& uniform_name, const GLTransform* value)
    {
        _transforms.insert({uniform_name, value});
    }

    void RenderTask::register_color(const std::string& uniform_name, const RGBA* value)
    {
        _colors_rgba.insert({uniform_name, value});
    }

    void RenderTask::register_color(const std::string& uniform_name, const HSVA* value)
    {
        _colors_hsva.insert({uniform_name, value});
    }

    const Shape* RenderTask::get_shape() const
    {
        return _shape;
    }

    const Shader* RenderTask::get_shader() const
    {
        return _shader == nullptr ? noop_shader : _shader;
    }

    const GLTransform* RenderTask::get_transform() const
    {
        return _transform == nullptr ? noop_transform : _transform;
    }
}