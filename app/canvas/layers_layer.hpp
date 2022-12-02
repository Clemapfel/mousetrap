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
            _current_frame = state::current_frame;
            _n_layers = state::layers.size();
            _canvas_size = *_owner->_canvas_size;

            _area.clear_render_tasks();

            while (_layer_shapes.size() < _n_layers)
                _layer_shapes.emplace_back(new Shape());

            float width = state::layer_resolution.x / _canvas_size.x;
            float height = state::layer_resolution.y / _canvas_size.y;

            for (size_t layer_i = 0; layer_i < _n_layers; ++layer_i)
            {
                auto* shape = _layer_shapes.at(layer_i);

                shape->as_rectangle(
                    {0.5 - width / 2, 0.5 - height / 2},
                    {width, height}
                );
                shape->set_texture(state::layers.at(layer_i)->frames.at(state::current_frame).texture);

                auto task = RenderTask(
                    _layer_shapes.at(layer_i),
                    nullptr,
                    _owner->_transform,
                    state::layers.at(layer_i)->blend_mode
                );

                _area.add_render_task(task);
            }
        }

        if (_current_frame != state::current_frame and _area.get_is_realized())
        {
            _current_frame = state::current_frame;
            for (size_t layer_i = 0; layer_i < _n_layers; ++layer_i)
                _layer_shapes.at(layer_i)->set_texture(
                    state::layers.at(layer_i)->frames.at(state::current_frame).texture
                );
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
        instance->_canvas_size = *instance->_owner->_canvas_size;

        for (size_t i = 0; i < instance->_n_layers; ++i)
            instance->_layer_shapes.emplace_back(new Shape());

        float width = state::layer_resolution.x / instance->_canvas_size.x;
        float height = state::layer_resolution.y / instance->_canvas_size.y;

        for (size_t layer_i = 0; layer_i < instance->_n_layers; ++layer_i)
        {
            auto& layer = state::layers.at(layer_i);
            auto& shape = instance->_layer_shapes.at(layer_i);

            shape->as_rectangle(
                    {0.5 - width / 2, 0.5 - height / 2},
                    {width, height}
            );
            //shape->set_texture(layer->frames.at(instance->_current_frame).texture);
        }

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

        area->queue_render();
    }

    Canvas::LayersLayer::reformat();

    void Canvas::LayersLayer::on_resize(GLArea* area, int w, int h, LayersLayer* instance)
    {
        instance->_canvas_size = {w, h};
        instance->reformat();
    }
}