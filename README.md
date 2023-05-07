# Mousetrap: Gtk4-Based GUI Library

> **NOTE**: This repo is under rapid active development and not intended to be forked or worked with by third parties

# Design Goals

## Widgets can only contain Widgets

If a widget is a container, only other widgets can be added to it. For example, we cannot add a string to a list, instead,
we have to first construct a label for that string, then add that label:

```cpp
auto text = "text";
auto label = Label(text);

auto list = ListView(Orientation::HORIZONTAL);
list.push_back(&label);
```

This abstracts away the [factory-patter](https://docs.gtk.org/gtk4/class.ListItemFactory.html) used by many of the more
complex GTK widgets such as `ListView`, `GridView` and `ColumnView`, making them an easy 1-line initializatino.

It also allows for greated flexiblity, as, similar to a `Box`, any type of object can be added to the container. 
Adding an `ImageDisplay` to a `GridView` is the same 1-line process as adding another `GridView` to a `GridView`. 

Furthermore mutating widgets that area already inserted is made easier this way. If we create a `ListView` containing
10 `Label` and we want to change the text of those labels, we simple call `set_text` on all the `Label` instances we 
created ourself, as opposed to either querying the container widget or clearing it and added 10 new strings.

