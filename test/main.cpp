#include <mousetrap.hpp>
using namespace mousetrap;

#include <thread>
#include <chrono>

void task_cb(GObject* self, GAsyncResult*, void* data)
{
    std::cout << "task called" << std::endl;
    //g_application_run(G_APPLICATION(self), 0, nullptr);
}

void task_thread_cb(GTask* task, GObject* self, gpointer data, GCancellable*)
{
    std::cout << "thread called" << std::endl;
    //g_task_return_boolean(task, true);
}

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

    auto* t = new std::thread([&](){
        app.run();
    });

    std::this_thread::sleep_for(std::chrono::seconds(5));
}