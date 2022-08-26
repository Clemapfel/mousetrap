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
    }

    ImageDisplay::ImageDisplay(const std::string& file)
    {
        _native = GTK_IMAGE(gtk_image_new_from_file(file.c_str()));
    }

    ImageDisplay::ImageDisplay(const Image& image)
    {
        _native = GTK_IMAGE(gtk_image_new_from_pixbuf(image.to_pixbuf()));
    }

    ImageDisplay::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }
}