# mousetrap {#mainpage}

Welcome to the documentation of mousetrap, a GTK4-based GUI engine. 

This documentatino features an [installation guide](01_installation.md), [manual](02_signals.md) and [index of functions](./annotated.html). To access the codebase directly, please visit [mousetraps GitHub page](https://github.com/clemapfel/mousetrap) instead.


## Introduction

GTK4 is extremely powerful, well-optimized, and utterly inscrutable. Mousetrap aims to address this by wrapping most of GTK4
in an easy-to-use, easy-to-understand, shiny new coat. 

It adds fully abstracted OpenGL-based native rendering capabilities, as well as audio-playback, making mousetrap a fully features multi-media GUI engine.

Mousetrap empowers user to create applications and widgets within a few minutes, as opposed to days.

---

### Use Cases

Mousetrap is ideal for:
+ complex desktop applications for Windows, Mac, and/or Linux
+ applications for illustration / animation
+ 2D video games only controlled with a mouse, keyboard, and/or touchscreen

While currently unsuitable, future updates will make mousetrap a good use case for:
+ applications for people living with disabilities, such as impaired vision or limited fine-motor skills
+ complex text editing software, such as IDEs
+ gamepad-controlled 2D video games
+ applications requiring 3D graphics, such as a model viewer

Mousetrap is unsuitable for:
+ people already intimately familiar with GTK4 & GLib
+ applications intended for non-desktop machines, such as mobile phones, video game consoles, microcontrollers
+ low-resolution displays (< 400x300 px)
+ applications requiring a large number of widgets on screen (> 100 000, for example viewing a dataset with millions of entries)
+ applications requiring a refresh rate different from that of the monitor

---

### Planned Features

(In order of priority, from highest to lowest)

+ complete Julia binding
+ Expose css styling and custom, per-application themes
+ Make list-based widgets (`ListView`, `ColumnView`, `GridView`, etc.) searchable
+ Expose `GtkSettings` interface
+ Expose `GResource` bundling system
+ Allow video game controller input
+ Add accessibility features
+ Add 3D capabilities to `RenderArea`
+ Expose the full `GtkTextView` interface

---

### License

Mousetrap is licensed under [GNU Lesser General Public License 3.0](https://en.wikipedia.org/wiki/GNU_Lesser_General_Public_License). 

In plain language, this allows users to use mousetrap for any application, be it commercial or academic, open- or closed-source. If users directly modify mousetrap itself, that fork has to be open source. In practice, this means any GitHub fork of mousetrap needs to be public, though the rest of the application using that fork does not.

---

### Credits

Mousetrap was created and designed by [C. Cords](https://clemens-cords.com)
