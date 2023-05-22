# mousetrap: Easy-to-Use GUI Library for C++ & Julia

GTK4 is extremely powerful, well-optimized, and utterly inscrutable. Mousetrap aims to address this by wrapping most of GTK4
in an easy-to-use, easy-to-understand new coat. Mousetrap empowers user to create applications and widgets within a few minutes, as opposed requiring weeks of studying and days of implementation.

## Features
+ Builds Apps for Linux, MacOS & Windows
+ C++-Side Garbage-Collection for Widgets
+ Fully abstracted, SFML-like native Rendering
+ Powerful Image Processing Capabilities

## Advantages over GTK
+ Allow connecting C++ Lambdas to Signals
+ Every Widget, no matter how complex, is initialized with a signle line
+ Abstracted OpenGL interface
+ Audio Playback
+ Step-by-Step Manual and Introduction, written for Beginners

## Showcase

### Hello World

To create a new application, only the following is necessary:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("hello.world");
    app.connect_signal_activate([](Application&)
    {
        auto window = Window(app);
        window.present()
    });
    
    return app.run();
}
```
---

#### Creating a Nested List

In GTK4, creating a `GtkTreeView` will take dozens of lines of code and more than 6 separator classes from GTK, Gio and Glib. In mousetrap, it can be initialized with a single line:

```cpp
auto tree_view = TreeListView();
tree_view.push_back(Label("new element"));
```
---

#### Adding a Keyboard Shortcut Action

```cpp
auto shortcut_action = Action("action.keyboard_shortcut_example", app);
shortcut_action.set_function([](Action*){
    std::cout << "shortcut called" << std::endl;
});
shortcut_action.set_shortcut("<Control>t");
```
---

#### OpenGL Rendering 

Rendering native shapes is fully abstracted, users will not have to call a single line of native OpenGL code to render custom shapes:

```cpp
auto canvas = RenderArea();
auto circle = Shape::Circle({0, 0}, 0.5);
circle.as_circle({0, 0}, 0.5);
canvas.add_render_task(circle);
```
![](docs/resources/render_area_circle_normalized.png)

Mousetrap offers [a huge variety of pre-defined shapes](docs/09_native_rendering.md#shapes), as well as fully abstracted object to create and compile shaders, apply spatial transforms, load and display texture, and even render to a anti-aliased texture.

---

#### Playing Music

```cpp
auto music = Music();
music.create_from_file("example.wav");
music.play();
```
---


This behavior would cause a deadlock in native GTK4.

## Documentation

Documentation is available [here](https://clemens-cords.com/mousetrap).

A minimal "hello world" project can be found in the `[example](./example)` folder here on GitHub.