#include <app/toolbox.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/canvas.hpp>

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
                &toolbox_select_shapes_fill,
                &toolbox_select_shapes_outline,
                &toolbox_select_marquee_neighborhood_select,
                &toolbox_select_marquee_rectangle,
                &toolbox_select_marquee_rectangle_add,
                &toolbox_select_marquee_rectangle_subtract,
                &toolbox_select_marquee_circle,
                &toolbox_select_marquee_circle_add,
                &toolbox_select_marquee_circle_subtract,
                &toolbox_select_marquee_polygon,
                &toolbox_select_marquee_polygon_add,
                &toolbox_select_marquee_polygon_subtract,
                &toolbox_select_brush,
                &toolbox_select_eraser,
                &toolbox_select_eyedropper,
                &toolbox_select_bucket_fill,
                &toolbox_select_line,
                &toolbox_select_rectangle_outline,
                &toolbox_select_rectangle_fill,
                &toolbox_select_circle_outline,
                &toolbox_select_circle_fill,
                &toolbox_select_polygon_outline,
                &toolbox_select_polygon_fill,
                &toolbox_select_gradient_dithered,
                &toolbox_select_gradient_smooth
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

        state::canvas->update();
    }

    void Toolbox::update()
    {
        select(state::active_tool);
    }
}
