# Chapter 5: Event Handling

In this chapter, we will learn:
+ How to handle user interaction, such as cursor movement, keyboard presses, or even touchpad stylus movements
+ What an event controller is
+ How to connect a controller to any widget

## Event Model

So far, we were able to react to a user interacting with the GUI through widgets. For example, if the user pressed the left mouse button while hovering over a `mousetrap::Button`, that button will emit the signal `clicked`. While powerful, this is also failry limiting. Widgets have a built-in way of handling user interactions, and we rely on a widget to forward this to our custom signal handlers. What if we want to trigger a button if the user hovers over it as opposed to clicking it? So far, this was impossible, however mousetrap offers a solution for this: event controllers.

### What is an Event?

When the user interacts with a computer in the physical world, they will control some kind of device, for example a mouse, keyboard, touchpad, or maybe even a webcam. This device will send data to the operating system, which processes the data into what is called an event. Pressing a keyboard key is an event, releasing the key is a new event. Moving the cursor by one unit is an event, pressing a stylus against a touchpad is an event, etc. Mousetrap is based on GTK4, which has a very powerful and versatile event abstraction. We don't have to deal with OS-specific events directly, instead it will automatically transform and distribute events for us.

To receive such an event, we need a `mousetrap::EventController`. That specific class is an interface, we cannot instantiate it directly, instead we have to instantiate any of its children. Each child class handles one or more types of events. Each type of event emits it's own signal, for example the event controller handling keyboard events will emit signal `key_pressed` if the user pressed a keyboard key, `key_released` if the user releases the keyboard key, etc. By connecting to these signals, we can trigger any kind of behavior we want, which empowers us to make our own custom widgets.

## Input Focus

Important to avoiding confusion is to understand the concept of [input focus](https://en.wikipedia.org/wiki/Focus_(computing)). In mousetrap and GUIs in general, each widget has a hidden property that indicates whether the widget currently has *focus*. Only exactly one widget can hold focus at a time. If a widget has focus, all it's children hold focus as well. For example, if the focused widget is a box, all widgets inside that focus hold focus. If the focus is just a button, no other widget except that button can hold focus.

Only a widget holding focus can receive input events. Which widget gets focus next is controlled by a somewhat complex heuristic. We do have direct control over this, however:

### Preventing Focus

Only `focusable` widgets can hold focus. We can make a widget focusable by calling `mousetrap::set_is_focusable`. Not all widgets are focusable by default, however any widget that has an intuitive way of interacting with it (such as a `Button`, `Entry`, `Scale`, `SpinButton` etc.) will be focusable by default. 

Widgets that cannot have focusable children and do not usually require user interaction (such as `Label` and `ImageDisplay`) will be unable to grab focus by default.

By setting `mousetrap::Widget::set_can_respond_to_focus` to false, a widget and all of its children are unable to receive focus under any circumstances. Most graphical backends indicate this to the user by making the widget slightly opaqued, it will appear "greyed-out". This should be reserved to temporarily disabling a widget, otherwise `set_is_focusable` is recommended.

### Requesting Focus

`mousetrap::Widget::grab_focus` will make a widget attempt to receive the current focus. If this is impossible, for example because the widget is greyed-out, nothing wil happen. We can check if a widget currently has focus by calling `mousetrap::Widget::get_has_focus`.

Many widgets will automatically grab focus if interacted with, for example if the user places the text cursor inside an `Entry`, that entry will grab focus. If a button is clicked, it will usually grab focus. We can make any widget, even those that do not usually require interaction, grab focus when clicked by setting `mousetrap::Widget::set_focus_on_click` to `true`.

## Connecting a Controller

We will now use our newly gained knowledge about input focus to create our very first event controller: `FocusEventController`. This controller reacts to a widget gainined or loosing input focus. This makes it easy to monitor, as we do not need to ask each widget manually via `get_has_focus`.

After instantiating it, it will not react to any events yet. We need to *connect* it to a widget. For this chapter, we will assume this widget is the toplevel window `state->window`. Because all widgets are contained in a window, this will make the controller connected to it react to all events targeted at any widget (that can have focus). 

We create and connect a `FocusEventController` like so:

\cpp_code_begin
```cpp
auto focus_controller = FocusEventController();
state->window.add_controller(focus_controller);
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end

## Gaining / Loosing Focus: FocusEventController

While the controller is now able to receive events, nothing will happen as a result of them. This is because we haven't connected to the controllers signals yet.

### Signals

`FocusEventController` has two signals:

| id             | signature                                   | emitted when...                                         |
|----------------|---------------------------------------------|---------------------------------------------------------|
| `focus_gained` | `(FocusEventController*, (Data_t)) -> void` | widget which currently does not have focus, gains focus |
| `focus_lost`   | `(FocusEventController*, (Data_t)) -> void` | widget which currently does have focus, looses focus    |

After connecting to these signals:

\cpp_code_begin
```cpp
auto focus_controller = FocusEventController();

focus_controller.connect_signal_focus_gained([](FocusEventController*){
    std::cout << "focus gained" << std::endl;
});

focus_controller.connect_signal_focus_lost([](FocusEventController*) {
    std::cout << "focus lost" << std::endl;
});

state->window.add_controller(focus_controller);
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end

we are able to easily monitor when a widget gains/looses input focus.

---

## Keyboard Keys: KeyEventController

To react to keyboard pressed, we use `mousetrap::KeyEventController`. This is one of the most commonly used controllers, so it is pertinent to talk about keys first.


### Modifiers

Mousetrap is based on GTK4, which splites keys into two groups: regular keys and *modifiers*.

A modifier is one of the following:
+ `Shift`
+ `Control`
+ `Alt`

These keys are not tracked directly, rather they are modifiers to the state of a key. For example, the key `C` can be pressed, not pressed, pressed while only shift is held, pressed while shift and alt are held, etc. To express all these options, mousetrap provides `mousetrap::ModifierState`, which holds information about which modifier keys are pressed.

To query the modifier state, we use static function of `KeyEventController`. A static function is a function that can be invoked without instantiating the class, meaning we do not need a `KeyEventController` to call them. The functions

+ \link mousetrap::KeyEventController::shift_pressed shift_pressed <br>
+ \link mousetrap::KeyEventController::control_pressed control_pressed <br>
+ \link mousetrap::KeyEventController::alt_pressed alt_pressed <br>

query whether the corresponding modifier key is currently down.

### Key Identification

Mousetrap uses two kinds of identifiers for keys, `KeyValue`s and `KeyCode`s. For most purposes, only the first is relevant for most users. `KeyCode` is a 32-bit int
designating the internal identifier of the key, as the users OS designates it. `KeyValue` identifies the key in an platform-independent manner, which is why testing against it should be preferred.

GTK4 provides key identifiers for almost every keyboard layout, a list of identifiers can be found [here](https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h#L38).

### Signals

Now that we know how to identify a code, we can look at the 3 signals of `KeyEventController:

| id                                                                                                                                               | signature                                                                    | emitted when...                                      |
|--------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------|------------------------------------------------------|
| `key_pressed`                                                                                   `(T*, KeyValue, KeyCode, ModifierState) -> bool` | user presses a key that is currently up                                      |                                                      |
| `key_released`                                                                                                                                   | `(T*, KeyValue, KeyCode, ModifierState) -> void`                             | user releases a key that is currently down           | 
 | `modifiers_changed`                                                                                                                              | `(T*, KeyValue key_value, KeyCode key_code, ModifierState modifier) -> bool` | a modifier key is pressed or released\cpp_code_begin |
               
Pressing and releasing a regular key are separate events, they can be used to keep track of whether a key is currently down. For each signal, we get access to the keyvalue, the less relevant key code and the modifier state, which we can query using the `KeyEventController` instance. For example, to test whether the user presses the space key while `state->window` holds input focus:
       
\cpp_code_begin
```cpp
auto key_controller = KeyEventController();
key_controller.connect_signal_key_pressed([](KeyEventController* instance, int32_t key_value, int32_t _, ModifierState state){
    if (key_value == GDK_KEY_space)
        std::cout << "space pressed" << std::endl;
});
state->window.add_controller(key_controller);
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end
                   
Where we choose `_` as the variable name for the key code to signifiy that it will go unused.

Note that `KeyEventController` should not be used to detect whether the user pressed a common key binding, for example `<Control>c`, as mousetrap offers a specialized, more convenient event controller for this purpose:

## Detecting Key Bindings: ShortcutController

We learned in chapter 2 that any funcionality of an application should be wrapped in an action. We can trigger actions using `Action::activate()` or by connecting them to certain widgets, such as `Button`. There is a third way to trigger actions, however, through **shortcuts**.

A shortcut, or keybinding, has the intuitive meaning of "any button combination". A shortcut in mousetrap is more limited however.

### Shortcut Trigger Syntax

Recall that keys in mousetrap are split into two groups, modifiers and non-modifiers. A 
shortcut is a button combination of any number of modifiers, including none, following by exactly one non-modifiers key. A few examples:

+ `a` (that is the `A` keyboard key) is a shortcut
+ `<Control><Shift>plus` (that is the `+` keyboard key) is a shortcut
+ `<Alt><Control><Shift>` is **not** a shortcut, because it does not contain a non-modifiers
+ `<Control>xy` (that is the `x` key *and* the `y` key) is **not** a shortcut, because it contains more than non-modifier key

Shortcuts are represented as a string which has a specific syntax. As seen above each modifier is enclosed in `<``>`. After the group of modifiers, the non-modifier keys identifier is place after the last modifiers `>` (if there is one). Which key has which string representation is viewable [here](https://docs.gtk.org/gdk4/#constants). Some more examples:

+ "Control + C" is `<Control>c`
+ "Alt + LeftArrow" is written as `<Alt>Left`
+ "Shift + 1" is written as `exclam`

That last one need explanation. On most keyboard layouts, to type `!` the user has to press the shift modifier, then press the `1` key. When "Shift + 1" is pressed, mousetrap does not retrieve this keyboard key event as is, instead it receive "exlamation mark without any modifiers". The identification of `!` is `exclam`.

To account for different keyboard layouts it is recommendable to only assign shortcuts that either have no modifiers, or if they have modifiers, only have the `a` to `z` as the non-modifier. If the keyboard layout of the end-user is known, more specialized shortcuts are allowed.

### Assigning a Shortcut to an Action

Unlike `KeyEventController`, `ShortcutEventController` does not have an signals to connect to. Instead, it will monitor or key events a widget it is controlling receives and trigger an *action* if that actions shortcut machtes the key event.

We already learned in the previous section on menus how to assign an action a shortcut. To create a shortcut controller that listens for that shortcut and triggers that action, the following steps are necessary:

1) create an action `action`
2) use `action.set_function` to assign behavior to an action
3) add a shortcut via `action.add_shortcut`
4) register the action with the application
5) instantiate a shortcut controller `shortcut_controller`
6) call `shortcut_controller.add_action`
7) add `shortcut_controller` as an event controller to a widget, usually a `Window`

From that point onwards, anytime the user presses an actions shortcut while the controlled widget has input focus, the action will trigger (unless it was disabled using `Action::set_enabled).

In code:

\cpp_code_begin
```cpp
// 1, 2: initialize action
auto action = Action("example.action");
action.set_function([](){
   std::cout << "triggered" << std::endl; 
});

// 3: assign action a shortcut
action.add_shortcut("<Control>t");

// 4: add action to application
app.add_action(action);

// 5: create shortcut controller
auto shortcut_controller = ShortcutController();

// make shortcut controller listen for shortcut of our action
shortcut_controller.add_action(actoin);

// add shortcut controller to a widget to start receiving events
window.add_controller(controller);
```
\cpp_code_end

\julia_code_begin
```julia
# todo
```
\julia_code_end


---

## Cursor Motion: MotionEventController
                                                                 
Now that we know how to handle keyboard events, we will turn our attention to mouse-based events. There are two types 
of events a mouse can emit, *cursor motion*  and *mouse button presses*. These are handled in different controllers, we 
will deal with cursor motion first.

The corresponding event controller `MotionEventController` has 3 signals:

| id             | signature                                                             | emitted when...                                              |
|----------------|-----------------------------------------------------------------------|--------------------------------------------------------------|
| `motion_enter` | `(MotionEventController*, double x, double y, (Data_t) -> void`       | Cursor enters the allocated space of the controlled widget   |
 | `motion` | `T*, double x, double y) -> void`                                     | Cursor moves inside allocated space of the controlled widget |
| `motion_leave` | `(MotionEventController*, (Data_t)) -> void`                          | Cursor leaves allocated space ot he controlled widget        |

`motion_enter` and `motion` supply the signal handler with 2 addition arguments, `x` and `y`. These are the absolute position of the cursor, in pixels.
For example, if the widget is a `Box` and it has allocated 500x200 pixels on screen, when the cursor is exactly at the center of the box, `x` will have a value of `250`, `y` will have a value of `100`.

Note that `motion` is only emitted when the cursor already entered the controlled widget. We cannot track a cursor that is outside the widget. To track all cursor movement, we would have to connect the controll to a window and make that window cover the entire desktop screen.

To track cursor position as the cursor moves over a window, we can do the following:

\cpp_code_begin
```cpp
auto motion_controller = MotionEventController();
motion_controller.connect_signal_motion([](MotionEventController*, double x, double y){
   std::cout << "Cursor Position: " << x << " " << y << std::endl;  
});
window.add_controller(motion_controller);
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end

---

## Mouse Button Presses: ClickEventController

With cursor movement taken care of, we now turn our attention to handling the other type of mouse event: button presses.

### Signals

The corresponding event controller is called `ClickEventController` which has 3 signals:

| id             | signature | emitted when...                                     |
|----------------|-----------|-----------------------------------------------------|
 | `click_pressed` | `(ClickEventController*, int32_t n_presses, double x, double y, (Data_t)) -> void` | any mouse button that is current up is pressed      |
| `click_released` | `(ClickEventController*, int32_t n_presses, double x, double y, (Data_t)) -> void` | any mouse button that is currently down is released |
| `click_stopped` | `(ClickEventController*, (Data_t)) -> void` | a sequence of multiple clicks stops                 |

Much like with `MotionEventController`, the signals provide the handler with `x` and `y`, the absolute position of the cursor when they click happend, in local widget-space. The first argument for two of the signals, `click_pressed` and `click_released`, `n_presses` is the number of clicks in the current sequence. For example `n_presses = 2` in a `click_pressed` means that this is the second time the mouse button was pressed in sequence. 

`click_stopped` signals that a sequence of clicks has stopped. It may be helpful to consider an example:

Let's say the user clicks the mouse button 2 times total, then stops clicking. This will emit the following events in this order:
1) `click_pressed` (n = 1)
2) `click_released` (n = 1)
3) `click_pressed` (n = 2)
4) `click_released` (n = 2)
5) `click_stopped`

This allows us to easily handle double-clicks without any external function keeping track of them. The delay after which a click sequence stops is system-dependent and usually decided by the window manager, not mousetrap.

### Differentiating Mouse Buttons

`ClickEventController` is one of a few event controllers that inherit from `mousetrap::SingleClickGesture`. This interface provides functionality that lets us distinguish between 
different mouse buttons. mousetrap supports up to 9 different mouse buttons, identified by `mousetrap::ButtonID`:

+ ButtonID::BUTTON_01 is usually the left mouse button or a touchpad click
+ ButtonID::BUTTON_02 is usually the right mouse button
+ ButtonID::ANY is used as a catch-all for all possible mouse buttons
+ ButtonID::BUTTON_03 - BUTTON_09 are subsequent hardware-specific buttons
+ ButtonID::NONE represents none of the above

To check which mouse button was pressed when a signal of `ClickEventController` was emitted, we use `get_current_button`, which returns an id above.
If we only want signals to emitted for certain buttons, we can use `set_only_list_to_button` to restrict the choice of button to one specific button,
or we can call `set_touch_only` to only listen to touch-event

For example, if we want to activate an action when a widget `some_widget` (that is usually not clickable), is clicked twice with the left mouse button, we can do the following:

\cpp_code_begin
```cpp
auto click_controller = ClickEventController();
click_controller.connect_signal_click_pressed([](ClickEventController* controller, int32_t n_presses, double x, doubel y){
    if (n_pressed == 2 and controller->get_current_button() == ButtonID::BUTTON_01)
        std::cout << "double click registered at: " << x << " " << y << std::endl;
});
some_widget.add_controller(click_controller);
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end

`ClickEventController` gives us full control over one or multiple mouse- / touchscreen clicks. There is a more specialized
controller dealing with mouse input, thought.

---

## Long-Presses: LongPressEventController

`LongPressEventController` reacts to a specific used input of pressing a mouse button or touchpad, keeping it at a spefici position and not releasing the
button for a long time. This is called a long press, and it is sometimes used to trigger additional behavior not related
to individual "short" clicks. Not that it is necessary for the cursor to stay in the same position for the entire durtion, if it moves, it is registered as a "pan" gesture, which we will talk about later.

`LongPressEventController` has 2 signals:

| id             | signature                                                          | emitted when...                                                                |
|----------------|--------------------------------------------------------------------|--------------------------------------------------------------------------------|
| `pressed` | `(LongPressEventController*, double x, double y, (Data_t) -> void` | emitted when button is held for long enough duration to register as long press |
| `press_cancelled` | `(LongPressEventController*, (Data_t)) -> void`  | emitted when button is released or moved too much before `pressed` was emitted |                   

Similar to `clicked`, `LongPressEventController` provides use with the location of the cursor.

We can modify how long the user has to hold the button for to register as a long-press by 
multiplying the default amount of time with a factor, which is the argument to `mousetrap::LongPressEventController::set_delay_factor`. Being a factor, a value of `1` means no change, a value of `2` means it will take twice as long and a value of `0.5` means it will take half as long.

`LongPressEventController`, like `ClickEventController`, inherits from `SingleClickGesture`, which allows us to differentiate between different mouse button or touchscreens just like before.

---

## Mousewheel-Scrolling: ScrollEventController

We know how to handle mouse buttons, so we now turn our attention to the mouses scroll wheel. This is usually a designated button on the physical mouse, but maybe also be triggered by the OS through a touchpad or touchscreen gesture, such as the "two finger scroll" common for most tablets.

### Signals

No matter how the scroll was inputted, `ScrollEventController` allows us to react to it with its 3+1 signals:

| id             | signature                                  | emitted when...                                         |
|----------------|--------------------------------------------|---------------------------------------------------------|
| `scroll_begin` | `(ScrollEventController*, (Data_t)) -> void` | scroll starts |
| `scroll` | `(ScrollEventController*, double delta_x, double delta_y, (Data_t)) -> void` | once each frame while the scroll is happening |
| `scroll_end` | `(ScrollEventController*, (Data_t)) -> void` | scroll end |

These three signals are fairly straighforward, when the user starts scrolling `scroll_begin` is emitted. Then, as the user keeps scrolling, `scroll` is emitted every tick to update us on the position of the scroll wheel. Signal `scroll` provides the signal handler with `delta_x` and `delta_y` which is the difference between the current position of the scroll wheel and the position at the start of the scroll (when `scroll_begin` was emitted). This difference is in 2D space, as some mice and most touchscreen allow scrolling in two dimensions. Once the user stops scrolling `scroll_end` is emitted once.

### Kinetic Scrolling

`ScrollEventController` has a fourth signal which reacts to *kinetic scrolling*. Kinetic scrolling is a feature of touchscreens
where the user can quickly scroll on the screen to make the scrolled widget behave as if it had inertia. The scroll distance of the widget is more than the distance the users actual fingers moved. Not all devices support this kind of scrolling, but if they do most of the event sequence will be the same as with regular scroll: `scroll_begin` once, then `scroll` many times, then `scroll_end` once the users finger leaves the touchscreen. Afterwards, while the widget is still scrolling because of intertia, the following signal is emitted:

| id                          | signature                                                                          | emitted when...                                              |
|-----------------------------|------------------------------------------------------------------------------------|--------------------------------------------------------------|
| `kinetic_scroll_decelerate` | `(ScrollEventController*, double x_velocity, double y_velocity, (Data_t)) -> void` | scroll ends but inertia should continue scrolling the widget |

`x_velocity` and `y_velocity` are the read-only current conceptual speed of the scroll. They automatically decay and the "friction" (the speed at which it decelerates) is device-dependent. Either way, for devices/widgets who support kinetic scrolling, we should connect to this signal to monitor the speed of scrolling as it slows down.

`mousetrap::ScrolledWindow`, from the previous chapter, has a default handler that uses kinetic scrolling for a smoother user
experience, for a widget like this connecting to `kinetic_scroll_decelerate` is highly adviced, regardless of the users hardware.

---

## Click-Dragging: DragEventController

In contrast to long-press, which is pressing a mouse button and holding it **while staying place**, a *click-drag* is pressing a mouse button, holding and moving it while it is held down.

Click drag are a common user interaction, for example to drag-and-drop something somewhere else or to change the value of a `Scale`. As such, it has its own dedicated event controller: `DragEventController`.

### Signals

`DragEventController`, just like `ClickEventController` and `LongPressEventController` inherits from `SingleClickGesture`, which exposes the interface to filter certain mouse buttons or touch screen events. Consult the previous sections on how to do this, the same applies here.

`DragEventController` has 3 signals similar to `ScrollEventController` that mark the start, update tick and end of a drag

| id             | signature | emitted when...                         |
|----------------|-----------|-----------------------------------------|
| `drag_begin` | `(DragEventController*, double start_x, double start_y, (Data_t)) -> void` | drag gesture starts                     |
| `drag` | `(DragEventController*, double offset_x, double offset_y, (Data_t)) -> void` | each frame while drag gesture is active |
| `drag_end` | `(DragEventController*, double offset_x, double offset_y, (Data_t)) -> void` | drag ends (user releases the button)    | 

For `drag_begin`, the signal handler is passed two arguments `start_x` and `start_y`. These are the cursor position, in local widget space, of the point where the drag started.

For `drag` and `drag_end` the two arguments have a different meaning. `offset_x` and `offset_y` are the *distance* from the current position of the cursor to where the drag started. So to get the current position of the cursor, we would have to add this offset to `(start_x, start_y)`. This is such a common task
that `DragEventController` has it's own member function `get_start_position` and `get_current_offset` which makes these coordinates available at any time a drag is activate.

To monitor the current cursor position as the user drags over the are of a Window `window`, we can do the following:

\cpp_code_begin
```cpp
auto drag_controller = DragEventController();
drag_controller.connect_signal_drag([](DragEventController* instance, double x_offset, double y_offset){
    
    float cursor_position_x = instance->get_start_position().x + x_offset; 
    float cursor_position_y = instance->get_start_position().y + y_offset;
    
    std::cout << "Cursor Position: " << cursor_position_x << " " << cursor_position_y << std::endl;
});
window.add_controller(drag_controller);
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end

---

## Panning: PanEventController

**Panning*  is similar to dragging, in that the user presses the mouse button or touchscreen, holds that press and moves the cursor to a different location. The difference between panning and dragging is that panning can only occure along exactly one of the two axis: left-right (the x-axis) or top-bottom (the y-axis). This is commenly used to slowly scroll a table horizontally or vertical, or to move an object along one of the axis.

Panning is handled by the appropriately named `PanEventController`, which is the first controller in this chapter that takes an argument to its constructor. We supply a `mousetrap::Orientation` which decideds along which axis it should listen for panning, `HORIZONTAL` for the x-axis, `VERTICAL` for the y-axis.

We can change the orientation after instantiation using `set_orientation`.

After instantiating the event controller, it only has 1 signal :

| id             | signature | emitted when...   |
|----------------|-----------|-------------------|
| `pan`  | `(PanEventController*, PanDirection, double offset, (Data_t)) -> void` | a pan gesture concludes |

We see that signal `pan` takes as its first argument an enum called `PanDirection`. This enum has four possible values, which specify the pan directiony

\copydoc mousetrap::PanDirection

Note that only two of these four can occurr, depending on which axis the controller listens to. 

The second argument to `pan` is `offset`, which gives us the distance from the initial position of the cursor, to the current position. We only have a single coordinate here because panning is locked along one axis, the other axis is constant and it's offset is 0.

Panning is intended to move an object by a given offset, not move it to a specific position. If your particular user interaction needs to know the starting position, `DragEventController` my be more suited for that need.

---

## Touchscreen Pinch-Zoom: PinchZoomEventController

We now move on to touchscreen-only gestured. So far it didn't not matter whether a mouse or the touchscreen was used to move the cursor, however there are a number of two-finger-only gestures that mousetrap recognizes. The first, here, is pinch-zooming, recognized by `PinchZoomController`.

Pinch-zooming is a gesture where the user touchs two fingers onto the touchscreen at the same time, holds both down and then changes the distance between the two. This is usually used to zoom a view in or out to change the size of an object.

Similar to `PanEventController`, `PinchZoomEventController` only has 1 signal:

| id             | signature | emitted when...                                         |
|----------------|-----------|---------------------------------------------------------|
| `scale_changed` | `(PinchZoomEventController*, double scale, (Data_t)) -> void` | distance between the two fingers changes, relative to the initial distance |

The second argument, `scale`, is a relative scale, that is the distance between the two fingers when the gesture started, divided by the distance of the two fingers currently.

For example, a user may start the gesture, at which point `scale` will be `1`. If the want to zoom in, they widen the distance between the finger, increasing `scale` to `2`, which means the distance is now twice as large as when it started. Similarly, pinching the fingers together until `scale_changed` provides a `scale` value of `0.5` means the application should zoom out, as opposed to in.

`scale_changed` is usually emitted once per frame when the gestures starts until it stops, applications should react to every tick as opposed to just the last. This will make the application feel more responsive and create a better user experience. 

To detect whether a user is currently zooming out (pinching) or zoomin in, we could do the following, where `window` is the widget the zoom gesture is being performed on:

\cpp_code_begin
```cpp
auto zoom_controller = PinchZoomController();
zoom_controller.connect_signal_scale_changed([](PinchZoomController*, double scale){
    if (scale < 1)
        std::cout << "zooming in" << std::endl;
    else if (scale > 1)
        std::cout << "zooming out" << std::endl;
});
window.add_controller(zoom_controller);
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end

---

## Touchscreen 2-Finger Rotate: RotateEventController

Another touchscreen-only, two-finger gesture is the 2-finger rotate. For this gesture, the user presses two fingers onto the touchscreen, keep the distance between them relatively steady and rotate both fingers around the center of the line between the two fingers. It sounds complicately when explained this granuarly but many users, including children, will intuitively use this gesture to "turn" something in a circular motion.

This gesture is handled by `RotateEventController`, which has 1 signal:

| id             | signature | emitted when...                                         |
|----------------|-----------|---------------------------------------------------------|
| `rotation_changed` | (RotateEventController*, double angle_absolute, double angle_delta, (Data_t)) -> void | relative angle between the two fingers changes |

It provides two arguments, `angle_absolute` and `angle_delta`.

`angle_absolute` provides the current angle between the two fingers, this angle is determined by a heuristic out of control of the application, this is to allow some fuzzyness in detecting the gesture. 

`angle_delta` is the difference between the current angle (`angle_absolute`) and the angle at the start of the gesture.

Both `angle_absolute` and `angle_delta` are provided in radians, to convert them we can use `mousetrap::Angle` like so, where `window` is the widget the rotation gesture is performed upon:

\cpp_code_begin
```cpp
auto rotation_controller = RotationEventController();
rotation_controller.connect_signal_rotation_changed([](RotationEventController*, double angle_absolute_rad, double angle_delta_rad){
    
    Angle absolute = mousetrap::radians(angle_absolute_rad);
    Angle delta = mousetrap::radians(angle_delta_rad);
    
    std::cout << "Current Angle: " << absolute.as_degrees() << "°" << std::endl;
});
window.add_controller(rotation_controller);
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end

---

## Touchscreen Swipe: SwipeEventController

The last touchscreen-only gesture is **swiping**, which is very similar to a drag, however when swiping the user starts the drag with two or more fingers, and usually completes it quickly. This is usually used to trigger a kinetic scroll, as discussed in the previous section on `ScrollEventController`. A common application for swiping is to switch between two pages of a `Stack`.

Swiping is recognized by `SwipeEventController`, which, much like the other touchscreen-only gesture controllers, only has 1 signal:

| id             | signature | emitted when...                                         |
|----------------|-----------|---------------------------------------------------------|
| `swipe` | `(SwipeEventController*, double x_velocity, double y_velocity, (Data_t)) -> void` | a swipe is recognized |

The signal handler is handed two arguments `x_velocity` and `y_velocity`, which describe the velocity along both the x and y direction. The vector `(x_velocity, y_velocity)` describes the direction of the swipe in 2d space.

To illustrate how to deduce the direction of the swipe, consider this example, where `window` is the widget controlled by the `SwipeEventController`:

\cpp_code_begin
```cpp
auto swipe_controller = SwipeEventController();
swipe_controller.connect_signal_swipe([](SwipeEventController*, double x_velocity, double y_velocity){
    
    // determine direction
    std::string x_direction = "";
    std::string y_direction = "";
    
    if (x_velocity < 0) 
        x_direction = "LEFT";
    else if (x_velocity > 0) // if velocity is == 0, string is not assigned
        x_direction = "RIGHT";
    
    if (y_velocity < 0) 
        y_direction = "UP";
    else if (y_velocity > 0)
        y_direction = "DOWN";
        
    std::cout << "swiping " << y_direction << " " << x_direction < std::endl;
});
window.add(swipe_controller);
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end

---

## Touchpad Stylus: StylusEventController

Common in illustration/animation related apps is the use of a touchpad stylus, this is a pen-like device that lets user control much more than just the position of the cursor, most basic models have a pressure sensor that detects how hard the user pressing the pens tip down, more advanced models have multiple extra buttons, tilt-sensor, etc..

All pens will have a detection mechanism for whether the pen is currently touching, currently not touching, or about to touch its track pad. `StylusEventController`, the
event controller handling these events, has 4 signals for this purpose:

### Signals

| id             | signature                                                        | emitted when...                                                                 |
|----------------|------------------------------------------------------------------|---------------------------------------------------------------------------------|
| `stylus_down` | `(StylusEventController*, double x, double y, (Data_t)) -> void` | stylus that is currently not touching the trackpad, touches it                  |
| `motion` | `(StylusEventController*, double x, double y, (Data_t)) -> void` | stylus in range of trackpad detection moves                                     |
| `stylus_up` | `(StylusEventController*, double x, double y, (Data_t)) -> void` | stylus that is currently touching the trackpad is lifter, no longer touching it |
| `proximity` | `(StylusEventController*, double x, double y, (Data_t)) -> void` | stylus enters range of trackpad without touching or not touching it             |

We recognize signal `motion` from `MotionEventController`. It behaves exactly the same, the two arguments `x` and `y` it forwards to the signal handle contain the current position of the cursor. 

Actually, `x` and `y` are provided by all 4 signals of `StylusEventController`, all of them also describe the position of the cursor.

### Querying Stylus Axis

None of the signals provide information about the before mentioned additional sensors of a stylus, such as pressure, tilte, etc.. This is because not all stylus devices come with these sensor, but all of them can communicate the pens position to the device.

Values of other sensors are called *axis*, the enum `DeviceAxis` describes all axes recognized by mousetrap:

\copydoc mousetrap::DeviceAxis

We can query the value of each axis using `mousetrap::StylusEventController::get_axis_value`. This function will return 0 if the axis is not present, to check whether a device has a specific axis we use `mousetrap::StylusEventController::has_axis`. This latter function is preferred to test whether an axis is present, as a valid, present axis may return a value of 0 (for example the pressue axis when a pen is not touching the touchpad).

### Querying Stylus Tool

Some stylus' have a "mode" function, where the user can choose between differen pen modes. This is driver specific, and not all devices support this feature. For those that do, we can use `StylusEventController::get_device_type` to check which mode is currently selected. The recognized modes are:

\copydoc mousetrap::ToolType

For devices without this feature, `ToolType::UNKNOWN` will be returned.

\cpp_code_begin
```cpp
```
\cpp_code_end

\julia_code_begin
```julia
# TODO
```
\julia_code_end

---