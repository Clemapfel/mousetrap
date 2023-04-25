## Chapter 6: Menus

In this chapter, we will learn:
+ How to create complex, nested Menus
+ All types of menu items provided by mousetrap
+ How to display menus using `PopoverMenu` and `MenuBar`

We've already seen basic use of menus in the [chapter on actions](03_actions.md#menus). We recall that a menu has two components, a **model** of type `mousetrap::MenuModel`, and a **view**, which is a widget displaying the menu. 

### MenuModel Architecture

`MenuModel` is a list of items. This means the items have a specific order, set by the order at which we insert items into the model. If item A is added before item B at runtime, then A will appear above item B.

\collapsible_note_begin{Running snippets from this section}

For this section, we'll be using the following `main.cpp`:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("example.menus.app");
    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);
        
        // declare dummy action
        auto action = Action("example.print_called", app);
        action.set_function([](Action*){
            std::cout << "called" << std::endl;
        });
        
        // outermost model
        auto model = MenuModel();
        
        // snippet goes here
        
        // create model view
        auto popover_menu = PopoverMenu(model);
        auto menu_button = PopoverMenuButton();
        menu_button.set_popover_menu(popover_menu);
        menu_button.set_margin(75);

        // add button to window
        window.set_child(menu_button);
        window.present();
    });

    return app.run();
}
```
\collapsible_note_end

There are multiple different types of menu items, all with their own purpose:

### Item Type #1: Action

The first and most simple item type we have already seen. Added via `MenuModel::add_action`, which takes both a label and an action, this item is a simple button with text. If the user clicks the button, the action is executed.

Similar to `Button`, if the action is disabled (via `Action::set_enabled`), the menu item will appear "greyed out", and cannot be activated.

```cpp
model.add_action("Type 01: Action", action);
```

\image html menus_01.png

### Item Type #2: Widgets

Secondly, we have perhaps the most powerful type of item: A custom widget. We add an item of this type using `add_widget`, which only takes a single argument, the widget itself. This widget can be arbitrarily complex, though it is usually not adviced to insert an entire columnview into a tiny menu. Good-practic examples include `Button`, `Entry`, `CheckButton`, `ToggleButton`, `Switch` and `Scale`.

All of these are interactable. If we instert a non-interactable widget such as `ImageDisplay`, it may confuse users because clicking that part of the menu does nothing. 

Widgets are the most flexible of menu items, while basically anything can be inserted into a menu, they should not be overused.  

```cpp
static auto entry = Entry();
entry.set_text("Type 02: Widget");
entry.connect_signal_activate([](Entry*){
    // do something here
});
model.add_widget(entry);
```

\todo entry does not live

\image html menus_02.png

### Item Type #3: Submenu

`MenuModel` can be **nested**, which means we can insert a `MenuModel` into another `MenuModel`. This is similar to a filsystem tree, a folder can contain regular files (menu itmes), or it can contain another folder, which in turn can also contain another folder. `MenuModel` can similarly be deeply nested, though it is usually not recommended to go deeper than 2 or 3 levels.

We call a `MenuModel` that is nested within another model a **submenu**. It will show up as a button with a label, along with an indicator that it is a submenu, usually a `>` shape. We add an item like this using `MenuModel::add_submenu`, which takes a title, used as the title of the submenu:

```cpp
auto submenu = MenuModel();
submenu.add_action("Subitem 01", action);
submenu.add_action("Subitem 02", action);

model.add_submenu("Type 03: Submenu", submenu);
```

Here we create a new model `submenu` with two simple placeholder items. We then added this model as a submenu to our outer `MenuModel` called `model`.

\image html menus_03.png

When clicking the submenu item "Type 03: Submenu" (window on the left), the menu slides over to reveal the submenu (window on the right).

### Item Type 04: Sections

Lastly we have **sections**. Sections are like submenus, in that they are a menu inside another menu. The difference is in the way this inner menu is displayed.

When adding a submenu with `MenuModel::add_submenu`, a single new item will appear in the outer menu. When clicked, the menu slides to reveal the submenu. With sections, the inner menu is instead inserted into the outer menu, both are shown at the same time. It's easiest to just show an example, we add a section using `MenuModel::add_section`:

```cpp
auto section = MenuModel();
section.add_action("SectionItem 01", action);
section.add_action("SectionItem 02", action);

model.add_section("Type 04: Section", section);
```

\image html menus_04.png

We see that the section label, `"Type 04: Section"` in this case, is used as a type of heading. All items in the inner menu are group together and shown separately. This makes menus more easy to parse, as developer can group items with a similar theme or functionality physically together.

## Displaying Menus: MenuBar

Now that we have learned to construct arbitrarily complex menu *models*, we should turn our attention to the *view*, a widget displaying the model. We've already seen one such widget, `PopoverMenu`, which is tied to a `PopoverMenuButton` which pops it up. This widget is well suited for context actions, it hides the menu unless the user spefically requests it to appear, which result in less clutter in the UI.

We have one other widget to display menu models, this one is visible most of time. It is called `MenuBar`, and used extensively by many applications:

\image html menus_menubar.html


We see that it is a horizontal list of items. This list is visible at all times, when the user clicks on of the items, a nested menu opens revealing additional options. Menus can again be nested multiple times.

Unlike `PopoverMenu`, `MenuBar` requires its `MenuModel` to have a certain structure: **all toplevel items have to be submenus**. What does this mean? Let's work through the menu shown in the image above. We created it like so:

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

Where, in a real application, each item will have a different action, of course.

To make the structure clearer, we can write the structure like we would write a folder structure:

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

Where any item with a `\` is a submenu.

We see that we have 4 models in total. 
The top-most menu model is called `model`, it is what `MenuBar` will be initialized with.
Next we have the model called `file_submenu`, which has the title `File`. It containes 5 items, `Open`, `Recent...`, `Save`, `Save As` and `Exit`. `Recent...` is itself a model, called `file_recent_submenu` in the above code. This model, in turn, has 3 items.

On the same level as `File`, we have a second menu `Help`.

The **toplevel** menu is `model`. It is used as the argument for the constructor of `MenuBar`. We see that all direct children of `model` are themself subemnus (they have `\` in the above version and were added using `add_submenu`). No directo child of `model` is an action- or widget item. This is what is required for `MenuBar`. All toplevel items have to be submenus (not sections). Apart from this requirement, the rest of the menu can have any arbitrary structure.

In summary:

For `MenuBar`, the toplevel menu can only have submenus as direct children.
For `PopoverMenu`, there are no requirements.

Another difference between `MenuBar` and `PopoverMenu` is the way the menus are displayed. When navigating through submenus in `PopoverMenu`, new submenus "slide" into view, taking over the entire popover. In `MenuBar`, this sliding animation does not happen, instead the current submenu and all of its "father" submenus (all submenus for whom the current one is a child) are visibl at the same time. This asthetic difference makes both type of menu model views suited for different purposes.

### Summary

Menus are extremely powerful, and, because of this, they are complex to understand. With practice and good software design, we can create deep, complex menus that are still easy to understand and use. We as developers should make this our first priority. By properly using the action interface and good UI design, a menu by itself can be just as powerful as an entire application.


