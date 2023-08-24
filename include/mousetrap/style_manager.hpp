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
            void set_property(StyleClassTarget target, const std::string& property, const std::string& css_value);

            /// @brief get property of target
            /// @param target
            /// @param property css property name
            /// @return css property value as string, or ""
            std::string get_property(StyleClassTarget target, const std::string& property) const;

        private:
            static bool validate_name(const std::string&);
            detail::StyleClassInternal* _internal = nullptr;
    };

    #define DEFINE_STYLE_CLASS_TARGET(NAME, value) constexpr const char* STYLE_TARGET_##NAME = value;
    DEFINE_STYLE_CLASS_TARGET(SELF, "")

    // ActionBar

    DEFINE_STYLE_CLASS_TARGET(ACTION_BAR, "actionbar")
    DEFINE_STYLE_CLASS_TARGET(ACTION_BAR_BOX_START, "actionbar>revealer>box>box.start")
    DEFINE_STYLE_CLASS_TARGET(ACTION_BAR_BOX_END, "actionbar>revealer>box>box.end")

    // AspectFrame

    DEFINE_STYLE_CLASS_TARGET(ASPECT_FRAME, "aspectframe");

    // Box

    DEFINE_STYLE_CLASS_TARGET(BOX, "box");

    // Button

    DEFINE_STYLE_CLASS_TARGET(BUTTON, "button");
    DEFINE_STYLE_CLASS_TARGET(BUTTON_PRESSED, "button:active");

    // CenterBox

    DEFINE_STYLE_CLASS_TARGET(CENTER_BOX, "box");

    // CheckButton

    DEFINE_STYLE_CLASS_TARGET(CHECK_BUTTON, "checkbutton");
    DEFINE_STYLE_CLASS_TARGET(CHECK_BUTTON_CHECK_MARK_ACTIVE, "check:checked")
    DEFINE_STYLE_CLASS_TARGET(CHECK_BUTTON_CHECK_MARK_INCONSISTENT, "check::indeterminate")
    DEFINE_STYLE_CLASS_TARGET(CHECK_BUTTON_CHECK_MARK_INACTIVE, "check:not(:checked):not(:indeterminate)")

    // ClampFrame

    DEFINE_STYLE_CLASS_TARGET(CLAMP_FRAME, "clamp");

    // ColumnView

    DEFINE_STYLE_CLASS_TARGET(COLUMN_VIEW, "columnview");

    // DropDown

    DEFINE_STYLE_CLASS_TARGET(DROP_DOWN, "dropdown");

    // Entry

    DEFINE_STYLE_CLASS_TARGET(ENTRY, "entry");
    DEFINE_STYLE_CLASS_TARGET(ENTRY_TEXT, "entry>text");

    // Expander

    DEFINE_STYLE_CLASS_TARGET(EXPANDER, "expander-widget");
    DEFINE_STYLE_CLASS_TARGET(EXPANDER_TITLE, "expander-widget>box>title");
    DEFINE_STYLE_CLASS_TARGET(EXPANDER_TITLE_ARROW, "expander-widget>box>title>expander");

    // Fixed
    // no css targets

    // FlowBox

    DEFINE_STYLE_CLASS_TARGET(FLOW_BOX, "flowbox");
    DEFINE_STYLE_CLASS_TARGET(FLOW_BOX_CHILD, "flowbox>flowboxchild");

    // Frame

    DEFINE_STYLE_CLASS_TARGET(FRAME, "frame");

    // Grid

    DEFINE_STYLE_CLASS_TARGET(GRID, "grid");

    // GridView

    DEFINE_STYLE_CLASS_TARGET(GRID_VIEW, "gridview");

    // HeaderBar

    DEFINE_STYLE_CLASS_TARGET(HEADER_BAR, "headerbar");

    // LevelBar

    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR, "levelbar")
    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR_TROUGH, "levelbar>trough")
    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR_BLOCK_LOW, "levelbar>trough>block.low")
    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR_BLOCK_HIGH, "levelbar>trough>block.high")
    DEFINE_STYLE_CLASS_TARGET(LEVEL_BAR_BLOCK_FULL, "levelbar>trough>block.full")

    // ###

    #define DEFINE_CSS_PROPERTY(NAME, value) constexpr const char* STYLE_PROPERTY_##NAME = value;
    DEFINE_CSS_PROPERTY(FOREGROUND_COLOR, "color")
    DEFINE_CSS_PROPERTY(COLOR, "color")
    DEFINE_CSS_PROPERTY(BACKGROUND_COLOR, "background-color")
    DEFINE_CSS_PROPERTY(OPACITY, "opacity")
    DEFINE_CSS_PROPERTY(FILTER, "filter")
    DEFINE_CSS_PROPERTY(FONT, "font")
    DEFINE_CSS_PROPERTY(FONT_FAMILY, "font-familiy")
    DEFINE_CSS_PROPERTY(FONT_SIZE, "font-size")
    DEFINE_CSS_PROPERTY(FONT_STYLE, "font-style")
    DEFINE_CSS_PROPERTY(FONT_WEIGHT, "font-weight")
    DEFINE_CSS_PROPERTY(FONT_TRANSFORM, "font-transform")
    DEFINE_CSS_PROPERTY(TEXT_DECORATION, "text-decoration")
    DEFINE_CSS_PROPERTY(TEXT_DECORATION_COLOR, "text-decoration-color")
    DEFINE_CSS_PROPERTY(TEXT_SHADOW, "text-shadow")
    DEFINE_CSS_PROPERTY(ICON_SIZE, "-gtk-icon-size")
    DEFINE_CSS_PROPERTY(TRANSFORM, "transform")
    DEFINE_CSS_PROPERTY(BORDER, "border")
    DEFINE_CSS_PROPERTY(BORDER_STYLE, "border-style")
    DEFINE_CSS_PROPERTY(BORDER_COLOR, "border-color")
    DEFINE_CSS_PROPERTY(BORDER_WIDTH, "border-width")
    DEFINE_CSS_PROPERTY(BORDER_RADIUS, "border-radius")
    DEFINE_CSS_PROPERTY(OUTLINE, "outline")
    DEFINE_CSS_PROPERTY(OUTLINE_STYLE, "outline-style")
    DEFINE_CSS_PROPERTY(OUTLINE_COLOR, "outline-color")
    DEFINE_CSS_PROPERTY(OUTLINE_WIDTH, "outline-width")
    DEFINE_CSS_PROPERTY(BOX_SHADOW, "box-shadow")
}
