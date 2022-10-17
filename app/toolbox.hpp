// 
// Copyright 2022 Clemens Cords
// Created on 10/16/22 by clem (mail@clemens-cords.com)
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
#include <include/gl_area.hpp>
#include <include/aspect_frame.hpp>
#include <include/grid_view.hpp>
#include <include/viewport.hpp>

#include <app/tools.hpp>
#include <app/app_component.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
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

                    ImageDisplay _has_popover_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "has_popover_indicator" + ".png", state::icon_scale);
                    ImageDisplay _child_selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "child_selected_horizontal" + ".png", state::icon_scale);
                    ImageDisplay _selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "selected_indicator" + ".png", state::icon_scale);

                    ImageDisplay _tool_icon = ImageDisplay(get_resource_path() + "icons/" + _id + ".png", state::icon_scale);

                    Overlay _overlay;
                    AspectFrame _aspect_frame = AspectFrame(1);

                    ListView _main = ListView(GTK_SELECTION_NONE);

                    ClickEventController _click_event_controller;
                    static void on_click_pressed(ClickEventController*, size_t, double, double, Icon* instance);
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
                new IconWithPopover(PENCIL, {}, this),
                new IconWithPopover(ERASER, {}, this),
                new IconWithPopover(EYEDROPPER, {}, this),
                new IconWithPopover(BUCKET_FILL, {}, this),
                new IconWithPopover(LINE, {}, this),
                new IconWithPopover("shapes_outline", {{RECTANGLE_OUTLINE, CIRCLE_OUTLINE, POLYGON_OUTLINE}}, this),
                new IconWithPopover("shapes_fill", {{RECTANGLE_FILL, CIRCLE_FILL, POLYGON_FILL}}, this),
                new IconWithPopover(GRADIENT_DITHERED, {{GRADIENT_DITHERED, GRADIENT_SMOOTH}}, this)
            };

            FlowBox _flow_box = FlowBox(GTK_ORIENTATION_HORIZONTAL);
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
        : _id(id), _owner(owner)
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
            _flow_box.push_back(e->operator Widget*());
        }

        _flow_box.set_hexpand(false);
        _flow_box.set_max_children_per_line(_elements.size() - 1);
            // for some reason going from 2 to 1 lines lags the ui, this prevents that

        select(state::active_tool);
    }

    Toolbox::operator Widget*()
    {
        return &_flow_box;
    }

    void Toolbox::select(ToolID id)
    {
        for (auto& element : _elements)
            element->set_tool_selected(id);

        if (id == "shapes_outline")
            id = POLYGON_OUTLINE;

        if (id == "shapes_fill")
            id = POLYGON_FILL;

        state::active_tool = id;
        _currently_selected = id;
    }

    void Toolbox::update()
    {
        select(state::active_tool);
    }
}