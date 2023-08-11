# mousetrap {#mainpage}

Welcome to the document of mousetrap, the stand-alone C++ component of the [mousetrap.jl GUI engine](https://github.com/Clemapfel/mousetrap.jl). If you are here because you want to use mousetrap in Julia, you are in the wrong place. Visit the [mousetrap.jl documentation](https://clemens-cords.com/mousetrap) instead.

This documentation is a stub, [most of the documentation efforts were focused on the Julia component](https://clemens-cords.com/mousetrap). This also includes testing, which is preformed by having the Julia wrapper call downstream C++ functions from Julia, as opposed to doing the testing C++-side.

---

## Installation

See [here](https://github.com/Clemapfel/mousetrap#dependencies) for how to install mousetrap.

---

## Credits

Both this C++ component, as well as the C++-Julia interface, and the Julia component of mousetrap were created by [C. Cords](https://clemens-cords.com).

C++ documentation was generated using [doxygen](https://www.doxygen.nl/), styled with a custom [doxygen-awesome theme](https://jothepro.github.io/doxygen-awesome-css/). 

---

## Usage

Most likely, you're here because you want to contribute to `mousetrap.jl` by modifying its back-end, so this page will focus on how to use the C++ component **after already having read the entire mousetrap.jl manual**.

Syntax is directly translatable as one would expect:

```julia
# in Julia:
button = Button()
set_child!(button, Label("Label"))
connect_signal_clicked!(button) do self::Button
    # ...
end
```

```cpp
// in C++:
auto button = Button();
button.set_child(Label("Label"));
button.connect_signal_clicked([](Button& self){
    // ...
});
```

With the exception that some signals have additional unused arguments or slightly different signatures. Furthermore, C++ mousetrap does not have Julia mousetraps `main`, in C++ we need to connect to the apps signal and `run` it manually.

## Examples

[In lieu of a proper manual](https://clemens-cords.com/mousetrap), examples for how to do common tasks will be given in this section.

### Hello World

Creating a window that display the label "Hello World".

```cpp
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        window.set_child(Label("Hello World"));
        window.present();
    });
    return app.run();
}
```

See the [GitHub README](https://github.com/Clemapfel/mousetrap#readme) for an example `CMakeLists.txt` to build this an all other examples in this section.

### Action

Triggering an `Action` using a `Button` or a keyboard shortcut.

```cpp
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto action = Action("example.action", app);
        action.set_function([](Action&){
            std::cout << "action called" << std::endl;
        });

        auto button = Button();
        button.set_child(Label("Click for Action"));
        button.set_action(action);
        
        action.add_shortcut("<Control>space");
        window.set_listens_for_shortcut_actions(action);

        window.set_child(button);
        window.present();
    });
    return app.run();
}
```

### Signals

Create a box of 3 widgets indicating a boolean state. Their states are kept in synch by connecting to each widgets signal, then setting the other 2 widgets states from within that signal handler.

Signals need to be blocked at strategic times to prevent an infinite loop.

```cpp
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto vbox = Box(Orientation::VERTICAL);

        static auto toggle_button = ToggleButton();
        static auto light_switch = Switch();
        static auto check_button = CheckButton();
        
        toggle_button.connect_signal_toggled([](ToggleButton& self){
            auto state = self.get_is_active();
            self.set_signal_toggled_blocked(true);
            light_switch.set_is_active(state);
            check_button.set_state(state ? CheckButtonState::ACTIVE : CheckButtonState::INACTIVE);
            self.set_signal_toggled_blocked(false);
        });

        light_switch.connect_signal_switched([](Switch& self, void*){
            auto state = self.get_is_active();
            self.set_signal_switched_blocked(true);
            toggle_button.set_is_active(state);
            check_button.set_state(state ? CheckButtonState::ACTIVE : CheckButtonState::INACTIVE);
            self.set_signal_switched_blocked(false);
        });

        check_button.connect_signal_toggled([](CheckButton& self){
            auto state = self.get_is_active();
            self.set_signal_toggled_blocked(true);
            toggle_button.set_is_active(state);
            light_switch.set_is_active(state);
            self.set_signal_toggled_blocked(false);
        });

        // layout
        
        for (auto* widget : std::vector<Widget*>{
            &toggle_button, &light_switch, &check_button
        })
        {
            widget->set_expand(false);
            widget->set_margin(10);
            widget->set_alignment(Alignment::CENTER);
        }

        vbox.push_back(toggle_button);
        vbox.push_back(light_switch);
        vbox.push_back(check_button);

        vbox.set_margin(10);
        window.set_child(vbox);
        window.present();
    });
    return app.run();
}
```

### AlertDialog

Create a `Window` with a `Button` that, when clicked, shows an `AlertDialog` the user has to dismiss.

```cpp
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        static auto dialog = AlertDialog("Is this a Dialog?");
        dialog.add_button("Yes");
        dialog.add_button("No");

        dialog.on_selection([&](AlertDialog& self, int button_id){
            std::cout << "User clicked: " << self.get_button_label(button_id) << std::endl;
        });

        auto button = Button();
        button.set_child(Label("Show Dialog"));
        button.connect_signal_clicked([](Button&){
            dialog.present();
        });

        window.set_child(button);
        window.present();
    });
    return app.run();
}
```

### MotionController

Create an area that queries the current cursor position, moving updating a label as the cursor moves around.

```cpp
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        static auto label = Label("(0, 0)");
        static auto fixed = Fixed();
        fixed.add_child(label, {50, 50});

        auto motion_controller = MotionEventController();
        motion_controller.connect_signal_motion([](MotionEventController&, int x, int y)
        {
            auto sstream = std::stringstream();
            sstream << "(" << x << ", " << y << ")";
            label.set_text(sstream.str());
            fixed.set_child_position(label, {x, y});
        });
        fixed.add_controller(motion_controller);
        fixed.set_cursor(CursorType::CROSSHAIR);

        auto background = Separator();
        auto background_frame = Frame();
        background_frame.set_child(background);
        background_frame.set_margin(10);

        auto overlay = Overlay();
        overlay.set_child(background_frame);
        overlay.add_overlay(fixed);

        window.set_child(overlay);
        window.present();
    });
    return app.run();
}
```

### Image

Fill a new image with a hue gradient, then display it as a widget.

```cpp
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto image = Image();
        image.create(300, 300, RGBA(0, 0, 0, 0));

        auto hue_step = 1.f / 300;
        for (uint64_t i = 0; i < image.get_size().x; ++i)
            for (uint64_t j = 0; j < image.get_size().y; ++j)
                image.set_pixel(i, j, HSVA(i * hue_step, 1, 1, 1));

        auto image_view = ImageDisplay();
        image_view.create_from_image(image);
        window.set_child(image_view);
        window.present();
    });
    return app.run();
}
```

### ColumnView

Create a 3x2 table filled with random widgets.

```cpp

using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto column_view = ColumnView();
        column_view.push_back_column("Column 01");
        column_view.push_back_column("Column 02");
        column_view.push_back_column("Column 03");

        column_view.push_back_row(Entry(), Button(), Label("Row1"));
        column_view.push_back_row(Separator(), CheckButton(), Switch());
        
        window.set_child(column_view);
        window.present();
    });
    return app.run();
}
```

### ListView

Fill a vertical list with random widgets, then connect to is selection model to monitor user selection:

```cpp
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto root = ListView(Orientation::VERTICAL, SelectionMode::SINGLE);

        // root list
        root.push_back(Entry());
        auto item_02_it = root.push_back(Label("Collapsible Item"));
        root.push_back(Label("Not Collapsible Item"));

        // nested list, children of `Collapsible Item` because we used `item_02_it` instead of `nullptr`
        root.push_back(Button(), item_02_it);
        root.push_back(Entry(), item_02_it);
        root.push_back(Switch(), item_02_it);

        // monitor when selection changed
        root.get_selection_model()->connect_signal_selection_changed([](SelectionModel&, uint64_t item_index, uint64_t item_count){
            std::cout << "Selected item is now: " << item_index << std::endl;
        });

        window.set_child(root);
        window.present();
    });
    return app.run();
}
```

### Menus

Create a menu that can contain 4 different kind of items.

```cpp

using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        
        auto model = MenuModel();

        // menu item type 1: Action

        auto menu_item_action = Action("example.menu_item", app);
        menu_item_action.set_function([](Action&){
            std::cout << "example.menu_item called" << std::endl;
        });

        model.add_action("Action Item", menu_item_action);

        // menu item type 2: Widget

        auto menu_item_widget = Box(Orientation::HORIZONTAL);
        menu_item_widget.push_back(Label("Enter Text: "));
        menu_item_widget.push_back(Entry());
        menu_item_widget.set_spacing(10);
        menu_item_widget.set_margin(10);

        model.add_widget(menu_item_widget);

        // menu item type 3: Submenu

        auto submenu_model = MenuModel();
        submenu_model.add_widget(Label("Submenu Item"));

        model.add_submenu("Submenu", submenu_model);

        // menu item type 4: Section

        auto section_dummy_action = Action("example.section_dummy", app);
        section_dummy_action.set_function([](Action&){});

        auto section_model = MenuModel();
        section_model.add_action("Section Item # 1", section_dummy_action);
        section_model.add_action("Section Item # 2", section_dummy_action);
        section_model.add_action("Section Item # 3", section_dummy_action);

        model.add_section("Section", section_model);

        // view, displays the menu

        auto model_view = PopoverMenu(model);
        auto button = PopoverButton(model_view);
        window.set_child(button);
        window.present();
    });
    return app.run();
}
```

### Revealer

Create a `Button` that, when clicked, plays an animation to show another widget.

```cpp
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        
        auto image = Image(150, 150, RGBA(1, 0, 1, 1));
        auto image_display = ImageDisplay();
        image_display.create_from_image(image);
        image_display.set_expand(true);
        image_display.set_size_request({0, 100});

        static auto revealer = Revealer();
        revealer.set_child(image_display);
        revealer.set_transition_duration(seconds(1.0));
        revealer.set_transition_type(RevealerTransitionType::SLIDE_DOWN);
        revealer.set_is_revealed(false);
        revealer.set_expand_vertically(true);

        auto button = Button();
        button.set_child(Label("Click to Reveal"));
        button.set_expand_vertically(false);
        button.connect_signal_clicked([](Button& button){
            revealer.set_is_revealed(not revealer.get_is_revealed());
        });

        auto content = Box(Orientation::VERTICAL);
        content.push_back(button);
        content.push_back(revealer);

        window.set_child(content);
        window.present();
    });
    return app.run();
}
```

### RenderArea

Render a rectangle using OpenGL.

```cpp
using namespace mousetrap;
int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);

        auto canvas = RenderArea();
        auto shape = Shape::Rectangle({-0.5, 0.5}, {1, 1});
        shape.set_vertex_color(0, HSVA(0.1, 1, 1, 1));
        shape.set_vertex_color(1, HSVA(0.3, 1, 1, 1));
        shape.set_vertex_color(2, HSVA(0.6, 1, 1, 1));
        shape.set_vertex_color(3, HSVA(0.9, 1, 1, 1));

        auto task = RenderTask(shape);
        canvas.add_render_task(task);

        // force canvas to always be square
        auto frame = AspectFrame(1.0);
        frame.set_child(canvas);
        frame.set_size_request({150, 150});
        window.set_child(frame);
        window.present();
    });
    return app.run();
}
```

