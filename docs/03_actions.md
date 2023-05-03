# Chapter 3: Actions

In this chapter, we will learn:
+ How and why to use the command pattern to encapsulate application functionality
+ How to create and use `mousetrap::Action`
+ How to trigger actions using `Button` and `PopoverMenu`

## Command Pattern

As we create more and more complex applications, keeping track of how / when to trigger which functionality gets harder and harder. An application can have hundreds, if not thousands of functions, all linked to one or more triggers such a buttons, menus, keyboard shortcuts, etc. Things will get out of hands very quickly, which is why there's a software design pattern just for this purpose, called the [**command pattern**](https://en.wikipedia.org/wiki/Command_pattern).

A **command**, henceforth called **action**, is an object which has the following components:
+ A **function**, which is the actions behavior
+ An **ID** which uniquely identifies the action

## Actions

As early as possible, we should drop the habit of defining application behavior inside a global function. Unless a function/lambda is used exactly once, it should be an action.

For example, in the previous chapter, we declared a `Button` with the following behavior when clicked:

```cpp
auto button = Button();
button.connect_signal_clicked([](Button* button){
    std::cout << "clicked" << std::endl;
});
```

If we wanted to have another, different button somewhere else trigger the same behavior, we would either have to copy-paste this lambda (which is a terrible idea for programming in general), or we have to create a global function and have both buttons call that function. As stated above, this works for very small projects, but is unsustainable for applications of average to high complexity. It is much more scalable and easy to maintain to encapsulate the function in an automatically managed object that is connected and disconnected to/from any number of triggers. `mousetrap::Action` offers this functionality.

When creating an action, we first need to choose an **ID**. An ID is any identifier containing the character `[a-zA-Z0-9_-.]`, that is all roman letters, numbers 0 to 9, `_`, `-` and `.`. The dot is usually reserved to simulate scoping, for example, an action could be called `image_file.save`, while another is called `text_file.save`. Both actions say what they do, `save` a file, but the prefix makes clear which part of the application they act in. We should always pick a clear, descriptive ID for an action. Much like with variable names in general, abbreviations and otherwise non-obvious terminology should be avoided.

Given this, we will call our example action that prints `clicked` "`example.print_clicked`". Armed with this appropriate ID, we create the action object like so:

```cpp
auto action = Action("example.print_clicked", app);
action.set_function([](){
    std::cout << "clicked" << std::endl;
})
```

Where `app` is a pointer to our application.

We see that we can specify an actions' behavior using `set_function`.  All functions linked to actions are required to have the signature `(Action*, (Data_t)) -> void`.

There are multiple way of triggering an action, for now, we recall that our `print_clicked` behavior should be tied to a button. This is such a common scenario that `Button` has a function for this: `set_action`, which automatically connects the action to the button, if the button is clicked, the action is triggered.

```cpp
auto button = Button();
button.set_action(action);
```
<br>

\collapsible_note_begin{main.cpp for this chapter}

To follow along with the code snippets in this chapter, we'll modify our `main.cpp` like so:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);
        
        // delcare action
        auto action = Action("example.print_clicked", app);
        action.set_function([](Action* action){
            std::cout << "clicked" << std::endl;
        });
      
        // declare button 
        auto button = Button();
        
        // connect action to button
        button.set_action(action);

        // add button to window
        button.set_margin(75);
        window.set_child(button);
        window.present();
    });

    // start main loop
    return app.run();
}
```

\image html button_clicked_action.png

```
clicked
```

\collapsible_note_end

## Disabling Actions

Not all functionality should be available at all times. For example, the "print document" button should not be available while a document is being loaded. We could achieve this functionality by manually blocking signals, however, this would need to be on a per-emitter basis, meaning if we have five buttons in different places of our app that all trigger printing, we would have to disable and later re-enable all five of them individually.

`Action` solves this, we can disable an action using `set_enabled`, which takes a boolean as its argument. Once disabled, our application will automatically block objects that were connected to the action. For `Button`, it will appear half-transparent, or "greyed out", signaling to the user that it cannot be interacted with at this time. The button is unlickable, clicking on it produces no animation.

## Action Maps

Eagle-eyed readers may have noticed that we need to supply an `Application*` to the `Action` constructor, despite the two seemingly not interacting. This is, because the `Application` instance is what stores our actions globally. Once we call `set_function` on an action, it is registered with the application under its ID, `example.print_clicked` in our case, and can now be accessed both internally and externally by everything in the application. The action's life-time is managed by the application, its destructor will only be called once nothing else references the action. This prevents developer errors in which we lose track of an action. The application keeps it for us.

If we do end up loosing a reference to the `Action` instance, we can retrieve it from our application instance:

```cpp
// after action.set_function(//...
app->get_action("example.print_clicked").activate();
```
```
clicked
```
Where `get_action` returns an `Action&`, and  `activate` manually calls the actions function.

By giving our action a descriptive ID, we have a very scalable and automated mechanism for keeping track of and blocking functions.

## Triggering Actions

`activate` and `Button` are not the only way to trigger actions, there are two additional ways that are commonly used in almost all applications: **menus** and **keyboard shortcuts**.

### Menus

One of the most common forms of interaction with a GUI application is in the form of a **menu**. A menu is a nested list of items. Each item has a label, if the user clicks the label, something happens. That "something", in mousetrap, is usually an invocation of an `Action`.

To create even the most simple menu with just one item, we need two objects, a [**model** and **view**](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller). The model holds the internal structure of the menu, including what label to show for which action, the order of the items, etc. The view displays the menu on screen:

```cpp
// create two actions
auto action_01 = Action("example.menu_model_action_01", app);
action_01.set_function([](Action*){
    std::cout << "01 triggered" << std::endl;
});

auto action_02 = Action("example.menu_model_action_02", app);
action_02.set_function([](Action*){
    std::cout << "02 triggered" << std::endl;
});

// create menu model
auto model = MenuModel();

// add actions to model
model.add_action("Action 01", action_01);
model.add_action("Action 02", action_02);

// create view, GUI elements that display the model
auto popover_menu = PopoverMenu(model);
auto popover_menu_button = PopoverButton();
popover_menu_button.set_popover_menu(popover_menu);

// add button to window
popover_menu_button.set_margin(75);
window.set_child(popover_menu_button);
window.present();
```

\collapsible_note_begin{Updating main.cpp}

Our `main.cpp` now looks like this:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);

        // create two actions
        auto action_01 = Action("example.menu_model_action_01", app);
        action_01.set_function([](Action*){
            std::cout << "01 triggered" << std::endl;
        });

        auto action_02 = Action("example.menu_model_action_02", app);
        action_02.set_function([](Action*){
            std::cout << "02 triggered" << std::endl;
        });

        // create model
        auto model = MenuModel();

        // add actions to model
        model.add_action("Action 01", action_01);
        model.add_action("Action 02", action_02);

        // create view, GUI elements that display the model
        auto popover_menu = PopoverMenu(model);
        auto popover_menu_button = PopoverButton();
        popover_menu_button.set_popover_menu(popover_menu);

        // add button to window
        popover_menu_button.set_margin(75);
        window.set_child(popover_menu_button);
        window.present();
    });

    // start main loop
    return app.run();
}
```

\collapsible_note_end

\image html two_actions_menu_model.png

Here, we created two actions `action_01` and `action_02`, each of which simply prints to the console. We then create a `MenuModel`, which is the model component of our menu. By calling `MenuModel::add_action`, we add both actions to this model. `add_action` takes two argument, a label that will be displayed on screen and the action that should be triggered.

Lastly, we create  `PopoverMenu` and `PopoverButton`. The former is a little window that sticks to a button, the latter is a button that, when clicked, shows the window. As seen in the screenshot above, the window now contains two items with our given labels. Clicking the first, we get:

```
01 triggered
```

By using actions instead of global functions, we effectively decoupled behavior and its triggers. The above `action_01` can be linked to a button, a second menu somewhere else or a completely unrelated part of the application, such as another library. We do not need to keep track of what it is connected to, modifying the action modifies all its potential triggers.

\note mousetraps menus are extremely powerful, which is why there is an [entire chapter dedicated to them](06_menus.md). For now, this simple menu is used it to illustrate how useful actions are when managing application-wide functionality.

## Shortcuts

Another advantage of actions is that we can associate a **keyboard shortcut** with it. More advanced users and/or users with physical disabilities will often wish to control an application without using the mouse too much. This can be accomplished by using shortcuts, which are a combination of keyboard keys that trigger behavior when pressed once.

We will learn how to trigger shortcuts in general in the [chapter on event handling](./05_events.md), for now we, can make navigation of our tiny menu easier by associating each of our actions with a keyboard shortcut. Let's say we want to trigger `action_01` using `Control + 1`, and `action_02` using `Control + 2`.

Using `Action::add_shortcut`, we add the shortcuts to actions like so:

```cpp
auto action_01 = Action("example.menu_model_action_01", app);
action_01.set_function([](Action*){
    std::cout << "01 triggered" << std::endl;
});
// add Control + 1 shortcut
action_01.add_shortcut("<Control>1");

auto action_02 = Action("example.menu_model_action_02", app);
action_02.set_function([](Action*){
    std::cout << "02 triggered" << std::endl;
});
// add Control + 2 shortcut
action_02.add_shortcut("<Control>2");
```

We do not need to change anything about how the menu is constructed. When we run our application and open our popover menu, we see that the application automatically added the keyboard shortcuts to the menu display:

\image html two_action_menu_model_with_shortcuts.png

While the menu is open, if the user presses the shown keybinding, the actions will trigger, just as if they clicked the corresponding item. By using `Action` instead of a regular function, we are given an automated mechanism of handling keyboard shortcuts.
