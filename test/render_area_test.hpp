//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/28/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class RenderAreaTest : public Widget
    {
        private:
            RenderArea _area;

            Shape _shape;
            RenderTask _shape_task = RenderTask(_shape);

            MultisampledRenderTexture _render_texture;
            Shape _render_texture_shape;
            RenderTask _render_texture_task = RenderTask(_render_texture_shape);
            AspectFrame _aspect_frame = AspectFrame(1);
            Frame _frame;

            MotionEventController _motion_controller;
            Shape _motion_shape;
            RenderTask _motion_task = RenderTask(_motion_shape);

            Box _box = Box(Orientation::HORIZONTAL);

        public:
            RenderAreaTest()
            {
                _shape.as_circle({0, 0}, 0.75, 5);
                _shape.set_color(RGBA(1, 0, 1, 1));
                _shape_task = RenderTask(_shape);

                _render_texture_shape.as_rectangle({-1, -1}, {2, 2});
                _render_texture_shape.set_texture(&_render_texture);
                _render_texture_task = RenderTask(_render_texture_shape);

                _motion_shape.as_rectangle({0, 0}, {0.1, 0.1});//.as_circle({-1, -1}, 0.02 * 2, 32);
                _motion_shape.set_color(RGBA(1, 0, 1, 1));

                _area.connect_signal_resize([](RenderArea* area, int32_t w, int32_t h, RenderAreaTest* instance){
                    instance->_render_texture.create(w, h);
                }, this);

                static auto circle = Shape();
                circle.as_circle({0, 0}, 0.5, 16);
                _area.add_render_task(circle);

                /*
                _area.connect_signal_render([](RenderArea* area, GdkGLContext* context, RenderAreaTest* instance) -> bool {

                    instance->_render_texture.bind_as_rendertarget();

                    glClearColor(0, 0, 0, 0);
                    glClear(GL_COLOR_BUFFER_BIT);
                    instance->_shape_task.render();
                    instance->_motion_task.render();
                    glFlush();

                    instance->_render_texture.unbind_as_rendertarget();

                    glClearColor(0, 0, 0, 0);
                    glClear(GL_COLOR_BUFFER_BIT);
                    instance->_render_texture_task.render();
                    glFlush();

                    return false;
                }, this);
                 */

                _area.set_tick_callback([](FrameClock clock, RenderAreaTest* instance) -> TickCallbackResult{

                    instance->_shape.rotate(degrees(1), {0, 0});

                    static float hue = 0;
                    hue += 0.003;

                    size_t n_vertices = instance->_shape.get_n_vertices();
                    for (size_t i = 0; i < n_vertices; ++i)
                        instance->_shape.set_vertex_color(i, HSVA(glm::fract(hue + float(i) / n_vertices), 1, 1, 1));

                    instance->_area.queue_render();
                    return TickCallbackResult::CONTINUE;
                }, this);

                _area.add_controller(_motion_controller);
                _motion_controller.connect_signal_motion([](MotionEventController*, int x, int y, RenderAreaTest* instance){
                    instance->_motion_shape.set_centroid(instance->_area.to_gl_coordinates({x, y}));
                    instance->_area.queue_render();
                }, this);

                _motion_controller.connect_signal_motion_enter([](MotionEventController*, int, int, RenderAreaTest* instance){
                    instance->_motion_shape.set_is_visible(true);
                }, this);

                _motion_controller.connect_signal_motion_leave([](MotionEventController*, RenderAreaTest* instance){
                    instance->_motion_shape.set_is_visible(false);
                }, this);

                _area.set_expand(true);
                _frame.set_child(_area);
                _aspect_frame.set_child(_frame);
                _box.push_back(_aspect_frame);
                _box.set_cursor(CursorType::CROSSHAIR);
            }

            operator NativeWidget() const override
            {
                return _box.operator NativeWidget();
            }
    };
}
