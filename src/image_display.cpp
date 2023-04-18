//
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#include <include/image_display.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        struct _ImageDisplayInternal
        {
            GObject parent;
            Vector2ui size;
        };

        DECLARE_NEW_TYPE(ImageDisplayInternal, image_display_internal, IMAGE_DISPLAY_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(ImageDisplayInternal, image_display_internal, IMAGE_DISPLAY_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_FINALIZE(ImageDisplayInternal, image_display_internal, IMAGE_DISPLAY_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(ImageDisplayInternal, image_display_internal, IMAGE_DISPLAY_INTERNAL)

        static ImageDisplayInternal* image_display_internal_new()
        {
            auto* self = (ImageDisplayInternal*) g_object_new(image_display_internal_get_type(), nullptr);
            image_display_internal_init(self);
            self->size = {0, 0};
            return self;
        }
    }

    void ImageDisplay::initialize()
    {
        if (_internal == nullptr)
        {
            _internal = detail::image_display_internal_new();
            g_object_ref(_internal);
            detail::attach_ref_to(G_OBJECT(get_native()), _internal);
        }
    }

    ImageDisplay::~ImageDisplay()
    {
        g_object_unref(_internal);
    }
    
    void ImageDisplay::update_size(size_t width, size_t height)
    {
        _internal->size.x = width;
        _internal->size.y = height;
    }

    ImageDisplay::ImageDisplay()
        : WidgetImplementation<GtkImage>(GTK_IMAGE(gtk_image_new()))
    {
        initialize();
        update_size(0, 0);
    }

    ImageDisplay::ImageDisplay(GtkImage* image)
        : WidgetImplementation<GtkImage>(image)
    {
        initialize();
        update_size(0, 0);
    }

    ImageDisplay::ImageDisplay(const std::string& file)
       : ImageDisplay()
    {
        create_from_file(file);
        initialize();
    }

    ImageDisplay::ImageDisplay(const Image& image)
        : ImageDisplay()
    {
        create_from_image(image);
        initialize();
    }

    ImageDisplay::ImageDisplay(const Icon& icon)
        : ImageDisplay()
    {
        create_from_icon(icon);
        initialize();
    }

    Vector2ui ImageDisplay::get_size() const
    {
        return _internal->size;
    }

    void ImageDisplay::create_from_image(const Image& image)
    {
        gtk_image_clear(get_native());
        gtk_image_set_from_pixbuf(get_native(), image.operator GdkPixbuf*());
    }

    void ImageDisplay::create_from_file(const std::string& path)
    {
        gtk_image_clear(get_native());

        GError* error = nullptr;
        auto* pixbuf = gdk_pixbuf_new_from_file(path.c_str(), &error);

        if (error != nullptr)
        {
            log::critical("In ImageDisplay::create_from_file: Unable to open file at \"" + path + "\": " + std::string(error->message), MOUSETRAP_DOMAIN);
            g_error_free(error);
        }

        gtk_image_set_from_pixbuf(get_native(), pixbuf);
        g_object_unref(pixbuf);
    }

    void ImageDisplay::create_from_icon(const Icon& icon)
    {
        auto size = icon.get_size() * Vector2ui(icon.get_scale());
        std::cout << size.x << " " << size.y << std::endl;
        update_size(size.x, size.y);

        gtk_image_set_from_paintable(get_native(), GDK_PAINTABLE(icon.operator GtkIconPaintable*()));
    }

    void ImageDisplay::create_as_file_preview(const FileDescriptor& file)
    {
        GError* error = nullptr;
        auto* pixbuf_maybe = gdk_pixbuf_new_from_file(file.get_path().c_str(), &error);
        g_error_free(error);

        if (pixbuf_maybe != nullptr)
        {
            gtk_image_set_from_pixbuf(get_native(), pixbuf_maybe);
            g_object_unref(pixbuf_maybe);
            update_size(0, 0);
        }
        else
        {
            auto* icon = g_content_type_get_icon(file.query_info(G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE).c_str());
            gtk_image_set_from_gicon(get_native(), G_ICON(icon));
            update_size(gdk_pixbuf_get_width(pixbuf_maybe), gdk_pixbuf_get_height(pixbuf_maybe));
            g_object_unref(icon);
        }
    }

    void ImageDisplay::clear()
    {
        gtk_image_clear(get_native());
    }

    void ImageDisplay::set_scale(int scale)
    {
        if (scale < -1)
            scale = -1;

        gtk_image_set_pixel_size(get_native(), scale);
    }
}