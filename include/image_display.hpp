// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/image.hpp>
#include <include/widget.hpp>

namespace mousetrap
{
    class ImageDisplay : public Widget
    {
        public:
            ImageDisplay();
            ImageDisplay(const std::string& file, size_t scale = 1);
            ImageDisplay(const Image&);

            Vector2ui get_size() const;
            operator GtkWidget*() override;

        private:
            GtkImage* _native;
            Vector2ui _size;
    };
}

#include <src/image_display.inl>
