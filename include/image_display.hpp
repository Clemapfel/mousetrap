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
            ImageDisplay(const std::string& file);
            ImageDisplay(const Image&);

            void set_scale(size_t);

            operator GtkWidget*() override;

        private:
            GtkImage* _native;
    };
}

#include <src/image_display.inl>
