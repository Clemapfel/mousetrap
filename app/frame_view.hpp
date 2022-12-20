// 
// Copyright 2022 Clemens Cords
// Created on 12/21/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
    class FrameView : public AppComponent
    {
        public:
            FrameView();

            void update() override;
            operator Widget*() override;

        private:
            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
    };
}

// ###

namespace mousetrap
{
    FrameView::FrameView()
    {
        _main.push_back(new SeparatorLine());
    }

    void FrameView::update()
    {}

    FrameView::operator Widget*()
    {
        return &_main;
    }
}