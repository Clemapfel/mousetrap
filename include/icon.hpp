//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/27/23
//

#pragma once

#include <include/gtk_common.hpp>
#include <string>

#include <include/window.hpp>

namespace mousetrap
{
    /// @brief id of an icon
    using IconID = std::string;

    class Icon;

    /// @brief database of icons, opens the systems fallback icon theme automatically, see https://docs.gtk.org/gtk4/class.IconTheme.html for more info
    class IconTheme
    {
        public:
            /// @brief construct as default
            IconTheme();

            /// @brief construct from window
            /// @param window
            IconTheme(const Window&);

            /// @brief expose as GtkIconTheme \internal
            operator GtkIconTheme*() const;

            /// @brief get list of registered icons
            /// @return vector of ids
            [[nodiscard]] std::vector<IconID> get_icon_names() const;

            /// @brief check if icon is registered in the theme
            /// @param icon
            /// @return true if icon is part of theme, false otherwise
            bool has_icon(const Icon&) const;

            /// @brief check if theme has an icon with id
            /// @param id
            /// @return true if icon exists, false otherwise
            bool has_icon(const IconID&) const;

            /// @brief add a resource path to the theme, the structure of the folder has to conform to https://www.freedesktop.org/wiki/Standards/icon-theme-spec/
            /// @param path
            void add_resource_path(const std::string&);

            /// @brief overwrite the resource path to the theme, the structure of the folder has to conform to https://www.freedesktop.org/wiki/Standards/icon-theme-spec/
            /// @param path
            void set_resource_path(const std::string&);

        private:
            void verify_folder_is_freedesktop_icon_theme_compliant(const std::string& function_scope, const std::string& path);

            GtkIconTheme* _native;
    };

    #ifndef DOXYGEN
    namespace detail
    {
        struct _IconInternal;
        using IconInternal = _IconInternal;
    }
    #endif

    /// @brief object representing an icon. Note that this does not hodl any image data, instead it points to a file on disk or to an entry in an mousetrap::IconTheme
    class Icon
    {
        public:
            /// @brief construct as empty icon
            Icon();

            /// @brief destructor
            ~Icon();

            /// @brief expose as GIcon \internal
            operator GIcon*() const;

            /// @brief expose as GtkIconPaintable \internal
            operator GtkIconPaintable*() const;

            /// @brief create icon from file
            /// @param path
            /// @param square_resolution target size of the icon, icons are always square. If the file is not a .svg file and the native resolution of the image is different from the requested resolution, performss linear scaling
            /// @param scale scale of the icon, or 1 for no scaling
            void create_from_file(const std::string& path, size_t square_resolution, size_t scale = 1);

            /// @brief create icon from theme
            /// @param theme
            /// @param id id of an icon
            /// @param square_resolution target size of the icon, icons are always square. If the file is not a .svg file and the native resolution of the image is different from the requested resolution, performss linear scaling
            /// @param scale scale of the icon, or 1 for no scaling
            void create_from_theme(const IconTheme& theme, const IconID&, size_t square_resolution, size_t scale = 1);

            /// @brief get the identifier of the icon
            /// @return id
            IconID get_name() const;

            /// @brief check if two icons point to the same resource
            /// @param other
            /// @return true if both icons point to the same resource, false otherwise
            bool operator==(const Icon& other) const;

            /// @brief check if two icons point to the same resource
            /// @param other
            /// @return false if both icons point to the same resource, true otherwise
            bool operator!=(const Icon& other) const;

            /// @brief get icon scale
            /// @return scale
            size_t get_scale() const;

            /// @brief get icon resolution, this is the resolution requested during create_from, not the resources native resolution
            /// @return resolution
            Vector2ui get_size() const;

        private:
            detail::IconInternal* _internal = nullptr;
    };
}
