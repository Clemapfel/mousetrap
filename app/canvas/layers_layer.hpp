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

    void Canvas::LayersLayer::refresh()
    {
        _area.queue_render();
    }

    Canvas::LayersLayer::~LayersLayer()
    {
        for (auto* shape : _layer_shapes)
            delete shape;

        delete _canvas_size;
    }

    void Canvas::LayersLayer::on_realize(Widget* widget, LayersLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        for (size_t i = 0; i < state::layers.size(); ++i)
            instance->_layer_shapes.emplace_back(new Shape());

        instance->reformat();
        instance->_area.clear_render_tasks();

        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
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

    void Canvas::LayersLayer::on_resize(GLArea* area, int w, int h, LayersLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->reformat();
    }

    void Canvas::LayersLayer::reformat()
    {
        float width = state::layer_resolution.x / _canvas_size->x;
        float height = state::layer_resolution.y / _canvas_size->y;

        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            auto& layer = state::layers.at(layer_i);
            auto& shape = _layer_shapes.at(layer_i);

            shape->as_rectangle(
                {0.5 - width / 2, 0.5 - height / 2},
                {width, height}
            );
            shape->set_texture(layer->frames.at(state::current_frame).texture);
        }

        _area.queue_render();
    }
}