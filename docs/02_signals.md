# Chapter 2: Signals

In this chapter, we will learn:
+ What signals and signal handlers are
+ How to connect to a signal
+ How to check which signature a signal expects
+ How and why to block signals

---

## Signal Architecture

Central to mousetrap, GTK, and many GUI libraries like QT, is what is called **signal architecture** or [**signal programming**](https://en.wikipedia.org/wiki/Signal_programming).

A signal, in this context, has 3 components:
+ an **ID**, which uniquely identifies it. IDs may not be shared between signals
+ an **emitter**, which is a non-signal object
+ a **callback** or **signal handler**, which is a function called when an emitter emits a signal

It may be easiest to consider an example:

One of the simplest interactions a human can have with a GUI program is clicking a button. In mousetrap, the \link mousetrap::Button `Button` class\endlink is made for this purpose. `Button` has the signal `clicked`, which is emitted when a user presses the left mouse button while the cursor is above the area of the button on screen.

In this case, the signals' **ID** is `clicked`, while the signal **emitter** is an instance of `Button`. When a user clicks the button (the graphical area on their screen), the in-memory object emits signal `clicked`. If we want to tie program behavior to the user clicking the button, we **connect a callback** (a function) to this signal. Once connected, when the button is clicked, `clicked` is emitted, which in turn will trigger invocation of the connected function.

```cpp
using namespace mousetrap;

// create `Button` instance
auto button = Button();

// create signal handler
auto on_signal_clicked = [](Button& button) -> void {
    std::cout << "clicked" << std::endl;
}

// connect signal handler to signal
button.connect_signal_clicked(on_signal_clicked);
```

\remarks `on_signal_clicked` above is a C++ lambda. If you are unfamiliar with lambda syntax, please consider working through [this tutorial](https://learn.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=msvc-170) before continuing. Lambdas are integral to modern C++ and will be used extensively.

In this section, code snippets will only show the relevant lines. To actually compile and run the code stated here, we need to modify our `main.cpp` from the last section:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(*app);

        // start of snippet
        auto button = Button();
        
        auto auto on_signal_clicked = [](Button& button) -> void {
            std::cout << "clicked" << std::endl;
        }
        
        button.connect_signal_clicked(on_signal_clicked);
        // end of snippet
       
        // add button to window so we can click it
        window.set_child(button); 
        window.present();
    });

    return app.run();
}
```

We can now recompile our executable and execute it. We see that a small window opens. The content area of the window contains our button. Clicking it, we get:

```
clicked
```

## SignalEmitters

`Button`, as most classes in mousetrap, inherits from an abstract class called `SignalEmitter`. An abstract class is any class with a member function that is [pure virtual](https://stackoverflow.com/questions/1306778/virtual-pure-virtual-explained), it can only be inherited from, but not instantiated.

Inheriting from `SignalEmitter` is equivalent to saying "this object can emit signals". Not all objects in mousetrap can emit all signals, but most can.

To be able to emit a signal with ID `clicked`, an object has to inherit from a **signal component** for that specific signals. Signal components have the name `has_signal_<id>`, where `<id>` is replaced with the ID of the signal. For example, `Button` inherits from `has_signal_clicked`, which means it can emit signal `clicked`.

When an object inherits from signal component `has_signal_<id>`, it gains the following functions:

+ `connect_signal_<id>`
+ `disconnect_signal_<id>`
+ `emit_signal_<id>`
+ `set_signal_<id>_blocked`
+ `get_signal_<id>_blocked`

For example, because `Button` inherits from `has_signal_clicked`, it gains the member functions `connect_signal_clicked`, `disconnect_signal_clicked`, `emit_signal_clicked`, `set_signal_clicked_blocked` and `get_signal_clicked_blocked`.

We will learn what each of these function do in this chapter, if we want to reference it or any other signal component, we can check the \link mousetrap::has_signal_clicked corresponding documentation page\endlink.  

### Connecting Signal Handlers

Above, we've already seen how to connect a signal handler to a specific signal of an emitter. What may not have been obvious is that the signal handler, the lambda `on_signal_clicked` in our above code snippet, is required to **conform to a specific signature**.

\collapsible_note_begin{Hint: Signatures}
A functions' **signature** describes a functions' return- and argument types. For example, the function

```cpp
void foo(int32_t i, const std::string& string) -> std::string {
    return std::to_string(i) + " " + string;
}
```
Has the signature `(int32_t, const std::string&) -> void`.  It takes an `int32_t` (32-bit integer) and a `const std::string&` (a const reference to a string) as its arguments. Its return type is `std::string`.

The lambda:
```cpp
auto on_signal_clicked = [](Button* button) -> void {
    std:cout << "clicked" << std::endl;
};
```

Has the signature `(Button*) -> void`, because it takes as its argument a `Button*` (pointer to an instance of `Button`), and returns `void`.

In general, a function with argument types `Arg1_t, Arg2_t, ...` and return type `Return_t` has the signature `(Arg1_t, Arg2_t, ...) -> Return_t`.

\collapsible_note_end

The lambda we connected to signal `clicked` of `Button` had the signature `(Button*) -> void`. The first argument of any signal handler will be a pointer to the signal emitter instance, `Button*` in our case. If there was an hypothetical signal emitter object of type `Foo`, its `connect_signal_clicked` would expect a lambda with the signature `(Foo*) -> void`.

Each signal requires its handler to conform to a specific signature, which may be different between signals. If the handler has the wrong signature, a compiler error will be thrown. This makes it important to know how to check which signal requires which signature. 

### Checking Signal Signature

Working with our example, signal `clicked` of class `Button`, let's say we do not know what function is able to connect to this signal.
To find out, we check the mousetrap documentation. Navigating to the \link mousetrap::Button corresponding page\endlink, we check "detailed description" (by clicking "more" at the very top of the page), which contains the following table:

\signals
\signal_clicked{Button}
\signal_activate{Button}
\widget_signals{Button}

We see that `Button` has a number of additional signals, for now, we just want to know which signature `clicked`, specifically, requires. Checking the table, it says `(Button*, (Data_t)) -> void`. `(Data_t)` refers to an **optional** argument for all signal handlers, meaning a function connected to signal `clicked` can have one of the following signatures:

```cpp
(Button*) -> void
(Button*, Data_t) -> void
```

Where `Data_t` is an arbitrary object type.

### Handing Data to Signal Handlers

While we do get passed a pointer to the signal emitter instance as the first argument to any signal handler, we often need to reference other objects from inside the signal handler. This may be to access a global state, another signal emitter, or a number of unrelated objects. Mousetrap allows for this by adding an optional, arbitrarily typed, *single* argument to the end of any signal handler signature. This object is often referred to as `data`, its type will be called `Data_t`.

Expanding on our previous example, if we want to send a customized message when the user clicks our button, we can change `on_signal_clicked` as follows:

```cpp
auto button = Button();

// add secondary argument of type string
auto on_signal_clicked = [](Button* button, std::string message) -> void {
    std::cout << message << std::endl;
}

// now, `connect_signal_<id>` requires a second argument of type `std::string`
button.connect_signal_clicked(on_signal_clicked, "custom message");
```
```
custom message
```

We changed the signature of `on_signal_clicked` from `(Button*) -> void` to `(Button*, std::string) -> void`, so in this case `Data_t` is `std::string`. Our string `"custom message"` will be stored along with the registered function (a lambda in this case). Any and all objects can be provided as `data`, however, they have to be packaged as exactly one argument (see collapsible note below).

The above can be written more succinctly like so:

```cpp
auto button = Button();
button.connect_signal_clicked([](Button* button, auto message) -> void {
    std::cout << message << std::endl;
}, "custom message");
```

Where the C++ compiler will automatically replace `auto` with the correct datatype.

\collapsible_note_begin{Hint: Grouping Arguments}

Because there is only one `data`, it may seem limiting as to what or how much data we can pass to the signal handlers. This is not true, 
because we can use a simple trick to group any amount of objects into a single argument:

Let's say we want to hand both a `std::string`, `int64_t` and an `std::vector<float>` to `connect_signal_clicked`. To achieve this, we create an [anonymous struct](https://learn.microsoft.com/en-us/cpp/cpp/anonymous-class-types?view=msvc-170) like so:

```cpp
// declare anonymous struct
using clicked_data_t = struct {
    std::string string;
    int64_t number;
    std::vector<float> vector;
};

// objects to hand to `connect_signal_clicked`
auto to_hand_string = "abcdef";
auto to_hand_number = 1234;
auto to_hand_vector = {1.5, 2.5, 3.5, 4.5};

// bundle objects into a single object
auto bundled_data = clicked_data_t{ 
    to_hand_string,
    to_hand_number,
    to_hand_vector
};

button.connect_signal_clicked([](Button* instance, clicked_data_t data){
    // access bundled objects
    auto received_string = data.string;
    auto received_number = data.number;
    auto received_vector = data.vector;
}, bundled_data);
```

Where we used curly braces `{}` when calling the `clicked_data_t` constructor invoke the [list initialization default constructor](https://learn.microsoft.com/en-us/cpp/cpp/initializing-classes-and-structs-without-constructors-cpp?view=msvc-170).

Using this technique, we can bundle up any number of objects and access them using simple `.` syntax from withing the signal handler.

\collapsible_note_end

### Blocking Signal Emission

If we want an object to *not* call the signal handler on signal emission, we have two options:

Using `disconnect_signal_<id>`, we can **disconnect** the signal, which will permanently remove the registered signal handler. This is a quite costly operation and should only rarely be necessary. A much more performant and convenient method is that of **blocking** the signal.

Blocking a signal will prevent invocation of the signal handler, not signal emission itself. This means, for our `Button` example, the user can still click the button. The button still emits the signal, however the connect handler is not called. From an end-user perspective, they will click the button and the animation will play, but nothing happens.

To block a signal, we use `set_signal_<id>_blocked`, which takes a boolean as its argument. We can check whether a signal is currently blocked using `get_signal_<id>_blocked`.

\collapsible_note_begin{Hint: Visually Disabling the Button}
We can inform end-users that a `Button` is no longer clickable using `Button::set_can_respond_to_input(false)`. This will physically prevent the user from clicking the button. We will learn more about functions like these in the [chapter on widgets](04_widgets.md).
\collapsible_note_end

When is blocking necessary? Consider the following use-case:

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

in which we manually emit a signal using `emit_signal_<id>`, which forces signal emission. This function is also automatically called when the user clicks the button, but we can call it at any time.

\collapsible_note_begin{Hint: Static Variables}
We can reference `button_01` and `button_02` from within the lambdas only because both are declared `static`, which means they are only initialized once, and their lifetime is as if they were in global scope (their destructors are not called when the current scope ends). See [here](https://en.cppreference.com/w/cpp/language/storage_duration#Static_local_variables) for more information.

\collapsible_note_end

Here, we declared two buttons. The intended behavior is that if the user clicks either one of the buttons, both buttons emit their signal. Clicking one button should always trigger both, regardless of which button is clicked first.

Running the above code as is and clicking `button_01`, we get the following output:

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

And our application deadlocks. This is of course extremely undesirable, so let's talk through why this happens.

When `button_01` is clicked, it emits signal `clicked`, which invokes the connected signal handler. Going line-by-line through the handler :
+ `button_01`s handler prints `"01 clicked"`
+ `button_01`s handler forces `button_02` to emit its signal through `emit_signal_clicked`
+ `button_02`s handler prints `"02 clicked"`
+ `button_02`s handler forces `button_01` to emit its signal through `emit_signal_clicked`
+ `button_01`'s handler prints `"01 clicked"`
+ etc.

We created an infinite loop.

We can avoid this behavior by **blocking signals** at strategic times:

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

Let's talk through what happens when the user clicks one of the two buttons now, again assuming `button_01` is the first to be clicked:

+ `button_01` invokes its signal handler
+ `button_01`s signal handler prints `01 clicked`
+ `button_01` blocks invocation of its own signal handler
+ `button_01` triggers signal emission on `button_02`
+ `button_02`s signal handler prints `02 clicked`
+ `button_02` blocks invocation of its own signal handler
+ `button_02` attempts to trigger signal emission on `button_01`, **but that button's signal is blocked, so nothing happens**
+ `button_02` unblocks itself
+ `button_01` unblocks itself

```
01 clicked
02 clicked
```

By correctly blocking signals, we get the correct behavior of both buttons being triggered exactly once. Because both buttons unblock themself at the end of the signal handler, after the two buttons are done, everything returns to the way it was before, meaning both buttons can be clicked once again.

To verify this is indeed the resulting behavior, we can compile this modified `main.cpp`

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

With our newfound knowledge about signals, we can recontextualize our `main.cpp`. We recognize that `app`, of type `Application` is a signal emitter. In `main`, we connect to one of its signals, `activate`, which has the signature `(Application*, (Data_t)) -> void`, though we do not supply any `data` as an argument.

`Application` has two signals, `activate` and `shutdown`. The former is invoked when initialization should happen, while `shutdown` is emitted when the application is ready to shut-off, usually once all windows associated with an application are closed.

Checking the \link mousetrap::Documentation documentation page on `Application`\endlink, we see that `shutdown` also requires the signature `(Application*, (Data_t)) -> void`. If we were to connect to it, we would modify our `main.cpp` like so:

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