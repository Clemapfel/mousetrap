# Chapter 3: Actions

In this chapter we will learn:
+ How to use the command pattern to encapsulate appliaction functionality
+ How to create and use `mousetrap::Action`
+ How to trigger actions using `Button` and `Menubar`

## Command Pattern

As we create more and more complex applications, keeping track of how to invoke which functionality gets harder and harder. An application can have hundreds of not thousands of functions, all linked to one or more triggers such a buttons, menus, keyboard shortcuts, etc. This will quickly get out of hand, which is why there is a better design pattern to declaring the act of "triggering a function", called the **command pattern**.

A **command**, henceforth called **action** is a C++ object which has the following components:
+ A **function**, which is the actions behavior
+ An **id** which uniquely identifies the action

The mousetrap class `Action` has two additional properties, a **state** and **shortcut**, both of which we will concern us with later on in this section.

## mousetrap::Action

As early as possible, we should drop the habit of defining application behavior inside a global function. For example, in the previous chapter we declared a `Button` with the following behavior:

```cpp
auto button = Button();
button.connect_signal_clicked([](Button* button){
    std::cout << "clicked" << std::endl;
});
```

Let's say instead of simply printing `clicked`, this button does a more complex operation, such as opening / saving a file or using the internet to connect to a server overseas. Often, we want this functionality to not be tied to a specific button, it is much more scalable and easy to maintain to encapsulate it as a automatically managed object. `Action` offers this functionality.

When creating an action, we first need to choose an *id*. An id is any identifier containing the character `[a-zA-Z0-9_.]`, that is all roman letters, number 0-9, `_` and `.`. The dot is usually reserved to simulate scoping, for example an action could be called `image_file.save` and `text_file.save`. Both actions say what they do, `save` a file, but the prefix makes clear which part of the application they act in.

We'll call our example action that prints `clicked`  "`example.print_clicked`". We create the action object like so:

```cpp
auto action = Action("example.print_clicked", app);
action.set_function([](){
    std::cout << "clicked" << std::endl;
})
```

Where `app` is a pointer to our application.

We see that we can set an actions behavior using `set_function`. This method acts a lot like connecting so a signal handler, it has a set signature `(Action*, (Data_t)) -> void`.

There are multiple way of triggering an action, for now, we recall that this behavior can be tied to a button. `Button` speficially has a function for this `set_action`, which links the action to the button. If the button is clicked, the action is triggered. 

```cpp
auto button = Button();
button.set_action(action);
```
<br>

\collapsible_note_begin

For this chapter, we'll modify our `main.cpp` like so:

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

Eagle-eyed readers may have noticed that we need to supply an `Application*` to the `Action` constructor, despite the two seemingly not interacting. This is, because the `Application` instance is what stores our actions globally. Once we call `set_function` on an action, it is registered with the application under it's id, `example.print_clicked` in our case, an can now be accessed both internally and externally by everything in the application.

For example, if we lost track of our `Action` instance but would like to call it manually, we can do so like so:

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

A menu is usually a list of labels. In mousetrap speficially, each of these labels has a name, the text displayed as the label, and a 















> ###########################


# Chapter 3: Actions

This chapter we will learn:
+ How to encapsulate functionality using the Command pattern
+ How to define `mousetrap::Action`s behavior
+ The difference between a stateless and stateful action
+ How to trigger actions using `MenuBar` and `Button`

## Introduction: Command Pattern

\cpp_code_begin
```cpp
```
\cpp_code_end

\julia_code_begin
\julia_todo
\julia_code_end

### Action Names and Functions

The most trivial use of an action is as that of a functor, an object which is callable. 

To create an action, we first have to pick a name. That name should be unique, and should only contain letters `
[a-zA-Z0-9._]*` (that is, all letters from `A` to `Z`, `a` to `z`, numbers from `0` to `9`, `_` and a `.`). It is 
common to prefix an action with it's scope, for example `file.save_as` or `app.shutdown`. This name is referred to 
as the actions **ID**, which is of type `mousetrap::ActionID`. 

After picking a name we can set an actions behavior by using `set_function`, whos argument can be either a lambda or 
C function:

\cpp_code_begin
```cpp
auto action = Action("example.test_action");
action.set_function([](){
   std::cout << "activated" << std::endl; 
});
```
\cpp_code_end

\julia_code_begin
\julia_todo
\julia_code_end

We can then trigger this action manuall by using `Action::activate`:

\cpp_code_begin
```cpp
action.activate();
```
\cpp_code_end

\julia_code_begin
```julia
activate(action)
```
\julia_code_end

Which prints `"activated"` immediately. 

### Passing data to actions

Sometimes we want action to be invoked with an argument. Instead of calling `Action::activate` with an argument, 
however, we need to specify the argument during `set_function`. This argument, hencefort `data` is usually a pointer 
to a local instance.

\cpp_code_begin
```cpp
auto instance = std::string("Data");
auto action.set_function([](std::string& data){
    std::cout << "Activated with: " << data << std::endl;
}, instance);
action.activate();
```
Where using `auto` for the argument type of the lambda makes the compiler automatically infer the type, 
`std::string` in this case.
\cpp_code_end

\julia_code_begin
\julia_todo
\julia_code_end

Which now prints `"Activated with: Data"`, as expected.

### Registering Actions with the Application

So far actions aren't any more useful for functions, but this is because we have been using them directly. Actions 
are meant to be stored globally, we do this by regsitering them with an application. Recall that we have our 
`mousetrap::Application` called `app` from the previous chapter. We can add an action to it using 
`Application::add_action`. The application takes ownership of the action, so even if the original `Action` object 
goes out of scope, it's internals (the actions id, functions, data, etc.) will be kept in memory until the action is 
removed with `Application::remove_action` or the application exits. 

We should register all actions with the applications, and most of our projects behavior should be contained in 
actions. This is because actions are well-defined and can be called simply by their ID. A typical user application 
in mousetrap will have hundreds of actions, which isn't a problem because we, as humans, don't have to keep track of 
them. That is done by `mousetrap::Application` and actionable widgets, which includes buttons and menus.

## Triggering Actions with Widgets

We will have a later chapter all about widgets, for now, just now that a widget is an object with a graphical 
representation. User interact with widgets, the screen you are looking at right now probably has dozens of widgets, 
the search bar where you enter the URL in your browser is a widget, each button around it is a widget, the window 
itself is a widget and even the "close" and "minimize" buttons are widgets. Any and all things with a graphical 
representation can be considered widgets and mousetraps has dozens of different types. For now, we just need to 
learn about maybe the simplest interactive widget: A button.

We modify our `main` from the previous chapter by adding both an action `button_action` and a `mousetrap::Button` 
like so:

\cpp_code_begin
```cpp
using namespace mousetrap;

inline Window* window = nullptr;

int main()
{
    auto app = Application("example.app");
    app.connect_signal_activate([](Application* app)
    {
        window = new Window(*app);
        
        // create the button action
        auto button_action = Action("example.button_action")
        
        // set button actions behavior
        button_action->set_function([](){
            std::cout << "button pressed" << std::endl;
        });
        
        // register the action with app
        app.add_action(button_action);
        
        // create a button
        auto button = Button();
        
        // put button into the window so we can access it in the GUI
        window.set_child(button);
        
        window.present();
    });
    
    app.connect_signal_shutdown([](Application*){
        delete window;
    })
    
    return run(app);
}
```
\julia_code_begin
\julia_todo
\julia_code_end

### Stateless Actions

### Stateful Actions

## Menus

## Keybindings