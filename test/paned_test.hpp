//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/12/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class PanedTest : public Widget
    {
        private:
            Paned paned = Paned(Orientation::HORIZONTAL);

            class Child : public Widget
            {
                private:
                    Separator body;
                    Label label;
                    Overlay overlay;
                    Frame frame;
                    Box main = Box(Orientation::HORIZONTAL);

                public:
                    Child(const std::string& name)
                        : label(name)
                    {
                        overlay.set_child(body);
                        overlay.add_overlay(label);
                        frame.set_child(overlay);
                        frame.set_margin(10);
                        main.push_back(frame);
                        main.set_expand(true);
                    }

                    operator NativeWidget() const override
                    {
                        return main.operator NativeWidget();
                    }
            };

            Child left_child = Child("LEFT");
            Child right_child = Child("RIGHT");

            Box main = Box(Orientation::VERTICAL);

        public:
            PanedTest()
            {
                main.push_back(paned);
                paned.set_start_child(left_child);
                paned.set_end_child(right_child);
            }

            operator NativeWidget() const override
            {
                return main.operator NativeWidget();
            }
    };
}
