//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 8/16/23
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/color.hpp>

#include <vector>
#include <map>

namespace mousetrap
{
    #ifndef DOXYGEN
    class StyleClass;
    namespace detail
    {
        struct _StyleClassInternal
        {
            GObject parent_instance;

            GtkCssProvider* provider;
            std::string* name;
            std::map<std::string, std::map<std::string, std::string>>* target_to_properties;
        };
        using StyleClassInternal = _StyleClassInternal;

        static bool validate_css_name(const std::string&);
    }
    #endif

    /// @brief style class manager singleton, keeps the global list of style classes updated
    struct StyleManager
    {
        public:
            /// @brief update / add a style class
            /// @param style class
            static void add_style_class(const StyleClass&);

            /// @brief remove style class
            /// @param style class
            static void remove_style_class(const StyleClass&);

            /// @brief retrieve style class
            /// @param name
            /// @return style class
            static StyleClass get_style_class(const std::string&);

            /// @brief run arbitrary css code
            /// @param css
            static void add_css(const std::string&);

            /// @brief define a color, can be used by prefixing the given name with `@`
            /// @param name
            /// @param value
            static std::string define_color(const std::string& name, RGBA color);

            /// @brief define a color, can be used by prefixing the given name with `@`
            /// @param name
            /// @param value
            static std::string define_color(const std::string& name, HSVA color);

            /// @brief convert mousetrap color to css
            /// @param color RGBA
            /// @return css string
            static std::string color_to_css(RGBA);

            /// @brief convert mousetrap color to css
            /// @param color HSVA
            /// @return css string
            static std::string color_to_css(HSVA);

        private:
            static inline std::vector<std::string> _named_colors = std::vector<std::string>();
            static inline GtkCssProvider* _named_color_provider = nullptr;
            static inline std::map<std::string, detail::StyleClassInternal*> _style_classes = std::map<std::string, detail::StyleClassInternal*>();
    };

    using StyleClassTarget = const std::string&;

    /// @brief class that collects css information
    class StyleClass
    {
        public:
            /// @brief construct with name
            /// @param unique name
            StyleClass(const std::string& name);

            /// @brief construct from internal
            /// @param internal
            StyleClass(detail::StyleClassInternal* internal);

            /// @brief destructor
            ~StyleClass();

            /// @brief expose as native object
            operator NativeObject() const;

            /// @brief get unique name
            /// @return name
            std::string get_name() const;

            /// @brief export as string
            /// @return valid css class definition
            std::string serialize() const;

            /// @brief set property of target
            /// @param target
            /// @param property css property name
            /// @param value css property value
            void set_property(StyleClassTarget target, const std::string& property, const std::string& value);

            /// @brief set property of self
            /// @param property css property name
            /// @param value css property value
            void set_property(const std::string& property, const std::string& value);

            /// @brief get property of target
            /// @param target
            /// @param property css property name
            /// @return css property value as string, or ""
            std::string get_property(StyleClassTarget target, const std::string& property) const;

            /// @brief get property of target
            /// @param target
            /// @param property css property name
            /// @return css property value as string, or ""
            std::string get_property(const std::string& property) const;

        private:
            detail::StyleClassInternal* _internal = nullptr;
    };

    // CSS PROPERTIES

    #define DEFINE_CSS_PROPERTY(NAME, value) constexpr const char* STYLE_PROPERTY_##NAME = value;
    DEFINE_CSS_PROPERTY(FOREGROUND_COLOR, "color")
    DEFINE_CSS_PROPERTY(COLOR, "color")
    DEFINE_CSS_PROPERTY(BACKGROUND_COLOR, "background-color")
    DEFINE_CSS_PROPERTY(OPACITY, "opacity")
    DEFINE_CSS_PROPERTY(FILTER, "filter")
    DEFINE_CSS_PROPERTY(FONT, "font")
    DEFINE_CSS_PROPERTY(FONT_FAMILY, "font-family")
    DEFINE_CSS_PROPERTY(FONT_VARIANT, "font-variant")
    DEFINE_CSS_PROPERTY(FONT_SIZE, "font-size")
    DEFINE_CSS_PROPERTY(FONT_STYLE, "font-style")
    DEFINE_CSS_PROPERTY(FONT_WEIGHT, "font-weight")
    DEFINE_CSS_PROPERTY(FONT_TRANSFORM, "font-transform")
    DEFINE_CSS_PROPERTY(CARET_COLOR, "caret-color")
    DEFINE_CSS_PROPERTY(TEXT_DECORATION, "text-decoration")
    DEFINE_CSS_PROPERTY(TEXT_DECORATION_COLOR, "text-decoration-color")
    DEFINE_CSS_PROPERTY(TEXT_DECORATION_STYLE, "text-decoration-style")
    DEFINE_CSS_PROPERTY(TEXT_SHADOW, "text-shadow")
    DEFINE_CSS_PROPERTY(ICON_SIZE, "-gtk-icon-size")
    DEFINE_CSS_PROPERTY(TRANSFORM, "transform")
    DEFINE_CSS_PROPERTY(TRANSFORM_ORIGIN, "transform-origin")
    DEFINE_CSS_PROPERTY(BORDER, "border")
    DEFINE_CSS_PROPERTY(BORDER_STYLE, "border-style")
    DEFINE_CSS_PROPERTY(BORDER_COLOR, "border-color")
    DEFINE_CSS_PROPERTY(BORDER_WIDTH, "border-width")
    DEFINE_CSS_PROPERTY(BORDER_RADIUS, "border-radius")
    DEFINE_CSS_PROPERTY(BORDER_SPACING, "border-spacing")
    DEFINE_CSS_PROPERTY(OUTLINE, "outline")
    DEFINE_CSS_PROPERTY(OUTLINE_STYLE, "outline-style")
    DEFINE_CSS_PROPERTY(OUTLINE_COLOR, "outline-color")
    DEFINE_CSS_PROPERTY(OUTLINE_WIDTH, "outline-width")
    DEFINE_CSS_PROPERTY(BOX_SHADOW, "box-shadow")
    DEFINE_CSS_PROPERTY(BACKGROUND_CLIP, "background-clip")
    DEFINE_CSS_PROPERTY(BACKGROUND_ORIGIN, "background-origin")
    DEFINE_CSS_PROPERTY(BACKGROUND_SIZE, "background-size")
    DEFINE_CSS_PROPERTY(BACKGROUND_POSITION, "background-position")
    DEFINE_CSS_PROPERTY(BACKGROUND_REPEAT, "background-repeat")
    DEFINE_CSS_PROPERTY(TRANSITION, "transition")
    DEFINE_CSS_PROPERTY(TRANSITION_PROPERTY, "transition-property")
    DEFINE_CSS_PROPERTY(TRANSITION_DURATION, "transition-duration")
    DEFINE_CSS_PROPERTY(TRANSITION_TIMING_FUNCTION, "transition-timing-function")
    DEFINE_CSS_PROPERTY(TRANSITION_DELAY, "transition-delay")
    DEFINE_CSS_PROPERTY(ANIMATION, "animation")
    DEFINE_CSS_PROPERTY(ANIMATION_NAME, "animation-name")
    DEFINE_CSS_PROPERTY(ANIMATION_DURATION, "animation-duration")
    DEFINE_CSS_PROPERTY(ANIMATION_TIMING_FUNCTION, "animation-timing-function")
    DEFINE_CSS_PROPERTY(ANIMATION_ITERATION_COUNT, "animation-iteration-count")
    DEFINE_CSS_PROPERTY(ANIMATION_DIRECTION, "animation-direction")
    DEFINE_CSS_PROPERTY(ANIMATION_PLAY_STATE, "animation-play-state")
    DEFINE_CSS_PROPERTY(ANIMATION_DELAY, "animation-delay")
    DEFINE_CSS_PROPERTY(ANIMATION_FILL_MODE, "animation-fill-mode")

    // PRE-MADE STYLE CLASSES

    #define DEFINE_PRE_MADE_STYLE_CLASS(NAME, css_name) static inline const StyleClass STYLE_CLASS_##NAME = StyleClass(css_name);
    DEFINE_PRE_MADE_STYLE_CLASS(SUGGESTED_ACTION, "suggested-action");
    DEFINE_PRE_MADE_STYLE_CLASS(DESTRUCTIVE_ACTION, "destructive-action");
    DEFINE_PRE_MADE_STYLE_CLASS(BUTTON_FLAT, "flat");
    DEFINE_PRE_MADE_STYLE_CLASS(BUTTON_RAISED, "raised");
    DEFINE_PRE_MADE_STYLE_CLASS(BUTTON_CIRCULAR, "circular");
    DEFINE_PRE_MADE_STYLE_CLASS(BUTTON_PILL, "pill");

    DEFINE_PRE_MADE_STYLE_CLASS(BOX_LINKED, "linked");
    DEFINE_PRE_MADE_STYLE_CLASS(TOOLBAR, "toolbar");
    DEFINE_PRE_MADE_STYLE_CLASS(DIMMED, "dim-label");

    DEFINE_PRE_MADE_STYLE_CLASS(FONT_HEADING_1, "title-1");
    DEFINE_PRE_MADE_STYLE_CLASS(FONT_HEADING_2, "title-2");
    DEFINE_PRE_MADE_STYLE_CLASS(FONT_HEADING_3, "title-3");
    DEFINE_PRE_MADE_STYLE_CLASS(FONT_HEADING_4, "title-4");
    DEFINE_PRE_MADE_STYLE_CLASS(FONT_LARGER, "heading");
    DEFINE_PRE_MADE_STYLE_CLASS(FONT_REGULAR, "body");
    DEFINE_PRE_MADE_STYLE_CLASS(FONT_CAPTION_HEADING, "caption-heading");
    DEFINE_PRE_MADE_STYLE_CLASS(FONT_CAPTION, "caption");
    DEFINE_PRE_MADE_STYLE_CLASS(FONT_MONOSPACE, "monospace");
    DEFINE_PRE_MADE_STYLE_CLASS(FONT_NUMERIC, "numeric");

    DEFINE_PRE_MADE_STYLE_CLASS(COLOR_ACCENT, "accent");
    DEFINE_PRE_MADE_STYLE_CLASS(COLOR_SUCCESS, "success");
    DEFINE_PRE_MADE_STYLE_CLASS(COLOR_WARNING, "warning");
    DEFINE_PRE_MADE_STYLE_CLASS(COLOR_ERROR, "error");

    DEFINE_PRE_MADE_STYLE_CLASS(ACTIVATABLE, "activatable");
    DEFINE_PRE_MADE_STYLE_CLASS(CARD, "card");

    DEFINE_PRE_MADE_STYLE_CLASS(LIST_VIEW_SIDEBAR, "navigation-sidebar");
    DEFINE_PRE_MADE_STYLE_CLASS(LIST_VIEW_RICH_LIST, "rich-list");
    DEFINE_PRE_MADE_STYLE_CLASS(LIST_VIEW_DATA_TABLE, "data-table");
    DEFINE_PRE_MADE_STYLE_CLASS(LIST_VIEW_MENUBAR, "menu");

    DEFINE_PRE_MADE_STYLE_CLASS(ICON_DROPSHADOW, "icon-dropshadow");
    DEFINE_PRE_MADE_STYLE_CLASS(ICON_SMALL, "lowres-icon");

    DEFINE_PRE_MADE_STYLE_CLASS(CHECK_BUTTON_ROUND, "selection-mode");

    DEFINE_PRE_MADE_STYLE_CLASS(ON_SCREEN_DISPLAY, "osd");
    DEFINE_PRE_MADE_STYLE_CLASS(BACKGROUND, "background");
    DEFINE_PRE_MADE_STYLE_CLASS(VIEW, "view");

    // STYLE TARGETS

    #define DEFINE_STYLE_CLASS_TARGET(NAME, value) constexpr const char* STYLE_TARGET_##NAME = value;
    DEFINE_STYLE_CLASS_TARGET(SELF, "")

    // ActionBar

    DEFINE_STYLE_CLASS_TARGET(ACTION_BAR, "actionbar")
    DEFINE_STYLE_CLASS_TARGET(ACTION_BAR_BOX_START, "actionbar>revealer>box>box.start") // widgets added via `push_front`
    DEFINE_STYLE_CLASS_TARGET(ACTION_BAR_BOX_END, "actionbar>revealer>box>box.end")     // widgets added via `push_back`

    // AspectFrame

    DEFINE_STYLE_CLASS_TARGET(ASPECT_FRAME, "aspectframe");

    // Box

    DEFINE_STYLE_CLASS_TARGET(BOX, "box");

    // Button

    DEFINE_STYLE_CLASS_TARGET(BUTTON, "button");
    DEFINE_STYLE_CLASS_TARGET(BUTTON_PRESSED, "button:active"); // while click animation is playing

    // CenterBox

    DEFINE_STYLE_CLASS_TARGET(CENTER_BOX, "box");

    // CheckButton

    DEFINE_STYLE_CLASS_TARGET(CHECK_BUTTON, "checkbutton");
    DEFINE_STYLE_CLASS_TARGET(CHECK_BUTTON_CHECK_MARK_ACTIVE, "check:checked")  // state is `ACTIVE`
    DEFINE_STYLE_CLASS_TARGET(CHECK_BUTTON_CHECK_MARK_INCONSISTENT, "check::indeterminate") // state is `INCONSISTENT`
    DEFINE_STYLE_CLASS_TARGET(CHECK_BUTTON_CHECK_MARK_INACTIVE, "check:not(:checked):not(:indeterminate)") // state is `INACTIVE`

    // ClampFrame

    DEFINE_STYLE_CLASS_TARGET(CLAMP_FRAME, "clamp");

    // ColumnView

    DEFINE_STYLE_CLASS_TARGET(COLUMN_VIEW, "columnview");

    // DropDown

    DEFINE_STYLE_CLASS_TARGET(DROP_DOWN, "dropdown");

    // Entry

    DEFINE_STYLE_CLASS_TARGET(ENTRY, "entry");
    DEFINE_STYLE_CLASS_TARGET(ENTRY_TEXT, "entry>text");    // letters of entries text

    // Expander

    DEFINE_STYLE_CLASS_TARGET(EXPANDER, "expander-widget");
    DEFINE_STYLE_CLASS_TARGET(EXPANDER_TITLE, "expander-widget>box>title"); // label widget
    DEFINE_STYLE_CLASS_TARGET(EXPANDER_TITLE_ARROW, "expander-widget>box>title>expander");  // arrow indicator next to label

    // Fixed
    // no css targets

    // FlowBox

    DEFINE_STYLE_CLASS_TARGET(FLOW_BOX, "flowbox");
    DEFINE_STYLE_CLASS_TARGET(FLOW_BOX_CHILD, "flowbox>flowboxchild,flowboxchild");  // any child

    // Frame

    DEFINE_STYLE_CLASS_TARGET(FRAME, "frame");

    // Grid

    DEFINE_STYLE_CLASS_TARGET(GRID, "grid");

    // GridView

    DEFINE_STYLE_CLASS_TARGET(GRID_VIEW, "gridview");
    DEFINE_STYLE_CLASS_TARGET(GRID_VIEW_CHILDREN, "gridview>child");    // any child
    DEFINE_STYLE_CLASS_TARGET(GRID_VIEW_SELECTED, "gridview>child:selected");   // child that is currently selected
    DEFINE_STYLE_CLASS_TARGET(GRID_VIEW_NOT_SELECTED, "gridview>child:not(:selected)");   // child that is not currently selected

    // HeaderBar

    DEFINE_STYLE_CLASS_TARGET(HEADER_BAR, "headerbar");

    // ImageDisplay

    DEFINE_STYLE_CLASS_TARGET(IMAGE_DISPLAY, "image");

    // Label

    DEFINE_STYLE_CLASS_TARGET(LABEL, "label");

    // LevelBar

    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR, "levelbar");
    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR_TROUGH, "levelbar>trough");  // background of level bar, does not change with level bar value
    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR_BLOCK_LOW, "levelbar>trough>block.low");  // block area with value in [0%, 25%]
    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR_BLOCK_HIGH, "levelbar>trough>block.high"); // block area with value in [25%, 75%]
    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR_BLOCK_FULL, "levelbar>trough>block.full"); // block area with value in [75%, 100%]

    // ListView

    DEFINE_STYLE_CLASS_TARGET(LIST_VIEW, "listview");
    DEFINE_STYLE_CLASS_TARGET(LIST_VIEW_CHILDREN, "listview>row");  // any child
    DEFINE_STYLE_CLASS_TARGET(LIST_VIEW_SELECTED, "listview>row:selected"); // child that is selected
    DEFINE_STYLE_CLASS_TARGET(LIST_VIEW_NOT_SELECTED, "listview>row:not(:selected)"); // child that is not selected

    // MenuBar

    DEFINE_STYLE_CLASS_TARGET(MENU_BAR, "menubar");
    DEFINE_STYLE_CLASS_TARGET(MENU_BAR_ITEM, "menubar>item"); // item in outermost menu model
    DEFINE_STYLE_CLASS_TARGET(MENU_BAR_DISABLED_ITEM, "menubar>item:disabled"); // item in outermost menu mode
    DEFINE_STYLE_CLASS_TARGET(MENU_BAR_SELECTED_ITEM, "menubar>item:selected");

    // Notebook

    DEFINE_STYLE_CLASS_TARGET(NOTEBOOK, "notebook");
    DEFINE_STYLE_CLASS_TARGET(NOTEBOOK_TABS, "notebook>header>tabs>tab")
    DEFINE_STYLE_CLASS_TARGET(NOTEBOOK_SELECTED_TAB, "notebook>header>tabs>tab:checked")

    // Overlay

    DEFINE_STYLE_CLASS_TARGET(OVERLAY, "overlay");

    // Paned

    DEFINE_STYLE_CLASS_TARGET(PANED, "paned");
    DEFINE_STYLE_CLASS_TARGET(PANED_HANDLE, "paned>separator");
    DEFINE_STYLE_CLASS_TARGET(PANED_START_CHILD, "paned>*:first-child");
    DEFINE_STYLE_CLASS_TARGET(PANED_END_CHILD, "paned>*:last-child");

    // Popover

    DEFINE_STYLE_CLASS_TARGET(POPOVER, "popover>contents");
    DEFINE_STYLE_CLASS_TARGET(POPOVER_ARROW, "popover>arrow");

    // PopoverButton

    DEFINE_STYLE_CLASS_TARGET(POPOVER_BUTTON, "menubutton");
    DEFINE_STYLE_CLASS_TARGET(POPOVER_BUTTON_INDICATOR, "menubutton>button arrow");

    // PopoverMenu

    DEFINE_STYLE_CLASS_TARGET(POPOVER_MENU, "popover.menu>contents");
    DEFINE_STYLE_CLASS_TARGET(POPOVER_MENU_ARROW, "popover.menu>arrow");

    // PopupMessageOverlay

    DEFINE_STYLE_CLASS_TARGET(POPUP_MESSAGE_OVERLAY, "toastoverlay");
    DEFINE_STYLE_CLASS_TARGET(POPUP_MESSAGE_OVERLAY_POPUP_MESSAGE, "toastoverlay>toast")
    DEFINE_STYLE_CLASS_TARGET(POPUP_MESSAGE_OVERLAY_POPUP_MESSAGE_CONTENT, "toastoverlay>toast>widget")
    DEFINE_STYLE_CLASS_TARGET(POPUP_MESSAGE_OVERLAY_POPUP_MESSAGE_ACTION_BUTTON, "toastoverlay>toast>button")
    DEFINE_STYLE_CLASS_TARGET(POPUP_MESSAGE_OVERLAY_POPUP_MESSAGE_CLOSE_BUTTON, "toastoverlay>toast>button.circular.float")

    // ProgressBar

    DEFINE_STYLE_CLASS_TARGET(PROGRESS_BAR, "progressbar>trough>progress");
    DEFINE_STYLE_CLASS_TARGET(PROGRESS_BAR_EMPTY, "progressbar>trough.empty");
    DEFINE_STYLE_CLASS_TARGET(PROGRESS_BAR_FULL, "progressbar>trough.full>progress");
    DEFINE_STYLE_CLASS_TARGET(PROGRESS_BAR_TROUGH, "progressbar>trough");
    DEFINE_STYLE_CLASS_TARGET(PROGRESS_BAR_TEXT, "progressbar>text");
    DEFINE_STYLE_CLASS_TARGET(PROGRESS_BAR_DURING_PULSE, "progressbar>progressbar progress.pulse");

    // RenderArea

    // no css targets

    // Revealer

    DEFINE_STYLE_CLASS_TARGET(REVEALER, "revealer");

    // Scale

    DEFINE_STYLE_CLASS_TARGET(SCALE, "scale");
    DEFINE_STYLE_CLASS_TARGET(SCALE_TROUGH, "scale>trough");
    DEFINE_STYLE_CLASS_TARGET(SCALE_TROUGH_FILL, "scale>trough>highlight");
    DEFINE_STYLE_CLASS_TARGET(SCALE_SLIDER, "scale>trough>slider");
    DEFINE_STYLE_CLASS_TARGET(SCALE_VALUE_TEXT, "scale>value");

    // ScrollBar

    DEFINE_STYLE_CLASS_TARGET(SCROLLBAR, "scrollbar");
    DEFINE_STYLE_CLASS_TARGET(SCROLLBAR_TROUGH, "scrollbar>range>trough");
    DEFINE_STYLE_CLASS_TARGET(SCOLLBAR_SLIDER, "scrollbar>range>trough>slider");

    // Separator

    DEFINE_STYLE_CLASS_TARGET(SEPARATOR, "separator");

    // SpinButton

    DEFINE_STYLE_CLASS_TARGET(SPIN_BUTTON, "spinbutton");
    DEFINE_STYLE_CLASS_TARGET(SPIN_BUTTON_TEXT, "spinbutton>text");
    DEFINE_STYLE_CLASS_TARGET(SPIN_BUTTON_BUTTON_INCREASE, "spinbutton>button.up");
    DEFINE_STYLE_CLASS_TARGET(SPIN_BUTTON_BUTTON_DECREASE, "spinbutton>button.down");

    // Spinner

    DEFINE_STYLE_CLASS_TARGET(SPINNER, "spinner:checked"); // only applied if set_is_spinning is true

    // Stack

    DEFINE_STYLE_CLASS_TARGET(STACK, "stack");

    // StackSwitcher

    DEFINE_STYLE_CLASS_TARGET(STACK_SWITCHER, "stackswitcher");

    // StackSidebar

    DEFINE_STYLE_CLASS_TARGET(STACK_SIDEBAR, "stacksidebar");

    // Switch

    DEFINE_STYLE_CLASS_TARGET(SWITCH, "switch");
    DEFINE_STYLE_CLASS_TARGET(SWITCH_SLIDER, "switch>slider");
    DEFINE_STYLE_CLASS_TARGET(SWITCH_NOT_ACTIVE, "switch:not(:checked)");
    DEFINE_STYLE_CLASS_TARGET(SWITCH_ACTIVE, "switch:checked");

    // TextView

    DEFINE_STYLE_CLASS_TARGET(TEXT_VIEW, "textview");
    DEFINE_STYLE_CLASS_TARGET(TEXT_VIEW_TEXT, "textview>text");
    /// \bug textview subnodes don't apply css correctly

    // TransformBin

    DEFINE_STYLE_CLASS_TARGET(TRANSFORM_BIN, "transformbin");

    // ToggleButton

    DEFINE_STYLE_CLASS_TARGET(TOGGLE_BUTTON, "button.toggle");
    DEFINE_STYLE_CLASS_TARGET(TOGGLE_BUTTON_ACTIVE, "button.toggle:checked");
    DEFINE_STYLE_CLASS_TARGET(TOGGLE_BUTTON_NOT_ACTIVE, "button.toggle:not(:checked)");

    // Viewport

    DEFINE_STYLE_CLASS_TARGET(VIEWPORT, "scrolledwindow");
    DEFINE_STYLE_CLASS_TARGET(VIEWPORT_HORIZONTAL_SCROLLBAR, "scrolledwindow>scrollbar.horizontal");
    DEFINE_STYLE_CLASS_TARGET(VIEWPORT_HORIZONTAL_SCROLLBAR_TROUGH, "scrolledwindow>scrollbar.horizontal>range>trough");
    DEFINE_STYLE_CLASS_TARGET(VIEWPORT_HORIZONTAL_SCROLLBAR_SLIDER, "scrolledwindow>scrollbar.horizontal>range>trough>slider");
    DEFINE_STYLE_CLASS_TARGET(VIEWPORT_VERTICAL_SCROLLBAR, "scrolledwindow>scrollbar.vertical");
    DEFINE_STYLE_CLASS_TARGET(VIEWPORT_VERTICAL_SCROLLBAR_TROUGH, "scrolledwindow>scrollbar.vertical>range>trough");
    DEFINE_STYLE_CLASS_TARGET(VIEWPORT_VERTICAL_SCROLLBAR_SLIDER, "scrolledwindow>scrollbar.vertical>range>trough>slider");

    // Window

    DEFINE_STYLE_CLASS_TARGET(WINDOW, "window");

    // Widget

    DEFINE_STYLE_CLASS_TARGET(WIDGET, "widget");

    // General
    DEFINE_STYLE_CLASS_TARGET(TEXT_ENTRY, "text");
    DEFINE_STYLE_CLASS_TARGET(TEXT_SELECTION, "selection");
}
