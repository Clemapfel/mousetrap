# Chapter 5: Event Handling

In this chapter, we will learn:
+ How to manually handle user interaction
+ What an event controller is
+ How to connect a controller to any widget

## Event Model

So far, we were able to react to a user interacting with the GUI through widgets. For example, if the user pressed the left mouse button while hovering over a `Button`, that button will emit the signal `clicked`. This is also fairly limiting. While widgets have a built-in way of handling user interactions, so far we relied on the widget on forwarding this to our custom signal handlers. What if we want to trigger a button if the user hovers over it as opposed to clicking it? So far, this was impossible. The solution to this? Event controllers.

### What is an Event?

When the user interacts with a computer in the physical world, they will control some kind of device, for example a mouse, keyboard, touchpad, or webcam. Through a driver, the device will send data to the operating system, which processes the data into what is called an **event**. Pressing a keyboard key is an event, releasing the key is a new event. Moving the cursor by one unit is an event, pressing a stylus against a touchpad is an event, etc. Mousetrap is based on GTK4, which has a very powerful and versatile event abstraction. We don't have to deal with OS-specific events directly, instead, the GTK backend automatically transform and distribute events for us, regardless of the operating system.

To receive events, we need a `mousetrap::EventController`, which is an abstract class, meaning we cannot instantiate it directly. Instead, we deal with one or more of its children. Each child class handles a conceptual type of event. To react to events, we connect to specific signals of the event controllers. For example, the event controller handling keyboard events will emit signal `key_pressed` if the user pressed a keyboard key, `key_released` if the user releases the keyboard key, etc. By connecting to these signals, we can trigger any kind of behavior we want. This, along with compound widgets, empowers us to create completely custom widgets and user interactions.

## Input Focus

Important to understand when dealing with events is the concept of [**input focus**](https://en.wikipedia.org/wiki/Focus_(computing)). In mousetrap (and GUIs in general), each widget has a hidden property that indicates whether the widget currently **holds focus**. If a widget holds focus, all its children hold focus as well. For example, if the focused widget is a box, all widgets inside that focus hold focus. If the focus is just a button, no other widget except that button can hold focus.

**Only a widget holding focus can receive input events**. Which widget gets focus next is controlled by a somewhat complex heuristic, usually using things like which window is on top and where the user last interacted with the GUI. For most situations, this mechanism works very well, we can choose to directly control this, however.

### Preventing Focus

Only `focusable` widgets can hold focus. We can make a widget focusable by calling `Widget::set_is_focusable`. Not all widgets are focusable by default, however, any widget that has a  way of interacting with it (such as a `Button`, `Entry`, `Scale`, `SpinButton` etc.) will be focusable by default. Furthermore, any widget that has a focusable child will be made focusable. Widget not focusable by default include `Label` and `ImageDisplay`.

By setting `Widget::set_can_respond_to_input` to false, a widget and all of its children are unable to receive focus under any circumstances. Most graphical backends indicate this to the user by making the widget slightly translucent, it will appear "greyed-out". This should be reserved to temporarily disabling a widget, otherwise `set_is_focusable` is recommended.

### Requesting Focus

`Widget::grab_focus` will make a widget attempt to receive the current focus. If this is impossible, for example because the widget is greyed-out, nothing will happen. We can check if a widget currently has focus by calling `Widget::get_has_focus`.

Many widgets will automatically grab focus if interacted with, for example, if the user places the text cursor inside an `Entry`, that entry will grab focus. If a button is clicked, it will usually grab focus. We can make any widget, even those that do not usually require interaction, grab focus when clicked by setting `Widget::set_focus_on_click` to `true`.

## Connecting a Controller

Using our newly gained knowledge about focus, we'll create our first event controller: `FocusEventController`. This controller reacts to a widget gained or loosing input focus. Using it, we can easily monitor whether a widget has focus.

After creating an event controller, it will not yet react to any events. We need to **add the controller to a widget**. For this chapter, we will assume that this widget is the top level window.

We create and connect a `FocusEventController` like so:

```cpp
auto focus_controller = FocusEventController();
window.add_controller(focus_controller);
```

## Gaining / Loosing Focus: FocusEventController

With controller now being able to receive events, we can connect to its signals.

### Signals

`FocusEventController` has two signals:

\signals
\signal_focus_gained{FocusEventController}
\signal_focus_lost{FocusEventController}

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

window.add_controller(focus_controller);
```
\cpp_code_end

we are able to easily monitor when a widget gains/looses input focus.

---

## Keyboard Keys: KeyEventController

Monitoring focus is rarely necessary. An almost ubiquitous application of events is to react to keyboard pressed. This is accomplished by  `KeyEventController`. Before we can talk about the controller itself, we need to talk about **keyboard keys**.

### Keys & Modifiers

Mousetrap is based on GTK4, which splits keys into two groups: regular keys and **modifiers**.

A modifier is one of the following:
+ `Shift`
+ `Control`
+ `Alt`

\note Additional modifiers include `AltGr`, `Meta` and `Win`. These are keyboard-layout and/or OS-specific. See [here](https://docs.gtk.org/gdk4/flags.ModifierType.html) for more information.

Modifiers are not tracked directly, rather, they *modify* the state of another key. For example, the key `C` can be pressed, not pressed, pressed while only `Shift` is held, pressed while `Shift` and `Àlt` are held, etc. To express all these options, mousetrap provides `mousetrap::ModifierState`, which holds information about which modifier keys are currently pressed.

To query the modifier state, we use static functions of `KeyEventController`. A static function is a function that can be invoked without instantiating the class, meaning we do not need a `KeyEventController` to call them. The functions

+ \link mousetrap::KeyEventController::shift_pressed shift_pressed <br>
+ \link mousetrap::KeyEventController::control_pressed control_pressed <br>
+ \link mousetrap::KeyEventController::alt_pressed alt_pressed <br>

query whether the corresponding modifier key is currently down. We access the modifier state from within signal handlers, as we will see soon.

### Key Identification

Mousetrap uses two kinds of identifiers for keys, `KeyValue`s and `KeyCode`s. For most purposes, only the first is relevant. `KeyCode` is a 32-bit integer
designating the internal identifier of the key, as the user's OS designates it. `KeyValue` identifies the key in a platform-independent manner, which is why it should be preferred.

GTK4 provides key identifiers for almost every keyboard layout, a list of identifiers can be found [here](https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h#L38).

### Signals

Now that we know how to identify a key, we can look at the 3 signals of `KeyEventController`:

\signals
\signal_key_pressed{KeyEventController}
\signal_key_released{KeyEventController}
\signal_modifiers_changed{KeyEventController}

We see that pressing and releasing a non-modifier key are separate events. These can be used to track whether a key is currently down.

For each signal, we get access to the key value, the less relevant key code and the modifier state, which we can query as stated earlier `KeyEventController`.

As an example, to test whether the user presses the space key (while `window` holds input focus):

```cpp
auto key_controller = KeyEventController();
key_controller.connect_signal_key_pressed([](KeyEventController* instance, int32_t key_value, int32_t, ModifierState state){
    if (key_value == GDK_KEY_space)
        std::cout << "space pressed" << std::endl;
});
window.add_controller(key_controller);
```

Where we choose to ignore the second `int32_t`, which is the os-specific key code.

Note that `KeyEventController` **should not be used to detect whether the user pressed a combination of keys**, for example `Control + C`. Mousetrap offers a purpose-built event controller for keybindings, which uses the `Action` architecture.

## Detecting Key Bindings: ShortcutEventController

We learned in the [chapter on actions](03_actions.md) that any functionality of an application should be wrapped in an `Action`. We can trigger actions using `Action::activate()` or with certain widgets, such as `Button` or `PopoverMenu`. Before, it was stated we can assign each action a shortcut using `Action::add_shortcut`. This shortcut will be automatically displayed in menus. There is a third way to trigger actions using shortcuts that does not depend on a widget: `ShortcutEventController`

A shortcut, or keybinding, has the intuitive meaning of "any button combination". A shortcut in mousetrap is more limited, however.

### Shortcut Trigger Syntax

Recall that keys in mousetrap are split into two groups, modifiers and non-modifiers. A
shortcut is a combination of **any number of modifiers**, including none, followed by **exactly one non-modifiers**. A few examples:

+ `a` (that is the `A` keyboard key) is a shortcut
+ `<Control><Shift>plus` (that is the `+` keyboard key) is a shortcut
+ `<Alt><Control><Shift>` is **not** a shortcut, because it does not contain a non-modifier
+ `<Control>xy` (that is the `X` key *and* the `Y` key) is **not** a shortcut, because it contains more than one non-modifier key

Shortcuts are represented as strings which has a specific syntax. As seen above, each modifier is enclosed in `<``>`. After the group of modifiers, the non-modifier keys identifier is placed after the last modifiers `>` (if there is one). Some more examples:

+ "Control + C" is `<Control>c`
+ "Alt + LeftArrow" is written as `<Alt>Left` (sic, `L` is capitalized)
+ "Shift + 1" is written as `exclam`

That last one requires explanation. On most keyboard layouts, to type `!` the user has to press the shift modifier key, then press the `1` key. When "Shift + 1" is pressed, mousetrap does not receive this keyboard key event as-is, instead, it receives a single key event for the `!` key, with no modifiers. The identification of `!` is `exclam`, hence why "Shift + 1" is written as `exclam`.

\collapsible_note_begin{Hint: Checking Key Identifiers}

An example on how to look up the key identifier as a string will be performed here.

Let's say we want to write the shortcut "Control + Space". We know that we can write "Control" as `<Control>`. Next, we navigate to https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h#L384, which, in line 384, says `GDK_KEY_space`. The identifier name is the last part of the constant name as a string, minus the `"GDK_KEY_"`. So for this constant `GDK_KEY_space`, the id is `space`. Therefore, we write "Control + Space" as `<Control>space`. For the left arrow, the constant is named `GDK_KEY_Left`, therefore its identifier is `Left`.

One more obscure example, to write "Alt + Beta Key", that is the `β` key on the somewhat rare Greek keyboard layout, we find the constant named `GDK_KEY_Greek_BETA` in [line 1049](https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h#L1049). Erasing `GDK_KEY_` again, the key's string identifier is `Greek_BETA`. "Alt + Beta Key" is therefore written as `<Alt>Greek_BETA`

If we make an error and use the wrong identifier, a warning is shown at runtime.

\collapsible_note_end

### Assigning a Shortcut to an Action

Unlike `KeyEventController`, `ShortcutEventController` does not have a signal to connect to. Instead, it will monitor key events automatically. If it recognizes a keyboard shortcut, it will trigger the corresponding action.

We already learned in the previous section on menus how to assign an action to a shortcut. To make a `ShortcutEventController` listen for the keybindings of an action, we have to call `ShortcutEventController::add_action`:

```cpp
// 1. initialize action
auto action = Action("example.action");
action.set_function([](){
   std::cout << "triggered" << std::endl; 
});

// 2: assign action a shortcut
action.add_shortcut("<Control>t");

// 3: create shortcut controller
auto shortcut_controller = ShortcutController();

// 4. make shortcut controller listen for shortcut of our action
shortcut_controller.add_action(actoin);

// 5. add shortcut controller to a widget to start receiving events
window.add_controller(controller);
```

While this may seem like a lot of steps, it is multiple times less work than scanning signals of `KeyboadEventController` manually. Furthermore, if we change the keyboard shortcut of an action, `ShortcutEventController` will automatically respect this change.

---

## Cursor Motion: MotionEventController

Now that we know how to handle keyboard events, we will turn our attention to mouse-based events. There are two types
of events a mouse can emit, **cursor motion**  and **mouse button presses**. These are handled in different controllers.

For cursor motion, the event controller is `MotionEventController`, which has 3 signals:

\signals
\signal_motion_enter{MotionEventController}
\siganl_motion{MotionEventController}
\signal_motion_leave{MotionEventController}

`motion_enter` and `motion` supply the signal handler with 2 addition arguments, `x` and `y`. These are the absolute position of the cursor in widget-space, in pixels.

\collapsible_note_begin{Hint: Widget-Space Coordinates}
In mousetrap, the coordinate system is anchored in the top left corner of the widgets' allocation. This means the top-left-most pixel of a widgets' allocated space is coordinate `(0, 0)`. The top-right-most pixel is `(0, width)`, where `width` is the absolute size of the widget, in pixels. The bottom-right-most pixel is `(width, height)`, while the bottom-left one is `(0, height)`.

If `motion` supplies us with the value of `x = 250` and `y = 400`, and our widget has the size `500x800`, then `(x, y)` is at the center of the widgets' allocation. We can check a widgets current allocated size using \link mousetrap::Widget::get_allocation `Widget::get_allocation` \endlink.

\collapsible_note_end

Note that `motion` is only emitted when the cursor already entered the controlled widget. We cannot track a cursor that is outside the widget. To track all cursor movement, we would have to connect the controller to a window and make that window cover the entire screen.

We can track movement of the cursor as it moves over `window` like so:

```cpp
auto motion_controller = MotionEventController();
motion_controller.connect_signal_motion([](MotionEventController*, double x, double y){
   std::cout << "Cursor Position: " << x << " " << y << std::endl;  
});
window.add_controller(motion_controller);
```
---

## Mouse Button Presses: ClickEventController

With cursor movement taken care of, we now turn our attention to handling the other type of mouse event: button presses.

A mouse button is any button on a mouse... no, really. Mice are hugely varied, with some having exactly one button, while some gaming mice have 6 or more buttons. If the user uses no mouse at all, for example when choosing to control the app with a trackpad or touchscreen, touchpad "taps" will be registered as left-mouse-button-presses.

We track mouse button pressed with `ClickEventController` which has 3 signals:

\signals
\signal_click_pressed{ClickEventController}
\signal_click_released{ClickEventController}
\signal_click_stopped{ClickEventController}

Much like with `MotionEventController`, the signals provide the handler with `x` and `y`, the absolute position of the cursor when they click happened, in widget-space.

The first argument for two of the signals, `click_pressed` and `click_released`, `n_presses` is the number of clicks in the current sequence. For example, `n_presses = 2` in a `click_pressed` means that this is the second time the mouse button was pressed in sequence, `click_stopped` signals that a sequence of clicks has stopped. It may be helpful to consider an example:

Let's say the user clicks the mouse button 2 times total, then stops clicking. This will emit the following events in this order:
1) `click_pressed` (n_pressed = 1)
2) `click_released` (n_pressed = 1)
3) `click_pressed` (n_pressed = 2)
4) `click_released` (n_pressed = 2)
5) `click_stopped`

This allows us to easily handle double-clicks without any external function keeping track of them. The delay after which a click sequence stops is system-dependent and usually decided by the user's operating system, not mousetrap.

### Differentiating Mouse Buttons

`ClickEventController` is one of a few event controllers that inherit from `SingleClickGesture`. \link mousetrap::SingleClickGesture This interface\endlink provides functionality that lets us distinguish between different mouse buttons. Mousetrap supports up to 9 different mouse buttons, identified by `mousetrap::ButtonID`:

+ ButtonID::BUTTON_01 is usually the left mouse button (or a touchpad tap)
+ ButtonID::BUTTON_02 is usually the right mouse button
+ ButtonID::ANY is used as a catch-all for all possible mouse buttons
+ ButtonID::BUTTON_03 - BUTTON_09 are additional mouse-model-specific buttons
+ ButtonID::NONE is none of the above

To check which mouse button was pressed when a signal of `ClickEventController` was emitted, we use `ClickEventController::get_current_button`, which returns an ID as stated above.
If we only want signals to be emitted for certain buttons, we can use `ClickEventController::set_only_listens_to_button` to restrict the choice of button. `ClickEventController::set_touch_only` filters all click events except those coming from touch devices.

If we want to activate an action when a widget `some_widget`, is clicked twice with the left mouse button, we can do the following:

```cpp
auto click_controller = ClickEventController();
click_controller.connect_signal_click_pressed([](ClickEventController* controller, int32_t n_presses, double x, double y){
    if (n_pressed == 2 and controller->get_current_button() == ButtonID::BUTTON_01)
        std::cout << "double click registered at: " << x << " " << y << std::endl;
});
some_widget.add_controller(click_controller);
```

While `ClickEventController` gives us full control over one or multiple clicks, there is a more specialized
controller dealing for a similar but different purpose: *long presses*.

---

## Long-Presses: LongPressEventController

`LongPressEventController` reacts to a specific sequence of events, called **long press**. A long press happens when the users pressed a mouse button, keeps the button depressed without moving the cursor. After enough time has passed, `LongPressEventController` will emit its signals:

\signals
\signal_pressed{LongPressEventController}
\signal_press_cancelled{LongPressEventController}

Similar to `clicked`, `LongPressEventController` provides use with the location of the cursor.

The delay after which a long press is registered as such can be changed with `LongPressEventController::set_delay_factor`. This is a factor, not an absolute value. Setting the factor to `1` means no change compared to the default, a factor of `2` means it will take twice as long before a long press is recognized, a value of `0.5` means it will take half as long.

`LongPressEventController`, like `ClickEventController`, inherits from `SingleClickGesture`, which allows us to differentiate between different mouse button or touchscreens just as before.

To check whether the user has held down the left mouse button, we do:

```cpp
auto long_press = LongPressEventController();
long_press.connect_signal_pressed([](LongPressEventController* controller, double x, double y){
if (controller->get_current_button() == ButtonID::BUTTON_01)
    std::cout << "long press registered at " << x << " " << y << std::endl;
});
window.add_controller(long_press);
```
---

\todo rest:

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