//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/26/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class ResizeCanvasDialog : public AppComponent
    {
        public:
            ResizeCanvasDialog(){};
            operator Widget*(){};

        private:
            Window _window;
    };
}
