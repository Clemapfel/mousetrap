//
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/gl_common.hpp>
#if MOUSETRAP_ENABLE_OPENGL_COMPONENT

#include <mousetrap/render_task.hpp>
#include <mousetrap/log.hpp>
#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(RenderTaskInternal, render_task_internal, RENDER_TASK_INTERNAL)

        static void render_task_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_RENDER_TASK_INTERNAL(object);
            G_OBJECT_CLASS(render_task_internal_parent_class)->finalize(object);

            delete self->_floats;
            delete self->_ints;
            delete self->_uints;
            delete self->_vec2s;
            delete self->_vec3s;
            delete self->_vec4s;
            delete self->_transforms;

            g_object_unref(self->_shape);
            g_object_unref(self->_shader);
        }

        DEFINE_NEW_TYPE_TRIVIAL_INIT(RenderTaskInternal, render_task_internal, RENDER_TASK_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(RenderTaskInternal, render_task_internal, RENDER_TASK_INTERNAL)

        static RenderTaskInternal* render_task_internal_new(const Shape& shape, const Shader* shader, const GLTransform& transform, BlendMode blend_mode)
        {
            auto* self = (RenderTaskInternal*) g_object_new(render_task_internal_get_type(), nullptr);
            render_task_internal_init(self);

            self->_shape = (detail::ShapeInternal*) shape.operator GObject*();

            if (self->noop_shader == nullptr)
                self->noop_shader = new Shader();

            if (shader == nullptr)
                self->_shader = (detail::ShaderInternal*) self->noop_shader->operator GObject*();
            else
                self->_shader = (detail::ShaderInternal*) shader->operator GObject*();

            self->_floats = new std::map<std::string, float>();
            self->_ints = new std::map<std::string, int>();
            self->_uints = new std::map<std::string, glm::uint>();
            self->_vec2s = new std::map<std::string, Vector2f>();
            self->_vec3s = new std::map<std::string, Vector3f>();
            self->_vec4s = new std::map<std::string, Vector4f>();
            self->_transforms = new std::map<std::string, GLTransform>();

            self->_transform = transform;
            self->_blend_mode = blend_mode;

            g_object_ref(self->_shape);
            g_object_ref(self->_shader);

            return self;
        }
    }

    RenderTask::RenderTask(const Shape& shape, const Shader* shader, const GLTransform& transform, BlendMode blend_mode)
    {
        _internal = detail::render_task_internal_new(shape, shader, transform, blend_mode);
        g_object_ref(_internal);
    }

    RenderTask::RenderTask(detail::RenderTaskInternal* internal)
    {
        _internal = g_object_ref(internal);
    }

    RenderTask::~RenderTask()
    {
        g_object_unref(_internal);
    }

    void RenderTask::render() const
    {
        auto shader = Shader(_internal->_shader);

        glUseProgram(shader.get_program_id());

        for (auto& pair : *_internal->_floats)
            shader.set_uniform_float(pair.first, pair.second);

        for (auto& pair : *_internal->_ints)
            shader.set_uniform_int(pair.first, pair.second);

        for (auto& pair : *_internal->_uints)
            shader.set_uniform_uint(pair.first, pair.second);

        for (auto& pair : *_internal->_vec2s)
            shader.set_uniform_vec2(pair.first, pair.second);

        for (auto& pair : *_internal->_vec3s)
            shader.set_uniform_vec3(pair.first, pair.second);

        for (auto& pair : *_internal->_vec4s)
            shader.set_uniform_vec4(pair.first, pair.second);

        for (auto& pair :*_internal-> _transforms)
            shader.set_uniform_transform(pair.first, pair.second);

        glEnable(GL_BLEND);
        set_current_blend_mode(_internal->_blend_mode);

        auto shape = Shape(_internal->_shape);
        shape.render(shader, _internal->_transform);

        set_current_blend_mode(BlendMode::NORMAL);
    }

    void RenderTask::set_uniform_float(const std::string& uniform_name, float value)
    {
        _internal->_floats->insert({uniform_name, value});
    }

    void RenderTask::set_uniform_int(const std::string& uniform_name, int value)
    {
        _internal->_ints->insert({uniform_name, value});
    }

    void RenderTask::set_uniform_uint(const std::string& uniform_name, glm::uint value)
    {
        _internal->_uints->insert({uniform_name, value});
    }

    void RenderTask::set_uniform_vec2(const std::string& uniform_name, Vector2f value)
    {
        _internal->_vec2s->insert({uniform_name, value});
    }

    void RenderTask::set_uniform_vec3(const std::string& uniform_name, Vector3f value)
    {
        _internal->_vec3s->insert({uniform_name, value});
    }

    void RenderTask::set_uniform_vec4(const std::string& uniform_name, Vector4f value)
    {
        _internal->_vec4s->insert({uniform_name, value});
    }

    void RenderTask::set_uniform_transform(const std::string& uniform_name, GLTransform value)
    {
        _internal->_transforms->insert({uniform_name, value});
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
        auto it = _internal->_floats->find(uniform_name);
        if (it == _internal->_floats->end())
        {
            log::critical("In RenderTask::get_uniform_float: No float with name `" + uniform_name + "` registered");
            return 0;
        }
        return it->second;
    }

    glm::int32_t RenderTask::get_uniform_int(const std::string& uniform_name)
    {
        auto it = _internal->_ints->find(uniform_name);
        if (it == _internal->_ints->end())
        {
            log::critical("In RenderTask::get_uniform_int: No int with name `" + uniform_name + "` registered");
            return 0;
        }
        return it->second;
    }

    glm::uint RenderTask::get_uniform_uint(const std::string& uniform_name)
    {
        auto it = _internal->_uints->find(uniform_name);
        if (it == _internal->_uints->end())
        {
            log::critical("In RenderTask::get_uniform_uint: No uint with name `" + uniform_name + "` registered");
            return 0;
        }
        return it->second;
    }

    Vector2f RenderTask::get_uniform_vec2(const std::string& uniform_name)
    {
        auto it = _internal->_vec2s->find(uniform_name);
        if (it == _internal->_vec2s->end())
        {
            log::critical("In RenderTask::get_uniform_vec2: No vec2 with name `" + uniform_name + "` registered");
            return {0, 0};
        }
        return it->second;
    }

    Vector3f RenderTask::get_uniform_vec3(const std::string& uniform_name)
    {
        auto it = _internal->_vec3s->find(uniform_name);
        if (it == _internal->_vec3s->end())
        {
            log::critical("In RenderTask::get_uniform_vec3: No vec3 with name `" + uniform_name + "` registered");
            return {0, 0, 0};
        }
        return it->second;
    }

    Vector4f RenderTask::get_uniform_vec4(const std::string& uniform_name)
    {
        auto it = _internal->_vec4s->find(uniform_name);
        if (it == _internal->_vec4s->end())
        {
            log::critical("In RenderTask::get_uniform_vec4: No vec4 with name `" + uniform_name + "` registered");
            return {0, 0, 0, 0};
        }
        return it->second;
    }

    RGBA RenderTask::get_uniform_rgba(const std::string& uniform_name)
    {
        auto it = _internal->_vec4s->find(uniform_name);
        if (it == _internal->_vec4s->end())
        {
            log::critical("In RenderTask::get_uniform_rgba: No vec4 with name `" + uniform_name + "` registered");
            return {0, 0, 0, 0};
        }
        auto out = it->second;
        return RGBA(out.x, out.y, out.z, out.w);
    }

    HSVA RenderTask::get_uniform_hsva(const std::string& uniform_name)
    {
        auto it = _internal->_vec4s->find(uniform_name);
        if (it == _internal->_vec4s->end())
        {
            log::critical("In RenderTask::get_uniform_hsva: No vec4 with name `" + uniform_name + "` registered");
            return {0, 0, 0, 0};
        }
        auto out = it->second;
        return HSVA(out.x, out.y, out.z, out.w);
    }

    GLTransform RenderTask::get_uniform_transform(const std::string& uniform_name)
    {
        auto it = _internal->_transforms->find(uniform_name);
        if (it == _internal->_transforms->end())
        {
            log::critical("In RenderTask::get_uniform_transform: No mat4x4 with name `" + uniform_name + "` registered");
            return GLTransform();
        }
        return it->second;
    }

    RenderTask::operator GObject*() const
    {
        return G_OBJECT(_internal);
    }

    /*
    const Shape* RenderTask::get_shape() const
    {
        std::cerr << "In RenderTask::get_shape: TODO" << std::endl;
        return _shape;
    }

    const Shader* RenderTask::get_shader() const
    {
        return _shader == nullptr ? _internal->noop_shader : _shader;
    }

    const GLTransform* RenderTask::get_transform() const
    {
        return _transform == nullptr ? _internal->noop_transform : _transform;
    }
     */
}

#endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT