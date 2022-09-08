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

                    {MARQUEE_NEIGHBORHODD_SELECT, {
                            {MARQUEE_RECTANGLE, MARQUEE_RECTANGLE_ADD, MARQUEE_RECTANGLE_SUBTRACT},
                            {MARQUEE_CIRCLE, MARQUEE_CIRCLE_ADD, MARQUEE_CIRCLE_SUBTRACT},
                            {MARQUEE_POLYGON, MARQUEE_POLYGON_ADD, MARQUEE_POLYGON_SUBTRACT}
                    }},

                    {PENCIL, {{}}},
                    {ERASER, {{}}},
                    {EYEDROPPER, {{}}},
                    {BUCKET_FILL, {{}}},
                    {LINE, {{}}},

                    {"shapes_outline", {
                            {RECTANGLE_OUTLINE, RECTANGLE_FILL},
                            {CIRCLE_OUTLINE, CIRCLE_FILL},
                            {POLYGON_OUTLINE, POLYGON_FILL}
                    }},

                    {GRADIENT_DITHERED, {{GRADIENT_SMOOTH}}},
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
                MotionEventController* popover_motion_event_controller;
            };

            using on_icon_with_popover_enter_data = struct { IconWithPopover* self; Toolbox* instance; };
            static void on_icon_with_popover_enter(GtkEventControllerMotion* self, gdouble x, gdouble y, void* instance);

            using on_popover_leave_data = IconWithPopover*;
            static void on_popover_leave(GtkEventControllerMotion* self, void* data);

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

            using on_global_shortcut_select_data = struct {Toolbox* instance; ToolID tool_id;};
            static void on_global_shortcut_select(void*);
            
            using on_global_shortcut_marquee_mode_shift_data = struct {Toolbox* instance; bool add;};
            static void on_global_shortcut_marquee_mode_shift(void*);
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

        popover_motion_event_controller = new MotionEventController();
        popover_box->add_controller(popover_motion_event_controller);

        popover_motion_event_controller->connect_leave(on_popover_leave, this);
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

    void Toolbox::on_popover_leave(GtkEventControllerMotion* self, void* data)
    {
        ((IconWithPopover*) data)->popover->popdown();
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

    void Toolbox::on_global_shortcut_select(void* data)
    {
        auto* instance = ((on_global_shortcut_select_data*) data)->instance;
        auto id = ((on_global_shortcut_select_data*) data)->tool_id;

        instance->select(id);
    }

    void Toolbox::on_global_shortcut_marquee_mode_shift(void* data)
    {
        auto* instance = ((on_global_shortcut_marquee_mode_shift_data*) data)->instance;
        auto add = ((on_global_shortcut_marquee_mode_shift_data*) data)->add;

        if (state::active_tool == MARQUEE_RECTANGLE or
            state::active_tool == MARQUEE_RECTANGLE_ADD or
            state::active_tool == MARQUEE_RECTANGLE_SUBTRACT)
        {
            if (add)
                instance->select(MARQUEE_RECTANGLE_ADD);
            else
                instance->select(MARQUEE_RECTANGLE_SUBTRACT);
        }
        else if (state::active_tool == MARQUEE_CIRCLE or
                 state::active_tool == MARQUEE_CIRCLE_ADD or
                 state::active_tool == MARQUEE_CIRCLE_SUBTRACT)
        {
            if (add)
                instance->select(MARQUEE_CIRCLE_ADD);
            else
                instance->select(MARQUEE_CIRCLE_SUBTRACT);
        }
        else if (state::active_tool == MARQUEE_POLYGON or
                 state::active_tool == MARQUEE_POLYGON_ADD or
                 state::active_tool == MARQUEE_POLYGON_SUBTRACT)
        {
            if (add)
                instance->select(MARQUEE_POLYGON_ADD);
            else
                instance->select(MARQUEE_POLYGON_SUBTRACT);
        }
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

        for (auto id : {PENCIL, ERASER, EYEDROPPER, BUCKET_FILL, LINE, RECTANGLE_OUTLINE, RECTANGLE_FILL, CIRCLE_OUTLINE, CIRCLE_FILL, POLYGON_OUTLINE, POLYGON_FILL, GRADIENT_DITHERED, GRADIENT_SMOOTH, MARQUEE_RECTANGLE, MARQUEE_CIRCLE, MARQUEE_NEIGHBORHODD_SELECT, MARQUEE_POLYGON})
            state::main_window->register_global_shortcut(
                    state::shortcut_map,
                    "toolbox." + id,
                    on_global_shortcut_select,
                    new on_global_shortcut_select_data{this, id}
            );

        state::main_window->register_global_shortcut(
                state::shortcut_map,
                "toolbox.marquee_mode_add",
                on_global_shortcut_marquee_mode_shift,
                new on_global_shortcut_marquee_mode_shift_data{this, true}
        );

        state::main_window->register_global_shortcut(
                state::shortcut_map,
                "toolbox.marquee_mode_subtract",
                on_global_shortcut_marquee_mode_shift,
                new on_global_shortcut_marquee_mode_shift_data{this, false}
        );

        select(MARQUEE_RECTANGLE);
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
        on_icon_click(nullptr, 1, -1, -1, &data); // sets state::active_tool_id
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
                {MARQUEE_NEIGHBORHODD_SELECT, {
                        "Magic Wand Select",
                        "Automatically select same-colored regions"
                }},

                // MARQUEE RECT

                {MARQUEE_RECTANGLE, {
                        "Rectangle Select",
                        "Select rectangular region\n(Replaces current selection)"
                }},

                {MARQUEE_RECTANGLE_ADD, {
                        "Rectangle Select (Add)",
                        "Select rectangular region\n(Adds to current selection)"
                }},

                {MARQUEE_RECTANGLE_SUBTRACT, {
                        "Rectangle Select (Subtract)",
                        "Select rectangular region\n(Subtracts from current selection)"
                }},

                // MARQUEE CIRCLE

                {MARQUEE_CIRCLE, {
                        "Ellipse Select",
                        "Select elliptical region\n(Replaces current selection)"
                }},

                {MARQUEE_CIRCLE_ADD, {
                        "Circle Select (Add)",
                        "Select elliptical region\n(Adds to current selection)"
                }},

                {MARQUEE_CIRCLE_SUBTRACT, {
                        "Circle Select (Subtract)",
                        "Select elliptical region\n(Subtracts from current selection)"
                }},

                // MARQUEE POLY

                {MARQUEE_POLYGON, {
                        "Lasso Select",
                        "Select polygonal region\n(Replaces current selection)"
                }},

                {MARQUEE_POLYGON_ADD, {
                        "Lasso Select (Add)",
                        "Select polygonal region\n(Adds to current selection)"
                }},

                {MARQUEE_POLYGON_SUBTRACT, {
                        "Lasso Select (Subtract)",
                        "Select polygonal region\n(Subtracts from current selection)"
                }},

                // TOOLS

                {PENCIL, {
                        "Pencil",
                        "Draw using brush shape and size"
                }},

                {ERASER, {
                        "Eraser",
                        "Erase using brush shape and size"
                }},

                {EYEDROPPER, {
                        "Color Select",
                        "Select color from canvas"
                }},

                {BUCKET_FILL, {
                        "Bucket Fill",
                        "Fill region with color"
                }},

                {LINE, {
                        "Line",
                        "Draw colored line using brush size"
                }},

                // SHAPES OUTLINE

                {"shapes_outline", {
                        "Shapes",
                        "Draw geometric shapes"
                }},

                {RECTANGLE_OUTLINE, {
                        "Rectangle (Outline)",
                        "Draw colored rectangle outline using brush size"
                }},

                {CIRCLE_OUTLINE, {
                        "Ellipses (Outline)",
                        "Draw colored ellipses outline using brush size"
                }},

                {POLYGON_OUTLINE, {
                        "Polygon (Outline)",
                        "Draw colored polygon outline using brush size"
                }},

                // SHAPES FILLED

                {"shapes_fill", {
                        "Shapes (Filled)",
                        "Draw solid geometric shapes"
                }},

                {RECTANGLE_FILL, {
                        "Rectangle (Filled)",
                        "Draw colored rectangle"
                }},

                {CIRCLE_FILL, {
                        "Ellipses (Filled)",
                        "Draw colored ellipse"
                }},

                {POLYGON_FILL, {
                        "Polygon (Filled)",
                        "Draw colored polygon"
                }},

                // GRADIENT

                {GRADIENT_DITHERED, {
                        "Gradient (Dithered)",
                        "Draw dithered gradient, from primary to secondary color"
                }},

                {GRADIENT_SMOOTH, {
                        "Gradient (Smooth)",
                        "Draw gradient, from primary to secondary Color"
                }},
        };

        auto it = tool_id_to_title_and_description.find(tool_id);
        if (it == tool_id_to_title_and_description.end())
        {
            std::cerr << "[ERROR] In Toolbox::generate_tooltip: No description found for " << tool_id << std::endl;
            return tool_id;
        }

        std::stringstream out;
        auto shortcut = get_shortcut(tool_id);
        out  << "<b>" << it->second.first << "</b>" << (not has_shortcut(tool_id) ? "" : "   [ " + shortcut + " ]") << "\n"
             << it->second.second;

        return out.str();
    }
}