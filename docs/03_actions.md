# Chapter 3: Actions

In this chapter we will learn:
+ How to use the command pattern to encapsulate appliaction functionality
+ How to create and use `mousetrap::Action`
+ How to trigger actions using `Button` and `PopoverMenu`

## Command Pattern

As we create more and more complex applications, keeping track of how / when to trigger which functionality gets harder and harder. An application can have hundreds, if not thousands of functions, all linked to one or more triggers such a buttons, menus, keyboard shortcuts, etc. Things will get out of hands very quickly, which is why there's a software design pattern just for this, the [**command pattern**](https://en.wikipedia.org/wiki/Command_pattern).

A **command**, henceforth called **action** is a an object which has the following components:
+ A **function**, which is the actions behavior
+ An **id** which uniquely identifies the action

The mousetrap class `Action` has two additional properties, a **state** and **shortcut**, both of which we will concern us with later on in this section.

## \link mousetrap::Action `Action` \endlink

As early as possible, we should drop the habit of defining application behavior inside a global function. For example, in the previous chapter we declared a `Button` with the following behavior:

```cpp
auto button = Button();
button.connect_signal_clicked([](Button* button){
    std::cout << "clicked" << std::endl;
});
```

Let's say instead of simply printing `clicked`, this button does a more complex operation, such as opening / saving a file or using the internet to connect to a server overseas. Often, we want this functionality to not be tied to a specific button, it is much more scalable and easy to maintain to encapsulate it as an automatically managed object that can be connected and disconnected to/from any number of triggers. `Action` offers this functionality.

When creating an action, we first need to choose an **id**. An id is any identifier containing the character `[a-zA-Z0-9_.]`, that is all roman letters, number 0-9, `_` and `.`. The dot is usually reserved to simulate scoping, for example, an action could be called `image_file.save`, while another is called `text_file.save`. Both actions say what they do, `save` a file, but the prefix makes clear which part of the application they act on. We should always pick a clear, descriptive id for an action. Much like with variable names, abbreviations and otherwise unclear termonology should be avoided.

In this spirit, we'll call our example action that prints `clicked`  "`example.print_clicked`". Armed with this appropriate id, we create the action object like so:

```cpp
auto action = Action("example.print_clicked", app);
action.set_function([](){
    std::cout << "clicked" << std::endl;
})
```

Where `app` is a pointer to our application.

We see that we can set an actions behavior using `set_function`. This method acts a lot like connecting so a signal handler, all functions linked to actions are required to have the signature `(Action*, (Data_t)) -> void`.

There are multiple way of triggering an action, for now, we recall our `print_clicked` behavior should be tied to a button. `Button` speficially has a function for this `set_action`, which links the action to the button. If the button is clicked, the action is triggered. 

```cpp
auto button = Button();
button.set_action(action);
```
<br>

\collapsible_note_begin

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
        button.set_margin(75);
        
        // connect action to button
        button.set_action(action);

        // add button to window
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

## Action Maps

Eagle-eyed readers may have noticed that we need to supply an `Application*` to the `Action` constructor, despite the two seemingly not interacting. This is, because the `Application` instance is what stores our actions globally. Once we call `set_function` on an action, it is registered with the application under its id, `example.print_clicked` in our case, and can now be accessed both internally and externally by everything in the application.

For example, if we lost track of our `Action` instance but would like to call it manually, we can retreieve it from our application instance:

```cpp
// after action.set_function(//...
app->get_action("example.print_clicked").activate();
```
```
clicked
```

Where `get_action` returns a `Action&`. By giving our action a descriptive id, we have a very scalable and automated mechanism of keeping track of and referencing functions. 

## Triggering Actions

`activate` and `Button` are not the only way to trigger actions, there are two additional ways that are commonly used in almost all applications: **menus** and **keyboard shortcuts**.

### Menus

One of the most common forms of interaction with a GUI application is that of *menus*. A menu is a nested list of items. Each item has a label, if the user clicks the label, something happens. That "something", in mousetrap, is invocation of an `Action`.

To create the most simple menu with just one item, we need two objects, a [**model** and **view**](https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller). The model holds the internal structure of the menu, including what label to show for which action. The view displays the menu, where each line of code is commented with what is happening

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

// create model
auto model = MenuModel();

// add actions to model
model.add_action("Action 01", action_01);
model.add_action("Action 02", action_02);

// create view, GUI elements that display the model
auto popover_menu = PopoverMenu(model);
auto popover_menu_button = PopoverMenuButton();
popover_menu_button.set_popover_menu(popover_menu);

// add button to window
popover_menu_button.set_margin(75);
window.set_child(popover_menu_button);
window.present();
```

\collapsible_note_begin

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
        auto popover_menu_button = PopoverMenuButton();
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

Here, we created two actions `action_01` and `action_02`, each of which simply prints to the console. We then created a `MenuModel`, which is the model component of our clickable menu. By calling `MenuModel::add_action`, we add both actions to this model. `add_action` takes two argument, as its second the action, and as its first the label which will be displayed by the menu on screen. 

Lastly we create  `PopoverMenu` and `PopoverMenuButton`. The former is a little window that sticks to a button, the latter is a button that, when clicked, show the window. As seen in the screenshot above, the window now contains two items with our given labels. Clicking the first, we get:

```
01 triggered
```

By using actions instead of global functions, we effectively decoupled behavior and its triggers. The above `action_01` can be linked to a button, a second menu somewhere else or a completely unrelated part of the application, such as another library.

## Shortcuts

Another advantage of actions is that we can associate a **keyboard shortcut** with it. More advanced users and/or users with physical disabilities will often wish to control an application without using the mouse too much. This can be accomplished by using shortcuts.

We will learn how to trigger shortcuts in general in the chapter on event handling, for now we can make navigation of a menu easier by associating each of ours actions with a keyboard shortcut. Let's say we want to trigger `action_01` using `Control + 1`, and `action_02` using `Control + 2`.

Using `Action::add_shortcut`, we add the shortcuts like so:

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

Now, when we run our application and open our popover menu, we see that the application automatically added these keyboard shortcuts to the menu display:

\image html two_action_menu_model_with_shortcuts.png

Now, while the menu is open, if the user pressed the set keybindings, the actions will trigger, just as if they clicked the corresponding item.

To learn more about the syntax used for the string that is the argument to `add_shortcut`, look forward to chapter 5.

