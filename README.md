# Mousetrap: Gtk4-based GUI Library

> **NOTE**: This repo is under rapid active development and not intended to be forked or worked with by third parties, as of October 2022

## Design Principles

### All Widgets Only Contain Other Widgets

Any container, be it Box, List, TreeView, GridView, etc. only contains other widgets. This is in contrast to the inconsistent 
Factory / Model-View design of, for example, `GtkTreeView`. Instead of inserting widgets into `GtkTreeView`, Gtk4 requires users
to construct a model holding non-widget items, specify a factory type, a render type for each type of item, 
then let `GtkTreeView` construct the widgets from  that model. In `mousetrap`, you can simply do:

```cpp
auto tree_view = ListTreeView();
Widget some_widget = // ...
tree_view.push_back(some_widget);
```

This is true for all renderable container / layout manager objects in `mousetrap`.

### 

## Dependencies
+ CMake 3.12+ including PkgConfig
+ OpenGL 3.2+
+ glm
+ GTK4
