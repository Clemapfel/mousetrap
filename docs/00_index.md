# mousetrap {#mainpage}

Welcome to the documentation of mousetrap, a GTK4-based GUI engine. This documentatino features a [manual](01_installation.md) and [index of functions](./annotated.html).

## Introduction

GTK4 is extremely powerful, well-optimized, and utterly inscrutable. Mousetrap aims to address this by wrapping most of GTK4
in an easy-to-use, easy-to-understand new coat. It adds fully abstracted native OpenGL capabilities and the abilitiy
to do audio playback, making mousetrap a fully capable multi-media GUI engine. Mousetrap empowers user to create applications and widgets within a few minutes, as opposed to days.

### Use Cases

Mousetrap is ideal for:
+ complex desktop applications for Windows, Mac, and/or Linux
+ graphical applications for illustration / animation
+ 2D video games only controlled with a mouse, keyboard, and/or touchscreen

While currently unsuitable, future updates will make mousetrap a good use case for:
+ applications for people living with disabilities, such as impaired vision or limited fine motor skills
+ complex text editing software, such as IDEs
+ gamepad-controlled 2d video games
+ static 3D displays, such as 3D plots or model viewers

Mousetrap is unsuitable for:
+ people already intimately familiar with GTK4 & GLib
+ applications intended for non-desktop machines, such as mobile phones, video game consoles, microcontrollers
+ low-resolution displays (< 400x300 px)
+ machines with no graphics card
+ machines with limited RAM (< 500mb)
+ applications requiring a large number of widgets on screen (> 100k, for example viewing a dataset with millions of entries)
+ applications requiring a refresh rate different from that of the monitor (for example, precise GUI-based physics simulations)

### Planned Features

(In order of priority, from highest to lowest)

+ complete Julia binding
+ Expose css styling and custom, per-application themes
+ Make list-based widgets (`ListView`, `ColumnView`, `GridView`, etc.) filterable, add searching functionality
+ Expose real-time GTK4 debugging features 
+ Expose `GtkSettings` interface
+ Allow video game controller input
+ Add accessibility features
+ Add 3D capabilities to `RenderArea`
+ + Expose the full `GtkTextView` interface

### License

`mousetrap` is licensed under [GNU Lesser General Public License 3.0](https://en.wikipedia.org/wiki/GNU_Lesser_General_Public_License). 

This means projects are free to use mousetrap for any purpose, including commercial, regardless of the user's product's license. However, if mousetrap itself is modified, that part of the library has to be published as open source. In practice, this means any fork of mousetraps should be public on GitHub, but the rest of your application or library is unaffected and may be closed-source, making it fully available to closed-source, for-profit projects.

