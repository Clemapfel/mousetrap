// 
// Copyright 2022 Clemens Cords
// Created on 9/3/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/flow_box.hpp>
#include <include/box.hpp>
#include <include/image_display.hpp>
#include <include/toggle_button.hpp>
#include <include/popover.hpp>

#include <app/global_state.hpp>

namespace mousetrap
{
    class Toolbox : public Widget
    {
        public:
            Toolbox();
            operator GtkWidget*() override;

        private:

            static inline const std::map<std::string, std::vector<std::vector<std::string>>> icon_mapping = {

                    {{"marquee_neighborhood_select"}, {
                        {"marquee_rectangle", "marquee_rectangle_add", "marquee_rectangle_subtract"},
                        {"marquee_circle", "marquee_circle_add", "marquee_circle_subtract"},
                        {"marquee_polygon", "marquee_polygon_add", "marquee_polygon_subtract"}
                    }},

                    {"pencil", {{}}},
                    {"eraser", {{}}},
                    {"eyedropper", {{}}},
                    {"bucket_fill", {{}}},
                    {"line", {{}}},

                    {"shapes_outline", {
                        {"rectangle_outline", "rectangle_fill"},
                        {"circle_outline", "circle_fill"},
                        {"polygon_outline", "polygon_fill"}
                    }},

                    {"gradient_dithered", {{"gradient_smooth"}}},
            };

            struct Icon : public Widget
            {
                Icon(const std::string& icon_path, const std::string& main, const std::vector<std::vector<std::string>>& children);
                operator GtkWidget*() override;

                MenuButton* menu_button;

                Button* main_button;
                ImageDisplay* main_label;

                Popover* popover;
                MotionEventController* motion_event_controller;

                Box* popover_menu_box;
                std::vector<ImageDisplay*> popover_menu_labels;
                std::vector<ListView*> popover_menu_rows;
                std::vector<Button*> popover_menu_buttons;

                bool popover_active = false;

                static void on_motion_enter(GtkEventControllerMotion* self, gdouble x, gdouble y, void* instance);
                static void on_motion_leave(GtkEventControllerMotion* self, void* instance);
                static void on_icon_activate(GtkButton* button, void* which_tool);
            };

            Box* _main;
            std::vector<Icon*> _icons;
    };
}

// ###

namespace mousetrap
{
    Toolbox::Toolbox()
    {
        _main = new Box(GTK_ORIENTATION_VERTICAL);
        for (auto& pair : icon_mapping)
        {
            _icons.emplace_back(new Icon(
                get_resource_path() + "icons/",
                pair.first,
                pair.second
            ));

            auto current = *_icons.back();
            _main->push_back(_icons.back());
        }
    }

    Toolbox::operator GtkWidget*()
    {
        return _main->operator GtkWidget*();
    }

    void Toolbox::Icon::on_motion_enter(GtkEventControllerMotion*, gdouble x, gdouble y, void* instance)
    {
        auto* self = (Toolbox::Icon*) instance;
        if (not self->popover_active)
        {
            self->popover_active = true;
            self->menu_button->popup();
        }
    }

    void Toolbox::Icon::on_motion_leave(GtkEventControllerMotion*, void* instance)
    {
        auto* self = (Toolbox::Icon*) instance;
        if (self->popover_active)
        {
            self->popover_active = false;
            self->menu_button->popdown();
        }
    }

    void Toolbox::Icon::on_icon_activate(GtkButton* button, void* which_tool)
    {
        auto s = ((std::string*) which_tool);
        std::cout << "In Toolbox::Icon::on_icon_toggled: [TODO] selected " << which_tool << std::endl;
    }

    Toolbox::Icon::Icon(const std::string& icon_path, const std::string& main,
                        const std::vector<std::vector<std::string>>& children)
    {
        menu_button = new MenuButton();
        main_button = new Button();
        popover = new Popover();

        main_button->set_icon(icon_path + main + ".png");
        main_button->connect_signal("activate", on_icon_activate, new std::string(main));
        main_button->set_visible(false);

        popover_menu_box = new Box(GTK_ORIENTATION_VERTICAL);

        for (auto& row : children)
        {
            if (row.empty())
                continue;

            popover_menu_rows.emplace_back(new ListView(GTK_ORIENTATION_HORIZONTAL));

            for (auto& child : row)
            {
                popover_menu_buttons.emplace_back(new Button());
                popover_menu_buttons.back()->set_icon(icon_path + child + ".png");
                popover_menu_buttons.back()->connect_signal("activate", on_icon_activate, new std::string(child));
                popover_menu_rows.back()->push_back(popover_menu_buttons.back());
            }

            popover_menu_box->push_back(popover_menu_rows.back());
        }

        popover->set_child(popover_menu_box);
        popover->set_relative_position(GTK_POS_RIGHT);

        if (not popover_menu_rows.empty())
        {
            menu_button->set_child(main_button);
            menu_button->set_popover(popover);
        }
    }

    Toolbox::Icon::operator GtkWidget*()
    {
        if (popover_menu_rows.empty())
            return main_button->operator GtkWidget*();
        else
            return menu_button->operator GtkWidget*();
    }
}