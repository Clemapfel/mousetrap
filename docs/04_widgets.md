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

### Revealer

### Expander

#### Signals

| id         | signature                      | emitted when...                                                       |
|------------|--------------------------------|-----------------------------------------------------------------------|
| `activate` | (Expander*, (Data_t)) -> void` | user pressed the enter key or otherwise activated the expander itself |



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

#### Justify Mode

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

## LevelBar

## Progressbar & Spinner 

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



