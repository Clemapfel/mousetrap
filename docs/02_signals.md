# Chapter 2: Signals

In this chapter, we will learn:
+ What signals and signal handlers are
+ How to check a which signature a signal expects
+ How to connect to a signal
+ How and why to block a signal

## Signal Architecture

Central to mousetrap, GTK and many GUI libraries like QT is what is called **signal architecture** or [**signal programming**](https://en.wikipedia.org/wiki/Signal_programming).

A signal has 3 components:
+ an **id**, which uniquely identifies it. Ids may not be shared between signals
+ an **emitter**, which is a non-signal object
+ a **callback**, which is a function called when an emitter emits a signal

It may be easiest to consider an example: One of the simplest interactions a human can have with a GUI program is clicking a button. In mousetrap, the \link mousetrap::Button `Button` \endlink class has a signal just for this, which has the id `clicked`.

In this case, the signal **id** is `clicked`, the signal **emitter** is an instance of `Button`. When a user clicks the button (the graphical area on their screen), the in-memory object emits signal `clicked`. If we want to tie behavior to user behavior, we can **connect** to signals like this by specifying a function. This function is executed whenever the corresponding signal is emitted. This function is the **callback**, also called the **signal handler**.

```cpp
// create Button instance
auto button = Button();

// create signal handler
auto on_signal_clicked = [](Button* button) -> void {
    std::cout << "clicked" << std::endl;
}

// connect signal handler to signal
button.connect_signal_clicked(on_signal_clicked);
```

\remarks `on_signal_clicked` above is a C++ lambda. If you are unfamiliar with lambda syntax, please consider working through [this tutorial](https://learn.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170) before continuing. Lambdas are intergral to modern C++ and will be extensively.

\collapsible_note_begin
In this section, code snippets will only show the relevant lines. If we want to actually compile and run the code stated here, we need to modify our `main.cpp` from the last section.

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);

        // start of snippet
        auto button = Button();
        
        auto auto on_signal_clicked = [](Button* button) -> void {
            std::cout << "clicked" << std::endl;
        }
        
        button.connect_signal_clicked(on_signal_clicked);
        // end of snippet
       
        window.set_child(button); // add button to window so we can click it
        window.present();
    });

    return app.run();
}
```
\collapsible_note_end

Now, when the user clicks the button, `clicked` is printed.

## SignalEmitters

`Button`, as most classes in mousetrap, inherits from an abstract class called `SignalEmitter`. Inherting from this class is equivalent to saying "this object can emit signals". Not all object can emit all signals, however. To be able to emit the signal with id `clicked`, an object has to inherit from a *signal component* for that signal. Signal component have the name `has_signal_<id>`, where `<id>` is the id of the signal. For example, `Button` inherits from the signal component `has_signal_clicked`. This means `Button` is able to emit the signal with id `clicked`.

When an object inherits from signal component `has_signal_<id>`, it gains the following methods

+ `connect_signal_<id>`
+ `disconnect_signal_<id>`
+ `emit_signal_<id>`
+ `set_signal_<id>_blocked`
+ `get_signal_<id>_blocked`

For example, because `Button` inherits from `has_signal_clicked`, it gains the member functions `connect_signal_clicked`, `disconnect_signal_clicked`, `emit_signal_clicked`, etc. We will

### Connecting Signal Handlers

We've aready seen how to connect a signal handler to the signal of an emitter, however, using `Button::connect_signal_clicked`, what may not have been obvious is that the signal handler, a lambda in our above code snippet, is required to conform to a specific signature.

\collapsible_note_begin
A functions **signature** descibres a functions return- and argument types. For example, the function

```cpp
void foo(int32_t i, const std::string& string) -> void {
    std::cout << i << " " << string << std::endl;
}
```

Has the signature `(int32_t, const std::string&) -> void`. 

The lambda:
```cpp
auto on_signal_clicked = [](Button* button) -> void {
    std:cout << "clicked" << std::endl;
};
```

Has the signature `(Button*) -> void`.

In general, we write function with argument types `Arg1_t, Arg2_t, ...` and return type `Return_t` as having the signature `(Arg1_t, Arg2_t, ...) -> Return_t`.

\collapsible_note_end

For example, the lambda we connected to signal `clicked` of `Button` had the signature `(Button*) -> void`. When the signal is emitted, the first argument of the signal handler will be a pointer to the signal emitter instance, `Button*` in our case.

Each signal requires its handler to conform to a specific signature. If the handler has the wrong signature, a compiler error will be thrown. This makes it important to know how to check which signal requires which signature.

### Checking Signal Signature

Working with our example, the signal `clicked` of class `Button`, let's say we do not know what function to connect to this signal. We can find out by checking the mousetrap documentation, navigating to the \link mousetrap::Button corresponding page \endlink, we check "detailed description" (by clicking "more" at the very top of the page), which contains the following table:

\todo TABLE

We see that `Button` has a number of additional signals, for now we just want to know which signature `clicked` specifically has. Checking the table, it says `(Button*, (Data_t)) -> void`, so there we go. The table also contains information of when each signal is emitted, which will be covered in the upcomping chapter on widgets.

`(Data_t)` refers to an **optional** argument for all signal handlers, which is an arbitrary object we can pass to it.

### Handing Data to Signal Handlers

While we do get passed a pointer to the signal emitter instance as the first argument to any signal handler, we often need to reference outside objects from inside the signal handler. This may be to access a global state, another signal emitter or an otherwise custom object. Mousetrap allows for this by adding an optional, arbitrarily typed, single argument to the end of any signal handler signature. This object is often referred to as `data`, so its type will be called `Data_t`.

Expanding on our previous example, if we want to send a customized message when the user clicks our button, we can change `on_signal_clicked` as follows:

```cpp
auto button = Button();

// add secondary argument of type string
auto on_signal_clicked = [](Button* button, std::string message) -> void {
    std::cout << message << std::endl;
}

// now, `connect_signal_<id>` requires a second argument on top of the handler
button.connect_signal_clicked(on_signal_clicked, "custom message");
```
```
custom message
```

We see that we changed the signature of `on_signal_clicked` from `(Button*) -> void` to `(Button*, std::string) -> void`. Our string `custom message` will be stored in the signal component for that signal and handed to our signal handler. Any and all objects can be provided as `data`, however they have to be packages as one argument.

The above can be written more succinctly like so:

```cpp
auto button = Button();
button.connect_signal_clicked([](Button* button, auto message) -> void {
    std::cout << message << std::endl;
}, "custom message");
```

Where the C++ compiler will automatically replace `auto` with the correct datatype.

### Blocking Signal Emission

If we want an object to seize invocation of the signal handler on signal emission, we have two options We can **disconnect** the signal, which will permanently remove the registered signal handler, using `disconnect_signal_<id>`. This is a quite costly operation and should rarely be used. A much more performant and convenient method is that of **blocking** signals:

Blocking a signal will prevent invocation of the signal handler, not signal emission. This means, for our `Button` example, the user can still click the button, however our connected lambda will not be executed.

We block a signal using `set_signal_<id>_blocked`, which takes a boolean as its argument. We can check whether a signal is currently blocked using `get_signal_<id>_blocked`.

Consider the following use-case, in which we manually emit a signal using `emit_signal_<id>`, which triggers identical behavior to that of another entity triggering emission, such as the user clicking the button:

```cpp
static auto button_01 = Button();
static auto button_02 = Button();

button_01.connect_signal_clicked([](Button*) -> void {
    std::cout << "01 clicked" << std::endl;
    button_02.emit_signal_clicked();
});

button_02.connect_signal_clicked([](Button*) -> void {
    std::cout << "02 clicked" << std::endl;
    button_01.emit_signal_clicked();
});
```

\collapsible_note_begin
We can reference `button_01` and `button_02` from within the lambdas only because both are declare `static`. Alternatively we could capture the buttons from within the lambda capture clauses (the `[]`).
\collapsible_note_end

Here, we declared two buttons. The intended behavior is that if the user clicks either one of the buttons, both buttons emit their signal. Right now, if the user clicks, let's say, `button_01` it will print `01 clicked`, then `button_01` invokes `button_02`, which prints `02 clicked`. `button_02` then causes `button_01` to invoke it's siganl, which creates a loop. The result is the following output:

```
01 clicked
02 clicked
01 clicked
02 clicked
01 clicked
02 clicked
01 clicked
02 clicked
...
```

And our application deadlocks. This is of cours extremely undesirable. We can avoid this behavior by **blocking signals**:

```cpp
button_01.connect_signal_clicked([](Button* self) -> void {
    std::cout << "01 clicked" << std::endl;
    
    // block signal emission on self
    self->set_signal_clicked_blocked(true);
    
    // trigger other button
    button_02.emit_signal_clicked();
    
    // unblock signal emission on self
    self->set_signal_clicked_blocked(false);
});

button_02.connect_signal_clicked([](Button* self) -> void {
    std::cout << "02 clicked" << std::endl;
    
    self->set_signal_clicked_blocked(true);
    button_01.emit_signal_clicked();
    self->set_signal_clicked_blocked(false);
});
```

Let's talk through what happens when the user clicks one of the two buttons, let's say `button_01`:

+ `button_01` invokes its signal handler
+ `button_01`s signal handler prints `01 clicked`
+ `button_01` blocks invocation of its own signal handler
+ `button_01` triggers signal emission on `button_02`
+ `button_02`s signal handler prints `02 clicked`
+ `button_02` blocks invocation of its own signal handler
+ `button_02` attempts to trigger signal emission on `button_01`, but that buttons signal is blocked, so nothing happend
+ `button_02` unblocks itself
+ `button_01` unblocks itself

```
01 clicked
02 clicked
```

By correctly blocking signals, we get the correct behavior of both buttons being triggered exactly once. Because both buttons unblock themself at the end of the signal handler, after the two buttons are done everything returns to the way it was before the first signal emission.

If we want to try out this behavior ourself, here is a modified `main.cpp`

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application* app) -> void
    {
        auto window = Window(*app);

        // declare buttons
        static auto button_01 = Button();
        static auto button_02 = Button();

        // connect handlers
        button_01.connect_signal_clicked([](Button* self){
            std::cout << "01 clicked" << std::endl;

            self->set_signal_clicked_blocked(true);
            button_02.emit_signal_clicked();
            self->set_signal_clicked_blocked(false);
        });

        button_02.connect_signal_clicked([](Button* self){
            std::cout << "02 clicked" << std::endl;

            self->set_signal_clicked_blocked(true);
            button_01.emit_signal_clicked();
            self->set_signal_clicked_blocked(false);
        });

        // create a container so we can put both buttons into the window
        auto box = Box(Orientation::HORIZONTAL);
        box.push_back(button_01);
        box.push_back(button_02);
        box.set_margin(75);
        
        window.set_child(box);
        window.present();
    });

    return app.run();
}
```

\image html double_button_signal_blocking.png

---

## Application Signals

With our newfound knowledge about signals, we can recontextualize our `main.cpp`. We recongize that `app`, of type \link mousetrap::Application `Application` \endlink is a signal emitter. In `main` we connect to one of its signals, `activate`, which has the signature `(Application*, (Data_t)) -> void`, though we do not supply any `data` as an argument. 

Application has two signals, `activate` and `shutdown`. The former is invoked when initialization should happen, while `shutdown` is emitted when the application is ready to shut-off, usually once all windows associated with an application are closed.

Checking the \link mousetrap::Documentation documentation page on `Application` \endlink, we see that `shutdown` also requires the signature `(Application*, (Data_t)) -> void`. If want to connect to it to safely free some kind state for our application, we now know how to modify `main.cpp` to do so:

<details><summary><b>Click to see the solution</b></summary>

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application* app) -> void {
        // (...)
    });
    
    // connect to signal `shutdown`
    app.connect_signal_shutodwn([](Application* app) -> void {
       // shutdown behavior here 
    });

    return app.run();
}
```
</details>

---