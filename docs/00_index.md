# mousetrap {#mainpage}

## Introduction

GTK4 is extremely powerful, well-optimized, and utterly inscrutable. Mousetrap aims to address this wrapping most of GTK4
in an easy-to-use, easy-to-understand, comfortable new coat. It adds fully abstracted native OpenGL capabilities, and the abilitiy
to do audio playback, making mousetrap a fully capable multi-media GUI engine, empowering users to create applications within a 
few minutes, as opposed to days.

Mousetrap features a step-by-step manual introducing readers to GUI programming, installation and troubleshooting guide, 
and full index of functions/classes - all written by a human.

Mousetrap is written in C++, but provides a first-party Julia binding, bringing GUI programming to the Julia environment.

## Hello World

Mousetrap was born out of the desire for GTK4 functionality to be available in a clean, expressive, easy-to-learn library. Things that 
would take dozens of lines in native GTK, can be accomplished with a single one-liner in mousetrap. 

Creating an app, with a window displaying the message "Hello World", can be accomplished like so:

\cpp_code_begin
```cpp
#include <include/mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("app.hello_world");
    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);
        auto label = Label("Hello World");
        window.set_child(label);
        window.present();
    });
  
    return app.run();
}
```
\cpp_code_end

\julia_code_begin
```julia
# \todo
```
\julia_code_end

### Use Cases

Mousetrap is ideal for:
+ complex desktop applications for Windows, Mac, and/or Linux
+ graphical applications for illustration / animation
+ 2D video games, controlled with a mouse, keyboard, and/or touchscreen
+ applications interfacing with local hardware, such as the disk or other physically connected devices
+ people with no experience developing GUIs, but moderate knowledge about C++ (or Julia, if the Julia binding is used) 

While currently unsuitable, future updates will make mousetrap a good use case for:
+ applications for less-able people, such as people living with impaired vision or limited fine motor skills
+ complex text editing software, such as IDEs
+ 2d video games controlled with joysticks
+ static 3D displays, such as 3D plots or model viewers
+ audio-editing or synthesizing software
+ internet-interfacing applications, such as matchmade video games or control interfaces for network-connected hardware 

Mousetrap is unsuitable for:
+ people already intimately familiar with GTK4 & GLib
+ people who have no prior knowledge on C++ (or Julia, if the Julia binding is used)
+ non-desktop machines, such as mobile phones, video game consoles, microcontrollers
+ low-resolution displays (< 400x300 px)
+ machines with no graphics card
+ machines with limited RAM (< 500mb)
+ graphically taxing 3D games
+ applications requiring a large number of widgets (> 100k, for example viewing a dataset with millions of entires)
+ applications requiring a refresh rate different than that of the monitor (for example precise GUI-based physics simulations)

### Planned Features

(In order of priority, from highest to lowest)

+ Make list-based widgets (`ListView`, `ColumnView`, `GridView`, etc.) filterable and add searching functionality
+ Expose `GtkSettings` interface
+ Allow SFML-based video game controller input
+ Expose the full `GtkTextView` interface
+ Add 3D capabilities to `RenderArea`
+ Expose the GLib-networking Library

### Credits

mousetrap was created by wrapping:
+ [GTK4](https://docs.gtk.org/gtk4/), created by GNOME 
+ [SFML Audio](https://github.com/SFML/SFML)
+ [OpenGL](https://www.opengl.org/), [OpenGL Mathematics](https://github.com/g-truc/glm) and [GLEW](https://glew.sourceforge.net/)

Software design and implementation by [C. Cords](https://www.clemens-cords.com)

