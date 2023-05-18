# Chapter 4: Widgets

In this chapter, we will learn:
+ What a widget is
+ Properties that all widgets share
+ All types of widgets supplied by mousetrap
+ How to create compound widgets

---
<details><summary><b><tt>main.cpp</tt> for this chapter</b></summary>

Snippets from this section can be run using the following main.cpp:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("example.app");
    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);
        
        // add snippet here
      
        window.set_child(/* widget from snippet */);
        window.present();
    });
    return app.run();
}
```
</details>

---

## What is a widget?

When creating a GUI, widgets are the central element to any and all applications. In general, a widget is anything that can be rendered on screen. Often wigdets are **interactable**, which means that the user can trigger behavior by interacting with the widget. In the case of `Button`, the widget is rendered to screen as a filled rectangle and it is interactable because the user can click that rectangle to trigger an animation and emit signal `clicked`.

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

Where the first argument of this signal, is a `Widget*` (**not** a `Button*`). Signals that all widgets share use `Widget*`. For class-specific signals, such as signal `clicked` for `Button`, the first argument is `Button*`.

For each widget type we can check their table of signals as described in the [chapter on signals](02_signals.md#checking-signal-signature).

## Widget Properties

All widgets share common properties that govern how they behave when the container they are in changes size, or at what position they will appear. Any function in this section can be called for any widget class.

### Size Request

When a widget is shown, it will dynamically **allocate its size**. This governs the amount of space a widget will take up on screen. This size is determined programmatically, based on the widgets properties, as well as the properties of its children and/or parents.

\collapsible_note_begin{Widget Children & Parents}

Some widgets can contain other widgets. For example, `Window` has a single **child**. `Button`, also has a child, which is the label of the button.

In general, if widget `A` is inside of widget `B`, for example by calling `A.set_child(B)`, then we say `B` is `A`s **child**, and `A` is `B`s **parent**.

Each widget that is not a window has exactly one parent. The number of children varies per widget, `Window` and `Button` have exactly on child, while other widgets may have a any number of children, or no children at all.

\collapsible_note_end

One property used for determinening the widgets size is its **size request**. This is the minimum amount of space the widget has to take up, expressed as a \{Vector2f}, its minimum width and height (in pixels).

For example, a widget that displays a string of text that is 60px wide and 15px tall, would have to have a size request of `{60, 15}` in order to display the text without cropping. This size is usually chosen dynamically, but we can manual control it using `Widget::set_size_request`. For an empty widget, the default size request is `{0, 0}`.

Setting either the width or height of the size-request to `0` signals to the layout manager that the widget should automatically choose that part of the size request.

Manipulating the size request like this is also called **size-hinting**.

### Accessing Widget Size

We can query information about a widgets current and target size using multiple functions:

`Widget::get_allocation` returns a \link mousetrap::Rectangle rectangle\endlink, which holds the current widgets position and its current size, both  in pixels. This size may or may not be equal to what we size-hinted the widget to.

```cpp
auto position = widget.get_allocation().top_left;
auto size = widget.get_allocation().size;
```

To access a widgets size-request (not the actual size), we use `Widget::get_size_request`:

```cpp
auto size_request = widget.get_size_request();
```

If we have not yet size-hinted the widget, this size will be `{0, 0}`.

Lastly, we can query the widgets **minimum possible size**, that is the size that is has to occupy no matter what, using `Widget::get_preferred_size`. This size is of course influenced on whether we size-hinted the widget.

The object returned by `get_preferred_size` also holds information about the size the widget would allocate naturally, if given infinite amount of space. This is called the **natural size**.

We access the minimum and natural size like so:

```cpp
auto minimum_size = widget.get_preferred_size().minimum_size;
auto preferred_size = widget.get_preferred_size().natural_size;
```

Layout management is very complex and the algorithm behind managing the size of all widgets is highly sophisiticated. All that users of mousetrap need to understand is how to influence this algorithm, not how exactly it works.

On top of a widgets size request, a widgets target allocated size depends on a number of other variables:

### Margin

Any widget has four margins: `start`, `end`, `top` and `bottom`. Usually, these correspond to empty space left, right, above, and below the widget respectively. Margins are unaffected by the widgets original size and expansion, they are simply added to the corresponding side of the widget. In this way, they work similar to the [css properties of the same name](https://www.w3schools.com/css/css_margin.asp), though in mousetrap, margins cannot be negative.

We use `Widget::set_margin_start`, `Widget::set_margin_end`, `Widget::set_margin_top` and `Widget::set_margin_bottom` to control each individual margin. Mousetrap also provides the convenience functions `Widget::set_margin_horizontal` and `Widget::set_margin_vertical`, which sets both `start` and `end', or `top` and `bottom` at the same time respectively.

Lastly `Widget::set_margin` sets all fours margins to the same value:

```cpp
auto widget = // ...
widget.set_margin_start(10);
widget.set_margin_end(10);
widget.set_margin_top(10);
widget.set_margin_bottom(10);

// equivalent to
widget.set_margin(10)
```

Margins are used extensively in UI design. They make an application look more professional and aesthetically pleasing. A good rule of thumb is that for a 1920x1080 display, the **margin unit** should be 10 pixels. That is, all margins should be a multiple of 10. If the display has a higher or lower resolution, the margin unit should be adjusted.

### Expansion

In previous chapters, we may have noticed that if we scale the window from our previous `main.cpp`s, the widget inside may or may not scale along with it. This is called **expansion**, which is a property of all widgets.

We can specify whether a widget should expand along the vertical or horizontal axis using `Widget::set_expand_horizontally` and `Widget::set_expand_vertically`, which take a single boolean as its argument. `Widget::set_expand` sets both properties to the same value at the same time.

```cpp
auto widget = // ...
widget.set_expand_horizontally(true);
widget.set_expand_vertically(true);

// equivalent to
widget.set_expand(true)
```

When `set_expand` is set to `true`, the widgets maximum size is essentially infinite. If it is set to `false`, the widget will not expand past its natural size.

### Alignment

Widget **alignment** governs how one or more widgets behave when grouped together, for example if they are all children of the same container widget.

An example: a `Button` size-hinted to 100x100 pixels has expansion disabled (`set_expand` was set to `false`). It has a margin of 0 and is placed inside a `Window`. When we scale the window, the button will not change size. *Alignment*, then, governs **where in the window the button is positioned**.

We can set alignment for the horizontal and vertical axis separately, using `Widget::set_horizontal_alignment` and `Widget::set_vertical_alignment`, which both take an enum value of type \a{Alignment} as their only argument. This enum has three possible values:
+ `Alignment::START`: left if horizontal, top if vertical
+ `Alignment::END`: right if horizontal, bottom if vertical
+ `Alignment::CENTER`: center of axis, regardless of orientation

For our example, the button would take on these locations based on which enum value we chose for each alignment axis:

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

Using alignment, size-hinting, and expansion, we can fully control where and at what size widgets appear on-screen.

### Visibility

Once a widgets allocated area enters the visible area on screen, it is **shown** (which we can track using the `shown` signal). Once it leaves the visible area, it is **hidden** (emitting `hide`).

`Widget::set_visible` sets the widgets visibility based on the boolean argument. Other than this, it behaves exactly as `Widget::show` and `Widget::hide`. If we hide a widget that is currently shown, it will disappear and its allocated size will be 0.

One way to make a widget invisible, without hiding it, is to control its **opacity**. Using `Widget::set_opacity`, if we set the opacity to 0, all other properties of the widget, including its size allocation and intractability are kept. It remains shown. The only thing that changes is the opacity of is that the graphical part element, which is now fully transparent and thus invisible to the user.

### Cursor Type

Each widget has a property governing what the user's cursor will look like while in the widgets allocated area. By default, this is a simple arrow. A widget intended for text-entry would want the cursor to be a [caret](https://en.wikipedia.org/wiki/Caret_navigation), while a clickable widget would likely want a pointer cursor.

We can manually choose the cursor type using `Widget::set_cursor`, by setting it to one of the following values, all of which are part of the enum \a{CursorType}:

| `CursorType` value  | Appearance                                                                             |
|---------------------|----------------------------------------------------------------------------------------|
| `NONE`              | no visible cursor                                                                      |
| `DEFAULT`           | small arrow, this is the default value widgets                                         |
| `POINTER`           | a hand, indicates that widget can be interacted with by pressing the mouse button once |
| `HELP`              | question mark, instructs user to open a help dialog                                   |
| `CONTEXT_MENU`      | pointer with `...` to indicate clicking will open a context menu                       |
| `PROGRESS`          | pointer with a small "currently loading" icon                                          |
| `WAIT`              | instructs user to wait for an action to finish                                         |
| `CELL`              | used when interacting with a table                                                     | 
| `CROSSHAIR`         | cross-hair shaped cursor, used for precise selections                                  |
| `TEXT`              | text-entry cursor                                                                      | 
| `MOVE`              | four-pointed arrow, used to move around object                                         |
| `GRAB`              | open hand, not yet grabbing                                                            |
| `GRABBING`          | closed hand, currently grabbing                                                        |
| `ALL_SCROLL`        | four-direction scroll                                                                  |
| `ZOOM_IN`           | lens, usually with a plus icon                                                         |
| `ZOOM_OUT`          | lens, usually with a minus icon                                                        |
| `COLUMN_RESIZE`     | left-right arrow                                                                       | 
| `ROW_RESIZE`        | up-down arrow                                                                          |
| `NORTH_RESIZE`      | up arrow                                                                               | 
| `EAST_RESIZE`       | right arrow                                                                            | 
| `SOUTH_RESIZE`      | bottom arrow                                                                           | 
| `WEST_RESIZE`       | left arrow                                                                             | 
| `NORTH_EAST_RESIZE` | top-right arrow                                                                        | 
| `SOUTH_EAST_RESIZE` | bottom-right arrow                                                                     | 
| `SOUTH_WEST_RESIZE` | bottom-left arrow                                                                      | 
| `NORTH_WEST_RESIZE` | top-left arrow                                                                         | 

```cpp
auto button = Button();

// set button cursor to pointer, indicates that it can be clicked
button.set_cursor(CursorType::POINTER);
```

The exact look of each pointer type will depend on the user's operating system and UI theme. To choose a fully custom cursor, we use `Widget::set_cursor_from_pixel`, which takes an `Image`. We will learn more about \a{Image} in the [chapter on image and sound](07_image_and_sound.md). Until then, this is how we set a cursor from a `.png` file on disk:

```cpp
auto widget = //...
widget.set_cursor_from_image(Image("/path/to/image.png"), {0, 0});
```

Where `"/path/to/image.png"` is the absolute path to the image file. `{0, 0}` is the cursor anchor, in pixels, which controls which part of the pixel is aligned with the conceptual center of the cursor.

### Tooltip

Each widget can optionally have a **tooltip**. This is a little window that opens when the cursor hovers over a widgets allocated area for enough time. Tooltips are usually reserved to simple text message, which we can set directly using `Widget::set_tooltip_text`:

```cpp
auto open_file_button = Button();
open_file_button.set_tooltip_text("Click to Open");
```
\image html widget_tooltip_text.png

If we want to use something more complex than just simple text, we can register an arbitrarily complex widget as a tooltip by calling `Widget::set_tooltip_widget`. As a matter of style, this widget should not be intractable, though there is no mechanism in place to enforce this.

### Tick Callback

\todo this section is not yet complete

```cpp
auto widget = // ...
grid.set_tick_callback([](FrameClock clock) -> TickCallbackResult{
    std::cout << "Time since last render: " << clock.get_time_since_last_frame().as_seconds() << std::endl;
    return TickCallbackResult::CONTINUE;
});
```

---

## Window

Windows are central to any application, as such, \a{Window} s constructor takes an `Application*` (pointer to `Application` instance). Once all windows of an application are closed, the application usually exists, though we can prevent this using \a{Application::hold}.

Windows are widgets, though they occupy somewhat of a special place. A `Window` cannot be the child of any other widget, it is **top-level**.
This means that any and all widgets are children of a `Window`, usually indirectly, as a `Window` can only contain a single widget, which we set using `Window::set_child`.
In most cases, this child will itself be a container widget that can have any number of children.

`Window` has three signals (on top of those inherited from `Widget`) that we can connect to:

\signals
\signal_close_request{Window}
\signal_activate_default_widget{Window}
\signal_activate_focused_widget{Window}

`activate_default_widget` is emitted when the default widget is activated. This is a widget designated as such via `Window::set_default_widget`. If the user presses the enter key while the window is shown, this widget will emit its `activate` signal. This is useful for small message dialogs that have, for example, an "ok" and "cancel" button. By making the "ok" button the default widget, the user can simply press enter to confirm, as opposed to having to move the mouse to physically click on the button.

Another of `Window`s signals,  `activate_focused_widget` is emitted when the widget that currently *holds input focus* is activated. We will learn what input focus is in the [chapter on event handling](05_events.md), for now, simply note that this signal exists.

Lastly, we have signal `close_request`, which requires some explanation.

### Close Request

When the window manager, which is the part of the user's operating system that deals with window layout and lifetime, request a window to close, the window does not immediately close. Instead, `Window` emits `close_request`, which we can connect to. This signal can be conceptualized as the window asking "should I really close right now?". If no signal handler is connected, the window does, which is the default behavior. If we do connect a signal handler, we can not only trigger custom behavior, but we can also **prevent closing of the window**.

We recall the signature of `close_request` to be `(Window*, (Data_t)) -> WindowCloseRequestResult`. This return type, \a{WindowCloseRequestResult} is an enum with two values `ALLOW_CLOSE` and `PREVENT_CLOSE`. Depending on which of the two is returned, the window does or does close when requested.

For example, to prevent a `Window` instance from closing until a global flag is set, we can do the following:

```cpp
auto window = Window(// ...

static bool should_close = false;
window.connect_signal_close_request([](Window*){

    std::cout << "Window is attempting to close" << std::endl;
    
    if (should_close)
        return WindowCloseRequestResult::ALLOW_CLOSE;
    else
        return WindowCloseRequestResult::PREVENT_CLOSE;
});
```

### Opening / Closing a Window

When we create a `Window` instance, it will be initially hidden. In this way, none of its widgets will be realized or shown, and the user has no way to know that the window exists. A `Window`s lifetime only begins once we call `Window::present`. This essentially opens the window and shows it to the user. We've seen this in our `main.cpp`:

```cpp
app.connect_signal_activate([](Application* app)
{
    // main window
    auto window = Window(*app);

    // initialization here

    // make main window actually visible
    window.present();
});
```

This is the case for all windows. For example, if we wanted to open another window only when a button in the main window is pressed, we can do the following:

```cpp
// in app.connect_signal_activate

auto main_window = Window(*app);
static auto secondary_window = Window(*app);

auto button = Button();
button.connect_signal_clicked([](Button*){
    secondary_window.present();
});
main_window.set_child(button);

main_window.present();
```

Where we declared `secondary_window` to be `static`, such that we can safely reference it from inside the lambda.

Conversely, if we want a window to go back to its initial state, where it is hidden from the user, we call `Window::hide`. If we want to close the window permanently, unrealizing all widgets inside it, we use `Window::close`.

### Window Properties

Other than the signals, we can set some basic properties of any `Window`:

#### Title

`Window::set_title` sets the name displayed in the window's header bar. By default, this name will be the name of the application. We can choose to hide the title by simply calling `window.set_title("")`.

#### Modality & Transience

When dealing with multiple windows, we can influence the way two windows interact with each other. Two of these interactions are determined by whether a window is **modal** and whether it is **transient** for another window.

By setting `Window::set_modal` to true, if the window is revealed, **all other windows of the application will be deactivated**, preventing user interaction with them. This also freezes animations, it essentially pauses all other windows. The most common use-case for this is for dialogs, for example if the user requests to close the application, it is common to open a small dialog requesting the user to confirm exiting the application. While this dialog is shown, the main window should be paused until the user chooses an action.

Using `Window::set_transient_for`, we can make sure a window will always be on top of another. `A.set_transient_for(B)` will make it so, while `A` overlaps `B` on the user's desktop, `A` will always be shown on top. This is again useful for message dialogs, if the dialog is not transient for the main window it could appear "behind" the main window, being essentially invisible to the user.

#### HeaderBar

The part on top of a window is called the **header bar**. By default, it will show the window title and a minimize-, maximize- and close-button. We can completely hide the header bar using `Window::set_is_decorated`, though this is rarely recommended.

All `Window`s have space for one more widget other than its child. To set it, we use `Window::set_titlebar_widget`. This will usually be a \a{HeaderBar}, though any widget can be inserted this way.

`HeaderBar` is a widget that has three areas where other widgets can be inserted, at the front (left of the title), at the end (right of the title), or as the title. This gives us full flexibility, we can put whatever we want anywhere in a window's header bar.

An alternative way of formatting the `HeaderBar` is with **header bar layouts**, which is supplied to `HeaderBar::set_layout`.

Layout is a string that is a list of button IDs. The four valid IDs are:

+ `maximize`: Maximize Button
+ `minimize`: Minimize Button
+ `close`: Close Button

We choose which buttons are allocated on which side of the title by using `:`. Buttons before `:` are shown left of the title, buttons after `:` are shown right of the title. A list of buttons is delineated with `,`.

Some examples, where we used the alternative constructor for `HeaderBar` which takes the layout string directly:

```cpp
window.set_title("mousetrap");
window.set_titlebar_widget(HeaderBar("close:minimize,maximize"));
```

\image html headerbar_close_minimize_maximize.png

```cpp
window.set_title("");
window.set_titlebar_widget(HeaderBar("close:"));
```

\image html headerbar_close_no_title.png


```cpp
window.set_title("mousetrap");
window.set_titlebar_widget(HeaderBar(":close"));
```

\image html headerbar_title_close.png

---

## Label

In contention for being *the* most used widget, `Label`s are important to understand. A `Label` displays static text. It is initialized as one would expect:

```cpp
auto label = Label("text");
```

To change a `Label`s text after initialization, we use `Label::set_text`. By default, a label will be a single-line of text, though it can also be used to display multi-line text. If this is the case, we have some extra options to choose how that text is formatted:

### Justify Mode

[Justification](https://en.wikipedia.org/wiki/Typographic_alignment) determines how words are distributed along the horizontal axis. There are 5 modes in total, all of which are part of the enum \a{JustifyMode}:

\image html text_justify_left.png "JustifyMode::LEFT"<br>
\image html text_justify_center.png "JustifyMode::CENTER"<br>
\image html text_justify_right.png "JustifyMode::RIGHT"<br>
\image html text_justify_fill.png "JustifyMode::FILL"<br>

Where the fifth mode is `JustifyMode::NONE` which arranges all text in exactly one line.

### Wrapping

Wrapping determines after which character in the text, a line break is inserted if the text's width exceeds that of the `Label`s parent widget. For wrapping to happen at all, the `JustifyMode` has to not be set to anything **other** than `JustifyMode::NONE`.

Wrapping mode are values of the enum \a{LabelWrapMode}, which has the following values:

| `LabelWrapMode` | Meaning                                                 | Example                 |
|-----------------|---------------------------------------------------------|-------------------------|
| `NONE`          | no wrapping                                             | `"humane mousetrap"`    |
| `ONLY_ON_WORD`  | line will only be split between two words               | `"humane\nmousetrap"`   |
| `ONLY_ON_CHAR`  | line will only be split between syllables, adding a `-` | `"hu-\nmane mousetrap"` |
| `WORD_OR_CHAR`  | line will be split between words and/or syllables       | see above               |

Where `\n` is the newline character.

By default, once text wrapping is enabled by setting the `JustifyMode` to something other than `JustifyMode::NONE`, `LabelWrapMode::WORD_OR_CHAR` is used as the wrap mode.

### Ellipsize Mode

If a line is too long for the available space **and wrapping is disabled**, **ellipsizing** will take place. The corresponding enum `EllipsizeMode` has four possible value:

| `EllipsizeMode` | Meaning                                                 | Example                     |
|-----------------|---------------------------------------------------------|-----------------------------|
| `NONE`          | text will not be ellipsized                             | `"Humane mousetrap engineer"` |
| `START`         | text starts with `...`, showing only the last few words | `"...engineer"`               |
| `END`           | text ends with `...`, showing only the first few words  | `"Humane mousetrap..."`       |
| `MIDDLE`        | "..." in the center, shows start and beginning           | `"Humane...engineer"`         |

### Markup

Labels support **markup**, which allows users to change properties about individual words or characters in a way very similar to formatting html. Markup in mousetrap uses [Pango attributes](https://docs.gtk.org/Pango/pango_markup.html), which allows for the following markup properties:

| `tag`        | Label String              | Result                  |
|--------------|---------------------------|-------------------------|
| `b`          | `<b>bold</b>`             | <b>bold</b>             |
| `i`          | `<i>italic</i>`           | <i>italic</i>           |
| `u`          | `<u>underline</u>`        | <u>underline</u>        |
| `s`          | `<s>strikethrough</s>`    | <s>strike-through</s>    |
| `tt`         | `<tt>inline_code</tt>`    | <tt>inline_code</tt>    |
| `small`      | `<small>small</small>`    | <small>small</small>    |
| `big`        | `<big>big</big>`          | <h3>big</h3>            |
| `sub`        | `x<sub>subscript</sub>`   | x<sub>subscript</sub>   |
| `sup`        | `x<sup>superscript</sup>` | x<sup>superscript</sup> |
| `&#` and `;` | `&#129700;`               | 🪤                      | 

Where in the last row, we used the [decimal html code](https://www.compart.com/en/unicode/U+1FAA4) for the mousetrap emoji that Unicode provides.

\note Pango only accepts the **decimal** code, not hexadecimal. For example, the mousetrap emoji has the decimal code `129700`, while its hexadecimal code is `x1FAA4`. To use this emote in text, we thus use `&#129700;`, **not** `&#x1FAA4;`. The latter will not work.

\note All `<`, `>` will be parsed as style tags when escaped with `\`. To prevent this, simply us `&lt;` (less-than) and `&gt;` (greater-than) instead of `<` and `>`. For example, we would write the formula `x < y` as `"x &lt; y"`.

For things like color, different fonts, and many more options, [consult the Pango documentation](https://docs.gtk.org/Pango/pango_markup.html).

---

## Box

\a{Box} is the simplest multi-widget container in mousetrap and is used extensively in almost any application. A box aligns its children horizontally or vertically, depending on whether we pass \a{Orientation::HORIZONTAL} or \a{Orientation::VERTICAL} to its constructor. We can change the orientation of a box after construction using `set_orientation`.

We can add widgets to the start, end, and after any other widget already inside the box using `Box::push_front`, `Box::push_back` and `Box::insert_after`, respectively:

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

\a{CenterBox} is a container that has exactly three children, situated around a centered child. `CenterBox` prioritizes keeping the designated center child centered at all costs, making it a better choice for a layout where symmetry is desired.

We use `CenterBox::set_start_child`, `CenterBox::set_center_child`, and `CenterBox::set_end_child` to choose the corresponding child widget.

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

This widget is often used as a background to another widget, to fill empty space, or as en element visually separating two sections. Often, we want to have the separator be a specific thickness. This can be accomplished using size-hinting. For example, to draw a horizontal line similar to the `<hr>` tag in HTML, we would do the following:

```cpp
auto hr = Separator();
separator.set_expand_horizontally(true);
separator.set_expand_vertically(false);
separator.set_size_request({
    0,  // width: any 
    3   // height: exactly 3 px
});
```

This will render as a 3-pixel high line that expands horizontally.

---

## ImageDisplay

\a{ImageDisplay} is used to display static images. It works by taking image data in RAM, deep-copying it into the graphics card memory,
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

After realization, we cannot change the contents of `ImageDisplay` directly. If the file on disk changes, `ImageDisplay` remains unchanged. If we want to update `ImageDisplay`, we need to call `create_from_file` manually again.

By default, `ImageDisplay` behaves just like any other widget, in that it scales freely, which, in case of a raster-based image, may distort the image. To prevent this, we can call `display.set_expand(false)` which prevents expansion of the widget, then size-hint it like so:

```cpp
display.set_expand(false);
display.set_size_request(display.get_size());
```

Where `ImageDisplay::get_size` returns the original resolution of the image it was created from. For example, for an `.png` file of size 75x35 px, this display would always be exactly 75x35 pixels on screen, meaning it is displayed at 1:1 resolution, avoiding any artifacting due to scaling.

---

## Button

We've seen `Button` in the previous chapter, it is one of the simplest and inuitive widgets, both for the user and developer.

\a{Button} has the following signals:

\signals
\signal_activate{Button}
\signal_clicked{Button}

Where physically clicking the button both emits `activate` and `clicked`, while calling `Widget::activate` only emits `activate`, not `clicked`.

To change the label of a button, we use `Button::set_child`. This will usually be a `Label` or `ImageDisplay`, though any arbitrary widget can be used as a child.

Other than the child widget, we can customize the look of a button further. `Button::set_has_frame` will change whether the button has a texture and outline to it, while `Button::set_is_circular` changes the button to a fully rounded appearance:

\image html button_types.png

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

Where the above shown buttons have the following properties:

| Button | set_has_frame | set_is_circular |
|--------|---------------|-----------------|
| 01  | true | false |
| 02 | false | false |
| 03 | true | true |

Buttons are ideal to trigger run-through `Action`s, which are action that do a single thing, then immediately exit. In a situation like this, we prefer [creating an `Action` and associating it with the button](03_actions.md) using `Button::set_action`. This is more scalable than connecting to signal `clicked`, though the latter certainly has its uses. For example, when the resulting behavior is unique to that button and does not appear anywhere else in our application, `connect_signal_clicked` should be used over `Action`.

---

## ToggleButton

`ToggleButton` is a specialized form of `Button`. It supports most of `Button`s methods/signals, including signal `clicked`, `set_child`, `set_has_frame` and `set_is_circular`.
Unique to `ToggleButton` is that, if clicked, the button will **remain pressed**. When clicked again, it returns to being unpressed. Anytime the internal state of the `ToggleButton` changes, the `toggled` signal will be emitted.

\signals
\signal_toggled{ToggleButton}
\signal_activate{ToggleButton}
\signal_clicked{ToggleButton}

In this way, `ToggleButton` can be used to track a boolean state.

To check whether the button is currently toggled, we use `ToggleButton::get_active`, which returns `true` if the button is currently pressed, `false` otherwise.

---

## CheckButton

\{CheckButton} is almost identical to `ToggleButton` in function, but not appearance. `CheckButton` is an empty box in which a checkmark appears when it is toggled. Just like before, we query whether it is pressed by calling `CheckButton::get_active`.

\signals
\signal_activate{CheckButton}
\signal_toggled{CheckButton}

`CheckButton` can be in one of three states, as opposed to two, all of which are represented by the enum \a{CheckButtonState}. The button can either be `ACTIVE`, `INACTIVE`, or `INCONSISTENT`. This changes the appearance of the button:

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

As the last widget intended to convey a boolean state to the user, we have \a{Switch}, which has the appearance similar to a light switch. It can be clicked or dragged in order to change its state. `Switch` does not emit `toggled`, instead we connect to the `activate` signal, which is emitted anytime the switch is flicked to either side.

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

Almost identical to `ToggleButton` in function, `Switch` should be preferred for applications where the boolean-indicating widget does not need a label, as `Switch` does not provide `set_child`.

---

## Adjustment

From widgets conveying a boolean state, we will now move on to widgets conveying a discrete number. These let the user choose a value from a **range**, which, in mousetrap, is represented by a signal emitter called \a{Adjustment}.

In general, a range has a **lower and upper value**. For example, the range `[0, 2]` has the `lower` of `0` and `upper` of `2`. A second property is the **step increment**, which is the minimum difference between two adjacent values in the range. For example, if our range is still `[0, 2]` and the step increment is `0.5`, then that includes the numbers `{0, 0.5, 1, 1.5, 2}`. If the step increment is `0.01`, `[0,2]` includes the numbers  `{0, 0.01, 0.02, ..., 1.98, 2.00}`.

Turning to the actual `Adjustment` class, it has four properties

+ `lower`: lower bound of the range
+ `upper`: upper bound of the range
+ `increment`: step increment
+ `value`: current value, in `[lower, upper]`

For example, expressing the previous range of `[0, 2]` with step increment `0.5`, we create an `Adjustment` like so:

```cpp
auto adjustment = Adjustment(
    1,      // value
    0,      // lower
    2,      // upper
    0.5     // increment    
);
```

Which will have the value of `1` on initialization.

We usually do not need to create our own `Adjustment`, rather, it is provided by a number of widgets. Notable about this is that the widget and its adjustment **are automatically kept in synch**. If we change any property of the `Adjustment`, the widget will change its appearance accordingly.

Adjustment has two signals:

\signals
\signal_value_changed{Adjustment}
\signal_properties_changed{Adjustment}

We can connect to `value_changed` to monitor the value property of an `Adjustment` (and thus whatever widget is controlled by it), while `properties_changed` is emitted when one of upper, lower or step increment changes, usually by calling `Adjustment::set_upper`, `Adjustment::set_lower` and/or `Adjustment::set_increment`.

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

`SpinButton` is used to pick an exact value from a range. The user can click the rectangular area and manually enter a value using the keyboard, or they can increase or decrease the current value by the step increment of the `SpinButton`s adjustment by pressing the plus or minus button, which are positioned according to the `SpinButton`s orientation.

We supply the properties of the range underlying the `SpinButton` in its constructor:

```cpp
// create SpinButton with range [0, 2] and increment 0.5
auto spin_button = SpinButton(0, 2, 0.5)
```

If we want to check any of the properties of the  `SpinButton`s range, we can either query the `Adjustment*` returned by `SpinButton::get_adjustment`, or we can get the values directly using `SpinButton::get_value`, `SpinButton::get_lower`, etc. This is just for the sake of convenience, both ways have identical behavior.

`SpinButton` has two signals:

\signals
\signal_value_changed{SpinButton}
\signal_wrapped{SpinButton}

Only the latter of which needs explanation, as we recognize `value_changed` from `Adjustment`.

When the user reaches one end of the `SpinButton`s range, which, for a range of `[0, 2]` would be either the value `0` or `2`, if the user attempts to increase or decrease the value further, nothing will happen. However, if we set `SpinButton::set_can_wrap` to `true`, the value will wrap all the way around. For example, trying to increase the value while it is `2`, the value will jump to `0`, and vice versa.

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

\a{Scale}, just like `SpinButton`, is a widget that allows a user to choose a value from the underlying `Adjustment`. This is done by click-dragging the knob of the scale, or clicking anywhere on its rail. In this way, it is usually harder to pick an exact decimal value on a `Scale` as opposed to a `SpinButton`, though we can aid in this task by displaying the exact value next to the scale. This has to first be enabled using `Scale::set_should_draw_value`:

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


`Scale`supports most of `SpinButton`s functions, including querying information about its underlying range and orienting it.

---           

## ScrollBar

Similar to `Scale`, \a{ScrollBar} is used to pick a value on a floating-point scale. It is often used as a way to choose which part of a widget should be shown on screen. For an already-automated way of doing this to control which part of a larger widget to show, see `ScrolledWindow`.

---

## LevelBar

\a{LevelBar} is used to display a fraction, often used to indicate the level of something, for example the volume of a playback device.

To create a `LevelBar`, we need to specify the minimum and maximum value of the range we wish to display. We can then set the current value using `LevelBar::set_value`, which takes the absolute current value. The resulting fraction is computed automatically based on the upper and lower limit we supplied to the constructor:

```cpp
// create level for range [0, 2]
auto level_bar = LevelBar(0, 2);
level_bar.set_value(1.0); // set to 50%
```

The bar will be oriented horizontally by default, but we can call `set_orientation` to change this.

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

The bar can also be used to display a discrete value (a range only consisting of integers), in which case the bar will be shown segmented. We can set the `LevelBar`s mode using `LevelBar::set_mode`, which takes either `LevelBarMode::CONTINUOUS` or `LevelBarMode::DISCRETE` as its argument.

---

## ProgressBar

A specialized case of indicating a continuous value is that of a **progress bar**. A progress bar is used to show how much of a task is currently complete. This is most commonly used during a multi-second loading animation, for example during the startup phase of an application. As more and more resources are loaded, the progress bar fills, which communicates to the user how long they will have to wait.

`ProgressBar` is built for this exactly this purpose. It does not have an upper or lower bound, they are fixed at `[0, 1]`. We can set the current fraction using `ProgressBar::set_fraction`. `ProgressBar` has a special animation trigger, which makes the bar "pulse". This is supposed to communicate to the user some progress was made. To trigger this pulse animation by simply calling `ProgressBar::pulse`. Note that this does not change the currently displayed fraction of the progress bar, it only plays the animation.

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

To signal progress where we do not have an exact fraction, we can use the \a{Spinner} widget. It simply displays an animated spinning icon, communicating progress. Using `Spinner::set_is_spinning`, we can control whether the animation is currently playing or not.

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

We've learned how to choose a widget allowing a boolean and discrete value to be shown/entered. We will now move on to inputting **text**, which is central to many desktop applications. \a{Entry} is the widget of choice for single-line text entry.

`Entry` is an area that, when clicked, allows the user to type freely. This current text is stored in an internal text buffer. We can access or modify the buffers content with `Entry::get_text` and `Entry::set_text`, respectively. `set_text` fully overrides the buffer, losing any data contained therein.

While we could control the size of an `Entry` using size-hinting, a less manual way is `Entry::set_max_length`, which takes an integer representing the number of characters that the prompts should make space for. For example, `entry.set_max_length(15)` will resize the entry such that it is wide enough to show 15 characters at the current font size.

`Entry` supports "password mode", which is when each character typed is replaced with a dot or otherwise made unreadable. This is to prevent a third bad actor reading entered passwords on our users screen. We can enter this mode with `Entry::set_text_visible`, where `false` is password-mode and `true` is regular mode.

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

Lastly, `Entry` is *activatable*. Users usually do this by pressing the enter key while the cursor is inside the `Entry`. This does not cause any behavior initially, but we can connect to the `activate` signal of `Entry` to choose a custom function to be called.

Other than `activate`, `Entry` has one more signal `text_changed`, which is emitted whenever the internal buffer changes, including when set programmatically using `set_text`.

\signals
\signal_activate{Entry}
\signal_text_changed{Entry}

Note that the user cannot insert a newline character using the enter key. As such, `Entry` should exclusively be used for text prompts which are exactly one line. For multi-line text entry, we should use the next widget instead.

## TextView

While we can technically input a newline character into `Entry` by copy pasting the corresponding control character, it is not possible to display two lines at the same time. For this purpose, we use \a{TextView}. It supports a number of basic text-editor features, including **undo / redo**, which are triggered by the user pressing Control + Z or Control + Y respectively. We as developers can also trigger this behavior manually with `TextView::undo` / `TextView::redo`.

Much like `Label`, we can set how the text aligns horizontally using `TextView::set_justify_mode`. To further customize how text is displayed, we can choose the **internal margin**, which is the distance between the frame of the `TextView` and the text inside of it. `TextView::set_left_margin`, `TextView::set_right_margin`, `TextView::set_top_margin` and `TextView::set_bottom_margin` allow us to choose these values freely.

`TextView` does **not** have the `activate` signal, pressing enter while the cursor is inside the widget will simply create a new line. Instead, it has the following signals, where `text_changed` behaves exactly like it does with `Entry`:

\signals
\signal_text_changed{TextView}
\signal_undo{TextView}
\signal_redo{TextView}

---

## Dropdown

Sometimes, we want users to be able to pick a value from a **set list of values**, which may or may not be numeric. \a{DropDown} allows for this.

When the `DropDown` is clicked, a window presents the user with a list of items. The user can click on any of these, at which point the dropdown will invoke the corresponding function for that item, which is set using \{DropDown::push_back}. This function takes three arguments:

+ **list label**: widget displayed inside the dropdown window
+ **selected label**: widget displayed once one of the items is selected
+ **callback**: function with signature `(DropDown*, (Data_t)) -> void`, which is invoked when a selection is made

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

\note When a `DropDown` is realized, the callback of whatever the first item we added is invoked exactly once

In praxis, we would want the callback to mutate some global property to keep track of which item is selected. Alternatively, we can query which item is currently selected by calling `DropDown::get_selected`. This function returns an **item ID**, which is obtained when we call `DropDown::push_back`:

```cpp
auto dropdown = DropDown();
auto id_01 = dropdown.push_back(Label("01"), Label("Option 01"), [](DropDown*){});
auto id_02 = dropdown.push_back(Label("02"), Label("Option 02"), [](DropDown*){});

// check if selected item is 01
bool item_01_selected = dropdown.get_selected() == id_01;
```

Where `[](DropDown*){}` is a lambda that simply does nothing (but still conforms to the `(DropDown*, (Data_t)) -> void` signature).

Using item IDs or each item's callback, we can trigger custom behavior whenever the user changes their selection to one of the dropdown items.

---

## Frame

\a{Frame} is a purely cosmetic widget that displays whatever child we choose using `Frame::set_child` in a frame with a small border and rounded corners:

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

`Frame` is rarely necessary, but will make GUIs seem more aesthetically pleasing and polished.

---

## AspectFrame

Not to be confused with `Frame`, \{AspectFrame} adds no graphical element to its singular child. Instead, the singular child widget added with `AspectFrame::set_child` will be forced to stay in a certain **aspect ratio**, its width to height ratio will stay constant regardless of the size allocation of the `AspectFrame`s parent.

We choose the aspect ratio either in `AspectFrame`s constructor or with `AspectFrame::set_ratio`, both of which accept a floating point ratio calculated as `width / height`. For example, if we want to force a widget to keep an aspect ratio of 4:3:

```cpp
auto child_widget = // ...
auto aspect_frame = AspectFrame(4.f / 3) // 4:3 aspect ratio
aspect_frame.set_child(child_widget);
```

Where we wrote `4.f / 3` instead of `4 / 3` because in C++, the latter would trigger [integer division](https://en.wikipedia.org/wiki/Division_(mathematics)#Of_integers) resulting in a ratio of `1` (instead of the intended `1.333...`).

---

## Overlay

So far, all widget containers have aligned their children such that they do not overlap. To render a widget on top of another, we have to use \a{Overlay}.

`Overlay` has one "base" widget, which is at the conceptual bottom of the overlay. We set this widget using `Overlay::set_child`. We can then add any number of widgets on top of the base widget using `Overlay::add_overlay`:

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

By default, `Overlay` will allocate exactly as much space as the base widget (set with `Overlay::set_child`) does. If one of the overlaid widgets takes up more space than the base widget, it will be truncated. We can avoid this by supplying a second argument to `Overlay::add_overlay`, which is a boolean indicated whether the overlay widget should be included in the entire container's size allocation. That is, if the overlaid widget is larger than the base widget, the `Overlay` will resize itself such that the entire overlaid widget is visible.

```cpp
overlay.add_overlay(overlay_widget, true); 
    // resize if overlay_widget allocates more space than child
``` 

When one interactable widget is shown partially overlapping another, only the top-most widget can be interacted with by the user. If we want the user to be able to access a lower layer, we need to make any widget on top non-interactable, either by calling `Widget::hide` or `Widget::set_can_respond_to_input(false)`.

---

## Revealer

While not technically necessary, animations can improve user experience drastically. Not only do they add visual style, they can hide abrupt transitions or small loading times. As such, they should be in any advanced GUI designer's repertoire.

One of the most common applications for animations is that of hiding or showing a widget. So far, when we called `Widget::hide` or `Widget::show`, the widget appears instantly, one frame after the function was called. This works, but when showing a large widget, other widgets around it will want to change their size allocation, which may drastically change the entire window's layout instantly.

To address this, mousetrap offers \a{Revealer}, which plays an animation to reveal or hide a widget.
`Revealer` always has exactly one child, set with `Revealer::set_child`. `Revealer` itself has no graphical element, it acts just like a `Box` with a single child.

To trigger the `Revealer`s animation and change whether the child widget is currently visible, we call `Revealer::set_revealed` which takes a boolean as its argument. If the widget goes from hidden to shown or shown to hidden, the animation will play with no further interaction needed.

Once the animation is done, signal `revealed` will be emitted:

\signals
\signal_revealed{Revealer}

Using this, we can trigger our own behavior, for example to update a widgets display or trigger additional animations.

### Transition Animation

We have control over the kind and speed of the transition animation. By calling `Revealer::set_transition_duration`, we can set the exact amount of time an animation should take. For example, to set the animation duration to 1 second:

```cpp
auto revealer = Revealer();
revealer.set_child(// ...
revealer.set_transition_duration(seconds(1));
```

Where `seconds` returns a `mousetrap::Time`.

Apart from the speed, we also have a choice of animation **type**, represented by the enum \a{RevealerTransitionType}. Animations include a simple cross-fade, sliding, swinging, or `NONE`, which instantly shows or hides the widget. For more information on the look of the animation, see the \link mousetrap::RevealerTransitionType related documentation page\endlink.

---

## Expander

\a{Expander} is similar to `Revealer`, in that it also has exactly one child widget, and it shows / hides the widget. Unlike `Revealer`, there is no animation attached to `Expander`. Instead, it hides the widget behind a collapsible label:

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

We set the `Expander`s child widget with `Expander::set_child`. We will furthermore want to specify a label widget, which is the widget shown next to the small arrow.  Set with`Expander::set_label_widget`, this widget will usually be a `Label`, though, again, any arbitrarily complex widget can be used.

Note that `Expander` should not be used for the purpose of large nested list, for example those displaying a file system tree. A purpose-built widget for this already exists. It is called `ListView` and we will learn how to use it later in this chapter.

---

## Paned

\a{Paned} is a widget that always has exactly two children. Between the two children, a visual barrier is drawn. The user can click on this barrier and drag it horizontally or vertically, depending on the orientation of the `Paned`. This gives the user the option to resize how much of a shared space two widgets allocated.

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

Assuming the `Paned`s orientation is `Orientation::HORIZONTAL`, we can set the child on the left using `Paned::set_start_child` and the child on the right with `Paned::set_end_child`. Both children have to be set to valid widgets in order for the user to have the option of interacting with the `Paned`. If we, for some reason, do not have two widgets but would still like to use a `Paned`, we should add a `Separator` as the other child.

`Paned` has two per-child properties: whether a child is **resizable** and whether it is **shrinkable**.

Resizable means that if the `Paned` changes size, the child should change size with it.

Shrinkable sets whether the side of the `Paned` can be made smaller than the allocated size of that side's child widget. If set to `true`, the user can drag the `Paned`s barrier, such that one of the widgets is partially or completely hidden, regardless of its size allocation.

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

`Paned::set_end_child_shrinkable(true)` made it possible to move the barrier such that the left child is partially covered.

---

## ScrolledWindow

By default, most containers will allocate a size that is equal to or exceeds the largest preferred size of its children. For example, if we create a widget that has a preferred size of 5000x1000 px and use it as the child of a `Window`, the `Window` will attempt to allocate 5000x1000 pixels on screen, making the window far larger than most screens can display. In situations like these, we should instead use a \a{ScrolledWindow}, which allows users to only view part of a larger widget:

```cpp
auto child = Separator();
child.set_size_request({5000, 5000});

auto scrolled_window = ScrolledWindow();
scrolled_window.set_child(child);

state->main_window.set_child(scrolled_window
```
\image html scrolled_window.png

Without the `ScrolledWindow`, the `Separator` child widget would force the outer `Window` to allocate 5000x5000 pixels, as we size-hinted it to be that size. By using `ScrolledWindow`, the `Window` is free to allocate any size, retaining resizability. The end-user can influence which area of the larger widget is currently visible by operating the scrollbars inherent to `ScrolledWindow`.

### Size Propagation

By default, `ScrolledWindow` will disregard the size of its child and simply allocate an area based on its `ScrolledWindow`s properties only. We can override this behavior by forcing `ScrolledWindow` to **propagate** the width or height of its child.

By calling `ScrolledWindow::set_propagate_natural_width(true)`, `ScrolledWindow` will assume the width of its child. Conversely, calling `ScrolledWindow::set_propagate_natural_width(true)` forces the window to allocate space equal to the height of its child.

### Scrollbar Policy

`ScrolledWindow` has two scrollbars, controlling the horizontal and vertical position. If we want to trigger behavior in addition to changing which part of the child widget `ScrolledWindow` displays, we can access each scrollbars `Adjustment` using `ScrolledWindow::get_horizontal_adjustment` and `ScrolledWindow::get_vertical_adjustment` respectively, then connect to the `Adjustment`s signals.

By default, once the cursor enters `ScrolledWindow`, both scrollbars will reveal themselves. If the cursor moves outside the `ScrolledWindow`, the scrollbars will hide again.

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

A \a{Popover} is a special kind of window. It is always **[modal](#modality--transience)**. Rather than having the normal window decoration with a close button and title, `Popover` closes dynamically or on demand. Showing the popover is called **pop up**, closing the popover is called **pop down**, `Popover` correspondingly has `Popover::popup` and `Popover::popdown` to trigger this behavior.

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

Popovers can only be while they are **attached** to another widget. We use `Popover::attach_to` to specify this widget, while `Popover::set_child` chooses which widget to display inside the popover. Like `Window`, `Popover` always has exactly one child.

Manually calling `popup` or `popdown` to show/hide the `Popover` can be a tedious process. Luckily, mousetrap offers a widget that fully automates this process for us.

## PopoverButton

Like `Button`, `PopoverButton` has a single child, can be circular, and has the `activate` signals. `PopoverButton`s purpose is usually to simply show or hide a `Popover`.

We first create the `Popover`, then connect it to the button using `PopoverButton::set_popover`. We do not use `Popover::attach_to`.

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

For 90% of cases, this is the way to go when we want to use a `Popover`. It is easy to set up and we don't have to manually control the popover position, or when to show or hide it.

The arrow character next to the `PopoverButton`s child indicates to the user that clicking it will reveal a popover. We can suppress this arrow by setting `PopoverButton::set_always_show_arrow` to `false`.

`PopoverButton` lets us control the relative position of the popover by setting `PopoverButton::set_popover_position` to one of the following: `RelativePosition::ABOVE`, `RelativePosition::LEFT_OF`, `RelativePosition::RIGHT_OF`, `RelativePosition::BELOW`, which will place the popover above, left of, right of or below the button respectively.

We will see one more use of `PopoverButton` in the [chapter on menus](06_menus.md), where we use `PopoverMenu`, a specialized form of `Popover` that shows a menu instead of an arbitrary widget child.

---

## Selectable Widgets: SelectionModel

We will now move on to **selectable widgets**, which tend to be the most complex and powerful widgets in mousetrap.

All selectable widgets have some things in common: They a) are widget containers supporting multiple children and b) provide `get_selection_model`, which returns a \{SelectioModel} representing the currently selected widget.

`SelectionModel` is not a widget, though it is a signal emitter. Similar to `Adjustment`, it is bound to a certain widget. Changing the widget updates the `SelectionModel`, changing the `SelectionModel` updates the widget. We usually do not construct `SelectionModel` directly, instead we access the underlying `SelectionModel` once we instanced a selectable widget.

`SelectionModel` provides signal `selection_changed`, which is emitted whenever the internal state of the `SelectionModel` changes.

\signals
\signal_selection_changed{SelectionModel}

The signal provides two arguments, `position`, which is the newly selected item, and `n_items`, which is the new number of currently selected items.

The latter is necessary because `SelectionModel`s can have one of three internal **selection modes**, represented by the enum \a{SelectionMode}:

+ `NONE`: Exactly 0 items are selected at all times
+ `SINGLE`: Exactly 1 item is selected at all times
+ `MULTIPLE`: 0 or more items can be selected

To show how `SelectionModel` is used, we first need to create our first selectable widget.

## ListView

\a{ListView} is a widget that arranges its children in a row (or column if its orientation is `VERTICAL`) in a way similar to `Box`. Unlike `Box`, `ListView` is *selectable*.

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

When creating the `ListView`, the first argument to its constructor is the \a{Orientation}, while the second is the underlying `SelectionModel`s mode. If left unspecified, `SelectionMode::NONE` is used.

Much like `Box`, `ListView` supports `ListView::push_back`, `ListView::push_front` and `ListView::insert` to insert any widget at the specified position.

`ListView` can be requested to automatically show separator in between to items. To show these, we simply call `ListView::set_show_separators(true)`.

### Nested Trees

By default, `ListView` displays its children in a linear list, either horizontally or vertically. `ListView` also supports **nested lists**, sometimes call a tree view:

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

Here, we have a triple nested list. The outer list has the items `outer item #01`, `outer item #02` and `outer item #03`. `outer item #02` is itself a list, with two children `inner item #01` and `inner item #02`, the latter of which is also a list with a single item.

When `ListView::push_back` is called, it returns an **iterator**. When we supply this iterator as the second argument to any of the widget-inserting functions, such as `ListView::push_back`, the new child will be inserted into a nested list start at the item the iterator was created from. If no iterator is specified, the item will be inserted in the top-level list.

```cpp
auto it_01 = list_view.push_back(/* outer item #01 */);
auto it_02 = list_view.push_back(/* outer item #02 */);

  auto inner_it_01 = list_view.push_back(/* inner item #01 */, it_02);
  auto inner_it_02 = list_view.push_back(/* inner item #02 */, it_02);
   
    auto inner_inner_it_01 = list_view.push_back(/* inner inner item #01 */, inner_it_02);
    
auto it_03 = list_view.push_back(/* outer item #03 */);
```

This means if we only want to show items in a simple, non-nested list, we can ignore the iterator return value completely.

### Reacting to Selection

In order to react to the user selecting a new item in our `ListView` (if its selection mode is anything other than `NONE`), we should connect to the lists `SelectionModel` like so:

```cpp
auto list_view = ListView(Orientation::HORIZONTAL, SelectionMode::SINGLE);

list_view.get_selection_model()->connect_signal_selection_changed(
    [](SelectionModel*, int32_t item_i, int32_t n_items){
        std::cout << "selected: " << item_i << std::endl;
    }
);
```

This process will be the same for any of the selectable widgets, as all of them provide `get_selection_model`, which returns a pointer to the underlying `SelectionModel`.

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

Items are dynamically allocated to rows and columns based on the space available to the `GridView` and the size of the children. We can use `GridView::set_min_n_columns` and `GridView::set_max_n_columns` to force one of either row or columns (depending on `Orientation`) to adhere to the given limit, which gives us more control over how the children are arranged.

Other than this, `GridView` supports the same functions as `ListView`, including `push_front`, `push_back`, `insert`, `get_selection_model`, `set_show_separators`, etc.

---

## Column View

\a{ColumnView} is used to display widgets as a table, which is split into rows and columns, the latter of which have a title.

To fill our `ColumnView`, we first instance it, then allocate a number of columns:

```cpp
auto column_view = ColumnView(SelectionMode::SINGLE);
column_view.push_back_column("Column 01");
column_view.push_back_column("Column 02");
column_view.push_back_column("Column 03");
```

To add a column at a later point, either to the start, end or at a specific position, we use `ColumnView::push_front_column`, `ColumnView::push_back_column`, or `ColumnView::insert_column`, respectively. Each of these functions takes as their argument the title used for the column.

Once we have all our columns set up, we can add child widgets either by using \a{ColumnView::set_widget} or the convenience function `push_back_row`, which adds a row of widgets to the end of the table:

```cpp
column_view.push_back_row(Label("1 | 1"), Label("1 | 2"), Label("1 | 3"));
column_view.push_back_row(Label("2 | 1"), Label("2 | 2"), Label("2 | 3"));
column_view.push_back_row(Label("3 | 1"), Label("3 | 2"), Label("3 | 3"));
```

\image html column_view_hello_world.png

\how_to_generate_this_image_begin
```cpp
auto column_view = ColumnView(SelectionMode::SINGLE);
auto col1 = column_view.push_back_column("Column 01");
auto col2 = column_view.push_back_column("Column 02");
auto col3 = column_view.push_back_column("Column 03");

column_view.push_back_row(Label("1 | 1"), Label("1 | 2"), Label("1 | 3"));
column_view.push_back_row(Label("2 | 1"), Label("2 | 2"), Label("2 | 3"));
column_view.push_back_row(Label("3 | 1"), Label("3 | 2"), Label("3 | 3"));

column_view.set_show_row_separators(true);
column_view.set_show_column_separators(true);
column_view.set_expand(true);

for (auto* column : {&col1, &col2, &col3})
column->set_is_resizable(true);

window.set_child(column_view);
```
\how_to_generate_this_image_end

Here, we use `Label`s as items in the `ColumnView`, but any arbitrarily complex widget can be used. Rows or columns do not require one specific widget type, we put any type of widget wherever we want.

---

## Grid

Not to be confused with `GridView`, \a{Grid} arranges its children in a **non-uniform** grid:

\image html grid.png

\how_to_generate_this_image_begin
```cpp
auto grid = Grid();

auto add_child = [&](size_t x, size_t y, size_t width, size_t height)
{
    auto overlay = Overlay();
    overlay.set_child(Separator());
    static size_t i = 0;
    auto label = Label((i < 9 ? "0" : "") + std::to_string(i++));
    label.set_alignment(Alignment::CENTER);
    overlay.add_overlay(label);

    auto frame = Frame();
    frame.set_child(overlay);
    frame.set_size_request({50, 50});

    auto box = Box();
    box.push_back(frame);

    grid.insert(box, {x, y}, width, height);
    return box;
};

add_child(0, 0, 1, 1);
add_child(0, 1, 2, 1);
add_child(0, 2, 1, 1);
add_child(1, 0, 2, 1);
add_child(2, 1, 1, 2);
add_child(1, 2, 1, 1);
add_child(3, 0, 1, 3);

grid.set_row_spacing(5);
grid.set_column_spacing(5);
grid.set_columns_homogenous(true);

grid.set_expand(true);
grid.set_margin(5);
window.set_child(grid);
```
\how_to_generate_this_image_end

Each widget on the grid has four properties, it's **x-index**, **y-index**, **width** and **height**. These are not in pixels, rather they are the conceptional position or number of cells in the grid.

For example, in the above figure, the widget labeled `00` has x- and y-index `0` and a width and height of `1`. The widget next to it, labeled `03` has an x-index `1`, y-index of `0`, a width of `2` and a height of `1`.

To add a widget to a grid, we need to provide the widget the position and size in the grid:

```cpp
grid.insert(
    /* child widget */
    {1, 0},  // x, y
    2,       // width
    1        // height
);
```

Where `width` and `height` are optional, with `1` being the default value for both arguments.

When a widget is added to a column or row not yet present in the grid, it is added automatically. Valid x- and y-indices are 0-based (`{0, 1, 2, ...}`), while width and height have to be a multiple of 1 (`{1, 2, ...}`).

Note that it is our responsibility to make sure a widgets position and size do not overlap with that of another widget. If carelessly inserted, one widget may obscure another, though in some cases this behavior may also be intentional.

`Grid::set_columns_homogenous` and `Grid::set_rows_homogenous` specify whether the `Grid` should allocate the exact same width for all columns or height for all rows, respectively.

Lastly, we can choose the spacing between each cell using `Grid::set_row_spacing` and `Grid::set_column_spacing`.

`Grid` can be seen as a more flexible version of `GridView`. It also arranges arbitrary widgets in columns and rows, but, unlike with `GridView`, in `Grid` a widget can occupy more than one row / column.

---

## Stack

\a{Stack} is a selectable widget that can only ever display exactly one child. We register a number of widgets with the `Stack`. All widget except the selected on will be hidden, while the selected widget will occupy the entire allocated space of the `Stack`:

```cpp
auto stack = Stack();
auto page_01 = // widget
auto page_02 = // widget

auto page_01_id = stack.add_child(page_01, "Page 01");
auto page_02_id = stack.add_child(page_02, "Page 02");

// make page_01 currently displayed widget
stack.set_visible_child(page_01_id);
```

Adding a widget with `Stack::add_child` will return the **stack ID** of that page. To make a specific widget be the currently shown widget, we use `Stack::set_visible_child`, which takes the stack ID we obtained.

We see above that `Stack::add_child` takes a second argument, which is the **page title**. This title is not used in the stack itself, rather, it is used for two widgets made to exclusively interact with the stack. These widgets are made to make selecting a specific stacks page easy on both the user and developer. If we use any of these two widgets, we will rarely have to manually select the visible child.

### StackSwitcher

\a{StackSwitcher} presents the user with a row of buttons, each of which use the corresponding stack child's title:

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

`StackSwitcher` has no other methods, it simply provides a user interface to control a `Stack`.

### StackSidebar

\a{StackSidebar} has the same purpose as `StackSwitcher`, though it displays the list of stack pages as a vertical list:

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

Other than this visual component, its purpose is identical to that of `StackSwitcher`.

### Transition Animation

When switching selecting a different page of the stack, regardless of how that selection was triggered, an animation transitioning from one page to the other plays. Similar to `Revealer`, we can influence the type and speed of animation in multiple ways:

+ `Stack::set_transition_duration` governs how long the animation will take until it is complete
+ `Stack::set_interpolate_size`, if set to `true`, makes it such that while the transition animation plays, the stack will change from the size of the previous child to the size of the current child. If set to `false`, this size-change happens instantly
+ `Stack::set_animation_type` governs the type of animation

Mousetrap provides a large number of different animation, which are represented by the enum \a{StackTransitionType}. These include cross-fading, sliding, and rotating between pages. For a full list of animation types, see the \link mousetrap::StackTransitionType corresponding documentation page\endlink.

---

## Notebook

\a{Notebook} is very similar to `Stack`, it always displays exactly one child. Unlike `Stack`, it comes with a built-in way for users to select which child to show:

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

We see that each notebook page has a tab with a title. This title widget will usually be a `Label`, though it can be any arbitrarily complex widget. When adding a page using `Notebook::push_back`, the first argument is the widget that should be used as the page, while the second argument is the widget that should be used as the label.

`Notebook` sports some additional features. Setting `Notebook::set_is_scrollable` to `true` allows users to change between pages by scrolling.
When `Notebook::set_tabs_reorderable` is set to `true`, the user can drag and drop pages to reorder them in any order they wish. Users can even **drag pages from one notebook to another**. In this way, `Notebook` is like a `Stack` with a number of complex features already implemented for us.

`Notebook` has a number of custom signals that reflect these multiple modes of interaction, these are in addition to all the signals offered by the underlying `SelectionModel`, which, just like with all selectable widgets, we obtain using `get_selection_model`.

\signals
\signal_page_added{Notebook}
\signal_page_reordered{Notebook}
\signal_page_removed{Notebook}
\signal_page_selection_changed{Notebook}

Where `_` is an unused argument. For example, we would connect to `page_selection_changed` like so:

```cpp
notebook.connect_signal_page_selection_changed([](Notebook*, void*, int32_t page_index){
    std::cout << "Selected Page is now: " << page_index << std::endl;
});
```

\todo refactor notebook signals to remove unused argument

Note that `Notebook` does not provide `get_selection_model`. Use the `page_selection_changed` signal to monitor page selection, and `Notebook::goto_page` to manually switch between pages instead.

---

---

## Compound Widgets

This concludes our tour of most of mousetraps widgets. Even though the selection is wide and powerful, we have yet to learn one of the more important things: **how do we make our own widgets?**

If we want to start from scratch and manually choose every pixel and mode of interaction, we will have to wait until the chapter on [native rendering](09_opengl.md). Not all custom widgets need to be that low-level, however. Often, we simply want a collection of already existing widgets to act as one. To illustrate this, we'll work through a simple example.

In previous sections, whenever a figure was shown, the C++ code used to generate that figure was supplied along with it. Take, for example, the figure from the section on `GridView`:

\image html grid_view.png

Here, `GridView` has 7 children. Each child is clearly a widget, as `GridView::push_back` requires a widget to be used as the argument. Looking closely, the children are actually a collection of at least 3 widgets:

+ a `Label` with the number
+ a `Separator` as the background
+ a `Frame` for the rounded outline

On top of these, we have an `Overlay` in order to layer all three graphical elements on top of each other. Lastly, an `AspectFrame` keeps the element square.

While we could create 5 individual widgets for every element of `GridView`, this would be highly tedious and not very idiomatic, as C++ is an object-oriented language. Instead, we should create a new, **compound widget**, a widget that is made up of multiple other widgets, but acts as one.

We first create a class like so:

```cpp
class CompoundWidget
{
    public:
        CompoundWidget(size_t id);
        
    private:
        Separator _separator;
        Label _label;
        Frame _frame;
        Overlay _overlay;
        AspectFrame _aspect_frame = AspectFrame(1);
};
```

All the widgets are private fields of the compound widget. This means, as long as an instance of `CompoundWidget` exists, the 5 widgets it contains will be kept in memory, but other objects cannot directly access the components.

We usually define how a compound widget is assembled in its constructor:

```cpp
// define constructor
CompoundWidget(size_t id)
: _label(std::to_string(id))
{
_overlay.set_child(_separator);
_overlay.add_overlay(_label);

_frame.set_child(_overlay);
_aspect_frame.set_child(_frame);
}
```

This constructor sets up all the widget like we discussed before.

The lowermost layer of the `Overlay` is the `Separator`, which will act as the background for the compound widget. On top of it, a `Label` added. We set the string of the label based on the ID given to the constructor.

The entire `Overlay` is first inserted into a `Frame`, then that frame is set as child of our `AspectFrame`, which has a ratio of `1`, keeping it square at all times.

We can now initialize our compound widget and add it to a window, right?

```cpp
auto instance = CompoundWidget(0);
window.set_child(compound_widget);
```
```
/home/mousetrap/test/main.cpp:201:26: error: non-const lvalue reference to type 'mousetrap::Widget' cannot bind to a value of unrelated type 'CompoundWidget'
        window.set_child(instance);
                         ^~~~~~~~
```
No, we cannot. As the error states, `CompoundWidget` cannot bind to a reference of type `mousetrap::Widget`.  For this to work, we need to declare `CompoundWidget` to be a widget.

This is accomplished by simply **inheriting from mousetrap::Widget**:

```cpp
class CompoundWidget : public Widget    // inherit
{
    public:
        // ctor
        CompoundWidget(size_t id);
        
        // function required by `Widget`
        operator NativeWidget() const override;
        
    private:
        Separator _separator;
        Label _label;
        Frame _frame;
        Overlay _overlay;
        AspectFrame _aspect_frame = AspectFrame(1);
};
```

Inheriting from `Widget` requires us to implement a single pure virtual function: `operator NativeWidget() const`, which we mark as `override`. Once this function is implemented, mousetrap is able to treat that object as a widget, allowing us to treat the compound widget as a single instance of `Widget`.

We implement this function as follows:

```cpp
CompoundWidget::operator NativeWidget() const 
{
     return _aspect_frame;
}
```

The returned value should be the **top-level** widget of our compound widget. All other parts of a compound widget are contained in the top-level widget, and the top-level widget is not contained in any other widget.

Writing out example `CompoundWidget` like this may make the widget order clearer:

```cpp
AspectFrame \
    Frame \
        Overlay \
            Label 
            Separator
``` 

Where each line ending in `\` is a widget container. A widget being indented means it is a child of the widget above.

We see that `AspectFrame` is the only widget that is not also a child of another widget. Therefore, `AspectFrame` is the top-level widget, hence the definition of `operator NativeWidget() const` above.

Putting it all together:

```cpp
/// compound_widget.hpp
struct CompoundWidget : public Widget
{
    public:
        CompoundWidget(size_t id)
        : _label(std::to_string(id))
        {
            _overlay.set_child(_separator);
            _overlay.add_overlay(_label);

            _frame.set_child(_overlay);
            _aspect_frame.set_child(_frame);
            _aspect_frame.set_margin(10);
        }

        operator NativeWidget() const override {
            return _aspect_frame;
        }

    private:
        Separator _separator;
        Label _label;
        Frame _frame;
        Overlay _overlay;
        AspectFrame _aspect_frame = AspectFrame(1);
};

/// main.cpp
auto instance = CompoundWidget(0);
window.set_child(instance);
```

\image html compound_widget.png

In this example, `CompoundWidget` is fairly simple. In practice, applications can have dozens if not hundreds of different compound widget, all made up of an even larger number of native widgets. Mousetrap has no issues having thousands of widgets in memory and on screen at the same time.

Indeed, an entire application will usually be one, giant compound widget, with a `Window` as the top-level widget.

In any case, `operator NativeWidget() const` is the glue that binds our custom objects to the pre-built way mousetrap allows widgets to be used. Once it is implemented (by simply returning the top-level widget), everything works as expected.

