#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    mousetrap::FORCE_GL_DISABLED = true;

    // declare application
    auto app = Application("test.app");

    // initialization
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto area = RenderArea(AntiAliasingQuality::GOOD);
        auto shape = Shape::Rectangle({-0.5, 0.5}, {1, 1});
        auto shader = Shader();
        auto blend_mode = BlendMode::MAX;
        auto transform = GLTransform();
        auto task = RenderTask(shape, &shader, transform, blend_mode);
        area.add_render_task(task);

        window.set_child(area);
        window.present();
    });

    // start main loop
    return app.run();
}