//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/render_task.hpp>
#include <mousetrap/log.hpp>
#include <iostream>

namespace mousetrap
{
    RenderTask::RenderTask(const Shape& shape, const Shader* shader, const GLTransform& transform, BlendMode blend_mode)
    {
        if (noop_shader == nullptr)
            noop_shader = new Shader();

        if (noop_transform == nullptr)
            noop_transform = new GLTransform();

        g_object_ref(shape.operator GObject *());

        _shape = (detail::ShapeInternal*) shape.operator GObject*();
        _shader = shader;
        _transform = transform;
        _blend_mode = blend_mode;
    }

    RenderTask::~RenderTask()
    {
        if (G_IS_OBJECT(_shape))
            g_object_unref(_shape);
    }

    void RenderTask::render() const
    {
        auto* shader = _shader == nullptr ? noop_shader : _shader;

        glUseProgram(shader->get_program_id());

        for (auto& pair : _floats)
            shader->set_uniform_float(pair.first, pair.second);

        for (auto& pair : _ints)
            shader->set_uniform_int(pair.first, pair.second);

        for (auto& pair : _vec2s)
            shader->set_uniform_vec2(pair.first, pair.second);

        for (auto& pair : _vec3s)
            shader->set_uniform_vec3(pair.first, pair.second);

        for (auto& pair : _vec4s)
            shader->set_uniform_vec4(pair.first, pair.second);

        for (auto& pair : _transforms)
            shader->set_uniform_transform(pair.first, pair.second);

        for (auto& pair : _colors_rgba)
            shader->set_uniform_vec4(pair.first, pair.second.operator glm::vec4());

        for (auto& pair : _colors_hsva)
            shader->set_uniform_vec4(pair.first, pair.second.operator glm::vec4());

        glEnable(GL_BLEND);
        set_current_blend_mode(_blend_mode);

        auto shape = Shape(_shape);
        shape.render(*shader, _transform);
        set_current_blend_mode(BlendMode::NORMAL);
    }

    void RenderTask::set_uniform_float(const std::string& uniform_name, float value)
    {
        _floats.insert({uniform_name, value});
    }

    void RenderTask::set_uniform_int(const std::string& uniform_name, int value)
    {
        _ints.insert({uniform_name, value});
    }

    void RenderTask::set_uniform_uint(const std::string& uniform_name, glm::uint value)
    {
        _uints.insert({uniform_name, value});
    }

    void RenderTask::set_uniform_vec2(const std::string& uniform_name, Vector2f value)
    {
        _vec2s.insert({uniform_name, value});
    }

    void RenderTask::set_uniform_vec3(const std::string& uniform_name, Vector3f value)
    {
        _vec3s.insert({uniform_name, value});
    }

    void RenderTask::set_uniform_vec4(const std::string& uniform_name, Vector4f value)
    {
        _vec4s.insert({uniform_name, value});
    }

    void RenderTask::set_uniform_transform(const std::string& uniform_name, GLTransform value)
    {
        _transforms.insert({uniform_name, value});
    }

    void RenderTask::set_uniform_rgba(const std::string& uniform_name, RGBA value)
    {
        set_uniform_vec4(uniform_name, value.operator glm::vec4());
    }

    void RenderTask::set_uniform_hsva(const std::string& uniform_name, HSVA value)
    {
        set_uniform_vec4(uniform_name, value.operator glm::vec4());
    }

    float RenderTask::get_uniform_float(const std::string& uniform_name)
    {
        auto it = _floats.find(uniform_name);
        if (it == _floats.end())
        {
            log::critical("In RenderTask::get_uniform_float: No float with name `" + uniform_name + "` registered");
            return 0;
        }
        return it->second;
    }

    glm::int32_t RenderTask::get_uniform_int(const std::string& uniform_name)
    {
        auto it = _ints.find(uniform_name);
        if (it == _ints.end())
        {
            log::critical("In RenderTask::get_uniform_int: No int with name `" + uniform_name + "` registered");
            return 0;
        }
        return it->second;
    }

    glm::uint RenderTask::get_uniform_uint(const std::string& uniform_name)
    {
        auto it = _uints.find(uniform_name);
        if (it == _uints.end())
        {
            log::critical("In RenderTask::get_uniform_uint: No uint with name `" + uniform_name + "` registered");
            return 0;
        }
        return it->second;
    }

    Vector2f RenderTask::get_uniform_vec2(const std::string& uniform_name)
    {
        auto it = _vec2s.find(uniform_name);
        if (it == _vec2s.end())
        {
            log::critical("In RenderTask::get_uniform_vec2: No vec2 with name `" + uniform_name + "` registered");
            return {0, 0};
        }
        return it->second;
    }

    Vector3f RenderTask::get_uniform_vec3(const std::string& uniform_name)
    {
        auto it = _vec3s.find(uniform_name);
        if (it == _vec3s.end())
        {
            log::critical("In RenderTask::get_uniform_vec3: No vec3 with name `" + uniform_name + "` registered");
            return {0, 0, 0};
        }
        return it->second;
    }

    Vector4f RenderTask::get_uniform_vec4(const std::string& uniform_name)
    {
        auto it = _vec4s.find(uniform_name);
        if (it == _vec4s.end())
        {
            log::critical("In RenderTask::get_uniform_vec4: No vec4 with name `" + uniform_name + "` registered");
            return {0, 0, 0, 0};
        }
        return it->second;
    }

    RGBA RenderTask::get_uniform_rgba(const std::string& uniform_name)
    {
        auto it = _vec4s.find(uniform_name);
        if (it == _vec4s.end())
        {
            log::critical("In RenderTask::get_uniform_rgba: No vec4 with name `" + uniform_name + "` registered");
            return {0, 0, 0, 0};
        }
        auto out = it->second;
        return RGBA(out.x, out.y, out.z, out.w);
    }

    HSVA RenderTask::get_uniform_hsva(const std::string& uniform_name)
    {
        auto it = _vec4s.find(uniform_name);
        if (it == _vec4s.end())
        {
            log::critical("In RenderTask::get_uniform_hsva: No vec4 with name `" + uniform_name + "` registered");
            return {0, 0, 0, 0};
        }
        auto out = it->second;
        return HSVA(out.x, out.y, out.z, out.w);
    }

    GLTransform RenderTask::get_uniform_transform(const std::string& uniform_name)
    {
        auto it = _transforms.find(uniform_name);
        if (it == _transforms.end())
        {
            log::critical("In RenderTask::get_uniform_transform: No mat4x4 with name `" + uniform_name + "` registered");
            return GLTransform();
        }
        return it->second;
    }

    /*
    const Shape* RenderTask::get_shape() const
    {
        std::cerr << "In RenderTask::get_shape: TODO" << std::endl;
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
     */
}