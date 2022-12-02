// 
// Copyright 2022 Clemens Cords
// Created on 10/31/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Canvas::LayersLayer::LayersLayer(Canvas* owner)
            : CanvasLayer(owner)
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);
    }

    Canvas::LayersLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::LayersLayer::update()
    {
        if (_n_layers != state::layers.size() and _area.get_is_realized())
        {
            _n_layers = state::layers.size();
            _current_frame = state::current_frame;
            _layer_resolution = state::layer_resolution;

            _area.make_current();
            while (_layer_shapes.size() < _n_layers)
                _layer_shapes.emplace_back(new Shape());

            reformat();
        }

        if (_layer_resolution != state::layer_resolution and _area.get_is_realized())
        {
            _current_frame = state::current_frame;
            _layer_resolution = state::layer_resolution;

            reformat();
        }

        if (_current_frame != state::current_frame and _area.get_is_realized())
        {
            _current_frame = state::current_frame;

            for (size_t layer_i = 0; layer_i < _n_layers; ++layer_i)
            {
                auto& layer = state::layers.at(layer_i);
                auto& shape = _layer_shapes.at(layer_i);
                shape->set_texture(layer->frames.at(_current_frame).texture);
            }
        }

        _area.queue_render();
    }

    Canvas::LayersLayer::~LayersLayer()
    {
        for (auto* shape : _layer_shapes)
            delete shape;
    }

    void Canvas::LayersLayer::on_realize(Widget* widget, LayersLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_current_frame = state::current_frame;
        instance->_n_layers = state::layers.size();
        instance->_layer_resolution = state::layer_resolution;

        for (size_t i = 0; i < instance->_n_layers; ++i)
            auto* shape = instance->_layer_shapes.emplace_back(new Shape());

        instance->reformat();

        instance->_area.clear_render_tasks();
        for (size_t layer_i = 0; layer_i < instance->_n_layers; ++layer_i)
        {
            auto task = RenderTask(
                instance->_layer_shapes.at(layer_i),
                nullptr,
                instance->_owner->_transform,
                state::layers.at(layer_i)->blend_mode
            );

            instance->_area.add_render_task(task);
        }

        instance->_area.queue_render();
    }

    void Canvas::LayersLayer::reformat()
    {
        float width = _layer_resolution.x / _canvas_size.x;
        float height = _layer_resolution.y / _canvas_size.y;

        for (size_t layer_i = 0; layer_i < _n_layers; ++layer_i)
        {
            auto& layer = state::layers.at(layer_i);
            auto& shape = _layer_shapes.at(layer_i);

            shape->as_rectangle(
                {0.5 - width / 2, 0.5 - height / 2},
                {width, height}
            );
            shape->set_texture(layer->frames.at(_current_frame).texture);
        }
    }

    void Canvas::LayersLayer::on_resize(GLArea* area, int w, int h, LayersLayer* instance)
    {
        instance->_canvas_size = {w, h};
        instance->reformat();
        instance->_area.queue_render();
    }
}