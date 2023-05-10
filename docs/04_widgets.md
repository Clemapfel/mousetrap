# Chapter 4: Widgets

In this chapter, we will learn:
+ What a widget is
+ Properties that all widgets share
+ All types of widgets supplied by mousetrap
+ How to create compound widgets

---

## What is a widget?

When creating a GUI, widgets are the central element to any and all applications. In general, a widget is anything that can be rendered on screen. Often, widgets have a way of interacting with them, for example, in the cas of `Button`, the widget is rendered to screen as a filled rectangle and the user can click on it to emit a signal.

## Widget Signals

All widgets are signal emitters, but not all signal emitters are widgets. As such, all widgets share a number of signals:

\signals
\signal_realize{Widget}
\signal_unrealize{Widget}
\signal_destroy{Widget}
\signal_hide{Widget}
\signal_show{Widget}
\signal_map{Widget}
\signal_unmap{Widget}

From this point onwards, any widget also has these signals. For example, if we want to trigger behavior when a `Button` is first shown, we connect to that signal like so:

```cpp
auto button = Button();
button.connect_signal_show([](Widget* self)
{
    std::cout << "button shown" << std::endl;
});
```

Where the first argument of this signal, as state in the table above, is a `Widget*` (**not** a `Button*`). Signals that all widgets share use `Widget*`. For class-specific signals, such as signal `clicked` for `Button`, the first argument is `Button*`.

If we are unsure about the signatures, recall that any class has the exact signatures listed in the detailed description, right here in the documentation.

## Widget Properties

All widgets share common properties that govern how they behave when the container they are in changes size, or at what position they will appear.

### Size Request

When a widget is shown, it will dynamically **allocate its size**. This governs the amount of space a widget will take up on screen. This size is determined programmatically, based on the widgets own properties, as well as the properties of its children or parents.

\collapsible_note_begin{Widget Children & Parents}

Some widgets can contain other widgets. For example, `Window` has a single **child**. `Button`, also has a child, which is the label of the button. If widget `A` is inside of widget `B`, for example by calling `A.set_child(B)`, then we say B is As **child**, and A is Bs **parent**.

Each widget that is not a window has exactly one parent. The number of children varies per widget, `Window` and `Button` has exactly on child, while other widgets may have a any number of children, or no children at all.

\collapsible_note_end

One property used for determinening the widgets size is its **size request**. The size request is the minimum amount of space the widget has to take up. For example, a widget displaying for letters, where each of the letter has a fixed size, would implicitely need to allocate at least as much space as those letters take up. For widgets without any children, the default size request is `{0, 0}`. 

We can manually control the size request property of any widget by calling `Widget::set_size_request`, which takes a 2d-vector, the target width and height of the widget. This is is also sometimes called **size-hinting**.

### Accessing Widget Size

We can query information about a widgets size using multiple functions.

`Widget::get_allocation` returns a \link mousetrap::Rectangle rectangle\endlink, which holds the current widgets position on screen and current size, in pixels. This size may or may not be equal to what we size-hinted the widget to.

```cpp
auto current_size = widget.get_allocation().size;
```

To access a widgets size request, we use `Widget::get_size_request`.

```cpp
auto size_hint = widget.get_size_request();
```

If we have not yet sizehinted the widget, this size will be `{0, 0}`.

Lastly, we can query the widgets **minimum possible size**, that is the size that is has to occupy no matter what, using `Widget::get_preferred_size`. The returned object of this function als holds information about the size the widget would allocate naturally, if given infinite amount of space. We access the minimum and preferred size like so:

```cpp
auto minimum_size = widget.get_preferred_size().minimum_size;
auto preferred_size = widget.get_preferred_size().natural_size;
```

### Margin

Any widget has four margins: `start`, `end`, `top` and `bottom`. Usually, these correspond to empty space left, right, above, and below the widget respectively. Margins are unaffected by the widgets original size and expansion, they are simply added to the corresponding side of the widget.

We use `Widget::set_margin_start`, `Widget::set_margin_end`, `Widget::set_margin_top` and `Widget::set_margin_bottom` to control each individual margin. Mousetrap also provides the convenience functions `Widget::set_margin_horizontal` and `Widget::set_margin_vertical`, which sets both `start` and `end', or `top` and `bottom` at the same time respectively.

Lastly `Widget::set_margin` sets all fours margins to the same value.

Margins should be used extensively, as they make a UI look more professional and asthetically pleasing. A good rule of thumb is that for a 1920x1080 display, the minimum **margin unit** should be 10 pixels. That is, all margins should be a multiple of 10. If the display has a higher or lower resolution, the margin unit should be adjusted. 

### Expansion

In previous chapters, we may have noticed that if we scale the window from our previous `main.cpp`s, the widget inside may scale along with it. This is called **expansion**, which is a property of all widgets.

We an specify wether a widget should expand along the vertical or horizontal axis using `Widget::set_expand_horizontally` and `Widget::set_expand_vertically`, which take a single boolean as its argument. `Widget::set_expand` sets both properties to the same value at the same time.

### Alignment

Widget **alignment** governs how one or multiple widget behave when expansion is disabled and they are inside another widget whos allocated area is larger than that of the child widget.

An example, a button sizehinted to 100x100 pixels has expansion disabled (`set_expand` was set to `false`). It has a margin of 0 and is placed inside a window. When we scale the window, the button will not change size. Alignment, then, governs where in the window the button is positioned. We can set alignment for the horizontal and vertical axis separately, using `Widget::set_horizontal_alignment` and `Widget::set_vertical_alignment`, which both take an enum value of type \a{Alignment} as their only argument. 

The button from this example would be located as follows:

| Vertical Alignment | Horizontal Alignment | Resulting Position  |
|--------------------|----------------------|---------------------|
| `START`            | `START`              | top left corner     |
| `START`            | `CENTER`             | top center          |
| `START`            | `END`                | top right corner    |
| `CENTER`           | `START`              | center left         |
| `CENTER`           | `CENTER`             | center              |
| `CENTER`           | `END`                | center right        |
| `END`              | `START`              | bottom left corner  |
| `END`              | `CENTER`             | bottom center       |
| `END`              | `END`                | bottom right corner |

Using this, sizehinting and expansion, we can fully control how and where widgets allocate size on screen.

### Visibility 

Once a widgets allocated area enters the visible area on screen, it is shown (which we can track using the `shown` signal). Once it leaves the visible area, it is hidden. `Widget::set_visible` sets the widgets visibility based on the boolean argument. Other than this, it behaves exactly as `Widget::show` and `Widget::hide`. That is, the widget will dissappear and its allocated space will be zero.

A way to make a widget invisiblie without hiding it, is to control its **opacity**. By setting the opacity to 0 using `Widget::set_opacity`, all other properties of the widget including its size allocation and interactability are kept, all that changes is that the graphical part of it becomes fully transparent.

### Cursor Type

Each widget has a property that sets what shape the cursor will have when it entres the widgets allocated area. By default, this is a simple pointer. We can manually choose the cursor type using `Widget::set_cursor` to one of the following values, all of which are part of the enum \a{CursorType}:

| `CursorType` value  | Appearance                                                       |
|---------------------|------------------------------------------------------------------|
| `NONE`              | no visible cursor                                                |
| `POINTER`           | small arrow, this is the default value widgets                   |
| `DEFAULT`           | default system cursor, usually same as `POINTER`                 |
| `HELP`              | questionmark, instructs user to open a help dialog               |
| `CONTEXT_MENU`      | pointer with `...` to indicate clicking will open a context menu |
| `PROGRESS`          | pointer with a small "currently loading" icon                    |
| `WAIT`              | instructs user to wait for an action to finish                   |
| `CELL`              | used when interacting with a table                               | 
| `CROSSHAIR`         | cross-hair shaped cursor, used for precise selections            |
| `TEXT`              | text-entry cursor                                                | 
| `MOVE`              | four-pointed arrow, used to move around object                   |
 | `GRAB`              | open hand, not yet grabbing                                      |
| `GRABBING`          | closed hand, currently grabbing                                  |
| `ALL_SCROLL`        | four-direction scroll                                            |
| `ZOOM_IN`           | lens, usually with a plus icon                                   |
| `ZOOM_OUT`          | lens, usually with a minus icon                                  |
| `COLUMN_RESIZE`     | left-right arrow                                                 | 
| `ROW_RESIZE`        | up-down arrow                                                    |
| `NORTH_RESIZE`      | up arrow                                                         | 
| `EAST_RESIZE`       | right arrow                                                      | 
| `SOUTH_RESIZE`      | bottom arrow                                                     | 
| `WEST_RESIZE`       | left arrow                                                       | 
| `NORTH_EAST_RESIZE` | top-right arrow                                                  | 
| `SOUTH_EAST_RESIZE` | bottom-right arrow                                               | 
| `SOUTH_WEST_RESIZE` | bottom-left arrow                                                | 
| `NORTH_WEST_RESIZE` | top-left arrow                                                   | 

We can also choose a fully custom cursor with `Widget::set_cursor_from_pixel`. We will learn more about \a{Image} in the [chapter on image and sound](07_image_and_sound.md). Until then, this is how we can use an image on disk as a cursor:

```cpp
auto widget = //...
widget.set_cursor_from_image(Image("/path/to/image.png"), {0, 0});
```

Where `"/path/to/image.png"` is the absolute path of the image, `widget` is the widget for whom we want to set the custom cursor. `{0, 0}` is the cursor anchors, in pixels, which controls how the image is aligned.

### Tooltip

Each widget can optionally have a **tooltip**. This is a little window that opens when the cursor hovers over a widgets allocated area for enough time. This is usually reserved to a simple text message, which we can set directly using `Widget::set_tooltip_text`:

```cpp
auto open_file_button = Button();
open_file_button.set_tooltip_text("Click to Open");
```
\image html widget_tooltip_text.png

If we want to use something more complex than just simple text, we can register an arbitrarily complex widget as a tooltip by calling `Widget::set_tooltip_widget`.

### Tick Callback

\todo this section is not yet complete

---

## Window

Windows are central to any application. \a{Window}s constructor takes an `Application*` because the two are linked. Once all windows of an application are closed, the application exits.

Windows are widgets, though they occupy somewhat of a special place. A `Window` cannot be the child of any other widget, it is **top-level**.
This means that any and all widgets are children of a `Window`, usually indirectly, as a `Window` can only contain a single widget.

We set this widget using `Window::set_child`. In most cases, the signel child will be a container widget that, in turn, can contain other widgets.

`Window` has three signals (on top of those inherited from `Widget`) that we can connect to:

\signals
\signal_close_request{Window}
\signal_activate_default_widget{Window}
\signal_activate_focused_widget{Window}

`activate_default_widget` is emitted when the default widget is activated. This is a widget designated as such via `Window::set_default_widget`. If the user presses the enter key while the window is shown, this widget will be activated. This is useful for small message dialogs that have, for example, an "ok" and "cancel" button. The user can press enter or escape to close the dialog, instead of manually having to click either button.

`activate_focused_widget` is activated when the widget that currently *holds input focus* is activated. We will learn what input focus is in the [chapter on event handling](05_events.md).

Lastly, we have signal `close_request`, which requries some explanation.

### Close Request

When the window manager, which is the part of the users operating system that deals with window layout and lifetime, request a window to close, the window does not immediately close. Instead, `Window` emits `close_request`, which we can connect to. If no signal handler is connected, the window simply closes. This is the default behavior. If we do connect a signal handler, we can not only trigger custom behavior, but we can also **prevent closing of the window**.

We recall the signature of `close_request` to be `(Window*, (Data_t)) -> WindowCloseRequestResult`. This return type, \a{WindowCloseRequestResult} is an enum with two values `ALLOW_CLOSE` and `PREVENT_CLOSE`. Depending on which of the two is returned, the window does or does close when requested.

### Window Properties

Other than the signals, we can set basic properties of the window.

#### Title

`Window::set_title` sets the name displayed in the windows header bar. By default, this name will be the name of the application. We can choose to hide the title by simply calling `set_title("")`.

#### Modality & Transience

When dealing with multiple windows, we can influence the way two windows interact with each other. Two of these interactions are determined by whether a window is **modal** and whether it is **transient** for another window.

By setting `Window::set_model` to true, if the window is revealed all other windows of the application will be deactivated, preventing interaction with them. The most common use-case for this is for dialogs, for example if the user requests to close the application, it's common to open a small dialog requesting the user to confirm exiting the application. While the dialog is shown, the main window is essentially paused until the user chooses an action.

Using `Window::set_transient_for`, we can make sure a window will always be on top of another. `A.set_transient_for(B)` will make it so while A overlaps B, A will always be on top. This is again useful for message dialogs, if the dialog is not transient for the main window it could appear "behind" the main window, being essentially invisible to the user.

---

## Label

In contention for being *the* most used widget, `Label`s are important to understand. A `Label` displays static text, it's initialized as one would expect:

```cpp
auto label = Label("text!")
```

To change a labels text after initialization, we use `Label::set_text`. By default, a label will be a single-line of text, but it can also be used to display multi-line text, in which case we have some additional options to control formatting.

### Justify Mode

Justification how words are destributed along the horizontal axis. There are 5 modes in total, which are part of the enum \a{JustifyMode}:

\image html text_justify_left.png "JustifyMode::LEFT"<br>
\image html text_justify_center.png "JustifyMode::CENTER" <br>
\image html text_justify_right.png "JustifyMode::RIGHT"<br>
\image html text_justify_fill.png "JustifyMode::FILL" <br>

Where the fifth mode is `JustifyMode::NONE` which arranges all text in exactly one line.

### Wrapping

Wrapping determines at which part of the text display a line break is inserted. For wrapping to happen at all, the `JustifyMode` has to not be set to anything **other** than `JustifyMode::NONE`.

| `JustifyMode` | meaning                                                 |
|------|---------------------------------------------------------|
| `NONE`  | no wrapping                                             | 
| `ONLY_ON_WORD` | line will only be split between two words               |
| `ONLY_ON_CHAR` | line will only be split between syllables, adding a `-` |
| `WORD_OR_CHAR` | line will be split between words and/or syllables        |

Where `WORD_OR_CHAR` is the default.

### Ellipsize Mode

If a line is too long for the available space and wrapping is disabled, **ellipzing** will take place. The corresponding enum has four possible value:

| `EllipsizeMode` | meaning | example |
|------|--------|---------|
| `NONE`  | text will not be ellipsize | "Humane mousetrap engineer" |
| `START` | text starts with `...`, showing only the last few words | "...engineer" |
| `END` | text end with `...`, showing only the first few words | "Humane mousetrap..."|
| `MIDDLE` | "..." in the center, shows start and beginnig | "Humane...engineer" |

### Markup

Labels support **markup**, which allows use to change properties about individual words or characters in a way very similar to html. Markup in mousetrap uses [pango attributes](https://docs.gtk.org/Pango/pango_markup.html), which allows for the following markup properties:

| `tag`        | Label String              | Result                  |
|--------------|---------------------------|-------------------------|
| `b`          | `<b>bold</b>`             | <b>bold</b>             |
| `i`          | `<i>italic</i>`           | <i>italic</i>           |
| `u`          | `<u>underline</u>`        | <u>underline</u>        |
| `s`          | `<s>strikethrough</s>`    | <s>strikethrough</s>    |
| `tt`         | `<tt>inline_code</tt>`    | <tt>inline_code</tt>    |
| `small`      | `<small>small</small>`    | <small>small</small>    |
| `big`        | `<big>big</big>`          | <big>big</big>          |
| `sub`        | `x<sub>subscript</sub>`   | x<sub>subscript</sub>   |
| `sup`        | `x<sup>superscript</sup>` | x<sup>superscript</sup> |
| `&#` and `;` | `&#129700;`               | 🪤                    | 

Where in the last row, we used the [decimal html code](https://www.compart.com/en/unicode/U+1FAA4) for the mousetrap emoji that unicode provides.

\note All `<`, `>` will be parsed as style tags event when escaped with `\`. To prevent this, simply us `&lt;` (less-than) and `&gt;` (greater-than) instead of `<` and `>`.

For things like color, other fonts, and many more options, [consult the pango documentation](https://docs.gtk.org/Pango/pango_markup.html).

---

## Box

Boxes are the simplest multi-widget container in mousetrap and are used extensively in almost any application. A box aligns its children horizontally or vertically, depending on whether we pass \a{Orientation::HORIZONTAL} or \a{Orientation::VERTICAL} to its constructor. We can change a boxes orientation any time after construction using `set_orientation`. 

We can add widgets to the start, end, and after any other widget already inside box using `Box::push_front`, `Box::push_back` and `Box::insert_after`, respectively: 

```cpp
auto box = Box(Orientation::HORIZONTAL);

auto start = Label("|start");
auto end = Label("end|");
auto center = Label("|middle|");

// add to start of box
box.push_front(start);

// add to end of box
box.push_back(end);

// add right after start
box.insert_after(center, start);
```

\image html box_start_center_end.png

Between any two children is an optional space, which we can specify using `Box::set_spacing`. This spacing is unrelated to the margins of its child widgets and will be applied between any two consecutive children, but not before the very first and after the very last child.

## CenterBox

\a{CenterBox} is a container that has exactly three children, situated around a centered child. `CenterBox` prioritizes keeping this child centered at all costs, making it an better choice for a layout where this is desired than other containers.

We use `CenterBox::set_start_child`, `CenterBox::set_center_child` and `CenterBox::set_end_child` to choose the corresponding child widget.

```cpp
auto center_box = CenterBox(Orientation::HORIZONTAL);
center_box.set_center_child(Button());
center_box.set_start_child(Label("start"));
center_box.set_end_child(Label("end"));
```

\image html center_box.png

`CenterBox` is orientable, meaning it also supplies `set_orientation` to choose the preferred layout.

---

## Separator

Perhaps the simplest widget is `Separator`. It simply fills its allocated area with a solid color:

```cpp
auto separator = Separator();
separator.set_margin(20);
separator.set_expand(true);
window.set_child(separator);
```

\image html separator_example.png

This widget is often used as a background to another widget, to fill empty space, or as en element visually separating two sections. Often we want to have the separator be a specific thickness. To create a horizontal line, we would usually sizehint it: `separator.set_size_request({0, 3})` will create a horizontal line, 3 pixels thick.

---

## ImageDisplay

`ImageDisplay` is used to display static images. It works by taking image data in RAM, deep-copying it into the graphics card memory,
then using it to display the image on screen.

Assuming we have an image at the absolute path `/resources/image.png`, we can create an `ImageDisplay` like so:

```cpp
auto display = ImageDisplay();
display.create_from_file("/resources/image.png");
```

The following image formats are supported by `ImageDisplay`:

| Format Name             | File Extensions |
|-------------------------|-----------------|
| PNG                     | `.png`  |
| JPEG                    | `.jpeg` `.jpe` `.jpg`  |
| JPEG XL image           | `.jxl`  |
| Windows Metafile        | `.wmf` `.apm`  |
| Windows animated cursor | `.ani`  |
| BMP                     | `.bmp`  |
| GIF                     | `.gif`  |
| MacOS X icon            | `.icns`  |
| Windows icon            | `.ico` `.cur`  |
| PNM/PBM/PGM/PPM         | `.pnm` `.pbm` `.pgm` `.ppm`  |
| QuickTime               | `.qtif` `.qif`  |
| Scalable Vector Graphics | `.svg` `.svgz` `.svg.gz`  |
| Targa                   | `.tga` `.targa`  |
| TIFF                    | `.tiff` `.tif`  |
| WebP                    | `.webp`  |
| XBM                     | `.xbm`  |
| XPM                     | `.xpm`  |

Afterwards, we cannot change the contents of `ImageDisplay` directly. If the file on disk changes, `ImageDisplay` remains unchanged. If we want to update `ImageDisplay`, we should call `create_from_file` with the same path again.

By default, `ImageDisplay` behaves just like any other wigdet, in that it scales freely, which, in case of a raster-based image, may distort the image. To prevent this, we can call `display.set_expand(false)` which prevents expansion of the widget, then sizehint it like so:

```cpp
display.set_size_request(display.get_size());
```

Where `ImageDisplay::get_size` returns the original resolution of the image it was created from. If we want to allow scaling (expansion) but want to keep the aspect ratio òf an `ImageDisplay` fixed, we can use the container widget `AspectFrame`, which we will learn about later in this section.

---

## Button

We've seen `Button` in the previous chapter, it is one of the simplest ways for a user to interact with an application.

`Button` has the following signals:

\signals
\signal_activate{Button}
\signal_clicked{Button}

Where physically clicking the button both emits `activate` and `clicked`, while calling `Widget::activate` only emits `activate`, not clicked.

To change the label of a button, we use `Button::set_child`. This will usually be a `Label` or `ImageDisplay`, though any arbitrary widget can be used as a child this way.

Other than the child widget, we can customize a buttons look. `Button::set_has_frame` will change wether the button has a texture outline to it, while `Button::set_is_circular` changes the button to a fully rounded appearance:

\image html button_types.png

Where the above shown buttons have following properties:

| Button | set_has_frame | set_is_circular |
|--------|---------------|-----------------|
| 01  | true | false |
| 02 | false | false |
| 03 | true | true |

\how_to_generate_this_image_begin
```cpp
auto normal = Button();
normal.set_child(Label("01"));

auto no_frame = Button();
no_frame.set_has_frame(false);
no_frame.set_child(Label("02"));

auto circular = Button();
circular.set_is_circular(true);
circular.set_child(Label("03"));

auto box = CenterBox(Orientation::HORIZONTAL);;
box.set_start_child(normal);
box.set_center_child(no_frame);
box.set_end_child(circular);

box.set_margin(75);
window.set_child(box);
```
\how_to_generate_this_image_end


Buttons are ideal to trigger run-through `Action`s, which are action that do a single thing, then immediately exit. For `Button`s, it is usually preferred to create an `Action` and use `Button::set_action` to associate the two, over connecting to one of the signals. The reasons for this were illustrated in the [chapter on actions](03_actions.md).

---

## ToggleButton

`ToggleButton` is a specialized form of `Button`. It supports most of `Button`s methods/signals, including signal `clicked`, `set_child`, `set_has_frame` and `set_is_circular`.
Unique to `ToggleButton` is that, if clicked, the button will remain pressed. When clicked again, it returns to being unpressed. Anytime the internal state of the `ToggleButton` changes, the `toggled` signal will be emitted.

\signals
\signal_toggled{ToggleButton}
\signal_activate{ToggleButton}
\signal_clicked{ToggleButton}

In this way, `ToggleButton` can be used to track a boolean state. To check whether the button is currently toggled, we use `ToggleButton::get_active`, which returns a `true` if the button is currently pressed, `false` otherwise.
\todo figure for untoggled, toggled

---

## CheckButton

`CheckButton` is almost identical to `ToggleButton` in function, but not appearance. `CheckButton` is an empty box in which a checkmark appears when it is toggled. Just like before, we query whether it is pressed by calling `CheckButton::get_active`.

\signals
\signal_activate{CheckButton}
\signal_toggled{CheckButton}

`CheckButton` can be in one of three states, as opposed to two, all of which are represented by the enum \a{CheckButtonState}. The button can either be active, inactive, or **inconsistent**. This changes the appearance of the button:

\image html check_button_states.png

\how_to_generate_this_image_begin
```cpp
auto active = CheckButton();
active.set_state(CheckButtonState::ACTIVE);
auto active_box = Box(Orientation::VERTICAL);
active_box.push_back(active);
active_box.push_back(Label("active"));

auto inconsistent = CheckButton();
inconsistent.set_state(CheckButtonState::INCONSISTENT);
auto inconsistent_box = Box(Orientation::VERTICAL);
inconsistent_box.push_back(inconsistent);
inconsistent_box.push_back(Label("inconsistent"));

auto inactive = CheckButton();
inactive.set_state(CheckButtonState::INACTIVE);
auto inactive_box = Box(Orientation::VERTICAL);
inactive_box.push_back(inactive);
inactive_box.push_back(Label("inactive"));

for (auto* button : {
   &active,
   &inconsistent,
   &inactive
})
   button->set_horizontal_alignment(Alignment::CENTER);

auto box = CenterBox(Orientation::HORIZONTAL);;
box.set_start_child(active_box);
box.set_center_child(inconsistent_box);
box.set_end_child(inactive_box);

box.set_margin(75);
window.set_child(box);
```
\how_to_generate_this_image_end

Note that `CheckButton::get_active` will only return true if the current state is specifically `CheckButtonState::ACTIVE`. Otherwise, `get_active` will return false. `toggled` is emitted whenever the state changes, regardless of which state the `CheckButton` was in.

---

## Switch

As the last widget intended to convey a boolean state to the user, we have `Switch`, which has the appearance of a light-switch. It can be clicked or dragged in order to change its state. `Switch` does not emit `toggled`, instead we connect to the `activate` signal, which is emitted anytime the switch is flicked to either side. 

\signals
\signals_activate{Switch}

\image html switch_states.png

\how_to_generate_this_image_begin
```cpp
auto active = Switch();
active.set_active(true);
auto active_box = Box(Orientation::VERTICAL);
active_box.push_back(active);
active_box.push_back(Label("active"));

auto inactive = Switch();
inactive.set_active(false);
auto inactive_box = Box(Orientation::VERTICAL);
inactive_box.push_back(inactive);
inactive_box.push_back(Label("inactive"));

for (auto* s : {&active, &inactive})
{
    s->set_horizontal_alignment(Alignment::CENTER);
    s->set_margin(10);
}

auto box = CenterBox(Orientation::HORIZONTAL);;
box.set_start_child(active_box);
box.set_end_child(inactive_box);

box.set_margin(75);
state->main_window.set_child(box);
```
\how_to_generate_this_image_end

---

## Adjustment

From widgets conveying a boolean state, we'll move to widgets conveying a discrete number. These let the user choose a value from a **range**, which, in mousetrap, is represented by a signal emitter called \a{Adjustment}. 

In general, a range has a **lower and upper value**. For example, the range `[0, 2]` has the `lower` of `0` and `upper` of `2`. A second property is the **step increment**, which is the minimum difference between two adjacent values in the range. For example, if our range is still `[0, 2]` and the step increment is `0.5`, then that includes the numbers `{0, 0.5, 1, 1.5, 2}`. If the step increment is `0.01`, `[0,2]` includes the numbers  `{0, 0.01, 0.02, ..., 1.98, 2.00}`.

Turning to the actual `Adjustment` class, it has four properties

+ `lower`: Lower bound of the range
+ `upper`: Upper bound of the range
+ `increment`: step increment
+ `value`: Current value, in `[lower, upper]`

For example, expressing the previous range of `[0, 2]` with step increment `0.5`, we create an `Adjustment` like so:

```cpp
auto adjustment = Adjustment(
    1,      // value
    0,      // lower
    2,      // upper
    0.5     // increment    
);
```

We usually do not need to create our own adjustment, rather it is provided by a widget. Notable about this is that the widget and its adjustment **are automatically kept in synch**. If we change any property of the adjustment, the widget will change its appearance accordingly, which we will see an example of shortly.

Adjustment has two signals:

\signals
\signal_value_changed{Adjustment}
\signal_properties_changed{Adjustment}

We can connect to `value_changed` to monitor the value of an `Adjustment`, and thus whatever widget is controlled by it.

`properties_changed` is emitted when one of upper, lower or step increment changes, usually by calling `Adjustment::set_upper`, `Adjustment::set_lower` and/or `Adjustment::set_increment`.

---

## SpinButton

\image html spin_button.png

\how_to_generate_this_image_begin
```cpp
auto horizontal = SpinButton(0, 2, 0.5);
horizontal.set_orientation(Orientation::HORIZONTAL);
horizontal.set_value(1);

// pad horizontal spin button with invisible separators above and below
auto horizontal_buffer = CenterBox(Orientation::VERTICAL);
horizontal_buffer.set_start_child(Separator(0));
horizontal_buffer.set_center_child(horizontal);
horizontal_buffer.set_end_child(Separator(0));

auto vertical = SpinButton(0, 2, 0.5);
vertical.set_orientation(Orientation::VERTICAL);
vertical.set_value(1);

auto box = CenterBox(Orientation::HORIZONTAL);;
box.set_start_child(horizontal_buffer);
box.set_end_child(vertical);

box.set_margin_horizontal(75);
box.set_margin_vertical(40);
window.set_child(box);
```
\how_to_generate_this_image_end

`SpinButton` is used to pick an exact value from a range. The user can click the rectangular area and manually enter a value, or they can increase or decrease the current value by the step increment of the `SpinButton`s adjustment by pressing the plus or minus button. 

We supply the properties of the range in `SpinButton`s constructor:

```cpp
// create SpinButton with range [0, 2] and increment 0.01
auto spin_button = SpinButton(0, 2, 0.5)
```

If we want to check what the properties of a `SpinButton`s range, we can either query `SpinButton::get_adjustment`, or we can get the values from the spin button directly using `SpinButton::get_value`, `SpinButton::get_lower`, etc.

`SpinButton` has two signals:

\signals
\signal_value_changed{SpinButton}
\signal_wrapped{SpinButton}

Only the latter of which needs explanation, as we recognize `value_changed` from `Adjustment`. When the user reaches one end of the `SpinButton`s range, which, for a range of `[0, 2]` would be either the value `0` or `2`, if the user attempts to increase or decrease the value further, nothing will happen. However, if we set `SpinButton::set_can_wrap` to `true`, the value will wrap all the way around. If the user attempts to increase the value while it is `2`, it will jump to `0`, and vice-versa.

---

## Scale

\image html scale_no_value.png
\how_to_generate_this_image_begin
```cpp
auto horizontal = Scale(0, 2, 0.5);
horizontal.set_orientation(Orientation::HORIZONTAL);
horizontal.set_value(1);
horizontal.set_size_request({200, 0});

auto vertical = Scale(0, 2, 0.5);
vertical.set_orientation(Orientation::VERTICAL);
vertical.set_value(1);
vertical.set_size_request({0, 200});

auto box = CenterBox(Orientation::HORIZONTAL);;
box.set_start_child(horizontal);
box.set_end_child(vertical);

box.set_margin_horizontal(75);
box.set_margin_vertical(40);
state->main_window.set_child(box);
```
\how_to_generate_this_image_end

\a{Scale}, just like `SpinButton` is a widget that allows a user to choose a value from an `Adjustment`. The user chooses the value by click-dragging the knob on the scale. In this way, it is usually harder to pick an exact decimal value on a scale. We can aid in this by displaying the exact value next to the scale, using `Scale::set_should_draw_value`:

\image html scale_with_value.png
\how_to_generate_this_image_begin
```cpp
auto horizontal = Scale(0, 2, 0.5);
horizontal.set_orientation(Orientation::HORIZONTAL);
horizontal.set_value(1);
horizontal.set_size_request({200, 0});
horizontal.set_should_draw_value(true);

auto vertical = Scale(0, 2, 0.5);
vertical.set_orientation(Orientation::VERTICAL);
vertical.set_value(1);
vertical.set_size_request({0, 200});
vertical.set_should_draw_value(true);

auto box = CenterBox(Orientation::HORIZONTAL);;
box.set_start_child(horizontal);
box.set_end_child(vertical);

box.set_margin_horizontal(75);
box.set_margin_vertical(40);
state->main_window.set_child(box);
```
\how_to_generate_this_image_end


`Scale`supports most of `SpinButton`s functions, including querying information about its underlying range.

---           

## ScrollBar

Similar to `Scale`, `ScrollBar` is used to pick a value on a floating-point scale. It is often used as a way to choose which part of a widget should be shown on screen. For an already-automated way of doing this, see `ScrolledWindow`. 

---

## LevelBar

\a{LevelBar} is used to display a fraction, often use to indicate the level of something, for example the the volume of a playback device.

To create a level bar, we need to specify the minimum and maximum value of the range we wish to display. We can then set the current value using `LevelBar::set_value`, which takes an absolute value. The resulting fraction is computed automatically based on the upper and lower limit we supplied to the constructor.

```cpp
// create level for range [0, 2]
auto level_bar = LevelBar(0, 2);
level_bar.set_value(1.0); // set to 50%
```

The bar will be oriented horizontally by default, but we can call `set_orientation` and change this.

Once the bar reaches 75%, it changes color:

\image html level_bar.png

\how_to_generate_this_image_begin
```cpp
auto box = Box(Orientation::VERTICAL);
box.set_spacing(10);
box.set_margin(10);

size_t n_bars = 5;
for (size_t i = 0; i < n_bars+1; ++i)
{
    float fraction = 1.f / n_bars;

    auto label = Label(std::to_string(int(fraction * 100)) + "%");
    label.set_size_request({50, 0});

    auto bar = LevelBar(0, 1);
    bar.set_value(fraction);
    bar.set_expand_horizontally(true);

    auto local_box = Box(Orientation::HORIZONTAL);
    local_box.push_back(label);
    local_box.push_back(bar);
    box.push_back(local_box);
}

window.set_child(box);
```
\how_to_generate_this_image_end

The bar can also be used to display a discrete value (a range only consisting of integers), in which case the bar will be shown segmented. We can set the `LevelBar`s mode using `set_mode`, which takes either `LevelBarMode::CONTINUOUS` or `LevelBarMode::DISCRETE` as its argument.

---

## Progressbar

A specialized case of indicating a continous value is that of a **progress bar**. A progress bar is used to show how much of a task is currently complete, this is most commonly used during a multi-second loading animation. As more and more resources are loaded, the progress bar fills, which communicates to the user how long they will have to wait.

`ProgressBar` is a widget built for this purpose. It does not have an upper or lower bound, it is alwasy assumed to be show values in the range `[0, 1]`. We can set the current fraction using `ProgressBar::set_fraction`. `ProgressBar` has a special animation trigger, which makes the bar "pulse", which is supposed to show the end user that the bar has changed fractions. We can trigger this animation by simply calling `ProgressBar::pulse`. Note that this does not change the currently displayed fraction of the progress bar.

\image html progress_bar.png

\how_to_generate_this_image_begin
```cpp
auto progress_bar = ProgressBar();
progress_bar.set_fraction(0.47);
progress_bar.set_vertical_alignment(Alignment::CENTER);
progress_bar.set_expand(true);

auto label = Label("47%");
label.set_margin_end(10);

auto box = Box(Orientation::HORIZONTAL);
box.set_homogeneous(false);
box.push_back(label);
box.push_back(progress_bar);
box.set_margin(10);

window.set_child(box);
```
\how_to_generate_this_image_end

## Spinner

To signal progress where we do not have an exact fraction, we can use the \a{Spinner} widget. This simply displays an animated spinning icon, communicating progress. Using `Spinner::set_is_spinning`, we can control whether the animation is currently playing or not.

\image html spinner.png

\how_to_generate_this_image_begin
```cpp
auto spinner = Spinner();
spinner.set_is_spinning(true);
window.set_child(spinner);
```
\how_to_generate_this_image_end

---

## Entry

We've learned how to choose a widget allowing a boolean and discrete value to be inputed, so we will now move on to inputting text. This is central to many desktop applications, as such, \a{Entry}, the widget used for single-line text entry, may be one of the most important widgets there is.

Entry is an area that, when clicked, allows the user to type freely. This current text is stored in an internal text buffer. We can its contents with `Entry::get_text`. `Entry::set_text`, conversely, overwrites this buffer with a custom message.

While we could control the widget of an `Entry` by using size request like any other widget, a less manual way to do this is `Entry::set_max_length`, which takes an integer, which represents the number characters that the prompts should make space for.

`Entry` supports "password mode", which is when each character typed is replaced with a dot, or otherwise made unreadable. This is such that a person looking over another persons shoulder can read a sensitive prompt, such as a message or password. We can enter and exit this mode using `Entry::set_text_visible`, where `false` is password-mode and `true` is the regular mode.

\image html entry_password_mode.png

\how_to_generate_this_image_begin
```cpp
auto clear = Entry();
clear.set_text("text");

auto password = Entry();
password.set_text("text");
password.set_text_visible(false);

auto box = Box(Orientation::VERTICAL);
box.set_spacing(10);
box.push_back(clear);
box.push_back(password);

box.set_margin_horizontal(75);
box.set_margin_vertical(40);
window.set_child(box);
```
\how_to_generate_this_image_end

Lastly, `Entry` is *activatable*. Users usually do this by pressing the enter key while the cursor is inside the entry. This does not cause any behavior initially, but we can connect to the `activate` signal of `Entry` to trigger behavior. 

Other than `activate`, `Entry` has one more signal `text_changed`, which is emitted whenever the internal buffer changes, including when set programmatically using `set_text`.

\signals
\signal_activate{Entry}
\signal_text_changed{Entry}

Note that the user cannot insert a newline character using the enter key. As such `Entry` should exclusively be used for text prompts which are exactly one line. For multi-line text entry, we should use the next widget instead.

## TextView

While we can technically input a newline character into `Entry`, it is not possible to display two lines at the same time. For this purpose, we use \a{TextView}. It supports a number of basic text-editor features, including **undo / redo**, which  are triggered by the user pressing Control + Z or Control + Y respectively. We can also trigger this behavior manually by calling `TextView::undo` / `TextView::redo`.

Much like `Label`, we can set how the text aligns horizontally using `TextView::set_justify_mode`. To further customize how text is displayed, we can choose the **internal margin**, which is the distance between the frame of the `TextView` and the text inside of it. `TextView::set_left_margin`, `TextView::set_right_margin`, `TextView::set_top_margin` and `TextView::set_bottom_margin` allow us to customize this freely.

`TextView` does **not** have the `activate` signal, pressing enter while the cursor is inside the widget will simply create a new line. Instead, it has the following signals, where `text_changed` behaves exactly like it does with `Entry`:

\signals
\signal_text_changed{TextView}
\signal_undo{TextView}
\signal_redo{TextView}

---

## Dropdown

Sometimes, we want users to be able to pick a value from a **set list of values**, which may or may not be numeric. \a{DropDown} allows for this, though its operation is somewhat unconventional.

When the dropdown widget is clicked, a window presents the user with a list of items. The user can click on any of these, at which point the dropdown will invoke the corresponding function for that item, which is set using \{DropDown::push_back}. This function takes three arguments:

+ **list label**: Widget displayed inside the dropdown window
+ **selected label**: Widget displayed once one of the items is selected
+ **callback**: Function with signature `(DropDown*, (Data_t)) -> void`, which is invoked when a selection is made

We usually want both labels to be an actual `Label`, though technically any widget can be used as the list and selected label.

As an example, consider the following snippet:

```cpp
auto dropdown = DropDown();
dropdown.push_back(
    Label("List Label"), // list label
    Label("Selected Label"), // selected label
    [](DropDown*) { std::cout << "selected" << std::endl; } // callback
);
```

\image dropdown_hello_world.png

Here, we created a dropdown and added a single item. The item has the list label `"List Label"`, which is displayed in the dropdown window when the user is making a selection. Once the user clicks this label, the callback will be invoked, which in this case simply prints `"selected"`. Lastly, we specified `"Selected Label"` to be the widget displayed once an element was selected. 

In praxis, we would want the callback to mutate some global property to keep track of which item is selected. Alternatively, we can query which item is currently selected by calling `DropDown::get_selected`. This function returns an **item id**, which is optained when we call `push_back`:

```cpp
auto dropdown = DropDown();
auto id_01 = dropdown.push_back(Label("01"), Label("Option 01"), [](DropDown*){});
auto id_02 = dropdown.push_back(Label("02"), Label("Option 02"), [](DropDown*){});

// check if selected item is 01
bool item_01_selected = dropdown.get_selected() == id_01;
```

Where `[](DropDown*){}` is a lambda that simply does nothing (but still conforms to the `(DropDown*, (Data_t)) -> void` signature).

Using item ids or the callback, we can trigger custom behavior whenever the user changes their selection of dropdown items.

---

## Frame

\a{Frame} is a purely cosmetical widget that displays whatever child we choose using `Frame::set_child` in a frame with a small border and rounded corners:

\image html frame_no_frame.png

\how_to_generate_this_image_begin
```cpp
auto left = Separator();
auto right = Separator();

auto frame = Frame();
frame.set_child(right);

for (auto* separator : {&left, &right})
{
    separator->set_size_request({50, 50});
    separator->set_expand(false);
}

auto box = CenterBox(Orientation::HORIZONTAL);
box.set_start_child(left);
box.set_end_child(frame);

box.set_margin_horizontal(75);
box.set_margin_vertical(40);
window.set_child(box);
```
\how_to_generate_this_image_end

Using `Frame::set_label_widget`, we can furthermore choose a label widget, which will be displayed above the child widget of the frame. This will usually be a `Label`, though `set_label_widget` accepts any kind of widget.

---

## AspectFrame

Not to be confused with `Frame`, \{AspectFrame} adds no graphical element to its singular child. Instead, child widgets added with `AspectFrame::set_child` will be forced to stay in a certain **aspect ratio**, their width to height ratio will stay constant, regardless of the size allocation of the `AspectFrame`s parent. 

We choose the aspect ratio either in the constructor or with `AspectFrame::set_ratio`, both of which accept a float ratio calculated as `width / height`. For example, if we want to force a widget to keep an aspect ratio of 4:3:

```cpp
auto child_widget = // ...
auto aspect_frame = AspectFrame(4.f / 3) // 4:3 aspect ratio
aspect_frame.set_child(child_widget);
```

Where we wrote `4.f / 3` instead of `4 / 3` because in C++, the latter would trigger [integer divison](https://en.wikipedia.org/wiki/Division_(mathematics)#Of_integers) resulting in a ratio of `1`, instead of the intended `1.333...`.

---

## Overlay

So far, all widget containers align widget such that they do not overlap. This is also why the margin property may not be negative. To render a widget on top of another, we have to use \a{Overlay}.

`Overlay` has one "base" widget, which is at the conceptual bottom of the overlay. We set this widget using `Overlay::set_child`. Then, we can add any number of widgets on top of the base widget using `Overlay::add_overlay`:

\image html overlay_two_buttons.png

\how_to_generate_this_image_begin
```cpp
 auto lower = Button();
lower.set_horizontal_alignment(Alignment::START);
lower.set_vertical_alignment(Alignment::START);

auto upper = Button();
upper.set_horizontal_alignment(Alignment::END);
upper.set_vertical_alignment(Alignment::END);

for (auto* button : {&lower, &upper})
    button->set_size_request({50, 50});

auto overlay = Overlay();
overlay.set_child(Separator());
overlay.add_overlay(lower);
overlay.add_overlay(upper);

auto aspect_frame = AspectFrame(1);
aspect_frame.set_child(overlay);

aspect_frame.set_margin(10);
window.set_child(aspect_frame);
```
\how_to_generate_this_image_end

By default, `Overlay` will allocate exactly as much space as the base widget (set with `set_child`) does. If one of the overlaid widgets takes up more space than the base widget, it will be truncated. We can change this by supplying a second argument to `add_overlay`, which is a boolean indicated whether the overlay widget should be included in the entire containers size allocation, that is, if the overlay widget is larger than the base widget, the overlay resize itself such that the entire overlay widget is shown:

```cpp
overlay.add_overlay(overlay_widget, true); // resize if overlay_widget allocates more space than child
``` 

When one interactable widget is shown partially overlapping another, only the top-most widget will be interactable. If we want the user to be able to access a lower layer, we need to make any widget on top non-interactable, either by calling `Widget::hide` or `Widget::set_can_respond_to_input(false)`.

---

## Revealer

While not technically necessary, animations can improve user experience drastically. Not only do they add visual style, they can hide abrupt transitions or small loading times. As such, animations should be in any advanced GUI designers repertoire.

One of the most common applications for animations is that of hiding or showing a widget. So far, when we called `Widget::hide` or `Widget::show`, the widget appears instantly, one frame after the function was called. This works but when showing a large widget, other widgets around it will want to change their size allocation, which can make for a less than ideal transition.

To address this, mousetrap offers \a{Revealer}, which plays an animation to reveal or hide a widget.
`Revealer` always has exactly one child, set with `Revealer::set_child`. `Revealer` itself has no graphical element, it acts just like a `Box` with exactly one child. `Revealer` only plays its animation when a widget is shown or hidden. To trigger this animation and change whether the child widget is currently visible, we call `Revealer::set_revealed` which takes a boolean. If the widget goes from hidden to shown or shown to hidden, the animation will automatically play.

Once the animation is done, signal `revealed` will be emitted:

\signals
\signal_revealed{Revealer}

Using this, we can trigger our own behavior, for example to update a widgets display or trigger additional animations.

### Transition Animation

We have control over the kind and speed of the transition animation. By calling `Revealer::set_transition_duration`, we can set the exact amount of time an animation should take. For example, to set the animation duration to one second:

```cpp
auto revealer = Revealer();
revealer.set_child(// ...
revealer.set_transition_duration(seconds(1));
```

Where `seconds` returns a `mousetrap::Time`. 

Apart from the speed, we also have a choice of animation **type**, represented by the enum \a{RevealerTransitionDuration}. Animations include a simple crossfade, sliding, swinging, or `NONE`, which instantly shows or hides the widget, as if the revealer was not present. For more informations on the type of animation, see the \link mousetrap::RevealerTransitionType related documentation page\endlink.

---

## Expander

\a{Expander} is similar to `Revealer`, in that is also has exactly one child widget and it shows / hides the widget. Unlike `Revealer`, there is no animation attached to `Expander`. Instead, it hides the widget behind a collapsible label:

\image html expander.png

\how_to_generate_this_image_begin
```cpp
auto child = Label("[expanded child]");
child.set_horizontal_alignment(Alignment::START);
child.set_margin(5);
child.set_margin_start(15);

auto expander = Expander();
expander.set_child(child);
expander.set_label_widget(Label("Expander"));

auto frame = Frame();
frame.set_child(expander);
frame.set_margin(50);
window.set_child(frame);
``` 
\how_to_generate_this_image_end

We set the `Expander`s child widget with `Expander::set_child`, Other than this, we also need to specify a label widget, which is the widget shown next to the small arrow. To specify it, we use `Expander::set_label_widget`.

Note that `Expander` should not be used for large nested list, for example those displaying a filesystem tree. A purpose-built widget for this exists called `ListView`, of which we will learn later in this chapter.

---

## Paned

\a{Paned} is widget that always has exactly two children. Between the two children, a visual barrier is drawn. The user can click on this barrier and drag it horizontally or vertically, depending on the orientation of the `Paned`. This gives the user the option to resize how much of a shared space two widgets allocated.

\image html paned_centered.png

\how_to_generate_this_image_begin
```cpp
auto left = Overlay();
left.set_child(Separator());
left.add_overlay(Label("left"));

auto right = Overlay();
right.set_child(Separator());
right.add_overlay(Label("right"));

for (auto* child : {&left, &right})
    child->set_margin(10);

auto paned = Paned(Orientation::HORIZONTAL);
paned.set_start_child(left);
paned.set_end_child(right);

state->main_window.set_child(paned);
```
\how_to_generate_this_image_end

Assuming the `Paned`s orientation is `Orientation::HORIZONTAL`, we can set the child on the left using `Paned::set_start_child` and the child on the right with `Paned::set_end_child`. Both childs have to be set to valid widgets in order for the user to have the option of interacting with the `Paned`. If we for some reason do not have two widgets but still like to use a `Paned`, we should add a `Separator` as the other child.

`Paned` has two per-child properties: whether a child is **resizable** and whether it is **shrinkable**.

Resizable means that if the paned changes size, the child should change size with it.

Shrinkable sets whether the side of the paned can be made smaller than the allocated size of that sides child widget. If set to true, the user can hide drag the paned barrier such that one of the widgets is complete hidden, regardless of its size allocation.

\image html paned_shrinkable.png

\how_to_generate_this_image_begin
```cpp
auto left = Overlay();
left.set_child(Separator());
left.add_overlay(Label("left"));

auto right = Overlay();
right.set_child(Separator());
right.add_overlay(Label("right"));

for (auto* child : {&left, &right})
    child->set_margin(10);

auto paned = Paned(Orientation::HORIZONTAL);
paned.set_start_child(left);
paned.set_end_child(right);

paned.set_start_child_shrinkable(true);
paned.set_end_child_shrinkable(true);

state->main_window.set_child(paned);
```
\how_to_generate_this_image_end

`Paned::set_end_child_shrinkable(true)` made it possible to move the barrier such that the right child is partially covered.

---

## ScrolledWindow

By default, most containers will allocate a size that is equal to or exceeds the largest preferred size of its children. For example, if we create a widget that has a preferred size of 5000x1000 px and use it as the child of a `Window`, the `Window` will attempt to allocate 5000x1000 pixels on screen, making the window far larger than most screens can display. In situations like these, we can use \a{ScrolledWindow}, which allows use to only view part of a larger widget:

```cpp
auto child = Separator();
child.set_size_request({5000, 5000});

auto scrolled_window = ScrolledWindow();
scrolled_window.set_child(child);

state->main_window.set_child(scrolled_window
```
\image html scrolled_window.png

Without the `ScrolledWindow`, the separator child widget would force the outer `Window` to also allocate 5000x5000 pixels. By using `ScrolledWindow`, the `Window` is free to allocate any size, retaining resizability. The end-user can influence which are of the larger widget is currently visible by operating the scrollbars inherent to `ScrolledWindow`.

### Size Propagation

By default, `ScrolledWindow` will disregard the size of its child and simply allocated based on its own properties, such as expansion and size request. We can override this behavior by forcing `ScrolledWindow` to **propagate** the width or height of its child. 

By calling `ScrolledWindow::set_propagate_natural_width(true)`, `ScrolledWindow` will assume the width of its child. Conversely, calling `ScrolledWindow::set_propagate_natural_width(true)` forces the window to allocate space equal to the height of its child.

### Scrollbar Policy

`ScrolledWindow` has two scrollbars, controlling the horizontal and vertical position. If we want to trigger behavior in addition to changing which part of the child widget `ScrolledWindow` displays, we can access each scrollbars `Adjustment` using `ScrolledWindow::get_horizontal_adjustment` and `ScrolledWindow::get_vertical_adjustment` respectively.

By default, once the cursor enters `ScrolledWindow`, both scrollbars will reveal themself. If the cursor stays outside of the `ScrolledWindow`, the scrollbars will hide again.

This behavior is controlled by the windows **scrollbar policy**, represented by the \a{ScrollbarVisibilityPolicy} enum:

+ `NEVER`: scrollbar is hidden permanently
+ `ALWAYS`: scrollbar is always shown, does not hide itself
+ `AUTOMATIC`: default behavior, see above

We can set the policy for each scrollbar individually using `ScrolledWindow::set_horizontal_scrollbar_policy` and `ScrolledWIndow::set_vertical_scrollbar_policy`.

### Scrollbar Position

Lastly, we can customize the location of both scrollbars at the same time using `ScrolledWindow::set_scrollbar_placement`, which takes one of the following values:

+ `CornerPlacement::TOP_LEFT`: horizontal scrollbar at the top, vertical scrollbar on the left
+ `CornerPlacement::TOP_RIGHT`: horizontal at the top, vertical on the right
+ `CornerPlacement::BOTTOM_LEFT`: horizontal at the bottom, vertical on the left
+ `CornerPlacement::BOTTOM_RIGHT`: horizontal at the bottom, vertical on the right

With this, scrollbar policy, size propagation, and being able to access the adjustment of each scrollbar individually, we have full control over every aspect of `ScrolledWindow`.

---

## Popovers

A \a{Popover} is a special kind of window. It is always **[modal](#modality--transience)**. Instead of the regular frame with a close button it instead closes when the user exists the window by clicking somewhere else. Showing the popover is called **pop up**, closing the popover is called **pop down**. 

\image html popover.png

\how_to_generate_this_image_begin
```cpp
auto popover = Popover();
auto child = Separator();
child.set_size_request({150, 200});
popover.set_child(child);

auto popover_button = PopoverButton();
popover_button.set_popover(popover);
popover_button.set_expand(false);
popover_button.set_margin(50);

auto aspect_frame = AspectFrame(1);
aspect_frame.set_child(popover_button);
window.set_child(aspect_frame);
```
\how_to_generate_this_image_end

Popovers can only be shown once attached to another widget. We use `Popover::set_child` to choose what widget to display inside the popover window and `Popover::attach_to` to choose which widget it should attach to.

Once attached, we can call `Popover::popup` and `Popover::popdown` to reveal or hide the window. While possible, this will take a few lines of code to setup. A simple one-line solution is to use a widget dedicated to revealing popovers: \a{PopoverButton}.

## PopoverButton

Like `Button`, `PopoverButton` has a single child, can be circular, and has the `activate` signals. `PopoverButton`s purpose, however, is not to trigger behavior. Instead, it is used to show a `Popover`

We first create the popover, then connect it to the button using `PopoverButton::set_popover`:

```cpp
auto popover = Popover();
popover.set_child(// ...
auto popover_button = PopoverButton();
popover_button.set_popover(popover);
```

\image html popover.png

\how_to_generate_this_image_begin
```cpp
auto popover = Popover();
auto child = Separator();
child.set_size_request({150, 200});
popover.set_child(child);

auto popover_button = PopoverButton();
popover_button.set_popover(popover);
popover_button.set_expand(false);
popover_button.set_margin(50);

auto aspect_frame = AspectFrame(1);
aspect_frame.set_child(popover_button);
window.set_child(aspect_frame);
```
\how_to_generate_this_image_end

For 90% of cases, this is the way to go when we want to use a `Popover`. It is easy to setup and we don't have to manually control the popover position, or when to show or hide it. The arrow next to the `PopoverButton`s child indicates to the user that clicking it will reveal a popover. We can surpress this arrow by setting `PopoverButton::set_always_show_arrow` to `false`.

`PopoverButton` lets us control the relative position of the popover by setting `PopoverButton::set_popover_position` to one of the following: `RelativePosition::ABOVE`, `RelativePosition::LEFT_OF`, `RelativePosition::RIGHT_OF`, `RelativePosition::BELOW`, which will place the popover above, left of, right of or below the button respectively. By default, the popover is shown on whichever ide of the button has the space for it.

To further customize the `Popover`, we have to call one of its member functions. `Popover::set_has_base_arrow` hides the triangular area which points to the widget it is attached to. 

We will see one more use of `PopoverButton` in the [chapter on menus](06_menus.md), where we use `PopoverMenu`, a specialized form of `Popover` that shows a menu.

---

## Selectable Widgets: SelectionModel

The next few selections will concern themself with **selectable widgets**. These tend to be the most complex and powerful widgets in mousetrap. They all have certain things in common: They a) are widget containers supporting multiple children and b) provide `get_selection_model`, which returns a \{SelectioModel} representing the currently selected widget.

`SelectionModel` is not a widget, though it is a signal emitter. Similar to `Adjustment`, it is bound to a certain widget. Changing the widget updates the `SelectionModel`, changing the `SelectionModel` updates the widget.

`SelectionModel` provides signal `selection_changed`, which is emitted whenever the internal state of the `SelectionModel` changes. 

\signals
\signal_selection_changed{SelectionModel}

The signal provides two arguments, `position`, which is the newly selected item, and `n_items`, which is the new number of currently selected items.

The latter is necessary because `SelectionModel`s can have one of three internal **selection modes**, represented by the enum \a{SelectionMode}:

+ `NONE`: Exactly 0 items are selected at all times
+ `SINGLE`: Exactly 1 item is selected at all times
+ `MULTIPLE`: 0 or more items can be selected

To illustrate use of `SelectionModel`, we should concern ourself with one of the selectable widgets directly.

## ListView

\a{ListView} is a widget that arranges its children in a line, similar to `Box`. Unlike `Box`, it is selectable:

\image html list_view_single_selection.png

\how_to_generate_this_image_begin
```cpp
auto list_view = ListView(Orientation::HORIZONTAL, SelectionMode::SINGLE);

auto child = [](size_t id)
{
    auto overlay = Overlay();
    overlay.set_child(Separator());

    auto label = Label((id < 10 ? "0" : "") + std::to_string(id));
    label.set_alignment(Alignment::CENTER);
    overlay.add_overlay(label);

    auto frame = Frame();
    frame.set_child(overlay);
    frame.set_size_request({50, 50});

    auto aspect_frame = AspectFrame(1);
    aspect_frame.set_child(frame);

    return aspect_frame;
};

for (size_t i = 0; i < 7; ++i)
    list_view.push_back(child(i));

window.set_child(list_view);
```
\how_to_generate_this_image_end

Where the blue border indicates that the 4th element (with label `"03"`) is currently selected.

When creating the `ListView`, the first argument to its constructor is the orientation, while the second one is the selection mode. If not specified, `SelectionMode::NONE` is used.

Much like `Box`, `ListView` supports `ListView::push_back`, `ListView::push_front` and `ListView::insert` to insert any widget child at the specified position. Unlike `Box`, for `ListView`, these functions return a value.

`ListView` can be requested to automatically show separator inbetween to items. To show these, we simply call `ListView::set_show_separators(true)`.

### Nested Trees

By default, `ListView` displays its children in a linear list, either horizontally or vertically. `ListView` also supports **nested lists**:

\image html list_view_nested.png

\how_to_generate_this_image_begin
```cpp
auto list_view = ListView(Orientation::VERTICAL);
auto child = [](const std::string& string)
{
    auto overlay = Overlay();
    overlay.set_child(Separator());

    auto label = Label(string);
    label.set_alignment(Alignment::CENTER);
    label.set_margin(5);
    overlay.add_overlay(label);

    auto frame = Frame();
    frame.set_child(overlay);

    return frame;
};

list_view.push_back(child("outer item #01"));
auto it = list_view.push_back(child("outer item #02"));
list_view.push_back(child("inner item #01"), it);
it = list_view.push_back(child("inner item #02"), it);
list_view.push_back(child("inner inner item #01"), it);
list_view.push_back(child("outer item #03"));

auto frame = Frame();
frame.set_child(list_view);
list_view.set_margin(50);
window.set_child(frame);
```
\how_to_generate_this_image_end

Here, we have a triple nested list. The outer list has the items `outer item #01`, `outer item #02` and `outer item #03`. `outer item #02` is itself a list, with wo children `inner item #01` and `inner item #02`, the latter of which is also a list with a single item.

When `ListView::push_back` is called, it returns an **iterator**. We can use this iterator as the second argument to `ListView::push_back` in order for that item to be inserted as a nested list. We would construct the above shown nested list like so:

```cpp
auto it_01 = list_view.push_back(/* outer item #01 */);
auto it_02 = list_view.push_back(/* outer item #02 */);

  auto inner_it_01 = list_view.push_back(/* inner item #01 */, it_02);
  auto inner_it_02 = list_view.push_back(/* inner item #02 */, it_02);
   
    auto inner_inner_it_01 = list_view.push_back(/* inner inner item #01 */, inner_it_02);
    
auto it_03 = list_view.push_back(/* outer item #03 */);
```

If we do not specify an iterator as the second argument to functions for item insertion (such as `ListView::push_back`), the item will be inserted into the top-level list. Thanks to this, if we just want a linear list, we do not have to think or deal with the iterators at all.

When a list item is a nested list, a downwards arrow will appear next to it, indicating to the user that they can click that item in order to reveal the nested list.

### Reacting to Selection

In order to react to the user selecting a new item in our `ListView` (if its selection mode is anything other than `NONE`), we connect to the lists `SelectionModel` like so:

```cpp
auto list_view = ListView(Orientation::HORIZONTAL, SelectionMode::SINGLE);

list_view.get_selection_model()->connect_signal_selection_changed(
    [](SelectionModel*, int32_t item_i, int32_t n_items){
        std::cout << "selected: " << item_i << std::endl;
    }
);
```

This process will be the same for any of the selectable widgets following this section on `ListView`.

\warning For any selectable widget, if it was initialized with `SelectionMode::NONE`, `selection_changed` will never be emitted. Make sure to manually specify the selection mode in the selectable widgets constructor, otherwise `NONE` will be chosen.

### Selection Mechanics

Usually, when the user clicks one of the children of `ListView` once, the selection is changed accordingly. However, we can add two more methods of selecting an item available to the user.

**Rubberband selection** is a method of selection where the user draws a rectangle by clickdragin the cursor over multiple items. This makes it easy to select many, physically close items at the same time. To allow for this kind of selection, we need to set `ListView::set_enable_rubberband_selection` to `true`. This method of selection is only available for `SelectionMode::MULTIPLE`.

Lastly, **single click** selection is a shortened from of regular selection. As opposed to having to press the mouse button to select an item, once `ListView::set_single_click_activate` is called, simply moving the cursor over one of the items is enough to select them. This method can be used if the selection mode is either `SINGLE` or `MULTIPLE`.

---

## GridView

\a{GridView} supports many of the same functions as `ListView`. Instead of displaying its children in a nested list, it shows them in a grid:

\image html grid_view.png

\how_to_generate_this_image_begin
```cpp
auto grid_view = GridView(Orientation::HORIZONTAL, SelectionMode::SINGLE);
grid_view.set_max_n_columns(4);

auto child = [](size_t id)
{
    auto overlay = Overlay();
    overlay.set_child(Separator());

    auto label = Label((id < 10 ? "0" : "") + std::to_string(id));
    label.set_alignment(Alignment::CENTER);
    overlay.add_overlay(label);

    auto frame = Frame();
    frame.set_child(overlay);
    frame.set_size_request({50, 50});

    auto aspect_frame = AspectFrame(1);
    aspect_frame.set_child(frame);

    return aspect_frame;
};

for (size_t i = 0; i < 7; ++i)
    grid_view.push_back(child(i));

window.set_child(grid_view);
```
\how_to_generate_this_image_end

Items are dynamically allocated to rows and columns based on the space available to the `GridView`. If we want more control over how many row/columns the grid view has, we can use `GridView::set_min_n_columns` and `GridView::set_max_n_columns` to force one of either row or columns (depending on `Orientation`) to adhere to the given limit.

Other than this, `GridView` supports the same functions as `ListView`, including `push_front`, `push_back`, `insert`, `get_selection_model`, set_enable_rubberband_selection`, `set_single_click_activate`, etc..

---

## Column View

\todo

--- 

## Stack

Unlike the pervious ones so far, \a{Stack} is a selectable widget that can only ever show one child. As such, it is useful for applications where a page-like layout is desired. The order of children depends on the order at which we call `Stack::add_child` in. This function returns an ID, which uniquely identifies that type of item. We need to keep track of this ID, as it is what allows us to later call `Stack::set_visible_child`, to change the stacks currently shown page to that with the supplied ID:

```cpp
auto stack = Stack();
auto page_01 = // widget
auto page_02 = // widget

auto page_01_id = stack.add_child(page_01, "Page 01");
auto page_02_id = stack.add_child(page_02, "Page 02");

// make page_01 currently displayed widget
stack.set_visible_child(page_01_id);
```

We can also change the currently displayed child by calling methods like `SelectionModel::select` on the internal selection model of the stack, which we acquire using `Stack::get_selection_model`. As state before, if the state of the `SelectionModel` changes, the display widget, `Stack` in this case, will change to match.

We see above that `Stack::add_child` takes an second argument, which is the **page title**. This title is not used in the stack itself, rather, it is used for two widgets made to exclusively interact with the stack. So far, only we as develoeprs where able to change which child is currently displayed. These two widgets give the user the option to choose themself.

### StackSwitcher

\a{StackSwitcher} presents the user with a row of buttons, each of which have use the current stacks childs title as its label:

```cpp
auto stack = Stack();
stack.add_child(/* child #01 */, "Page 01");
stack.add_child(/* child #02 */, "Page 02");
stack.add_child(/* child #03 */, "Page 03");

auto stack_switcher = StackSwitcher(stack);

auto box = Box(Orientation::VERTICAL);
box.push_back(stack);
box.push_back(stack_switcher);
```

\image html stack_switcher.png

\how_to_generate_this_image_begin
```cpp
auto stack = Stack();
auto child = [](size_t id)
{
    auto overlay = Overlay();
    overlay.set_child(Separator());

    auto label = Label(std::string("Stack Child #") + (id < 10 ? "0" : "") + std::to_string(id));
    label.set_alignment(Alignment::CENTER);
    overlay.add_overlay(label);

    auto frame = Frame();
    frame.set_child(overlay);
    frame.set_size_request({300, 300});

    auto aspect_frame = AspectFrame(1);
    aspect_frame.set_child(frame);

    return aspect_frame;
};

stack.add_child(child(01), "Page 01");
stack.add_child(child(02), "Page 02");
stack.add_child(child(01), "Page 03");

auto stack_switcher = StackSwitcher(stack);

stack.set_expand(true);
stack.set_margin(10);
stack_switcher.set_expand_vertically(false);

auto box = Box(Orientation::VERTICAL);
box.push_back(stack);
box.push_back(stack_switcher);
window.set_child(box);
```
\how_to_generate_this_image_end

We see that we need to supply the `Stack` instance the `StackSwitcher` should control in `StackSwitcher`s constructor.

### StackSidebar

`StackSidebar` presents the user with a vertical list of labels, again using the title supplied for each of `Stack`s children:

```cpp
auto stack = Stack();
stack.add_child(/* child #01 */, "Page 01");
stack.add_child(/* child #02 */, "Page 02");
stack.add_child(/* child #03 */, "Page 03");

auto stack_sidebar = StackSidebar(stack);

auto box = Box(Orientation::HORIZONTAL);
box.push_back(stack);
box.push_back(stack_sidebar);
```

\image html stack_sidebar.png

\how_to_generate_this_image_begin
```cpp
auto stack = Stack();
auto child = [](size_t id)
{
    auto overlay = Overlay();
    overlay.set_child(Separator());

    auto label = Label(std::string("Stack Child #") + (id < 10 ? "0" : "") + std::to_string(id));
    label.set_alignment(Alignment::CENTER);
    overlay.add_overlay(label);

    auto frame = Frame();
    frame.set_child(overlay);
    frame.set_size_request({300, 300});

    auto aspect_frame = AspectFrame(1);
    aspect_frame.set_child(frame);

    return aspect_frame;
};

stack.add_child(child(01), "Page 01");
stack.add_child(child(02), "Page 02");
stack.add_child(child(01), "Page 03");

auto stack_sidebar = StackSidebar(stack);

stack.set_expand(true);
stack.set_margin(10);

auto box = Box(Orientation::HORIZONTAL);
box.push_back(stack);
box.push_back(stack_sidebar);
window.set_child(box);
```
\how_to_generate_this_image_end

`StackSidebar` and ``StackSwitcher` have no other member functions, their only use is to control the currently revealed child of a `Stack`.

### Transition Animation

When switching between two pages in a stack, an animation plays transitioning from one to the other. Similar to `Revealer`, we can influence this animation in multiple ways:

+ `Stack::set_transition_duration` governs how long the animation will take until it is complete
+ `Stack::set_interpolate_size`, if set to `true`, makes it such that while the transition animation plays, the stack will change from the size of the previous child to the size of the current child
+ `Stack::set_animation_type` governs the type of animation

Mousetrap provides a large number of different animation, which are represented by the enum \a{StackTransitionType}. They including crossfade, sliding and rotating the child widget. For a full list of animation types, see the \link mousetrap::StackTransitionType corresponding documentation page\endlink.

---

## Notebook

\a{Notebook} is very similar to `Stack`, it also displays exactly one child at a time. Unlike `Stack`, it comes with a built-in way for users to select which child to show:

\image html notebook.png

\how_to_generate_this_image_begin
```cpp
auto notebook = Notebook();
auto child = [](size_t id)
{
    auto overlay = Overlay();
    overlay.set_child(Separator());

    auto label = Label(std::string("Notebook Child #") + (id < 10 ? "0" : "") + std::to_string(id));
    label.set_alignment(Alignment::CENTER);
    overlay.add_overlay(label);

    auto frame = Frame();
    frame.set_child(overlay);
    frame.set_size_request({300, 300});

    auto aspect_frame = AspectFrame(1);
    aspect_frame.set_child(frame);

    return aspect_frame;
};

notebook.push_back(child(01), Label("Page 01"));
notebook.push_back(child(02), Label("Page 02"));
notebook.push_back(child(01), Label("Page 03"));
notebook.set_tabs_reorderable(true);

window.set_child(notebook);
``` 
\how_to_generate_this_image_end

`Notebook` sports some additional feature, by setting `Notebook::set_is_scrollable` to `true`, the user can change between pages by scrolling. Furthermore, once `Notebook::set_tabs_reorderable` is set to `true`, the user can drag and drop pages to reorder them. Users can even **drag pages from one notebook to another**.

This complex behavior necessitate notebook having a number of custom signals:

\signals
\signal_page_added{Notebook}
\signal_page_reordered{Notebook}
\signal_page_removed{Notebook}
\signal_page_selection_changed{Notebook}
\widget_signals{Notebook}

Where `_` is an unusued argument. For example, we would connect to `page_selection_changed` like so:

```cpp
notebook.connect_signal_page_selection_changed([](Notebook*, void*, int32_t page_index){
    std::cout << "Selected Page is now: " << page_index << std::endl;
});
```

\todo refactor notebook signals to remove unused argument

---

## Compound Widgets


