// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    ImageDisplay::ImageDisplay()
    {
        _native = GTK_IMAGE(gtk_image_new());
        _size = {0, 0};
    }

    ImageDisplay::ImageDisplay(const std::string& file, size_t scale)
    {
        GError* error = nullptr;
        auto* pixbuf = gdk_pixbuf_new_from_file(file.c_str(), &error);

        if (error != nullptr)
        {
            std::cerr << "[WARNING] In ImageDisplay::ImageDisplay: Unable to open file at \"" << file << "\": " << error->message << std::endl;
        }

        if (scale != 1)
        {
            auto* scaled = gdk_pixbuf_scale_simple(
                pixbuf,
                scale * gdk_pixbuf_get_width(pixbuf),
                scale * gdk_pixbuf_get_height(pixbuf),
                GDK_INTERP_NEAREST
            );
            _native = GTK_IMAGE(gtk_image_new_from_pixbuf(scaled));
            _size = {scale * gdk_pixbuf_get_width(pixbuf), scale * gdk_pixbuf_get_height(pixbuf)};
        }
        else
        {
            _native = GTK_IMAGE(gtk_image_new_from_pixbuf(pixbuf));
            _size = {gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf)};
        }
    }

    ImageDisplay::ImageDisplay(const Image& image)
    {
        _native = GTK_IMAGE(gtk_image_new_from_pixbuf(image.to_pixbuf()));
    }

    Vector2ui ImageDisplay::get_size() const
    {
        return _size;
    }

    ImageDisplay::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }
}