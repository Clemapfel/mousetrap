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
#include <app/app_component.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
    class Toolbox : public AppComponent
    {
        public:
            Toolbox(GtkOrientation);
            ~Toolbox();
            
            operator Widget*() override;
            void update() override;
            
            void select(ToolID);

        private:
            static inline const std::vector<std::pair<std::string, std::vector<std::vector<std::string>>>> icon_mapping_old = {

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

            static inline const std::vector<std::pair<std::string, std::vector<std::vector<std::string>>>> icon_mapping = {

                    {MARQUEE_RECTANGLE, {{MARQUEE_RECTANGLE_ADD, MARQUEE_RECTANGLE_SUBTRACT}}},
                    {MARQUEE_CIRCLE, {{MARQUEE_CIRCLE_ADD, MARQUEE_CIRCLE_SUBTRACT}}},
                    {MARQUEE_POLYGON, {{MARQUEE_POLYGON_ADD, MARQUEE_POLYGON_SUBTRACT}}},
                    {MARQUEE_NEIGHBORHODD_SELECT, {{}}},

                    {PENCIL, {{}}},
                    {ERASER, {{}}},
                    {EYEDROPPER, {{}}},
                    {BUCKET_FILL, {{}}},
                    {LINE, {{}}},

                    {"shapes_outline", {
                          {RECTANGLE_OUTLINE, CIRCLE_OUTLINE, POLYGON_OUTLINE},
                    }},

                    {"shapes_fill", {
                           {RECTANGLE_FILL, CIRCLE_FILL, POLYGON_FILL},
                    }},

                    {GRADIENT_DITHERED, {{}}},
                    {GRADIENT_SMOOTH, {{}}},
            };

            struct Icon
            {
                Icon(ToolID id, bool is_vertical);
                operator Widget*();

                ToolID id;

                Overlay overlay;

                ImageDisplay label;
                ImageDisplay popover_indicator;
                ImageDisplay selected_indicator;
                ImageDisplay child_selected_indicator;

                ClickEventController click_event_controller;
                MotionEventController motion_event_controller;
            };

            using on_icon_click_data = struct { Icon* self; Icon* parent; Toolbox* instance; ToolID tool_id; };
            static void on_icon_click(ClickEventController* self, gint n_press, gdouble x, gdouble y, on_icon_click_data* user_data);
            static void on_icon_without_popover_enter(MotionEventController* self, gdouble x, gdouble y, Toolbox* instance);

            struct IconWithPopover
            {
                IconWithPopover(Icon* main, std::vector<std::vector<Icon*>> children, bool is_vertical);
                operator Widget*();

                bool is_vertical;

                Box main_box;
                Popover popover;
                std::vector<ListView> popover_rows;
                Box popover_box;

                MotionEventController main_motion_event_controller;
                MotionEventController popover_motion_event_controller;
            };

            using on_icon_with_popover_enter_data = struct { IconWithPopover* self; Toolbox* instance; };
            static void on_icon_with_popover_enter(MotionEventController* self, gdouble x, gdouble y, on_icon_with_popover_enter_data* instance);
            static void on_popover_leave(MotionEventController* self, IconWithPopover* data);

            std::map<ToolID, Icon*> _icons;

            std::vector<IconWithPopover*> _icons_with_popover;
            ListView _main;
            MotionEventController motion_event_controller;

            static void on_main_leave(MotionEventController* self, Toolbox* data);
            static void on_main_motion(MotionEventController* self, gdouble x, gdouble y, Toolbox* data);
            Vector2f* last_known_position = new Vector2f{0, 0};

            std::string generate_tooltip(ToolID);

            using on_global_shortcut_select_data = struct {Toolbox* instance; ToolID tool_id;};
            static void on_global_shortcut_select(on_global_shortcut_select_data*);
            
            using on_global_shortcut_marquee_mode_shift_data = struct {Toolbox* instance; bool add;};
            static void on_global_shortcut_marquee_mode_shift(on_global_shortcut_marquee_mode_shift_data*);

            bool _is_orientation_vertical;
    };
}

// ###

namespace mousetrap
{
    Toolbox::operator Widget*()
    {
        return &_main;
    }

    Toolbox::Icon::Icon(ToolID id, bool is_vertical)
        : id(id),
          label(get_resource_path() + "icons/" + id + ".png", state::icon_scale),
          popover_indicator(get_resource_path() + "icons/" + "has_popover_indicator" + ".png", state::icon_scale),
          selected_indicator(get_resource_path() + "icons/" + "selected_indicator" + ".png", state::icon_scale),
          child_selected_indicator(get_resource_path() + "icons/child_selected_" + (is_vertical ? "vertical" : "horizontal") + ".png", state::icon_scale),
          overlay(),
          click_event_controller(),
          motion_event_controller()
    {
        size_t scale = state::icon_scale;

        label.set_size_request({32 * scale, 32 * scale});
        label.set_expand(false);
        label.set_align(GTK_ALIGN_CENTER);

        popover_indicator.set_size_request({48 * scale, 48 * scale});
        popover_indicator.set_opacity(0);

        selected_indicator.set_size_request({48 * scale, 48 * scale});
        selected_indicator.set_opacity(0);

        child_selected_indicator.set_size_request({48 * scale, 48 * scale});
        child_selected_indicator.set_opacity(0);

        overlay.set_child(&selected_indicator);
        overlay.add_overlay(&child_selected_indicator);
        overlay.add_overlay(&label);
        overlay.add_overlay(&popover_indicator);

        overlay.add_controller(&click_event_controller);
        overlay.add_controller(&motion_event_controller);
    }

    Toolbox::~Toolbox()
    {
        for (auto& pair : _icons)
            delete pair.second;

        for (auto* icon_with : _icons_with_popover)
            delete icon_with;
    }

    Toolbox::Icon::operator Widget*()
    {
        return &overlay;
    }

    void Toolbox::update()
    {
        select(state::active_tool);
    }

    Toolbox::IconWithPopover::IconWithPopover(Icon* main, std::vector<std::vector<Icon*>> children, bool is_orientation_vertical)
        : is_vertical(is_orientation_vertical),
          main_box(GTK_ORIENTATION_VERTICAL),
          popover(),
          popover_box(GTK_ORIENTATION_VERTICAL),
          main_motion_event_controller(),
          popover_motion_event_controller()
    {
        main->popover_indicator.set_opacity(1);

        main_box.push_back(main->operator Widget*());
        popover.set_autohide(false);

        for (auto& row : children)
        {
            popover_rows.emplace_back(is_vertical ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL);

            for (auto& child : row)
                popover_rows.back().push_back(child->operator Widget*());

            popover_box.push_back(&popover_rows.back());
        }

        popover.attach_to(&main->overlay);
        popover.set_child(&popover_box);
        popover.set_relative_position(is_vertical ? GTK_POS_RIGHT : GTK_POS_BOTTOM);

        main_box.add_controller(&main_motion_event_controller);
        popover_box.add_controller(&popover_motion_event_controller);
        popover_motion_event_controller.connect_signal_motion_leave(on_popover_leave, this);
    }

    Toolbox::IconWithPopover::operator Widget*()
    {
        return &main_box;
    }

    void Toolbox::on_icon_with_popover_enter(MotionEventController*, gdouble x, gdouble y,
                                             on_icon_with_popover_enter_data* data)
    {
        auto* self = data->self;
        auto* instance = data->instance;

        for (auto& icon_with : instance->_icons_with_popover)
        {
            if (icon_with == self)
                icon_with->popover.popup();
            else
                icon_with->popover.popdown();
        }
    }

    void Toolbox::on_popover_leave(MotionEventController* self, IconWithPopover* data)
    {
        data->popover.popdown();
    }

    void Toolbox::on_icon_click(ClickEventController*, gint n_press, gdouble x, gdouble y, on_icon_click_data* data)
    {
        auto* self = ((on_icon_click_data*) data)->self;
        auto* parent = ((on_icon_click_data*) data)->parent;
        auto* instance = ((on_icon_click_data*) data)->instance;
        auto id = ((on_icon_click_data*) data)->tool_id;

        for (auto& pair : instance->_icons)
        {
            pair.second->child_selected_indicator.set_opacity(pair.second == parent);
            pair.second->selected_indicator.set_opacity(pair.second == self);
            pair.second->popover_indicator.set_visible(pair.second != parent);
        }

        state::active_tool = id;
    }

    void Toolbox::on_icon_without_popover_enter(MotionEventController*, gdouble x, gdouble y, Toolbox* instance)
    {
        for (auto& icon_with : instance->_icons_with_popover)
            icon_with->popover.popdown();
    }

    void Toolbox::on_main_leave(MotionEventController* self, Toolbox* instance)
    {
        auto size = (instance->operator Widget*())->get_size();

        if (instance->_is_orientation_vertical)
        {
            if (instance->last_known_position->x > 0.75 * size.x)
                return;
        }
        else
        {
            if (instance->last_known_position->y > 0.75 * size.y)
                return;
        }

        for (auto icon_with : instance->_icons_with_popover)
            icon_with->popover.popdown();
    }

    void Toolbox::on_main_motion(MotionEventController* self, gdouble x, gdouble y, Toolbox* instance)
    {
        instance->last_known_position->x = x;
        instance->last_known_position->y = y;
    }

    void Toolbox::on_global_shortcut_select(on_global_shortcut_select_data* data)
    {
        auto* instance = data->instance;
        auto id = data->tool_id;

        instance->select(id);
    }

    void Toolbox::on_global_shortcut_marquee_mode_shift(on_global_shortcut_marquee_mode_shift_data* data)
    {
        auto* instance = data->instance;
        auto add = data->add;

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

    Toolbox::Toolbox(GtkOrientation orientation)
        : _main(_is_orientation_vertical ? GTK_ORIENTATION_VERTICAL: GTK_ORIENTATION_HORIZONTAL),
          motion_event_controller()
    {
        _is_orientation_vertical = orientation == GTK_ORIENTATION_VERTICAL;

        _main.set_show_separators(true);

        auto add_icon = [&](ToolID id) -> Icon* {
            auto inserted = _icons.insert({id, new Icon(id, _is_orientation_vertical)});
            inserted.first->second->overlay.set_cursor(GtkCursorType::POINTER);
            return _icons.at(id);
        };

        for (auto& pair : icon_mapping)
        {
            auto main_icon = add_icon(pair.first);
            main_icon->click_event_controller.connect_signal_click_pressed(on_icon_click, new on_icon_click_data{main_icon, nullptr, this, pair.first});
            main_icon->operator Widget*()->set_tooltip_text(generate_tooltip(pair.first));
            std::vector<std::vector<Icon*>> children;

            for (auto& row : pair.second)
            {
                if (not row.empty())
                    children.emplace_back();

                for (auto& child : row)
                {
                    auto* to_add = add_icon(child);
                    children.back().emplace_back(to_add);
                    children.back().back()->click_event_controller.connect_signal_click_pressed(on_icon_click, new on_icon_click_data{to_add, main_icon, this, child});
                    children.back().back()->operator Widget*()->set_tooltip_text(generate_tooltip(child));
                }
            }

            if (children.empty())
            {
                _main.push_back(main_icon-> operator Widget*());
                main_icon->motion_event_controller.connect_signal_motion_enter(on_icon_without_popover_enter, this);
            }
            else
            {
                _icons_with_popover.emplace_back(new IconWithPopover(main_icon, children, _is_orientation_vertical));
                auto& current = _icons_with_popover.back();
                current->main_motion_event_controller.connect_signal_motion_enter(on_icon_with_popover_enter, new on_icon_with_popover_enter_data{current, this});
                _main.push_back(current->operator Widget*());
            }
        }

        _main.add_controller(&motion_event_controller);
        motion_event_controller.connect_signal_motion_leave(on_main_leave, this);
        motion_event_controller.connect_signal_motion(on_main_motion, this);

        for (auto id : {PENCIL, ERASER, EYEDROPPER, BUCKET_FILL, LINE, RECTANGLE_OUTLINE, RECTANGLE_FILL, CIRCLE_OUTLINE, CIRCLE_FILL, POLYGON_OUTLINE, POLYGON_FILL, GRADIENT_DITHERED, GRADIENT_SMOOTH, MARQUEE_RECTANGLE, MARQUEE_CIRCLE, MARQUEE_NEIGHBORHODD_SELECT, MARQUEE_POLYGON})
            state::main_window->register_global_shortcut<on_global_shortcut_select_data*>(
                    state::shortcut_map,
                    "toolbox." + id,
                    on_global_shortcut_select,
                    new on_global_shortcut_select_data{this, id}
            );

        state::main_window->register_global_shortcut<on_global_shortcut_marquee_mode_shift_data*>(
                state::shortcut_map,
                "toolbox.marquee_mode_add",
                on_global_shortcut_marquee_mode_shift,
                new on_global_shortcut_marquee_mode_shift_data{this, true}
        );

        state::main_window->register_global_shortcut<on_global_shortcut_marquee_mode_shift_data*>(
                state::shortcut_map,
                "toolbox.marquee_mode_subtract",
                on_global_shortcut_marquee_mode_shift,
                new on_global_shortcut_marquee_mode_shift_data{this, false}
        );

        select(MARQUEE_RECTANGLE);
    }

    void Toolbox::select(ToolID id)
    {
        if (id == "shapes_fill" or id == "SHAPES_FILL")
            id = RECTANGLE_FILL;

        if (id == "shapes_outline" or id == "SHAPES_OUTLINE")
            id = RECTANGLE_OUTLINE;

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