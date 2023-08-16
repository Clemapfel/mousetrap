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
            std::map<std::string, std::string>* values;
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
            StyleClass get_style_class(const std::string&) const;

            /// @brief give color a name that can be used for style class properties, colors like that need to be prefixed with '@`
            /// @param name
            /// @param color
            void register_named_color(const std::string& name, RGBA color);

            /// @brief give color a name that can be used for style class properties, colors like that need to be prefixed with '@`
            /// @param name
            /// @param color
            void register_named_color(const std::string& name, HSVA color);

        private:
            static inline std::vector<std::string> _named_colors = std::vector<std::string>();
            static inline GtkCssProvider* _named_color_provider = nullptr;
            static inline std::map<std::string, detail::StyleClassInternal*> _style_classes = std::map<std::string, detail::StyleClassInternal*>();
    };

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

            /// @brief set css property with raw value
            /// @param property_name
            /// @param value
            void set_property(const std::string& property_name, const std::string& property_value);

            /// @brief get raw css property value
            /// @param property_name
            /// @return value as string
            std::string get_property(const std::string& property_name);

            /// @brief export as string
            /// @return valid css class definition
            std::string serialize() const;

        private:
            static bool validate_name(const std::string&);
            detail::StyleClassInternal* _internal = nullptr;
    };

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
