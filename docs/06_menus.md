# Chapter 6: Menus

In this chapter, we will learn:
+ How to create complex, nested menus
+ How to display menus using `PopoverMenu` and `MenuBar`
+ Best-practice style guide for menus

---

In the [chapter on actions](./03_actions) we learned that we can trigger an action using [`Button`](@ref), by assigning an action to it using [`set_action!`](@ref).
this works if we want to have a GUI element that has a few actions, in reality, an application can have hundreds of different actions. Asking users to trigger these using
an equal number of buttons would be unsustainable. For situations like these, we should instead turn to **menus**.

## MenuModel Architecture

In mousetrap, menus follows the [model-view architectural pattern](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller). To create a menu, we need
the menus model, which holds information about how the menu is structured, along with a view, which takes the models and transforms it into an interactable
widget users can manipulate.

!!! Note "Running Snippets from this Section"

    To follow along with code snippets from this section, we can use the following `.jl` file:
    ```julia
    using mousetrap
    main() do app::Application
        window = Window(app)

        # create the model
        model = MenuModel()

        # snippet here

        # create the view
        popover_menu = PopoverMenu(model)
        popover_button = PopoverButton()
        set_popover_menu!(popover_button)

        # add view to window 
        set_child!(window, popover_button)
        present!(window)
    end
    ```

## Menu Items

[`MenuModel`](@ref), the model component of mousetrap menus, is a list of **menu items**, in a specific order. If item A is added before item B at runtime, then A will appear above item B. There are multiple different types of menu items, all with their own purpose. The function we choose to add an item to the model determines the items type. There are 4 types of menu items, which we will go over in this section.

### Item Type #1: Action

The first and most simple item type is called "action". Added via [`add_action!`](@ref), which takes both a label and an [`Action`](@ref) instance, this item is a simple button with text. If the user clicks the button, the action is executed.

Similar to `Button`, if the action is disabled (via [`set_enabled`](@ref)), the menu item will appear "greyed out" and cannot be activated.

```cpp
model.add_action("Type 01: Action", action);
```

\image html menus_01.png

### Item Type #2: Widgets

Secondly, we have perhaps the most powerful type of item: A custom widget. We add an item of this type using `add_widget`, which only takes a single argument, the widget itself. This widget can be arbitrarily complex, though it is usually not adviced to insert an entire `ColumnView` into a tiny menu. Good-practic examples include `Button`, `Entry`, `CheckButton`, `ToggleButton`, `Switch` and `SpinButton`, all of which are interactable.

It's possible to instert a non-interactable widget, such as `ImageDisplay`, but this will usually make for a confusing user experience, as user will expect any part of a menu to be interactable.

```cpp
static auto entry = Entry();
entry.set_text("Type 02: Widget");
entry.connect_signal_activate([](Entry*){
    // do something here
});
model.add_widget(entry);
```

\image html menus_02.png

Widgets are the most flexible of menu items. They should be used with caution, and only if absolutely necessary. It is often better to create an action that opens a separate dialog which contains the widget, as opposed to directly adding the widget to the menu.

### Item Type #3: Submenu

`MenuModel` can be **nested**, which means we can insert a `MenuModel` into another `MenuModel`. This is similar to `ListView` or filsystem tree: a folder can contain regular files (menu items), or it can contain another folder (menu model), which in turn can also contain another folder. `MenuModel` can similarly be nested, though it is usually not recommended to go deeper than 2 or 3 levels.

We call a `MenuModel` that is nested within another model a **submenu**. It will show up as a button with a label, along with an indicator that it is a submenu, usually a `>` shape. Clicking this item will reveal the submenu.

To add this type of submenu item, we use `MenuModel::add_submenu`, which takes a title and menu model as its two arguments:

```cpp
auto submenu = MenuModel();
submenu.add_action("Subitem 01", action);
submenu.add_action("Subitem 02", action);

model.add_submenu("Type 03: Submenu", submenu);
```

Here, we created a new model `submenu` with two simple placeholder items. We then added this model as a submenu to our outer model, called `model`.

\image html menus_03.png

When clicking the submenu item "Type 03: Submenu" (window on the left), the menu slides over to reveal the submenu (window on the right).

### Item Type 04: Sections

Lastly we have **sections**. Sections are like submenus, in that they are a menu inside another menu. The difference is in the way this inner menu is displayed.

When adding a submenu with `MenuModel::add_submenu`, a single new item will appear in the outer menu. When clicked, the menu "slides" to reveal the submenu. With sections, added with `MenuModel::add_section`, the inner menu is instead inserted into the outer menu directly; both are shown at the same time. It's easiest to just show an example:

```cpp
auto section = MenuModel();
section.add_action("SectionItem 01", action);
section.add_action("SectionItem 02", action);

model.add_section("Type 04: Section", section);
```

\image html menus_04.png

We see that the section label, `"Type 04: Section"`, in this case, is used as a type of heading. All items from the inner menu are group together and shown inside the outer menu. In this way, developers can group items with a similar theme or functionality physically together, making large menus easier to parse.

---

## Displaying Menus

### PopoverMenu

Now that we have learned to construct arbitrarily complex menu *models*, we should turn our attention to the *view*, a widget displaying a `MenuModel`.

We've already seen one such widget earlier in this chapter. `PopoverMenu`, which is usually tied to a `PopoverButton` used to control whether the menu is revealed, is well-suited for this task. Asking the user to reveal the menu manually by clicking a button reduces clutter in the UI.

The `set_always_show_arrow` property of `PopoverButton` indicates to the user that clicking the button will reveal a menu, informing users of where to click to access menus.

### MenuBar

Mousetrap offers one other widget to display menu models, called \a{MenuBar}. Unlike `PopoverMenu`, this menu view is always visible. `MenuBar`s are often used exactly once in almost any application, and their appearance should be familiar, even to users of completely different UI engines:

\image html menus_menubar.html

We see `MenuBar` it is a horizontal list of items. When the user clicks on one of the items, a nested menu opens. Just like before, menus can be nested a theoretically infinite amount of times.

Unlike `PopoverMenu`, `MenuBar` requires its `MenuModel` to have a certain structure: **all top-level items have to be submenus**.

What does this mean? Let's work through the menu shown in the image above. We created it like so using the following snippet:

```cpp
auto model = MenuModel();

auto file_submenu = MenuModel();
file_submenu.add_action("Open", /* action */);

auto file_recent_submenu = MenuModel();
file_recent_submenu.add_action("Project 01", /* action */);
file_recent_submenu.add_action("Project 02", /* action */);
file_recent_submenu.add_action("Other...", /* action */);
file_submenu.add_submenu("Recent...", file_recent_submenu);

file_submenu.add_action("Save", /* action */);
file_submenu.add_action("Save As", /* action */);
file_submenu.add_action("Exit", /* action */);

auto help_submenu = MenuModel();
model.add_submenu("File", file_submenu);
model.add_submenu("Help", help_submenu);

auto menubar = MenuBar(model);
```

Where, in a real application, each item will have a different action.

To make the nesting easier to understand, we can write the structure of the model similar to how we would describe a folder structure:

```
model \
    File \
        Open
        Recent... \
            Project 01
            Project 02
            Other...
        Save
        Save As
        Exit
    Help \
        (...)
```

Where any line suffixed with a `\` is a submenu.

We see that we have four models in total. The top-most menu model is called `model`, it is what `MenuBar` will be initialized with.
Next, we have the model called `file_submenu`, which has the title `File`. It containes five items, titled `Open`, `Recent...`, `Save`, `Save As` and `Exit`. `Recent...` is a submenu-type item, created from a `MenuModel` called `file_recent_submenu` in the above code. This model, in turn, has three items.  On the same level as `File`, we have a second menu `Help`.

The **top-level** menu is `model`. It is used as the argument for the constructor of `MenuBar`. We see that all direct children of `model` (`File` and `Help`) **are themself submenus** (they were added using `add_submenu`). No direct child of `model` is an action-, widget- or section-type item. This is what is required for `MenuBar`. All top-level items have to be submenus.

Apart from this requirement, the rest of the menu can have any arbitrary structure.

In summary:

+ For `MenuBar`, the toplevel menu can only have submenus as direct children
+ For `PopoverMenu`, there are no requirements

This, along with the aesthetic difference, makes both types of menu model view suited for different purposes. Usually, an application will only have a single `MenuBar` at the top of the main menu, while it may have many different `PopoverMenu`s all over.

---

## Style Endnote

Menus are extremely powerful and, conversely, they are complex to understand. With practice and good software / UI design, we can create deep, complex menus that are still easy to understand and use. We, as developers, should make this our first priority. The action-interface and model-view design are nice, but first and foremost our menus should be stable and easy to use by end users.

Some additional notes:

### Ellipses

Some may be curious as to why some menu items have `...` added at the end of their labels, while some have not. This is not a universal standard, but it is common for `...` to indicate that clicking this item will open another window, menu, or dialog. If clicking an item simply triggers an action (such as `Save` or `Exit`), `...` is ommitted. If the item opens a window, widget, or submenu, `...` is appended to the menu label, as is the case with `Recent...` above.

### Maximum Menu Depth

Regarding menu depth, best practice is to never go deeper than 3 levels. The above example with `File > Recent... > Project 01` shows a good situation in which a 3-level-deep menu may be justified. Going any deeper is rarely advice, adding a section should always be considered before adding a submenu.

### Section Grouping

Lastly, some schools of UI design believe that **every menu item should be inside a section**. For example, if we were to follow this philosophy for our above example, we would redesign it like so:

\image html menus_menubar_with_sections.html

This adds considerable complexity to the code (adding 4 models, one for each section, making our total 8). In return, items are grouped logically and each item gets a "heading", which helps make long menus easier to understand. Every developer has to decide for themself whether this is a good choice or not. What all should agree on, however, is that if something makes the developers job harder but makes the users experience better, it is worth the effort.

