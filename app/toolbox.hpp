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
#include <include/time.hpp>

#include <app/global_state.hpp>

namespace mousetrap
{
    class Toolbox : public Widget
    {
        public:
            Toolbox();
            operator GtkWidget*() override;

            void set_scale(size_t);

        private:

            static inline const std::vector<std::pair<std::string, std::vector<std::vector<std::string>>>> icon_mapping = {

                    {"marquee_neighborhood_select", {
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

                std::map<Button*, std::string> button_to_tool_id;

                void popup();
                void popdown();

                Box* main_box;
                Button* main_button;
                ImageDisplay* main_label;

                Popover* popover = nullptr;
                Box* popover_box;
                std::vector<ImageDisplay*> popover_menu_labels;
                std::vector<Button*> popover_menu_buttons;
                std::vector<ListView*> popover_menu_rows;

                bool popover_active = false;

                MotionEventController* main_icon_enter_controller;
                MotionEventController* popover_leave_controller;
                FocusEventController* popover_loose_focus_controller;
            };

            using on_main_icon_enter_data = std::pair<Icon*, std::vector<Icon*>*>;
            static void on_main_icon_enter(GtkEventControllerMotion* self, gdouble x, gdouble y, void* instance);

            using on_popover_leave_data = Icon*;
            static void on_popover_leave(GtkEventControllerMotion* self, void* instance);

            using on_icon_activate_data = struct { std::string tool_id; std::vector<Icon*>* icons; };
            static void on_icon_activate(GtkButton* button, void* data);

            using on_popover_loose_focus_data = Icon*;
            static void on_popover_focus_lost(GtkEventControllerFocus* self, void*);

            static std::string generate_tooltip(const std::string& tool_id);

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

        for (auto* icon : _icons)
        {
            icon->main_icon_enter_controller = new MotionEventController();
            icon->main_icon_enter_controller->connect_enter(on_main_icon_enter, new on_main_icon_enter_data{icon, &_icons});
            icon->main_label->add_controller(icon->main_icon_enter_controller);

            if (icon->popover != nullptr)
            {
                icon->popover_leave_controller = new MotionEventController();
                icon->popover_leave_controller->connect_leave(on_popover_leave, icon);
                icon->popover_box->add_controller(icon->popover_leave_controller);

                icon->popover_loose_focus_controller = new FocusEventController();
                icon->popover_loose_focus_controller->connect_leave(on_popover_focus_lost, icon);
                icon->popover_box->add_controller(icon->popover_loose_focus_controller);
            }

            icon->main_button->connect_signal("clicked", on_icon_activate, new on_icon_activate_data{
                icon->button_to_tool_id.at(icon->main_button),
                &_icons
            });

            for (auto* button : icon->popover_menu_buttons)
                button->connect_signal("clicked", on_icon_activate, new on_icon_activate_data{
                        icon->button_to_tool_id.at(button),
                        &_icons
                });
        }
    }

    void Toolbox::set_scale(size_t n)
    {
        for (auto* icon : _icons)
        {
            icon->main_label->set_size_request({32 * n, 32 * n});
            for (auto* label : icon->popover_menu_labels)
                label->set_size_request({32 * n, 32 * n});
        }
    }

    Toolbox::operator GtkWidget*()
    {
        return _main->operator GtkWidget*();
    }

    void Toolbox::Icon::popup()
    {
        if (popover != nullptr and not popover_active)
        {
            popover->set_autohide(false);
            popover->popup();
            popover_active = true;
        }
    }

    void Toolbox::Icon::popdown()
    {
        if (popover != nullptr and popover_active)
        {
            popover->popdown();
            popover_active = false;
        }
    }

    void Toolbox::on_main_icon_enter(GtkEventControllerMotion*, gdouble x, gdouble y, void* instance)
    {
        auto* self = ((on_main_icon_enter_data*) instance)->first;
        auto* others = ((on_main_icon_enter_data*) instance)->second;

        if (not self->popover_active)
        {
            for (auto* icon : *others)
                if (icon != self)
                    icon->Icon::popdown();

            self->Icon::popup();
        }
    }

    void Toolbox::on_popover_leave(GtkEventControllerMotion*, void* instance)
    {
        auto* self = (on_popover_leave_data) instance;

        if (self->popover_active)
            self->Icon::popdown();
    }

    void Toolbox::on_popover_focus_lost(GtkEventControllerFocus*, void* instance)
    {
        std::cout << "called" << std::endl;
        auto* self = (on_popover_loose_focus_data) instance;
        self->Icon::popdown();
    }

    void Toolbox::on_icon_activate(GtkButton*, void* instance)
    {
        auto data = (on_icon_activate_data*) instance;

        if (data->tool_id == "shapes_outline")
            data->tool_id = "rectangle_outline";

        if (data->tool_id == "shapes_fill")
            data->tool_id = "rectangle_fill";

        std::cout << data->tool_id << std::endl;

        for (auto* icon : *(data->icons))
            icon->popdown();
    }

    Toolbox::Icon::Icon(const std::string& icon_path, const std::string& main,
                        const std::vector<std::vector<std::string>>& children)
    {
        main_button = new Button();
        main_label = new ImageDisplay(icon_path + main + ".png");
        main_label->set_scale(4);
        main_label->set_size_request({32, 32});

        main_button->set_child(main_label);
        main_button->set_has_frame(false);
        main_button->set_tooltip_text(generate_tooltip(main));

        button_to_tool_id.insert({main_button, main});

        main_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        main_box->push_back(main_button);

        if (children.empty() or children.at(0).empty())
            return;

        popover = new Popover();
        popover_box = new Box(GTK_ORIENTATION_VERTICAL);

        for (auto& row : children)
        {
            if (row.empty())
                continue;

            popover_menu_rows.emplace_back(new ListView(GTK_ORIENTATION_HORIZONTAL));

            for (auto& child : row)
            {
                popover_menu_buttons.emplace_back(new Button());

                popover_menu_labels.emplace_back(new ImageDisplay(icon_path + child + ".png"));
                popover_menu_labels.back()->set_size_request({32, 32});

                popover_menu_buttons.back()->set_child(popover_menu_labels.back());
                popover_menu_buttons.back()->set_has_frame(false);

                popover_menu_buttons.back()->set_tooltip_text(generate_tooltip(child));

                button_to_tool_id.insert({popover_menu_buttons.back(), child});
                popover_menu_rows.back()->push_back(popover_menu_buttons.back());
            }

            popover_box->push_back(popover_menu_rows.back());
        }

        popover->attach_to(main_button);
        popover->set_child(popover_box);
        popover->set_relative_position(GTK_POS_RIGHT);

        popover->connect_signal("realize", gtk_popover_popup, popover);
    }

    Toolbox::Icon::operator GtkWidget*()
    {
        return main_box->operator GtkWidget*();
    }

    std::string Toolbox::generate_tooltip(const std::string& tool_id)
    {
        static const std::string color_start = "<span foreground=\"#FF77BB\">";
        static const std::string color_end = "</span>";

        static auto get_shortcut = [](const std::string& action) -> std::string {
            return "<tt><b>" + color_start + state::shortcut_map->get_shortcut_as_string("toolbox", action) + color_end + "</b></tt>";
        };

        static auto has_shortcut = [](const std::string& action) -> bool {
            return not state::shortcut_map->get_shortcut_as_string("toolbox", action).empty();
        };

        std::map<std::string, std::pair<std::string, std::string>> tool_id_to_title_and_description =
        {
                {"marquee_neighborhood_select", {
                    "Marquee: Neighborhood Select",
                    "Selects all pixels in connected region of identical Color\n(Replaces current selection)"
                }},

                // MARQUEE RECT

                {"marquee_rectangle", {
                    "Rectangle Select",
                    "Selects all pixels in rectangular region\n(Replaces current selection)"
                }},

                {"marquee_rectangle_add", {
                    "Rectangle Select (Add)",
                    "Selects all pixels in rectangular region\n(Adds to current selection)"
                }},

                {"marquee_rectangle_subtract", {
                    "Rectangle Select (Subtract)",
                    "Selects all pixels in rectangular region\n(Subtracts from current selection)"
                }},

                // MARQUEE CIRCLE

                {"marquee_circle", {
                    "Circle Select",
                    "Selects all pixels in elliptical region\n(Replaces current selection)"
                }},

                {"marquee_circle_add", {
                    "Circle Select (Add)",
                    "Selects all pixels in elliptical region\n(Adds to current selection)"
                }},

                {"marquee_circle_subtract", {
                    "Circle Select (Subtract)",
                    "Selects all pixels in elliptical region\n(Subtracts from current selection)"
                }},

                // MARQUEE POLY

                {"marquee_polygon", {
                    "Polygon Select",
                    "Selects all pixels in polygonal region\n(Replaces current selection)"
                }},

                {"marquee_polygon_add", {
                    "Polygon Select (Add)",
                    "Selects all pixels in polygonal region\n(Adds to current selection)"
                }},

                {"marquee_polygon_subtract", {
                    "Polygon Select (Subtract)",
                    "Selects all pixels in polygonal region\n(Subtracts from current selection)"
                }},

                // TOOLS

                {"pencil", {
                    "Pencil",
                    "Draws using the current brush and primary color"
                }},

                {"eraser", {
                   "Eraser",
                   "Draws by setting an areas opacity to 0"
                }},

                {"eyedropper", {
                   "Color Picker",
                   "Selects primary color from a pixel on the canvas"
                }},

                {"bucket_fill", {
                   "Bucket Fill",
                   "Fills a connected region with primary color"
                }},

                {"line", {
                    "Line Shape",
                    "Draws a straight line between two points"
                }},

                // SHAPES OUTLINE

                {"shapes_outline", {
                    "Shapes (Outline)",
                    "Draws the outline of geometric shapes using the primary color"
                }},

                {"rectangle_outline", {
                    "Rectangle (Outline)",
                    "Draws rectangle outline using the primary color"
                }},

                {"circle_outline", {
                    "Ellipses (Outline)",
                    "Draws ellipses outline using the primary color"
                }},

                {"polygon_outline", {
                    "Polygon (Outline)",
                    "Draws polygon outline using the primary color"
                }},

                // SHAPES FILLED

                {"shapes_fill", {
                    "Shapes (Filled)",
                    "Draws solid geometric shapes using primary color"
                }},

                {"rectangle_fill", {
                    "Rectangle (Filled)",
                    "Draws filled rectangle using the primary color"
                }},

                {"circle_fill", {
                    "Ellipses (Filled)",
                    "Draws filled ellipses using the primary Color"
                }},

                {"polygon_fill", {
                    "Polygon (Filled)",
                    "Draws filled polygon using the primary Color"
                }},

                // GRADIENT

                {"gradient_dithered", {
                    "Gradient (Dithered)",
                    "Draws a dithered gradient, from primary to secondary Color"
                }},

                {"gradient_smooth", {
                   "Gradient (Smooth)",
                   "Draws a gradient, from primary to secondary Color"
                }},
        };

        auto it = tool_id_to_title_and_description.find(tool_id);
        if (it == tool_id_to_title_and_description.end())
        {
            std::cout << "[ERROR] In Toolbox::generate_tooltip: No description found for " << tool_id << std::endl;
            return tool_id;
        }

        std::stringstream out;
        auto shortcut = get_shortcut(tool_id);
        out  << "<b>" << it->second.first << "</b>" << (not has_shortcut(tool_id) ? "" : "   [ " + shortcut + " ]") << "\n"
             << it->second.second;

        return out.str();
    }
}