//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/12/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class WidgetLayoutTest : public Widget
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
            Box child_box = Box(Orientation::HORIZONTAL);
            Frame child_box_frame = Frame();
            ScrolledWindow child_window = ScrolledWindow();

            Label halignment_label = Label("halign: ");
            DropDown halignment_dropdown;
            Box halignment_box = Box(Orientation::HORIZONTAL);

            Label valignment_label = Label("valign: ");
            DropDown valignment_dropdown;
            Box valignment_box = Box(Orientation::HORIZONTAL);

            Label hexpand_label = Label("hexpand: ");
            CheckButton hexpand_button;
            Box hexpand_box = Box(Orientation::HORIZONTAL);

            Label vexpand_label = Label("vexpand: ");
            CheckButton vexpand_button;
            Box vexpand_box = Box(Orientation::HORIZONTAL);

            Label margin_label = Label("margin: ");
            SpinButton margin_scale = SpinButton(0, 100, 1);
            Box margin_box = Box(Orientation::HORIZONTAL);

            ListView config_list = ListView(Orientation::VERTICAL);
            Box main = Box(Orientation::VERTICAL);

        public:
            WidgetLayoutTest()
            {
                child_box_frame.set_child(child);
                child_box.push_back(child_box_frame);
                child_window.set_child(child_box);

                halignment_box.push_back(halignment_label);
                halignment_box.push_back(halignment_dropdown);

                halignment_dropdown.push_back(Label("Alignment::START"), Label("Alignment::START"), [](DropDown*, WidgetLayoutTest* instance){
                    instance->child_box_frame.set_horizontal_alignment(Alignment::START);
                }, this);

                halignment_dropdown.push_back(Label("Alignment::CENTER"), Label("Alignment::CENTER"), [](DropDown*, WidgetLayoutTest* instance){
                    instance->child_box_frame.set_horizontal_alignment(Alignment::CENTER);
                }, this);

                halignment_dropdown.push_back(Label("Alignment::END"), Label("Alignment::END"), [](DropDown*, WidgetLayoutTest* instance){
                    instance->child_box_frame.set_horizontal_alignment(Alignment::END);
                }, this);


                valignment_box.push_back(valignment_label);
                valignment_box.push_back(valignment_dropdown);
                valignment_dropdown.push_back(Label("Alignment::START"), Label("Alignment::START"), [](DropDown*, WidgetLayoutTest* instance){
                    instance->child_box_frame.set_vertical_alignment(Alignment::START);
                }, this);

                valignment_dropdown.push_back(Label("Alignment::CENTER"), Label("Alignment::CENTER"), [](DropDown*, WidgetLayoutTest* instance){
                    instance->child_box_frame.set_vertical_alignment(Alignment::CENTER);
                }, this);

                valignment_dropdown.push_back(Label("Alignment::END"), Label("Alignment::END"), [](DropDown*, WidgetLayoutTest* instance){
                    instance->child_box_frame.set_vertical_alignment(Alignment::END);
                }, this);

                hexpand_box.push_back(hexpand_label);
                hexpand_box.push_back(hexpand_button);
                hexpand_button.connect_signal_toggled([](CheckButton* button, WidgetLayoutTest* instance){
                    instance->child_box_frame.set_expand_horizontally(button->get_active());
                }, this);

                vexpand_box.push_back(vexpand_label);
                vexpand_box.push_back(vexpand_button);
                vexpand_button.connect_signal_toggled([](CheckButton* button, WidgetLayoutTest* instance){
                    instance->child_box_frame.set_expand_vertically(button->get_active());
                }, this);

                margin_box.push_back(margin_label);
                margin_box.push_back(margin_scale);
                margin_scale.connect_signal_value_changed([](SpinButton* button, WidgetLayoutTest* instance){
                    instance->child_box_frame.set_margin(button->get_value());
                }, this);

                for (auto* widget : std::vector<Widget*>{
                    &halignment_box,
                    &valignment_box,
                    &hexpand_box,
                    &vexpand_box,
                    &margin_box
                })
                    config_list.push_back(*widget);

                config_list.set_expand_vertically(false);

                main.push_back(child_window);
                main.push_back(config_list);
            }

            operator NativeWidget() const override
            {
                return main.operator NativeWidget();
            }
    };
}
