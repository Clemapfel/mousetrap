// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/shape.hpp>
#include <include/shader.hpp>
#include <include/transform.hpp>

#include <map>

namespace rat
{
    class RenderTask
    {
        public:
            RenderTask(Shape*, Shader* = nullptr, Transform* = nullptr);

            void register_float(const std::string& uniform_name, float*);
            void register_int(const std::string& uniform_name, int*);
            void register_vec2(const std::string& uniform_name, Vector2f*);
            void register_vec3(const std::string& uniform_name, Vector3f*);
            void register_vec4(const std::string& uniform_name, Vector4f*);
            void register_transform(const std::string& uniform_name, Transform*);
            
            void render();

            Shape* get_shape();
            Shader* get_shader();
            Transform* get_transform();
            
        private:
            Shape* _shape;
            Shader* _shader;
            Transform* _transform;
            
            static inline Shader* noop_shader = nullptr;
            static inline Transform* noop_transform = nullptr;
            
            std::map<std::string, float*> _floats;
            std::map<std::string, int*> _ints;
            std::map<std::string, Vector2f*> _vec2s;
            std::map<std::string, Vector3f*> _vec3s;
            std::map<std::string, Vector4f*> _vec4s;
            std::map<std::string, Transform*> _transforms;
    };
}

// #####################################################################################################

namespace rat
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