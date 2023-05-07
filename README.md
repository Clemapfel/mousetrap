# mousetrap: Easy-to-Use GUI Library for C++ & Julia

GTK4 is extremely powerful, well-optimized, and utterly inscrutable. Mousetrap aims to address this by wrapping most of GTK4
in an easy-to-use, easy-to-understand new coat. Mousetrap empowers user to create applications and widgets within a few minutes, as opposed requiring weeks of studying and days of implementation.

## Features
+ For Linux, MacOS & Windows
+ C++-Side Garbage-Collection for Widgets
+ Fully abstracted, SFML-like native Rendering
+ Powerful Image Processing Capabilities
+ Events TODO

## Advantages over GTK
+ Allow connecting C++ Lambdas to Signals
+ Every Widget, no matter how complex, is created with a One-Liner
+ Abstracted OpenGL interface
+ Audio Playback
+ Step-by-Step Manual and Introduction, written for Beginners

## Showcase

### Hello World

To create a new application, only the following is necessary:

```cpp
int main()
{
    auto app = Application("hello.world");
    app.connect_signal_activate([](Application*)
    {
        auto window = Window(app);
        window.present()
    });
    
    return app.run();
}
```
---

#### Creating a TreeView

In GTK4, creating a `GtkTreeView` will take dozens of lines and will use up to 6 different classes. In mousetrap, it just takes one:

```cpp
auto tree_view = TreeListView();
tree_view.push_back(Label("new element"));
```
\todo figure

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

Initialization of the OpenGL state and rendering is fully automatic, you just need to pick which shape you want to render, that's it:

```cpp
auto canvas = RenderArea();
auto circle = Shape();
circle.as_circle({0, 0}, 0.5);
canvas.add_render_task(circle);
```
\todo figure

The following shapes are already implemented: point, triangle, rectangle, circle, ellipse, line, line-strap, convex polygon, rectangle frame, circular ring, elliptic ring, wireframe

Also supported: Shaders, Transforms, Textures, RenderTexture, Multisample Anti Aliasing

---

#### Adding a Widget to a Menu

```cpp
```

---

#### Playing Music

```cpp
auto music = Music();
music.create_from_file("example.wav");
music.play();
```
---

## Documentation

Documentation is available [here](https://clemens-cords.com/mousetrap).

A minimal "hello world" project can be found in the `[example](./example)` folder here on GitHub.