//
// Created by clem on 3/27/23.
//

#include <include/icon.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        struct _IconInternal
        {
            GObject parent;
            
            GIcon* native = nullptr;
            GtkIconPaintable* paintable = nullptr;

            size_t resolution = 0;
            size_t scale = 1;
        };

        DECLARE_NEW_TYPE(IconInternal, icon_internal, ICON_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(IconInternal, icon_internal, ICON_INTERNAL)

        static void icon_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_ICON_INTERNAL(object);
            G_OBJECT_CLASS(icon_internal_parent_class)->finalize(object);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(IconInternal, icon_internal, ICON_INTERNAL)

        static IconInternal* icon_internal_new()
        {
            auto* self = (IconInternal*) g_object_new(icon_internal_get_type(), nullptr);
            icon_internal_init(self);
            
            self->native = nullptr;
            self->paintable = nullptr;
            self->resolution = 0;
            self->scale = 1;
            
            return self;
        }
    }
    
    Icon::Icon()
        : _internal(detail::icon_internal_new())
    {}

    Icon::~Icon()
    {
        if (G_IS_OBJECT(_internal))
            g_object_unref(_internal);
    }

    Icon::operator GIcon*() const
    {
        return _internal->native;
    }

    Icon::operator GtkIconPaintable*() const
    {
        return _internal->paintable;
    }

    size_t Icon::get_scale() const
    {
        return _internal->scale;
    }

    Vector2ui Icon::get_size() const
    {
        return Vector2ui(_internal->resolution);
    }

    void Icon::create_from_file(const std::string& path, size_t square_resolution, size_t scale)
    {
        if (_internal->scale == 0)
            _internal->scale = 1;

        auto* file = g_file_new_for_path(path.c_str());
        _internal->native = g_file_icon_new(file);
        _internal->paintable = gtk_icon_paintable_new_for_file(file, square_resolution, scale);

        g_free(file);
        _internal->resolution = square_resolution;
        _internal->scale = scale;
    }

    void Icon::create_from_theme(const IconTheme& theme, const IconID& id, size_t square_resolution, size_t scale)
    {
        _internal->resolution = square_resolution;
        _internal->scale = scale;

        GError* error = nullptr;
        _internal->native = g_icon_new_for_string(id.c_str(), &error);
        _internal->paintable = gtk_icon_theme_lookup_icon(
            theme.operator GtkIconTheme*(),
            id.c_str(),
            nullptr,
            square_resolution,
            scale,
            GtkTextDirection::GTK_TEXT_DIR_LTR,
            GtkIconLookupFlags::GTK_ICON_LOOKUP_FORCE_REGULAR
        );

        if (error != nullptr)
        {
            log::critical(std::string("In Icon::create_from_theme: ") + error->message, MOUSETRAP_DOMAIN);
            g_error_free(error);
        }
    }

    IconID Icon::get_name() const
    {
        if (_internal->native == nullptr)
            return IconID();

        return g_icon_to_string(_internal->native);
    }

    bool Icon::operator==(const Icon& other) const
    {
        if (this->_internal->native == nullptr or other._internal->native == nullptr)
            return false;

        return g_icon_equal(this->_internal->native, other._internal->native);
    }

    bool Icon::operator!=(const Icon& other) const
    {
        return not (*this == other);
    }

    IconTheme::IconTheme()
        : _native(gtk_icon_theme_new())
    {}

    IconTheme::IconTheme(const Window& window)
        : _native(gtk_icon_theme_get_for_display(gtk_widget_get_display(window.operator GtkWidget*())))
    {}

    IconTheme::operator GtkIconTheme*() const
    {
        return _native;
    }

    std::vector<IconID> IconTheme::get_icon_names() const
    {
        std::vector<std::string> out;
        char** list = gtk_icon_theme_get_icon_names(_native);
        size_t i = 0;
        while (list[i] != nullptr)
        {
            out.push_back(list[i]);
            i += 1;
        }

        g_strfreev(list);
        return out;
    }

    bool IconTheme::has_icon(const Icon& icon) const
    {
        return gtk_icon_theme_has_gicon(_native, icon.operator GIcon*());
    }

    bool IconTheme::has_icon(const IconID& id) const
    {
        return gtk_icon_theme_has_icon(_native, id.c_str());
    }

    void IconTheme::add_resource_path(const std::string& path)
    {
        verify_folder_is_freedesktop_icon_theme_compliant("set_resource_path", path);

        gtk_icon_theme_add_resource_path(_native, path.c_str());
        gtk_icon_theme_add_search_path(_native, path.c_str());
    }

    void IconTheme::set_resource_path(const std::string& path)
    {
        verify_folder_is_freedesktop_icon_theme_compliant("set_resource_path", path);

        std::array<const char*, 2> paths = {
            path.c_str(),
            nullptr
        };

        gtk_icon_theme_set_resource_path(_native, paths.data());
        gtk_icon_theme_set_search_path(_native, paths.data());
    }

    void IconTheme::verify_folder_is_freedesktop_icon_theme_compliant(const std::string& function_scope, const std::string& path)
    {
        std::string reason = "";

        // TODO

        return;

        error:
            std::cerr << "[ERROR] In mousetrap::IconTheme::" << function_scope << ": Folder at " << path << " is not a valid icon theme folder. Reason: " << reason << std::endl;
            std::cerr << "\t For more information, see https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html" << std::endl;
    }
}
