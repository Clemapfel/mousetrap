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

## Frame

`Frame` is a purely cosmetical widget that displays whatever child we choose using `Frame::set_child` in a frame with a small border and rounded corners:

```cpp
auto child_widget = // ...
auto frame = Frame();
frame.set_child(child_widget);
```

\todo image

Using `Frame::set_label_widget`, we can furthermore choose a label widget, which will be displayed above the child widget of the frame. This will usually be a `Label`, though `set_label_widget` accepts any kind of widget.

---

## AspectFrame

Not to be confused with `Frame`, `AspectFrame` adds no graphical element to its single child. Instead, the widget set via `AspectFrame::set_child` will be forced to stay in a certain **aspect ratio**. We choose the aspect ratio either in the constructor or with `AspectFrame::set_ratio`, both of which accept a float that is `width / height`. For example, if we want an aspect ratio of 4 by 3:

```cpp
auto child_widget = // ...
auto aspect_frame = AspectFrame(4.f / 3) // 4:3 aspect ratio
aspect_frame.set_child(child_widget);
```

Where we wrote `4.f / 3` instead of `4 / 3` because in C++, the latter would trigger [integer divison](https://en.wikipedia.org/wiki/Division_(mathematics)#Of_integers) resulting in a ratio of `1`, instead of the intended `1.333...`.

---

## Overlay

So far, all widget containers align widget such that the do not overlap, which is also why the margin property may not be negative. There is one container that allows overlapping: `Overlay`.

`Overlay` has one "base" widget, which is at the conceptual bottom of the overlay. We set this widget using `Overlay::set_child`. Then, we can add any number of widgets on top of the base widget using `Overlay::add_overlay`:

```cpp
auto child_widget = // ...
auto overlay_widget = // ...

auto overlay = Overlay();
overlay.set_child(child_widget);
overlay.add_overlay(overlay_widget);
```

\todo image

By default, `Overlay` will allocate exactly as much space as the base widget (set with `set_child`) does. If one of the overlaid widgets takes up more space than the base widget, it will be truncated. We can change this by supplying a second argument to `add_overlay`, which is a boolean indicated whether the overlay widget should be included in the entire containers size allocation, that is, if the overlay widget is larger than the base widget, the overlay resize itself such that the entire overlay widget is shown:

```cpp
overlay.add_overlay(overlay_widget, true);
``` 

When constructing complex compound widgets, we often want widgets to overlap each other. This can cause problems for interactibility: if the user clicks on an area that is occupied by two or more widgets, which widget should be activated? By default, only the **top-most** widget will be activated. If we want a different layer to be activated instead, we have to deactivate interaction with all widgets above it, either by calling `Widget::hide` or `Widget::set_can_respond_to_input(false)`. 

---

## ScrolledWindow

By default, most containers will allocate a size that is equal to or exceeds the largest preferred size of its children. For example, if we create a widget that has a preferred size of 5000x1000 px and use it as the child of a `Window`, the `Window` will attempt to allocate 5000x1000 px on screen, making the window far larger than most screens can display. We can prevent this by instead adding the widget to a `ScrolledWindow`, which truncates it while giving the user a way to choose which part of the large widget they want to see:

```cpp
auto large_widget = // ...
auto scrolled_window = ScrolledWindow();
scrolled_window.set_child(large_widget);
window.set_child(scrolled_window);
```

Now, the window is free to allocate any size smaller than the widget. The user can choose which part to display be triggering the scroll bars native to `ScrolledWindow`.

\todo image

### Size Propagation

By default, `ScrolledWindow` will size itself according to the prefferred size of the `ScrolledWindow` itself, not of its child. This means we can control the size just like any other widget. Sometimes we do want `ScrolledWindow` to follow its child however.

To force `ScrolledWindow` to assume the width of its child, we call `ScrolledWindow::set_propagate_natural_width(true)`, for height we use `ScrolledWindow::set_propagate_natural_height(true)`. For example, if the child has a preferred size of 5000x1000 and we set `propagate_natural_height` to true, the windows width will be whatever the properties of the `ScrolledWindow` itself determin, while the windows height will be 1000, the natural height of its child.

### Scrollbar Policy

`ScrolledWindow` has two scrollbars, controlling the horizontal and vertical position. If we want to trigger behavior in addition to changing which part of the chid widget `ScrolledWindow` displays, we can access each scrollbars `Adjustment` using `ScrolledWindow::get_horizontal_adjustment` and `ScrolledWindow::get_vertical_adjustment` respectively.

The default behavior is that if the users cursor enters the `ScrolledWindow`, both of the scrollbars will reveal themself. Sometimes, we do not want one or both of the scrollbars to behave this way, for example hide one of them completely. This behavior of the scroll bars is controlled with a **policy** which is one of the following values:

+ `ScrollbarVisibilityPolicy::NEVER`: scrollbar is hidden permanently
+ `ScrollbarVisibilityPolicy::ALWAYS`: scrollbar is always shown, does not hide itself
+ `ScrollbarVisibilityPolicy::AUTOMATIC`: default behavior, see above

We can set the policy for either scrollbar using `ScrolledWindow::set_horizontal_scrollbar_policy` and `ScrolledWIndow::set_vertical_scrollbar_policy`.

### Scrollbar Position

Lastly, we can customize where the scrollbar appear in the window. We choose the position of both at the same time using `ScrolledWindow::set_scrollbar_placement`, which takes one of the following values:

+ `CornerPlacement::TOP_LEFT`: horizontal scrollbar at the top, vertical scrollbar on the left
+ `CornerPlacement::TOP_RIGHT`: horizontal at the top, vertical on the right
+ `CornerPlacement::BOTTOM_LEFT`: horizontal at the bottom, vertical on the left
+ `CornerPlacement::BOTTOM_RIGHT`: horizontal at the bottom, vertical on the right

Using this, the scrollbars policy and each scrollbars adjustment, we can hook into every component of the `ScrolledWindow`, making it fully customizable. This adn the common usecase of fitting a large widget into a variable-size container makes `ScrolledWindow` one of the more commonly used container widgets.

---

## Paned

`Paned` is widget that always has exactly two children. Between the two children, a barrier is drawn. The user can click on this barrier and drag it horizontally or vertically, depending on the orientation of the `Paned`. This gives the user the option to resize how much space a widget is allocated by hand.

\image html paned_centered.png

Assuming the `Paned`s orientation is `Orientation::HORIZONTAL`, we can set the child on the left using `Paned::set_start_child` and the child on the right with `Paned::set_end_child`. Both childs have to be set to valid widgets in order for the user to have the option of interacting with the `Paned`. If we for some reason do not have two widgets but still like to use a `Paned`, we should set the other child to a `Separator`.

`Paned` has two per-child properties: whether a child is **resizable** and whether it is **shrinkable**.

Resizable means that if the paned changes size, the child should change size with it.

Shrinkable sets whether the side of the paned can be made smaller than the allocated size of that sides child widget. If set to true, the user can hide drag the paned barrier to complee hide the widget, regardless of its size allocation.

\image html paned_shrinkable.png `Paned::set_end_child_shrinkable(true)` made it possible to move the barrier such that the right child is partially covered"


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

For progress where we do not have an exact fraction, we can use the `Spinner` widget, which simply displays an animated spinning loading icon. Using `Spinner::set_is_spinning`, we can control whether the animation is currently playing or not.

---

## Entry

Being able to input text with the keyboard is central to desktop applications, as such, `Entry`, the widget used for single-line text entry, may be one of the most important widgets.

Entry is an area that, when clicked, allows the user to write freely. This text is written to an internal text buffer. We can access the text in that buffer with `Entry::get_text`. `Entry::set_text` overwrites this buffer with a custom message.

We can of course control the width of an `Entry` by sizehinting it like any other widget, though a less manual way to do this is `Entry::set_max_length`, which takes an integer that is the number of characters that the prompts should make size for. 

`Entry` supports "password mode", which is when each character typed is replaced with a dot, or otherwise made unreadable. This is such that a person looking over another persons shoulder can read a sensitive prompt, such as a message or password. We can enter and exit this mode using `Entry::set_text_visible`, where `false` is password-mode and `true` is the regular mode.

Lastly, `Entry` is activatable, which is usually done by pressing the enter key while the cursor is inside the entry. This does not cause any behavior initially, but we can connect to the `activate` signal of `Entry` to trigger behavior.

Other than `activate`, `Entry` has one more signal `text_changed`, which is emitted whenever the internal buffer changes, including when set programmatically using `set_text`.

\signals 
\signal_activate{Entry}
\signal_text_changed{Entry}

## TextView

While we can technically input a newline character into `Entry`, it is not possible to display two lines at the same time. For this purpose, we use `TextView`. It supports a number of basic text-editor features, including **undo / redo**  are triggered by the user pressing Control + Z or Control + Y respectively. We can also trigger this behavior manually by calling `TextView::undo` / `TextView::redo`.

Much like `Label`, we can set how the text aligns horizontally using `TextView::set_justify_mode`. To further customize how text is displayed, we can choose the **internal margin**, which is the distance between the frame of the `TextView` and the text inside of it. `TextView::set_left_margin`, `TextView::set_right_margin`, `TextView::set_top_margin` and `TextView::set_bottom_margin` allow us to customize this freely.

`TextView` does not have the `activate` signal, pressing enter while the cursor is inside the widget will simply create a new line. 

`TextView` has the following signals, where `text_changed` behaves exactly like it does with `Entry`:

\signals
\signal_text_changed{TextView}
\signal_undo{TextView}
\signal_redo{TextView}

---

## Popovers

\todo figure popover 

A popover is a special kind of window. It is always **[modal](#window)**, instead of the regular frame with a close button it instead closes when the user exists the window by clicking somewhere else. Showing the popover is called **pop up**, close the popover is called **pop down**. 

To create a generic popover we can place freely, we use `Popover`. The popover cannot be displayed by itself, it has to attach to another widget:

\todo code example

Then we need to manually call `Popover::popoup` or `Popover::popdown` from inside a signal handler. This can be finnicky, which is why there is a widget that automates this for use. `PopoverButton`:

## PopoverButton

Like `Button`, `PopoverButton` has a single child, can be circular, and has the `activate` signals. `PopoverButton` purpose, however, like it's name suggest, is to automatically show / hide a `Popover`. 

We first create the popover, then connect it to the button using `PopoverButton::set_popover`:

```cpp
auto popover = Popover();
popover.set_child(// ...
auto popover_button = PopoverButton();
popover_button.set_popover(popover);
```

\todo figure with blank popover

For 90% of cases, this is the way to go when we want to use a `Popover`. It's easy and we don't have to manually control the popover position or when to open/close it. The arrow next to the `PopoverButton`s child indicates to the user that clicking it will reveal a popover. We can surpress this arrow by setting `PopoverButton::set_always_show_arrow` to `false`.

`PopoverButton` lets us control the relative position of the popover by setting `PopoverButton::set_popover_position` to one of the following: `RelativePosition::ABOVE`, `RelativePosition::LEFT_OF`, `RelativePosition::RIGHT_OF`, `RelativePosition::BELOW`, which will place the popover above, left of, right of or below the button respectively. Before `set_popover_position` is called, a `PopoverButton` instance will dynamically choose where to display the popover, based on the window position and screen layout.

To further customize the `Popover`, we have to call one of its member functions. `Popover::set_has_base_arrow` hides the triangular area which points to the widget it is attached to. 

We will see one more use of `PopoverButton` in the [next chapter on menus](06_menus.md), where we can have it create a popover to show a menu for us.

---

## Selectable Widget Containers

## SelectionModel

### Signals

| id                 | signature                                | emitted when...                                      |
|--------------------|------------------------------------------|------------------------------------------------------|
| `selection_changed` |  `(SelectionModel* , int32_t position, int32_t n_items, (Data_t)) -> void` | selection of widget controlled by this model changes |

## ListView

`ListView` is one of the more commonly used widgets. At first, it may seem very similar to `Box`. `ListView` can either be horizontal or vertical, arranging its children in a line. Much like `Box`, children can be added to the front, end, or at a specific position in the container. Unlike `Box`, `ListView::push_front`, `ListView::insert` and `ListView::push_back` have a return value, which returns an **iterator**. We can supply this iterator as an additional argument to functions inserting widgets, which will create a **nested list**. It's best to see an example:


### Signals

| id         | signature                                                       | emitted when...                                                              |
|------------|-----------------------------------------------------------------|------------------------------------------------------------------------------|
| `activate` | `(ListView*, (Data_t)) -> void`                                  | user pressed the enter key or otherwise activates view or a widget inside it |

## GridView

### Signals

| id         | signature                      | emitted when...                                                                  |
|------------|--------------------------------|----------------------------------------------------------------------------------|
| `activate` | `(GridView*, (Data_t)) -> void` | user pressed the enter key or otherwise activates the view or a widget inside it |

---

## Stack

### Signals

`Stack` does not directly emit signals, but we can connect so its `SelectionModel` to track changes in the stacks current page

### StackSwitcher

### StackSidebar

---

## Notebook

| id                       | signature                                                     | emitted when...                                                                  |
|--------------------------|---------------------------------------------------------------|----------------------------------------------------------------------------------|
| `page_added`             | `(Notebook*, void* _, uint32_t page_index, (Data_t)) -> void` | |
| `page_removed`           | `(Notebook*, void* _, uint32_t page_index, (Data_t)) -> void`                                                            | |
| `page_reordered`         | `(Notebook*, void* _, uint32_t page_index, (Data_t)) -> void`                                                            | |
| `page_selection_changed` | `(Notebook*, void* _, uint32_t page_index, (Data_t)) -> void`                                                            | |

Where `_` is an unused argument.

## Column View

### Signals

| id         | signature                         | emitted when...                                                                  |
|------------|-----------------------------------|----------------------------------------------------------------------------------|
| `activate` | `(ColumnView*, (Data_t)) -> void` | user pressed the enter key or otherwise activates the view or a widget inside it |

## DropDown

---

## Additional Widget Container

## Grid

## Fixed



