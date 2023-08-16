//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 8/16/23
//

#pragma once

#include <mousetrap/gtk_common.hpp>

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
        DEFINE_INTERNAL_MAPPING(StyleClass);
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

        private:
            static std::map<std::string, detail::StyleClassInternal*> _style_classes;
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

            /// @brief set css property with raw value, this is unsafe
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
            detail::StyleClassInternal* _internal = nullptr;

    };
}
