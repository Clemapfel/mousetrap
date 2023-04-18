//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/13/23
//

#pragma once

#include <mousetrap.hpp>

using namespace mousetrap;

class SignalsChapter : public Widget
{
    private:
        Button button;

    public:
        operator NativeWidget() const override {
            return button;
        }

        SignalsChapter()
        {
            button.connect_signal_clicked([](Button* button, std::string data){
                std::cout << data << std::endl;
            }, "clicked");
        }
};
