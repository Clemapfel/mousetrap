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

                    static inline Texture* has_popover_indicator_texture = nullptr;
                    static inline Texture* child_selected_indicator_texture = nullptr;
                    static inline Texture* selected_indicator_texture = nullptr;

                    Texture* _tool_texture = nullptr;
                    GLArea _render_area;

                    static void on_render_area_realize(Widget*, Icon* instance);
                    static void on_render_area_resize(GLArea*, int, int, Icon* instance);

                    Shape* _tool_shape;
                    Shape* _has_popover_indicator_shape;
                    Shape* _selected_indicator_shape;
                    Shape* _child_selected_indicator_shape;

                    AspectFrame _aspect_frame = AspectFrame(1);
            };

            class IconWithPopover
            {
                public:
                    IconWithPopover(ToolID main, std::vector<std::vector<ToolID>> children, Toolbox* owner);
                    operator Widget*();

                private:
                    ToggleButton _parent_button;

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
        _render_area.connect_signal_realize(on_render_area_realize, this);
        _render_area.connect_signal_resize(on_render_area_resize, this);
        _aspect_frame.set_child(&_render_area);
    }

    void Toolbox::Icon::on_render_area_realize(Widget* widget, Icon* instance)
    {
        GLArea* area = (GLArea*) widget;
        area->make_current();

        if (has_popover_indicator_texture == nullptr)
        {
            has_popover_indicator_texture = new Texture();
            has_popover_indicator_texture->create_from_file(get_resource_path() + "icons/" + "has_popover_indicator" + ".png");
        }

        if (child_selected_indicator_texture == nullptr)
        {
            child_selected_indicator_texture = new Texture();
            child_selected_indicator_texture->create_from_file(get_resource_path() + "icons/" + "child_selected_horizontal" + ".png");
        }

        if (selected_indicator_texture == nullptr)
        {
            selected_indicator_texture = new Texture();
            selected_indicator_texture->create_from_file(get_resource_path() + "icons/" + "selected_indicator" + ".png");
        }

        instance->_tool_texture = new Texture();
        instance->_tool_texture->create_from_file(get_resource_path() + "icons/" + instance->_id + ".png");

        float offset = 1 - instance->_tool_texture->get_size().x / selected_indicator_texture->get_size().x;
        std::cout << offset << std::endl;

        instance->_tool_shape = new Shape();
        instance->_tool_shape->as_rectangle({offset, offset}, {1 - 2*offset, 1 - 2*offset});
        instance->_tool_shape->set_texture(instance->_tool_texture);

        instance->_has_popover_indicator_shape = new Shape();
        instance->_has_popover_indicator_shape->as_rectangle({0, 0}, {1, 1});
        instance->_has_popover_indicator_shape->set_texture(has_popover_indicator_texture);

        instance->_selected_indicator_shape = new Shape();
        instance->_selected_indicator_shape->as_rectangle({0, 0}, {1, 1});
        instance->_selected_indicator_shape->set_texture(selected_indicator_texture);

        instance->_child_selected_indicator_shape = new Shape();
        instance->_child_selected_indicator_shape->as_rectangle({0, 0}, {1, 1});
        instance->_child_selected_indicator_shape->set_texture(child_selected_indicator_texture);

        area->add_render_task(instance->_tool_shape);
        area->add_render_task(instance->_child_selected_indicator_shape);
        area->add_render_task(instance->_selected_indicator_shape);
        area->add_render_task(instance->_has_popover_indicator_shape);

        area->set_size_request({
            selected_indicator_texture->get_size().x * state::icon_scale,
            selected_indicator_texture->get_size().y * state::icon_scale,
        });

        area->queue_render();
    }

    void Toolbox::Icon::on_render_area_resize(GLArea* area, int w, int h, Icon* instance)
    {
        area->queue_render();
    }

    void Toolbox::Icon::set_has_popover(bool b)
    {
        _has_popover_indicator_shape->set_visible(b);
    }

    void Toolbox::Icon::set_selection_indicator_visible(bool b)
    {
        _selected_indicator_shape->set_visible(b);
    }

    void Toolbox::Icon::set_child_selection_indicator_visible(bool b)
    {
        _child_selected_indicator_shape->set_visible(b);
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
        _popover.attach_to(&_parent_button);
    }

    Toolbox::IconWithPopover::operator Widget*()
    {
        return _parent_icon->operator Widget*();
    }

    // TOOLBOX

    Toolbox::Toolbox()
    {
        for (auto& e : _elements)
        {
            e.operator Widget *()->set_halign(GTK_ALIGN_START);
            _flow_box.push_back(e);
        }

        _flow_box.set_max_children_per_line(5);
    }

    Toolbox::operator Widget*()
    {
        return &_flow_box;
    }

    void Toolbox::update()
    {}
}