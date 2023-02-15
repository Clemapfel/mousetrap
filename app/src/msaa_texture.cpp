//
// Created by clem on 2/15/23.
//

#include <app/msaa_texture.hpp>

namespace mousetrap
{
    MSAATexture::MSAATexture()
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
        _area.connect_signal_render(on_area_render, this);
        _area.set_size_request({800, 800});
        _area.add_tick_callback([](FrameClock clock, MSAATexture* instance) {

            if (not instance->_area.get_is_realized())
                return true;

            instance->_transform->rotate(degrees(0.1), {0, 0});
            instance->_area.queue_render();
            return true;
        }, this);
    }

    MSAATexture::operator Widget*()
    {
        return &_area;
    }

    void MSAATexture::on_area_realize(Widget* widget, MSAATexture* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        auto as_ellipse = [](Vector2f center, float x_radius, float y_radius, size_t n_vertices){

            const float step = 360.f / n_vertices;

            std::vector<Vector2f> out;

            for (float angle = 0; angle < 360; angle += step)
            {
                auto as_radians = angle * M_PI / 180.f;
                out.emplace_back(
                        center.x + cos(as_radians) * x_radius,
                        center.y + sin(as_radians) * y_radius
                );
            }

            return out;
        };

        instance->_shape = new Shape();
        instance->_shape->as_wireframe(as_ellipse({0.5, 0.5}, 0.4, 0.4, 11));
        instance->_shape->set_color(RGBA(1, 0, 1, 1));
        instance->_shape_task = new RenderTask(instance->_shape, nullptr, instance->_transform);

        area->queue_render();
    }

    void MSAATexture::on_area_resize(GLArea* area, int w, int h, MSAATexture* instance)
    {
        instance->_canvas_size = {w, h};
        area->queue_render();
    }

    bool MSAATexture::on_area_render(GLArea* area, GdkGLContext*, MSAATexture* instance)
    {
        area->make_current();

        static GLNativeHandle before = [](){
            GLint before = 0;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &before);
            return before;
        }();

        glEnable(GL_BLEND);
        set_current_blend_mode(BlendMode::NORMAL);

        {
            glClearColor(0, 0, 0, 0.6);
            glClear(GL_COLOR_BUFFER_BIT);
            instance->_shape_task->render();
            glFlush();
        }

        return true;
    }
}
