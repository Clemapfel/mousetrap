//
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/image_display.hpp>
#include <mousetrap/log.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        struct _ImageDisplayInternal
        {
            GObject parent;
            GtkImage* native;
            Vector2ui size;
        };

        DECLARE_NEW_TYPE(ImageDisplayInternal, image_display_internal, IMAGE_DISPLAY_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(ImageDisplayInternal, image_display_internal, IMAGE_DISPLAY_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_FINALIZE(ImageDisplayInternal, image_display_internal, IMAGE_DISPLAY_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(ImageDisplayInternal, image_display_internal, IMAGE_DISPLAY_INTERNAL)

        static ImageDisplayInternal* image_display_internal_new(GtkImage* image)
        {
            auto* self = (ImageDisplayInternal*) g_object_new(image_display_internal_get_type(), nullptr);
            image_display_internal_init(self);
            self->native = image;
            self->size = {0, 0};
            return self;
        }
    }

    void ImageDisplay::initialize()
    {
        if (_internal == nullptr)
        {
            _internal = detail::image_display_internal_new(GTK_IMAGE(operator NativeWidget()));
            g_object_ref(_internal);
            detail::attach_ref_to(G_OBJECT(GTK_IMAGE(operator NativeWidget())), _internal);
        }
    }

    ImageDisplay::ImageDisplay(detail::ImageDisplayInternal* internal) 
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(ImageDisplay, realize),
          CTOR_SIGNAL(ImageDisplay, unrealize),
          CTOR_SIGNAL(ImageDisplay, destroy),
          CTOR_SIGNAL(ImageDisplay, hide),
          CTOR_SIGNAL(ImageDisplay, show),
          CTOR_SIGNAL(ImageDisplay, map),
          CTOR_SIGNAL(ImageDisplay, unmap)
    {
        _internal = g_object_ref(_internal);
    }
    
    ImageDisplay::~ImageDisplay()
    {
        g_object_unref(_internal);
    }

    NativeObject ImageDisplay::get_internal() const 
    {
        return G_OBJECT(_internal);
    }
    
    void ImageDisplay::update_size(uint64_t width, uint64_t height)
    {
        _internal->size.x = width;
        _internal->size.y = height;
    }

    ImageDisplay::ImageDisplay()
        : Widget(gtk_image_new()),
          CTOR_SIGNAL(ImageDisplay, realize),
          CTOR_SIGNAL(ImageDisplay, unrealize),
          CTOR_SIGNAL(ImageDisplay, destroy),
          CTOR_SIGNAL(ImageDisplay, hide),
          CTOR_SIGNAL(ImageDisplay, show),
          CTOR_SIGNAL(ImageDisplay, map),
          CTOR_SIGNAL(ImageDisplay, unmap)
    {
        initialize();
        update_size(0, 0);
    }

    ImageDisplay::ImageDisplay(GtkImage* image)
        : Widget(GTK_WIDGET(image)),
          CTOR_SIGNAL(ImageDisplay, realize),
          CTOR_SIGNAL(ImageDisplay, unrealize),
          CTOR_SIGNAL(ImageDisplay, destroy),
          CTOR_SIGNAL(ImageDisplay, hide),
          CTOR_SIGNAL(ImageDisplay, show),
          CTOR_SIGNAL(ImageDisplay, map),
          CTOR_SIGNAL(ImageDisplay, unmap)
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

    bool ImageDisplay::create_from_file(const std::string& path)
    {
        gtk_image_clear(GTK_IMAGE(operator NativeWidget()));

        GError* error = nullptr;
        auto* pixbuf = gdk_pixbuf_new_from_file(path.c_str(), &error);

        if (error != nullptr)
        {
            log::critical("In ImageDisplay::create_from_file: Unable to open file at \"" + path + "\": " + std::string(error->message), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return false;
        }

        gtk_image_set_from_pixbuf(GTK_IMAGE(operator NativeWidget()), pixbuf);

        _internal->size.x = gdk_pixbuf_get_width(pixbuf);
        _internal->size.y = gdk_pixbuf_get_height(pixbuf);

        g_object_unref(pixbuf);
        return true;
    }

    void ImageDisplay::create_from_image(const Image& image)
    {
        gtk_image_clear(GTK_IMAGE(operator NativeWidget()));
        gtk_image_set_from_pixbuf(GTK_IMAGE(operator NativeWidget()), image.operator GdkPixbuf*());
        _internal->size = image.get_size();
    }

    void ImageDisplay::create_from_icon(const Icon& icon)
    {
        auto size = icon.get_size() * Vector2ui(icon.get_scale());
        update_size(size.x, size.y);

        gtk_image_set_from_paintable(GTK_IMAGE(operator NativeWidget()), GDK_PAINTABLE(icon.operator GtkIconPaintable*()));
    }

    void ImageDisplay::create_as_file_preview(const FileDescriptor& file)
    {
        GError* error = nullptr;
        auto* pixbuf_maybe = gdk_pixbuf_new_from_file(file.get_path().c_str(), &error);
        g_error_free(error);

        if (pixbuf_maybe != nullptr)
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(operator NativeWidget()), pixbuf_maybe);
            g_object_unref(pixbuf_maybe);
            update_size(0, 0);
        }
        else
        {
            auto* icon = g_content_type_get_icon(file.query_info(G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE).c_str());
            gtk_image_set_from_gicon(GTK_IMAGE(operator NativeWidget()), G_ICON(icon));
            update_size(gdk_pixbuf_get_width(pixbuf_maybe), gdk_pixbuf_get_height(pixbuf_maybe));
            g_object_unref(icon);
        }
    }

    void ImageDisplay::clear()
    {
        gtk_image_clear(GTK_IMAGE(operator NativeWidget()));
        update_size(0, 0);
    }

    void ImageDisplay::set_scale(int scale)
    {
        if (scale < -1)
            scale = -1;

        gtk_image_set_pixel_size(GTK_IMAGE(operator NativeWidget()), scale);
    }

    int ImageDisplay::get_scale() const
    {
        return gtk_image_get_pixel_size(GTK_IMAGE(operator NativeWidget()));
    }
}