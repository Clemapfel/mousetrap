# Chapter 4: Widgets

In this chapter, we will learn:
+ What a widget is
+ Properties that all widgets share
+ All types of widgets supplied by mousetrap
+ How to create compound widgets

## What is a widget?

When creating a GUI, widgets are the central element to any and all applications. In general, a widget is anything that can be rendered on screen. Often, widgets have a way of interacting with them, for example, in the cas of `Button`, the widget is rendered to screen as a filled rectangle and the user can click on it to emit a signal.

All widgets are signal emitters, but not all signal emitters are widgets. As such, all widgets share a number of signals:

\signals
\widget_signals{Widget}


## Widget Properties

All widgets share common properties that govern how they behave when the container they are in changes size, or at what position they will appear.

### Size Request

We may have noticed that in previous chapter, if we resized the window manually, most of the time the widgets inside dynamically resized themself to fit into the new windows size.

There are two properties that control this behavior, **size request** and **expansion**.

By calling `Widget::set_expand_horizontally` and `Widget::set_expand_vertically`, we can choose whether a widget should dynamically resize itself along the x- and y-axis. Mousetrap offers `Widget::set_expand`, which sets both the horizontal and vertical expansion at the same time.

Regardless of whether a widget does or does not expand, any widget will always at least allocate size requested using `Widget::set_size_request`. This functions takes a vector of two ints, which govern the minimum size in pixels. For example, 

### Margin

Any widget has four margins: `start`, `end`, `top` and `bottom`. Usually, these correspond to empty space left, right, above and below the widget respectively. If a widget has a natural 


#### Expansion

#### Alignment

#### Visibility

#### Cursor

#### Tooltip

---

## Window

Windows are central to any application. Any `Window` instance takes the application itself as the only argument to its constructor. This is because `Window` and  `Application` are linked internally. For example, if all `Window`s are closed, the application exits.

All widgets are contained in a `Window`, that is, every widget has a sequence of parents that ends with a `Window`. `Window` is therefore what is called the most **toplevel** widget.

Windows are containers, but can only contain a single widget inside of them. We set the windows child using `Window::set_child`. This child will usually be a container that contain more than one widget, so we are not limited by windows 1-child limitation.

### Signals

`Window` has three signals (on top of those inherited from `Widget`) that we can connect to:

| id                        | signature                                         | emitted when...                                                                             |
|---------------------------|---------------------------------------------------|---------------------------------------------------------------------------------------------|
 | `close_request`           | `(Window*, (Data_t)) -> WindowCloseRequestResult` | window manager requests the window to close, for example by the user pressing the "x" button |
| `activate_default_widget` | `(Window*, (Data_t)) -> void`                     | default widget is activate (see below)                                                      |
| `activate_focused_widget` | `(Window*, (Data_t)) -> void`                      | `activate` emitted on currently focused widget                                              |

`activate_default_widget` is emitted when the default widget is activated. This is a widget designated as such via `Window::set_default_widget`. If the user presses the enter key while the window itself holds focus, this widget will be activated. This is useful for small message dialogs that have, for example, an "ok" button.

`activate_focused_widget` is self-explanatory (it is emitted when the widget that is currently in focus emits `activate`). Signal `close_request` is not.

### Close Request

When the window manager, which is the part of the users operating system that deals with window layout and lifetime, request a window to close, the window does not immediately close. Instead, `close_request` is emitted first. The return value of `close_request` determines what happens next, if the return value is `WindowCloseRequestResult::ALLOW_CLOSE` then the window will close. If the result is `WindowCloseRequestResult::PREVENT_CLOSE`, the window will stay open. We can use this to for example delay closing of a window until a certain filesystem operation is done. 

If no handler is connected to `close_request`, the default handler will always return `ALLOW_CLOSE`.

## Box

Boxes are the simplest multi-widget container in mousetrap and are used extensively in almost any application. A box aligns its children horizontally or vertically, depending on the argument passed to the boxes constructor. 

We can add widgets to the start, end or after a specific widget using `push_front`, `push_back`, and `insert_after`, respectively.

\todo screenshot

Between any two children is an optional space, which we can specify using `Box::set_spacing`. This spacing is unrelated to the margins of its child widgets and will be applied between any two consecutive children, but not before the very first and after the very last child.

## CenterBox

`CenterBox` is a widget that has exactly three children, at the start, end and center of the allocated area. `CenterBox` prioritizes keeping the center widget centered at all costs, making it an ideal choice for a layout where this is desired.

We set the start widget with `CenterBox::set_start_child`, the center widget with `CenterBox::set_center_child` and the end widget with `CenterBox::set_end_child`.

`CenterBox` is orientable, meaning it also supplies `set_orientation` to choose the preferred layout.

## Overlay

So far, all widget containers align widget such that the do not overlap, which is why the margin property may not be negative. There is one container that allows overlapping: `Overlay`.

`Overlay` has one "base" widget, which is at the conceptual bottom of the overlay. We set this widget using `Overlay::set_child`. Then, we can add any number of widgets on top of the base widget using `Overlay::add_overlay`:

```cpp
auto child_widget = // ...
auto overlay_widget = // ...

auto overlay = Overlay();
overlay.set_child(child_widget);
overlay.add_overlay(overlay_widget);
```

\todo image

By default, `Overlay` will allocate exactly as much space as the base widget does. If one of the overlaid widgets takes up more space than the base widget, it will be truncated. We can change this by supplying a second argument to `add_overlay`, which is a boolean indicated whether the overlay widget should be included in the entire containers allocation, that is, if the overlay widget is larger than the base widget, should the overlay resize itself such that the entire overlay widget is shown:

```cpp
overlay.add_overlay(overlay_widget, true);
``` 

\todo image

When constructing complex compound widgets, we often want to widgets to overlap each other. This can cause problems, if the user clicks on an area that is occupied by two or more widgets, which widget should be activated? By default, only the top-most widget will be activated. If we want a different layer to be activated instead, we have to deactivate interaction with all widgets above it, either by calling `Widget::hide` or `Widget::set_can_respond_to_input(false)`. 

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

## Revealer

`Revealer` is a "flair" widget, meaning it does not have any direct functionality. Instead, it is used to animate widgets appearing and dissapearing.

It has exactly one child, set via `Revealer::set_child`. By default, the revealer will hide the child and not allocate any space. When we call `Revealer::set_revelead(true)`, the animation will start playing and the child widget will be displayed to the user.

`Revealer` has a custom signal, `revealed` which is emitted once the transition animation finished playing:

\signals
\signal_relevaed{Revealer}

We can change the type of animation using `Revealer::set_transition_type`, which takes \link mousetrap::RevealerTransitionType of many enum values\endlink. By default, it will be set to `RevealerTransitionType::CROSSFADE`, which fades the widget into view.

We can change the speed of the animation using `Revealer::set_transition_duration`, which takes a time duration as `mousetrap::Time`:

```cpp
// set duration as 1.5s
revealer.set_transition_duration(seconds(1.5));
```

`Revealer`s are rarely necessary, but can increase GUI "snappyness" and elevate the user experience of our app.

---

## Expander

`Expander` is similar to revealer, in that it shows and hides a widget. While `Revealer` is controlled by the developer, `Expander` is controlled by the user. It creates a button-like widget with a label. When it is clicked, the `Expander` reveals its child. It acts identical to the `<details>` html entity:

<details><summary>like this</summary>
 (revealed text goes here)
</details>

We choose the label widget with `Expander::set_label_widget` and the child with `Expander::set_child`. Both are optional, if no label widget is set the label will be a simple arrow.

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

## Label

In contention for being *the* most used widget, `Label`s are important to understand. A `Label` display text and is initialized as one would expect:

```cpp
auto label = Label("text!")
```
We can change a labels text after initialization by calling `set_text`. While this is nice `Label` provides a number of other high-lever features, making it suitable even for long blocks of text or entire book pages.

### Justify Mode

Justification is how multiple words in a line are arranged. There are 5 modes in totla, all part of the enum `JustifyMode`:

\image html text_justify_left.png "JustifyMode::LEFT"<br>
\image html text_justify_center.png "JustifyMode::CENTER" <br>
\image html text_justify_right.png "JustifyMode::RIGHT"<br>
\image html text_justify_fill.png "JustifyMode::FILL" <br>

Where the fifth mode is `JustifyMode::NONE` which arranges all text in exactly on line.

### Wrapping

Wrapping is at which part of the text a newline is created. For wrapping to happen at all, the `JustifyMode` has to not be set to `JustifyMode::NONE`, otherwise it forces the text in a single line so no wrapping is possible.

For the other modes, the enum `LabelWrapMode` has the following values:
| `JustifyMode` | meaning |
|------|--------|
| `NONE`  | no wrapping |
| `ONLY_ON_WORD` | line will only be split between two words 
| `ONLY_ON_CHAR` | line will only be split inbetween syllables
| `WORD_OR_CHAR` | line will be split between words or syllables

Where `WORD_OR_CHAR` is the default.

### Ellipsize Mode

If a line is too long for the available space and wrapping is disabled, **ellipzing** will take place. The corresponding enum has 4 value:

| `EllipsizeMode` | meaning | example |
|------|--------|---------|
| `NONE`  | text will not be ellipsize | "Humane mousetrap engineer" |
| `START` | text starts with `...`, showing only the last few words | "...engineer" |
| `END` | text end with `...`, showing only the first few words | "Humane mousetrap..."|
| `MIDDLE` | "..." in the center, shows start and beginnig | "Humane ... engineer" |

### Markup

Labels support **markup**, which is a way to modify how texts are displayed. Mousetraps labels are based on pango attributes, which is amazing at rendering text because it handles unicode flawlessly and supports many html-like styles:

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

## Button

We've seen `Button` in the previous chapter, it is one of the simplest ways for a user to interact with an application.

`Button` has the following signals:

\signals
\signal_activate{Button}
\signal_clicked{Button}

Where physically clicking the button both emits `activate` and `clicked`, while calling `Widget::activate` only emits `activate`, not clicked. 

To change the label of a button, we use `Button::set_child`. This will usually be a label or image, though any arbitrary widget can be used as a child this way.

Other than the child widget, we can customize a buttons look. `Button::set_has_frame` will change wether the button has a texture outline to it, while `Button::set_is_circular` changes the button to a fully rounded appearance:

\todo image for all 3 buttons

---

## ToggleButton

`ToggleButton` is a specialized form of `Button`. It supports most of methods/signals, including signal `clicked` and `set_child`, `set_has_frame` and `set_is_circular`.
Unique to `ToggleButton` is that, if clicked, it stays pressed in, emitting the `toggled` signal (see below). In this way, `ToggleButton` can be used to track a boolean state. To check whether the button is currently toggled, we use `ToggleButton::get_active`, which returns a boolean.

\signals
\signal_toggled{ToggleButton}
\signal_activate{ToggleButton}
\signal_clicked{ToggleButton}

\todo figure for untoggled, toggled 

---
                             
## CheckButton

`CheckButton` is almost identical to `ToggleButton` in function, but not appearance. `CheckButton` is an empty box in which a checkmark appears when it is toggled. Just like before, we query whether it is pressed by calling `CheckButton::get_active`.

\signals
\signal_activate{CheckButton}
\signal_toggled{CheckButton}

\todo figure check, unchecked, inconsistent

---

## Switch

The last widget that is meant to convey a boolean state to the user, we have `Switch`, which has the appearance of a flick-switch. It can be clicked or dragged to the other state. `Switch` does not emit `toggled`, instead we listen to the `activate` signal and query it's state using `Switch::get_active`.

\signals
\signals_activate{CheckButton}

\todo figure switch on/off

---

## Adjustment

The next few widgets we will be discussing are used to let the user choose a value from a **range**, which, in mousetrap, is represented by a signal emitter called `Adjustment`. In general, a range has a **lower and upper value**. For example, the range `[0, 2]` has the `lower` of `0` and `upper` of `2`. A second property is the **step increment**, which is the minimum difference between two adjacent values in the range. For example, if our range is still `[0, 2]` and the step increment is `0.5`, then that includes the numbers `{0, 0.5, 1, 1.5, 2}`. If the step increment is `0.01`, `[0,2]` include the numbers  `{0, 0.01, 0.02, ..., 1.98, 2.00}`. 

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

We usually do not need to create our own adjustment, rather it is provided by a widget. Notable about this is that the widget and its adjustment are automatically kept in synch. If we change any property of the adjustment, the widget will change its appearance accordingly, which we will see an example of shortly.

Adjustment has two signals:

\signals
\signal_value_changed{Adjustment}
\signal_properties_changed{Adjustment}

We can connect to `value_changed` to monitor the value of an `Adjustment`, and thus whatever widget is controlled by it.

`properties_changed` is emitted when one of upper, lower or step increment is changed, usually through `Adjustment::set_upper`, `Adjustment::set_lower` and/or `Adjustment::set_increment`.

---

## SpinButton

\todo figure spinbutton in both orientations

`SpinButton` is used to pick an exact value from a range. The user can click the rectangular area and manually enter a value, or they can increase or decrease the current value by the step increment of the `SpinButton`s adjustment. We supply the properties of the range in `SpinButton`s constructor:

```cpp
// create SpinButton with range [0, 2] and increment 0.01
auto spin_button = SpinButton(0, 2, 0.01)
```

`SpinButton` has two signals:

\signals
\signal_value_changed{SpinButton}
\signal_wrapped{SpinButton}

Only the latter of which needs explanation, as we recognize `value_changed` from `Adjustment`. When the user reaches one end of the `SpinButton`s range, which, for a range of `[0, 2]` would be either the value `0` or `2`, by default the value will not change. By setting `SpinButton::set_should_wrap(true)`, instead of stopping, the value will wrap to the other end of the range. So if the user increments at value `2` it will jump to `0`, and if the user decrements at value `0`, it will jump to 2.

We can check what the properties of a `SpinButton`s range are by either calling `SpinButton::get_adjustment` and querying the returned adjustment, or we can use the functions like `SpinButton::get_value`, `SpinButton::get_lower`, etc. directly.

---

## Scale

\todo figure scale in both orientations

`Scale`, just like `SpinButton` is a widget that chooses a value from an `Adjustment`. The user chooses the value by click-dragging the knob on the scale. In this way, it is usually harder to pick an exact decimal value on a scale. We can aid in this by displaying the exact value next to the scale, using `Scale::set_should_draw_value`:

\todo figure scale with value drawn

---           

## ScrollBar

Similar to `Scale`, `ScrollBar` is used to pick a value on a floating-point scale. It is often used as a way to choose which part of a widget should be shown on screen. For an already-automated way of doing this, see `ScrolledWindow`.


---

## LevelBar

`LevelBar` is used to display a fraction, often use to indicate the level of something such as the volume of a playback device.

To create a level bar, we need to specify the minimum and maximum value of the range we wish to display. We can then set the current value using `LevelBar::set_value`:

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

The bar can also be used to display a discrete value (a range only consisting of integers), in which case the bar will be shown segmented. We can set the level bars mode using `set_mode`, which takes either `LevelBarMode::CONTINUOUS` or `LevelBarMode::DISCRETE` as its argument.

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



