# Chapter 4: Widgets

In this chapter, we will learn:
+ What a widget is
+ Properties that all widgets share
+ All types of widgets supplied by mousetrap
+ How to create compound widgets

## mousetrap::Widget
#### Signals

| id          | signature               | emitted when...                                               |
|-------------|-------------------------|---------------------------------------------------------------|
| `realize`   | `(T*, (Data_t) -> void` | widget itself and all its parents are realized                |               
| `unrealize` | `(T*, (Data_t) -> void` | the display surface associated with the widget is deallocated |
| `hide`      | `(T*, (Data_t) -> void` | widget that is currently visible is                           |
| `show`      | `(T*, (Data_t) -> void` | widget that is currently hidden is shown                      |                    
| `map`       | `(T*, (Data_t) -> void` | once the widget iself and all its parents become visible      |
| `unmap`     | `(T*, (Data_t) -> void` | widget itself or any of its parents are hidden                |  
| `destroy`   | `(T*, (Data_t) -> void` | all references to a widget are released                       |

Where `T` is the fully specialized widget class, not `Widget*`. For example, for `Separator`, the signature of `realize` would be `(Separator*, (Data_t)) -> void`.

### Widget Properties

All widgets share common properties that govern how they behave when the container they are in changes size, or at what position they will appear.

#### Margin

#### Size Request

#### Expansion

#### Alignment

#### Visibility

#### Cursor

#### Tooltip

---

## Common Widget Containers

### Window

Windows are central to any application. Any `Window` instance takes the application itself as the only argument to its constructor. This is because `Window` and  `Application` are linked internally. For example, if all `Window`s are closed, the application exits.

All widgets are contained in a `Window`, that is, every widget has a sequence of parents that ends with a `Window`. `Window` is therefore what is called the most **toplevel** widget.

Windows are containers, but can only contain a single widget inside of them. We set the windows child using `Window::set_child`. This child will usually be a container that contain more than one widget, so we are not limited by windows 1-child limitation.

#### Signals

`Window` has three signals (on top of those inherited from `Widget`) that we can connect to:

| id                        | signature                                         | emitted when...                                                                             |
|---------------------------|---------------------------------------------------|---------------------------------------------------------------------------------------------|
 | `close_request`           | `(Window*, (Data_t)) -> WindowCloseRequestResult` | window manager requests the window to close, for example by the user pressing the "x" button |
| `activate_default_widget` | `(Window*, (Data_t)) -> void`                     | default widget is activate (see below)                                                      |
| `activate_focused_widget` | `(Window*, (Data_t)) -> void`                      | `activate` emitted on currently focused widget                                              |

`activate_default_widget` is emitted when the default widget is activated. This is a widget designated as such via `Window::set_default_widget`. If the user presses the enter key while the window itself holds focus, this widget will be activated. This is useful for small message dialogs that have, for example, an "ok" button.

`activate_focused_widget` is self-explanatory (it is emitted when the widget that is currently in focus emits `activate`). Signal `close_request` is not.

#### Close Request

When the window manager, which is the part of the users operating system that deals with window layout and lifetime, request a window to close, the window does not immediately close. Instead, `close_request` is emitted first. The return value of `close_request` determines what happens next, if the return value is `WindowCloseRequestResult::ALLOW_CLOSE` then the window will close. If the result is `WindowCloseRequestResult::PREVENT_CLOSE`, the window will stay open. We can use this to for example delay closing of a window until a certain filesystem operation is done. 

If no handler is connected to `close_request`, the default handler will always return `ALLOW_CLOSE`.

### Box

Boxes are the simplest multi-widget container in mousetrap and are used extensively in almost any application. A box aligns its children horizontally or vertically, depending on the argument passed to the boxes constructor. 

We can add widgets to the start, end or after a specific widget using `push_front`, `push_back`, and `insert_after`, respectively.

\todo screenshot

Between any two children is an optional space, which we can specify using `Box::set_spacing`. This spacing is unrelated to the margins of its child widgets and will be applied between any two consecutive children, but not before the very first and after the very last child.


### CenterBox

### Overlay

### Frame

### AspectFrame

### ScrolledWindow

#### Signals

| id             | signature                                                                  | emitted when...                                       |
|----------------|----------------------------------------------------------------------------|-------------------------------------------------------|
| `scroll_child` | `(ScrolledWindow*, ScrollType type, bool is_horizontal, (Data_t)) -> void` | user requests scroll action through with a keybinding |

### Paned

`Paned` is widget that always has exactly two children. Between the two children, a barrier is drawn. The user can click on this barrier and drag it horizontally or vertically, depending on the orientation of the `Paned`. This gives the user the option to resize how much space a widget is allocated by hand.

\image html paned_centered.png

Assuming the `Paned`s orientation is `Orientation::HORIZONTAL`, we can set the child on the left using `Paned::set_start_child` and the child on the right with `Paned::set_end_child`. Both childs have to be set to valid widgets in order for the user to have the option of interacting with the `Paned`. If we for some reason do not have two widgets but still like to use a `Paned`, we should set the other child to a `Separator`.

`Paned` has two per-child properties: whether a child is **resizable** and whether it is **shrinkable**.

Resizable means that if the paned changes size, the child should change size with it.

Shrinkable sets whether the side of the paned can be made smaller than the allocated size of that sides child widget. If set to true, the user can hide drag the paned barrier to complee hide the widget, regardless of its size allocation.

\image html paned_shrinkable.png `Paned::set_end_child_shrinkable(true)` made it possible to move the barrier such that the right child is partially covered"

---

### Revealer

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

## Interactive Widgets

## Button

### Signals

| id        | signature                     | emitted when...               |
|-----------|-------------------------------|-------------------------------|
| `activate` | `(Button*, (Data_t)) -> void` | widget is activated, for example by clicking the button, pressing enter while it holds focus, or by calling `activate()` |
| `clicked` | `(Button*, (Data_t)) -> void` | button is clicked by the user |

## ToggleButton

### Signals

| id        | signature                           | emitted when...               |
|-----------|-------------------------------------|-------------------------------|
| `activate` | `(ToggleButton*, (Data_t)) -> void` | widget is activated, for example by clicking the button, pressing enter while it holds focus, or by calling `activate()` |
| `clicked` | `(ToggleButton*, (Data_t)) -> void` | button is clicked by the user |
| `toggled` | `(ToggleButton*, (Data_t)) -> void` | buttons state changes |                                   
## CheckButton

### Signals

| id        | signature                          | emitted when...               |
|-----------|------------------------------------|-------------------------------|
| `activate` | `(CheckButton*, (Data_t)) -> void` | widget is activated, for example by clicking the button, pressing enter while it holds focus, or by calling `activate()` |
| `toggled` | `(CheckButton*, (Data_t)) -> void` | buttons state changes | 

## Switch

### Signals

| id        | signature                     | emitted when...               |
|-----------|-------------------------------|-------------------------------|
| `activate` | `(Switch*, (Data_t)) -> void` | widget is activated, for example by clicking the button, pressing enter while it holds focus, or by calling `activate()` |

## Adjustments

### Signals

| id        | signature                     | emitted when...               |
|-----------|-------------------------------|-------------------------------|
| `value_changed` | `(Adjustment*, (Data_t)) -> void` | `value` property changes |
| `properties_changed` | `(Adjustment*, (Data_t)) -> void` | property that isn't `value` changes |

## SpinButton

### Signals

| id        | signature                         | emitted when...                                            |
|-----------|-----------------------------------|------------------------------------------------------------|
| `value_changed` | `(SpinButton*, (Data_t)) -> void` | `value` property changes                                   |
| `wrapped` | `(SpinButton*, (Data_t)) -> void` | `set_should_wrap` is set to true and value under/overflows |

## Scale

### Signals

| id        | signature                    | emitted when...                                            |
|-----------|------------------------------|------------------------------------------------------------|
| `value_changed` | `(Scale*, (Data_t)) -> void` | `value` property changes          |              

## ScrollBar

### Signals

`ScrollBar` does not have any signals, but we can connect to the signals of the  `Adjustment` controlling the scrollbar.

## Entry

### Signals

| id         | signature                                                        | emitted when...                                                               |
|------------|------------------------------------------------------------------|-------------------------------------------------------------------------------|
| `activate` | `(Entry*, (Data_t)) -> void`                                     | widget is activated, for example by pressing enter while it holds input focus |
| `text_changed` | `(Entry*, (Data_t)) -> void`                                     | text buffer changes                 |

## TextView

### Signals

| id         | signature                       | emitted when...                                                                          |
|------------|---------------------------------|------------------------------------------------------------------------------------------|
| `text_changed` | `(TextView*, (Data_t)) -> void` | text buffer changes                                                                      |
| `undo` | `(TextView*, (Data_t)) -> void` | undo keybinding is pressed, this signal can be emitted directly to trigger this behavior |                 
| `undo` | `(TextView*, (Data_t)) -> void` | redo keybinding is pressed, this signal can be emitted directly to trigger this behavior |  

## Revealer

## PopoverMenuButton & Popovers

### Signals: Popover

| id       | signature                      | emitted when...      |
|----------|--------------------------------|----------------------|
| `closed` | `(Popover*, (Data_t)) -> void` | popover is closed    |  

### Signals: PopoverMenu

| id       | signature                          | emitted when...      |
|----------|------------------------------------|----------------------|
| `closed` | `(PopoverMenu*, (Data_t)) -> void` | popover is closed    |

### Signals: PopoverMenuButton

| id         | signature                                | emitted when...                                                                          |
|------------|------------------------------------------|------------------------------------------------------------------------------------------|
| `activate` | `(PopoverMenuButton*, (Data_t)) -> void` | button is clicked or otherwise activated |            

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

---

## Additional Widget Container

## Grid

## Fixed



