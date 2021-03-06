// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    RenderTask::RenderTask(Shape* shape, Shader* shader, Transform* transform)
    {
        if (shape == nullptr)
            throw std::invalid_argument("In RenderTask::RenderTask: shape == nullptr");

        if (noop_shader == nullptr)
            noop_shader = new Shader();

        if (noop_transform == nullptr)
            noop_transform = new Transform();

        _shape = shape;
        _shader = shader;
        _transform = transform;
    }

    void RenderTask::render()
    {
        if (_shape == nullptr)
        {
            std::cerr << "[WARNING] In RenderTask::render: trying to render a shape pointer that is no longer valid" << std::endl;
            return;
        }

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

        _shape->render(*shader, *transform);
    }

    void RenderTask::register_float(const std::string& uniform_name, float* value)
    {
        _floats.insert({uniform_name, value});
    }

    void RenderTask::register_int(const std::string& uniform_name, int* value)
    {
        _ints.insert({uniform_name, value});
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

    void RenderTask::register_transform(const std::string& uniform_name, Transform* value)
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

    Shape* RenderTask::get_shape()
    {
        return _shape;
    }

    Shader* RenderTask::get_shader()
    {
        return _shader == nullptr ? noop_shader : _shader;
    }

    Transform* RenderTask::get_transform()
    {
        return _transform == nullptr ? noop_transform : _transform;
    }
}