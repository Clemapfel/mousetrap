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

        private:
            ToolID _currently_selected = state::active_tool;

            class Icon
            {
                public:
                    Icon(ToolID id, Toolbox*);
                    operator Widget*();

                    void set_selection_indicator_visible(bool);
                    void set_child_selection_indicator_visible(bool);
                    void set_has_popover(bool);

                private:
                    Toolbox* _owner;
                    ToolID _id;

                    ImageDisplay _has_popover_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "has_popover_indicator" + ".png", state::icon_scale);
                    ImageDisplay _child_selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "child_selected_horizontal" + ".png", state::icon_scale);
                    ImageDisplay _selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "selected_indicator" + ".png", state::icon_scale);

                    ImageDisplay _tool_icon = ImageDisplay(get_resource_path() + "icons/" + _id + ".png", state::icon_scale);

                    Overlay _overlay;
                    AspectFrame _aspect_frame = AspectFrame(1);
            };

            class IconWithPopover
            {
                public:
                    IconWithPopover(ToolID main, std::vector<std::vector<ToolID>> children, Toolbox* owner);
                    operator Widget*();

                private:
                    Viewport _main;

                    std::vector<Box> _popover_rows;
                    Box _popover_box = Box(GTK_ORIENTATION_VERTICAL);
                    Popover _popover;

                    Icon* _parent_icon;
                    std::vector<Icon*> _child_icons;
            };

            std::deque<IconWithPopover> _elements
            {
                IconWithPopover(MARQUEE_RECTANGLE, {{MARQUEE_RECTANGLE_ADD, MARQUEE_RECTANGLE_SUBTRACT}}, this),
                IconWithPopover(MARQUEE_CIRCLE, {{MARQUEE_CIRCLE_ADD, MARQUEE_CIRCLE_SUBTRACT}}, this),
                IconWithPopover(MARQUEE_POLYGON, {{MARQUEE_POLYGON_ADD, MARQUEE_POLYGON_SUBTRACT}}, this),
                IconWithPopover(MARQUEE_NEIGHBORHODD_SELECT, {{}}, this),
                IconWithPopover(PENCIL, {{}}, this),
                IconWithPopover(ERASER, {{}}, this),
                IconWithPopover(EYEDROPPER, {{}}, this),
                IconWithPopover(BUCKET_FILL, {{}}, this),
                IconWithPopover(LINE, {{}}, this),
                IconWithPopover("shapes_outline", {{RECTANGLE_OUTLINE, CIRCLE_OUTLINE, POLYGON_OUTLINE}}, this),
                IconWithPopover("shapes_fill", {{RECTANGLE_FILL, CIRCLE_FILL, POLYGON_FILL}}, this),
                IconWithPopover(GRADIENT_DITHERED, {{}}, this),
                IconWithPopover(GRADIENT_SMOOTH, {{}}, this)
            };

            FlowBox _flow_box = FlowBox(GTK_ORIENTATION_HORIZONTAL);
    };
}
//

namespace mousetrap
{
    // ICON

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
    }

    void Toolbox::Icon::set_has_popover(bool b)
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
        return &_aspect_frame;
    }

    // ICON WITH POPOVER

    Toolbox::IconWithPopover::IconWithPopover(ToolID main, std::vector<std::vector<ToolID>> children, Toolbox* owner)
        : _parent_icon(new Icon(main, owner))
    {
        _popover_rows.reserve(children.size());
        for (auto& row : children)
        {
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
    }

    Toolbox::IconWithPopover::operator Widget*()
    {
        return &_main;
    }

    // TOOLBOX

    Toolbox::Toolbox()
    {
        for (auto& e : _elements)
        {
            e.operator Widget *()->set_halign(GTK_ALIGN_START);
            _flow_box.push_back(e);
        }

        _flow_box.set_hexpand(false);
        _flow_box.set_max_children_per_line(_elements.size() - 1);
            // for some reason going from 2 to 1 lines lags the ui, this prevents that
    }

    Toolbox::operator Widget*()
    {
        return &_flow_box;
    }

    void Toolbox::update()
    {}
}