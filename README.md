# Mousetrap: Gtk4-based GUI Library

> **NOTE**: This repo is under rapid active development and not intended to be forked or worked with by third parties, as of August 2022

## Features
+ OpenGL-based canvas, replaces Cairo entirely resulting in improved performance and hardware-accelerated rendering of widgets
+ Shapes, Shaders, GPU- and CPU-side Texture Manipulation
+ Abstracts all [GIO](https://docs.gtk.org/gio/) functionality and C-style memory handling 

All widgets capable of having children can only have any other widgets as children. This means `ListView`, `GridView`, `TreeView` 
  are completely generic out-of-the-box, as the underlying models wraps all objects, not just a specific type. This makes setting them up
 a one-liner:

```cpp
auto list_view = TreeListView();

auto label = Label("test");
auto button = Button();
auto other_list_view = TreeListView();

list_view.push_back(label);
list_view.push_back(button);
list_view.push_back(other_list_view);
```

+ Allow signals to trigger runtime-lambdas, which is impossible using just the Gtk4 C-Library

## Dependencies
+ CMake 3.12+ including PkgConfig
+ OpenGL 3.2+
+ glm
+ GTK4
