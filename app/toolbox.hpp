// 
// Copyright 2022 Clemens Cords
// Created on 10/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/tools.hpp>
#include <app/app_component.hpp>
#include <app/global_state.hpp>
#include <app/tooltip.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        Action toolbox_select_shapes_fill_action = Action("toolbox.select_shapes_fill");
        Action toolbox_select_shapes_outline_action = Action("toolbox.select_shapes_outline");

        Action toolbox_select_marquee_neighborhood_select_action = Action("toolbox.select_marquee_neighborhood_select");
        Action toolbox_select_marquee_rectangle_action = Action("toolbox.select_marquee_rectangle");
        Action toolbox_select_marquee_rectangle_add_action = Action("toolbox.select_marquee_rectangle_add");
        Action toolbox_select_marquee_rectangle_subtract_action = Action("toolbox.select_marquee_rectangle_subtract");
        Action toolbox_select_marquee_circle_action = Action("toolbox.select_marquee_circle");
        Action toolbox_select_marquee_circle_add_action = Action("toolbox.select_marquee_circle_add");
        Action toolbox_select_marquee_circle_subtract_action = Action("toolbox.select_marquee_circle_subtract");
        Action toolbox_select_marquee_polygon_action = Action("toolbox.select_marquee_polygon");
        Action toolbox_select_marquee_polygon_add_action = Action("toolbox.select_marquee_polygon_add");
        Action toolbox_select_marquee_polygon_subtract_action = Action("toolbox.select_marquee_polygon_subtract");

        Action toolbox_select_brush_action = Action("toolbox.select_brush");
        Action toolbox_select_eraser_action = Action("toolbox.select_eraser");
        Action toolbox_select_eyedropper_action = Action("toolbox.select_eyedropper");
        Action toolbox_select_bucket_fill_action = Action("toolbox.select_bucket_fill");
        Action toolbox_select_line_action = Action("toolbox.select_line");

        Action toolbox_select_rectangle_outline_action = Action("toolbox.select_rectangle_outline");
        Action toolbox_select_rectangle_fill_action = Action("toolbox.select_rectangle_fill");
        Action toolbox_select_circle_outline_action = Action("toolbox.select_circle_outline");
        Action toolbox_select_circle_fill_action = Action("toolbox.select_circle_fill");
        Action toolbox_select_polygon_outline_action = Action("toolbox.select_polygon_outline");
        Action toolbox_select_polygon_fill_action = Action("toolbox.select_polygon_fill");
        Action toolbox_select_gradient_dithered_action = Action("toolbox.select_gradient_dithered");
        Action toolbox_select_gradient_smooth_action = Action("toolbox.select_gradient_smooth");
    }

    class Toolbox : public AppComponent
    {
        public:
            Toolbox();

            void update() override;
            operator Widget*() override;

            void select(ToolID);

        private:
            ToolID _currently_selected = state::active_tool;

            class Icon
            {
                public:
                    Icon(ToolID id, Toolbox*);
                    operator Widget*();

                    void set_selection_indicator_visible(bool);
                    void set_child_selection_indicator_visible(bool);
                    void set_has_popover_indicator_visible(bool);

                    ToolID get_id();

                private:
                    Toolbox* _owner;
                    ToolID _id;

                    ActionID _action_id;

                    ImageDisplay _has_popover_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "has_popover_indicator" + ".png", state::icon_scale);
                    ImageDisplay _child_selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "child_selected_horizontal" + ".png", state::icon_scale);
                    ImageDisplay _selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "selected_indicator" + ".png", state::icon_scale);

                    ImageDisplay _tool_icon = ImageDisplay(get_resource_path() + "icons/" + _id + ".png", state::icon_scale);

                    Overlay _overlay;
                    AspectFrame _aspect_frame = AspectFrame(1);

                    ListView _main = ListView(GTK_SELECTION_NONE);

                    ClickEventController _click_event_controller;
                    static void on_click_pressed(ClickEventController*, size_t, double, double, Icon* instance);

                    Tooltip _tooltip;
            };

            class IconWithPopover
            {
                public:
                    IconWithPopover(ToolID main, std::vector<std::vector<ToolID>> children, Toolbox* owner);
                    operator Widget*();

                    void set_tool_selected(ToolID);

                private:
                    Toolbox* _owner = nullptr;
                    Viewport _main;

                    std::vector<Box> _popover_rows;
                    Box _popover_box = Box(GTK_ORIENTATION_VERTICAL);
                    Popover _popover;

                    Icon* _parent_icon;
                    std::vector<Icon*> _child_icons;

                    ClickEventController _click_event_controller;
                    static void on_parent_icon_click_pressed(ClickEventController*, size_t, double, double, IconWithPopover* instance);
            };

            std::vector<IconWithPopover*> _elements =
            {
                new IconWithPopover(MARQUEE_RECTANGLE, {{MARQUEE_RECTANGLE_ADD, MARQUEE_RECTANGLE, MARQUEE_RECTANGLE_SUBTRACT}}, this),
                new IconWithPopover(MARQUEE_CIRCLE, {{MARQUEE_CIRCLE_ADD, MARQUEE_CIRCLE, MARQUEE_CIRCLE_SUBTRACT}}, this),
                new IconWithPopover(MARQUEE_POLYGON, {{MARQUEE_POLYGON_ADD, MARQUEE_POLYGON, MARQUEE_POLYGON_SUBTRACT}}, this),
                new IconWithPopover(MARQUEE_NEIGHBORHODD_SELECT, {}, this),
                new IconWithPopover(BRUSH, {}, this),
                new IconWithPopover(ERASER, {}, this),
                new IconWithPopover(EYEDROPPER, {}, this),
                new IconWithPopover(BUCKET_FILL, {}, this),
                new IconWithPopover(LINE, {}, this),
                new IconWithPopover(SHAPES_OUTLINE, {{RECTANGLE_OUTLINE, CIRCLE_OUTLINE, POLYGON_OUTLINE}}, this),
                new IconWithPopover(SHAPES_FILL, {{RECTANGLE_FILL, CIRCLE_FILL, POLYGON_FILL}}, this),
                new IconWithPopover(GRADIENT_DITHERED, {{GRADIENT_DITHERED, GRADIENT_SMOOTH}}, this)
            };

            static inline ToolID _shapes_fill_forwarding_id = POLYGON_FILL;
            static inline ToolID _shapes_outline_forwarding_id = POLYGON_OUTLINE;

            ListView _element_container = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);

            SeparatorLine _spacer_left, _spacer_right;
            Box _outer = Box(GTK_ORIENTATION_HORIZONTAL);
    };
}
//

namespace mousetrap
{
    // ICON

    void Toolbox::Icon::on_click_pressed(ClickEventController*, size_t, double, double, Icon* instance)
    {
        instance->_owner->select(instance->_id);
    }

    Toolbox::Icon::Icon(ToolID id, Toolbox* owner)
        : _owner(owner), _id(id), _action_id("toolbox.select_" + id)
    {
        for (auto* w : {&_has_popover_indicator_icon, &_child_selected_indicator_icon, &_selected_indicator_icon, &_tool_icon})
        {
            w->set_size_request(w->get_size());
            w->set_expand(false);
            w->set_align(GTK_ALIGN_CENTER);
        }

        _overlay.set_child(&_selected_indicator_icon);
        _overlay.add_overlay(&_tool_icon);
        _overlay.add_overlay(&_child_selected_indicator_icon);
        _overlay.add_overlay(&_has_popover_indicator_icon);
        _aspect_frame.set_child(&_overlay);

        _main.push_back(&_aspect_frame);

        _click_event_controller.connect_signal_click_pressed(on_click_pressed, this);
        _main.add_controller(&_click_event_controller);

        _tooltip.set_title(state::tooltips_file->get_value("toolbox." + id, "title"));
        _tooltip.set_description(state::tooltips_file->get_value("toolbox." + id, "description"));

        auto add_shortcut = [&](const std::string& id)
        {
            auto value = state::keybindings_file->get_value("toolbox", "select_" + id);
            if (value != "never")
            {
                _tooltip.add_shortcut(
                    value,
                    state::keybindings_file->get_comment_above("toolbox", "select_" + id)
                );
            }
        };

        if (id == SHAPES_OUTLINE)
        {
            for (auto name : {RECTANGLE_OUTLINE, CIRCLE_OUTLINE, POLYGON_OUTLINE})
                add_shortcut(name);
        }
        else if (id == SHAPES_FILL)
        {
            for (auto name : {RECTANGLE_FILL, CIRCLE_FILL, POLYGON_FILL})
                add_shortcut(name);
        }
        else
            add_shortcut(id);


        operator Widget*()->set_tooltip_widget(_tooltip);
    }

    void Toolbox::Icon::set_has_popover_indicator_visible(bool b)
    {
        _has_popover_indicator_icon.set_opacity(b ? 1 : 0);
    }

    void Toolbox::Icon::set_selection_indicator_visible(bool b)
    {
        _selected_indicator_icon.set_opacity(b ? 1 : 0);
    }

    void Toolbox::Icon::set_child_selection_indicator_visible(bool b)
    {
        _child_selected_indicator_icon.set_opacity(b ? 1 : 0);
    }

    Toolbox::Icon::operator Widget*()
    {
        return &_main;
    }

    ToolID Toolbox::Icon::get_id()
    {
        return _id;
    }

    // ICON WITH POPOVER

    void Toolbox::IconWithPopover::on_parent_icon_click_pressed(ClickEventController*, size_t, double, double, IconWithPopover* instance)
    {
        for (auto* element : instance->_owner->_elements)
            if (not element->_popover_rows.empty())
                element->_popover.popdown();

        if (not instance->_popover_rows.empty())
            instance->_popover.popup();
    }

    Toolbox::IconWithPopover::IconWithPopover(ToolID main, std::vector<std::vector<ToolID>> children, Toolbox* owner)
        : _owner(owner), _parent_icon(new Icon(main, owner))
    {
        _popover_rows.reserve(children.size());
        for (auto& row : children)
        {
            if (row.empty())
                continue;

            _popover_rows.emplace_back(GTK_ORIENTATION_HORIZONTAL);
            for (auto& child : row)
            {
                _child_icons.emplace_back(new Icon(child, owner));
                _popover_rows.back().push_back(_child_icons.back()->operator Widget*());
            }
        }

        for (auto& row : _popover_rows)
            _popover_box.push_back(&row);

        _popover.set_child(&_popover_box);
        _popover.attach_to(&_main);
        _main.set_child(_parent_icon->operator Widget*());

        _parent_icon->set_selection_indicator_visible(false);
        _parent_icon->set_child_selection_indicator_visible(false);
        _parent_icon->set_has_popover_indicator_visible(_child_icons.size() != 0);

        for (auto* child : _child_icons)
        {
            child->set_selection_indicator_visible(false);
            child->set_child_selection_indicator_visible(false);
            child->set_has_popover_indicator_visible(false);
        }

        _click_event_controller.connect_signal_click_pressed(on_parent_icon_click_pressed, this);
        _parent_icon->operator Widget*()->add_controller(&_click_event_controller);
    }

    Toolbox::IconWithPopover::operator Widget*()
    {
        return &_main;
    }

    void Toolbox::IconWithPopover::set_tool_selected(ToolID id)
    {
        _parent_icon->set_has_popover_indicator_visible(not _child_icons.empty());

        if (_parent_icon->get_id() == id)
        {
            _parent_icon->set_selection_indicator_visible(true);
            _parent_icon->set_child_selection_indicator_visible(false);
        }
        else
        {
            _parent_icon->set_selection_indicator_visible(false);
            _parent_icon->set_child_selection_indicator_visible(false);
        }

        for (auto* child : _child_icons)
        {
            if (child->get_id() == id)
            {
                _parent_icon->set_selection_indicator_visible(false);
                _parent_icon->set_child_selection_indicator_visible(true);
                _parent_icon->set_has_popover_indicator_visible(false);

                child->set_selection_indicator_visible(true);
            }
            else
                child->set_selection_indicator_visible(false);
        }
    }

    // TOOLBOX

    Toolbox::Toolbox()
    {
        for (auto e : _elements)
        {
            e->operator Widget *()->set_halign(GTK_ALIGN_START);
            _element_container.push_back(e->operator Widget*());
        }

        _element_container.set_align(GTK_ALIGN_CENTER);
        _element_container.set_hexpand(false);

        _spacer_right.set_hexpand(true);
        _spacer_left.set_hexpand(true);

        _outer.push_back(&_spacer_left);
        _outer.push_back(&_element_container);
        _outer.push_back(&_spacer_right);

        _outer.set_hexpand(true);

        using namespace state::actions;

        for (auto* action : {
            &toolbox_select_shapes_fill_action,
            &toolbox_select_shapes_outline_action,
            &toolbox_select_marquee_neighborhood_select_action,
            &toolbox_select_marquee_rectangle_action,
            &toolbox_select_marquee_rectangle_add_action,
            &toolbox_select_marquee_rectangle_subtract_action,
            &toolbox_select_marquee_circle_action,
            &toolbox_select_marquee_circle_add_action,
            &toolbox_select_marquee_circle_subtract_action,
            &toolbox_select_marquee_polygon_action,
            &toolbox_select_marquee_polygon_add_action,
            &toolbox_select_marquee_polygon_subtract_action,
            &toolbox_select_brush_action,
            &toolbox_select_eraser_action,
            &toolbox_select_eyedropper_action,
            &toolbox_select_bucket_fill_action,
            &toolbox_select_line_action,
            &toolbox_select_rectangle_outline_action,
            &toolbox_select_rectangle_fill_action,
            &toolbox_select_circle_outline_action,
            &toolbox_select_circle_fill_action,
            &toolbox_select_polygon_outline_action,
            &toolbox_select_polygon_fill_action,
            &toolbox_select_gradient_dithered_action,
            &toolbox_select_gradient_smooth_action
        }){
            std::stringstream which;
            for (size_t i = std::string("toolbox.select_").size(); i < action->get_id().size(); ++i)
                which << (char) action->get_id().at(i);

            action->set_function([id = std::string(which.str())](){
                ((Toolbox*) state::toolbox)->select(id);
            });
            state::add_shortcut_action(*action);
        }

        select(state::active_tool);
    }

    Toolbox::operator Widget*()
    {
        return &_outer;
    }

    void Toolbox::select(ToolID id)
    {
        if (id == SHAPES_OUTLINE)
            id = POLYGON_OUTLINE;

        if (id == SHAPES_FILL)
            id = POLYGON_FILL;

        for (auto& element : _elements)
            element->set_tool_selected(id);

        state::active_tool = id;
        _currently_selected = id;

        state::update_canvas();
    }

    void Toolbox::update()
    {
        select(state::active_tool);
    }
}