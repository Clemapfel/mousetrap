// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/image.hpp>
#include <include/widget.hpp>

namespace mousetrap
{
    class ImageDisplay : public WidgetImplementation<GtkImage>
    {
        public:
            ImageDisplay();
            ImageDisplay(const std::string& file, size_t scale = 1);
            ImageDisplay(const Image&);
            ImageDisplay(GdkPixbuf*);

            Vector2ui get_size() const;

        private:
            Vector2ui _size;
    };
}
