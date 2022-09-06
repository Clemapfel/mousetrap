// 
// Copyright 2022 Clemens Cords
// Created on 9/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/flow_box.hpp>
#include <include/box.hpp>
#include <include/image_display.hpp>
#include <include/toggle_button.hpp>
#include <include/popover.hpp>
#include <include/time.hpp>
#include <include/overlay.hpp>
#include <include/get_resource_path.hpp>
#include <include/list_view.hpp>

#include <app/tools.hpp>

namespace mousetrap
{
    class Toolbox : public Widget
    {
        public:
            Toolbox();
            operator GtkWidget*() override;

            void select(ToolID);

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
                Icon(ToolID id);
                operator GtkWidget*() override;

                ToolID id;

                Overlay* overlay;

                ImageDisplay* label;
                ImageDisplay* popover_indicator;
                ImageDisplay* selected_indicator;
                ImageDisplay* child_selected_indicator;

                ClickEventController* click_event_controller;
                MotionEventController* motion_event_controller;
            };

            using on_icon_click_data = struct { Icon* self; Icon* parent; Toolbox* instance; ToolID tool_id; };
            static void on_icon_click(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, void* user_data);

            using on_icon_without_popover_enter_data = Toolbox*;
            static void on_icon_without_popover_enter(GtkEventControllerMotion* self, gdouble x, gdouble y, void* instance);

            struct IconWithPopover : public Widget
            {
                IconWithPopover(Icon* main, std::vector<std::vector<Icon*>> children);
                operator GtkWidget*();

                Box* main_box;
                Popover* popover;
                std::vector<ListView*> popover_rows;
                Box* popover_box;

                MotionEventController* main_motion_event_controller;
            };

            using on_icon_with_popover_enter_data = struct { IconWithPopover* self; Toolbox* instance; };
            static void on_icon_with_popover_enter(GtkEventControllerMotion* self, gdouble x, gdouble y, void* instance);

            std::map<ToolID, Icon*> _icons;

            std::vector<IconWithPopover*> _icons_with_popover;
            ListView* main;
            MotionEventController* motion_event_controller;

            using on_main_leave_data = Toolbox*;
            static void on_main_leave(GtkEventControllerMotion* self, void* data);

            using on_main_motion_data = Toolbox*;
            static void on_main_motion(GtkEventControllerMotion* self, gdouble x, gdouble y, void* data);
            Vector2f* last_known_position = new Vector2f{0, 0};

            std::string generate_tooltip(ToolID);
    };
}

// ###

namespace mousetrap
{
    Toolbox::Icon::Icon(ToolID id)
        : id(id)
    {
        label = new ImageDisplay(get_resource_path() + "icons/" + id + ".png");
        label->set_size_request({32, 32});
        label->set_expand(false);
        label->set_align(GTK_ALIGN_CENTER);

        popover_indicator = new ImageDisplay(get_resource_path() + "icons/" + "has_popover_indicator" + ".png");
        popover_indicator->set_size_request({48, 48});
        popover_indicator->set_opacity(0);

        selected_indicator = new ImageDisplay(get_resource_path() + "icons/" + "selected_indicator" + ".png");
        selected_indicator->set_size_request({48, 48});
        selected_indicator->set_opacity(0);

        child_selected_indicator = new ImageDisplay(get_resource_path() + "icons/" + "child_selected_indicator" + ".png");
        child_selected_indicator->set_size_request({48, 48});
        child_selected_indicator->set_opacity(0);

        overlay = new Overlay();
        overlay->set_child(selected_indicator);
        overlay->add_overlay(child_selected_indicator);
        overlay->add_overlay(label);
        overlay->add_overlay(popover_indicator);

        click_event_controller = new ClickEventController();
        overlay->add_controller(click_event_controller);
        
        motion_event_controller = new MotionEventController();
        overlay->add_controller(motion_event_controller);
    }

    Toolbox::Icon::operator GtkWidget*()
    {
        return overlay->operator GtkWidget*();
    }

    Toolbox::IconWithPopover::IconWithPopover(Icon* main, std::vector<std::vector<Icon*>> children)
    {
        main->popover_indicator->set_opacity(1);

        main_box = new Box(GTK_ORIENTATION_VERTICAL);
        main_box->push_back(main);

        popover = new Popover;
        popover->set_autohide(false);
        popover_box = new Box(GTK_ORIENTATION_VERTICAL);

        for (auto& row : children)
        {
            popover_rows.emplace_back(new ListView(GTK_ORIENTATION_HORIZONTAL));

            for (auto* child : row)
                popover_rows.back()->push_back(child);

            popover_box->push_back(popover_rows.back());
        }

        popover->attach_to(main->overlay);
        popover->set_child(popover_box);
        popover->set_relative_position(GTK_POS_RIGHT);

        main_motion_event_controller = new MotionEventController();
        main_box->add_controller(main_motion_event_controller);
    }

    Toolbox::IconWithPopover::operator GtkWidget*()
    {
        return main_box->operator GtkWidget*();
    }

    void Toolbox::on_icon_with_popover_enter(GtkEventControllerMotion*, gdouble x, gdouble y, void* data)
    {
        auto* self = ((on_icon_with_popover_enter_data*) data)->self;
        auto* instance = ((on_icon_with_popover_enter_data*) data)->instance;

        for (auto* icon_with : instance->_icons_with_popover)
        {
            if (icon_with == self)
                icon_with->popover->popup();
            else
                icon_with->popover->popdown();
        }
    }

    void Toolbox::on_icon_click(GtkGestureClick*, gint n_press, gdouble x, gdouble y, void* data)
    {
        auto* self = ((on_icon_click_data*) data)->self;
        auto* parent = ((on_icon_click_data*) data)->parent;
        auto* instance = ((on_icon_click_data*) data)->instance;
        auto id = ((on_icon_click_data*) data)->tool_id;

        for (auto& pair : instance->_icons)
        {
            pair.second->child_selected_indicator->set_opacity(pair.second == parent);
            pair.second->selected_indicator->set_opacity(pair.second == self);
            pair.second->popover_indicator->set_visible(pair.second != parent);
        }

        state::active_tool = id;
    }

    void Toolbox::on_icon_without_popover_enter(GtkEventControllerMotion*, gdouble x, gdouble y, void* data)
    {
        auto* instance = (Toolbox*) data;
        for (auto* icon_with : instance->_icons_with_popover)
            icon_with->popover->popdown();
    }

    void Toolbox::on_main_leave(GtkEventControllerMotion* self, void* data)
    {
        auto* instance = (Toolbox*) data;

        auto size = instance->get_size();
        if (instance->last_known_position->x > 0.75 * size.x)
            return; // don't popdown if leaving to the right

        for (auto* icon_with : instance->_icons_with_popover)
            icon_with->popover->popdown();
    }

    void Toolbox::on_main_motion(GtkEventControllerMotion* self, gdouble x, gdouble y, void* data)
    {
        auto* instance = (Toolbox*) data;
        instance->last_known_position->x = x;
        instance->last_known_position->y = y;
    }

    Toolbox::Toolbox()
    {
        main = new ListView(GTK_ORIENTATION_VERTICAL);
        main->set_show_separators(true);

        auto add_icon = [&](ToolID id) -> Icon* {
            _icons.insert({id, new Icon(id)});
            return _icons.at(id);
        };

        for (auto& pair : icon_mapping)
        {
            auto main_icon = add_icon(pair.first);
            main_icon->click_event_controller->connect_pressed(on_icon_click, new on_icon_click_data{main_icon, nullptr, this, pair.first});
            main_icon->set_tooltip_text(generate_tooltip(pair.first));
            std::vector<std::vector<Icon*>> children;

            for (auto& row : pair.second)
            {
                if (not row.empty())
                    children.emplace_back();

                for (auto& child : row)
                {
                    auto* to_add = add_icon(child);
                    children.back().emplace_back(to_add);
                    children.back().back()->click_event_controller->connect_pressed(on_icon_click, new on_icon_click_data{to_add, main_icon, this, child});
                    children.back().back()->set_tooltip_text(generate_tooltip(child));
                }
            }

            if (children.empty())
            {
                main->push_back(main_icon);
                main_icon->motion_event_controller->connect_enter(on_icon_without_popover_enter, this);
            }
            else
            {
                _icons_with_popover.emplace_back(new IconWithPopover(main_icon, children));
                auto* current = _icons_with_popover.back();
                current->main_motion_event_controller->connect_enter(on_icon_with_popover_enter, new on_icon_with_popover_enter_data{current, this});
                main->push_back(current);
            }
        }

        motion_event_controller = new MotionEventController();
        main->add_controller(motion_event_controller);
        motion_event_controller->connect_leave(on_main_leave, this);
        motion_event_controller->connect_motion(on_main_motion, this);

        select(MARQUEE_RECTANGLE_ADD);
    }

    Toolbox::operator GtkWidget*()
    {
        return main->operator GtkWidget*();
    }

    void Toolbox::select(ToolID id)
    {
        Icon* parent = nullptr;
        Icon* child = _icons.at(id);

        for (auto& pair : icon_mapping)
        {
            if (pair.first == id)
                goto done;
            else
            {
                for (auto& row : pair.second)
                {
                    for (auto& child : row)
                    {
                        if (child == id)
                        {
                            parent = _icons.at(pair.first);
                            goto done;
                        }
                    }
                }
            }
        }
        done:;

        auto data = on_icon_click_data{child, parent, this, id};
        on_icon_click(nullptr, 1, -1, -1, &data);
    }

    std::string Toolbox::generate_tooltip(ToolID tool_id)
    {
        static const std::string color_start = "<span foreground=\"#FF88BB\">";
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