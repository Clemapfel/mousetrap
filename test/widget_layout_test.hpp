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

            Child child = Child("CHILD");

            Label halignment_label = Label("halign: ");
            DropDown halignment_dropdown;

            Label valignment_label = Label("valign: ");
            DropDown valignment_dropdown;

            Label hexpand_label = Label("hexpand: ");
            CheckButton hexpand_button;

            Label vexpand_label = Label("vexpand: ");
            CheckButton vexpand_button;

            Box main = Box(Orientation::VERTICAL);

        public:
            PanedTest()
            {

            }

            operator NativeWidget() const override
            {
                return main.operator NativeWidget();
            }
    };
}
