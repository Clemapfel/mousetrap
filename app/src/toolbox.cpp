//
// Created by clem on 1/24/23.
//

#include <app/toolbox.hpp>
#include <app/project_state.hpp>

namespace mousetrap
{
    Toolbox::ToolIcon::ToolIcon(const std::string& image_id, Toolbox* owner)
        : _tool_icon(get_resource_path() + "icons/" + image_id + ".png"), _owner(owner)
    {
        for (auto* w : {&_has_popover_indicator_icon, &_child_selected_indicator_icon, &_selected_indicator_icon, &_tool_icon})
        {
            w->set_size_request(w->get_size());
            w->set_expand(false);
            w->set_align(GTK_ALIGN_CENTER);
        }

        _overlay.set_child(&_selected_indicator_icon);
        _overlay.add_overlay(&_tool_icon);
        _overlay.add_overlay(&_has_popover_indicator_icon);
        _overlay.add_overlay(&_child_selected_indicator_icon);

        set_selection_indicator_visible(false);
        set_child_selection_indicator_visible(false);
        set_has_popover_indicator_visible(false);

        _main.set_child(&_overlay);

        _click_event_controller.connect_signal_click_pressed([](ClickEventController*, size_t, double, double, ToolIcon* instance) {
            if (instance->_on_click)
                instance->_on_click();
        }, this);
        _main.add_controller(&_click_event_controller);
    }

    Toolbox::ToolIcon::operator Widget*()
    {
        return &_main;
    }

    void Toolbox::ToolIcon::set_has_popover_indicator_visible(bool b)
    {
        _has_popover_indicator_icon.set_opacity(b ? 1 : 0);
    }

    void Toolbox::ToolIcon::set_selection_indicator_visible(bool b)
    {
        _selected_indicator_icon.set_opacity(b ? 1 : 0);
    }

    void Toolbox::ToolIcon::set_child_selection_indicator_visible(bool b)
    {
        _child_selected_indicator_icon.set_opacity(b ? 1 : 0);
    }

    Toolbox::ClickForPopover::ClickForPopover()
    {
        _popover.attach_to(&_label_box);
        _popover.set_child(&_popover_box);

        _label_click_controller.connect_signal_click_pressed([](ClickEventController*, size_t, double, double, ClickForPopover* instance) {
            instance->_popover.popup();
        }, this);
        _label_box.add_controller(&_label_click_controller);

        _popover_click_controller.connect_signal_click_pressed([](ClickEventController*, size_t, double, double, ClickForPopover* instance) {
            //instance->_popover.popdown();
        }, this);
        _popover_box.add_controller(&_popover_click_controller);
    }

    Box& Toolbox::ClickForPopover::get_label_box()
    {
        return _label_box;
    }

    Box& Toolbox::ClickForPopover::get_popover_box()
    {
        return _popover_box;
    }

    Toolbox::ClickForPopover::operator Widget*()
    {
        return &_label_box;
    }

    Toolbox::Toolbox()
    {
        for (auto id : {
            MARQUEE_NEIGHBORHODD_SELECT,
            MARQUEE_RECTANGLE,
            MARQUEE_RECTANGLE_ADD,
            MARQUEE_RECTANGLE_SUBTRACT,
            MARQUEE_CIRCLE,
            MARQUEE_CIRCLE_ADD,
            MARQUEE_CIRCLE_SUBTRACT,
            MARQUEE_POLYGON,
            MARQUEE_POLYGON_ADD,
            MARQUEE_POLYGON_SUBTRACT,
            BRUSH,
            ERASER,
            COLOR_SELECT,
            BUCKET_FILL,
            LINE,
            RECTANGLE_OUTLINE,
            RECTANGLE_FILL,
            CIRCLE_OUTLINE,
            CIRCLE_FILL,
            POLYGON_OUTLINE,
            POLYGON_FILL,
            GRADIENT
        })
        {
            auto* inserted = _id_to_icons.insert({id, new ToolIcon(tool_id_to_string(id), this)}).first->second;
            inserted->set_on_click([id = id](){
                std::cout << tool_id_to_string(id) << std::endl;
                active_state->set_active_tool(id);
            });
        }

        _filled_shapes_popover.get_label_box().push_back(_filled_shapes_icon);
        for (auto id : {ToolID::RECTANGLE_FILL, ToolID::CIRCLE_FILL, ToolID::POLYGON_FILL})
            _filled_shapes_popover.get_popover_box().push_back(_id_to_icons.at(id)->operator Widget*());

        _outline_shapes_popover.get_label_box().push_back(_outline_shapes_icon);
        for (auto id : {ToolID::RECTANGLE_OUTLINE, ToolID::CIRCLE_OUTLINE, ToolID::POLYGON_OUTLINE})
            _outline_shapes_popover.get_popover_box().push_back(_id_to_icons.at(id)->operator Widget*());

        _marquee_rectangle_popover.get_label_box().push_back(_marquee_rectangle_icon);
        for (auto id : {ToolID::MARQUEE_RECTANGLE_ADD, ToolID::MARQUEE_RECTANGLE_REPLACE, ToolID::MARQUEE_RECTANGLE_SUBTRACT})
            _marquee_rectangle_popover.get_popover_box().push_back(_id_to_icons.at(id)->operator Widget*());

        _marquee_circle_popover.get_label_box().push_back(_marquee_circle_icon);
        for (auto id : {ToolID::MARQUEE_CIRCLE_ADD, ToolID::MARQUEE_CIRCLE_REPLACE, ToolID::MARQUEE_CIRCLE_SUBTRACT})
            _marquee_circle_popover.get_popover_box().push_back(_id_to_icons.at(id)->operator Widget*());

        _marquee_polygon_popover.get_label_box().push_back(_marquee_polygon_icon);
        for (auto id : {ToolID::MARQUEE_POLYGON_ADD, ToolID::MARQUEE_POLYGON_REPLACE, ToolID::MARQUEE_POLYGON_SUBTRACT})
            _marquee_polygon_popover.get_popover_box().push_back(_id_to_icons.at(id)->operator Widget*());

        _list_view.push_back(*_id_to_icons.at(MARQUEE_NEIGHBORHODD_SELECT));
        _list_view.push_back(_marquee_rectangle_popover);
        _list_view.push_back(_marquee_circle_popover);
        _list_view.push_back(_marquee_polygon_popover);
        _list_view.push_back(*_id_to_icons.at(BRUSH));
        _list_view.push_back(*_id_to_icons.at(ERASER));
        _list_view.push_back(*_id_to_icons.at(BUCKET_FILL));
        _list_view.push_back(*_id_to_icons.at(COLOR_SELECT));
        _list_view.push_back(*_id_to_icons.at(LINE));

        _list_view.push_back(_filled_shapes_popover);
        _list_view.push_back(_outline_shapes_popover);

        _list_view.push_back(*_id_to_icons.at(GRADIENT));

        _id_to_listview_positions = {
                {MARQUEE_NEIGHBORHODD_SELECT, 0},
                {MARQUEE_RECTANGLE_REPLACE, 1},
                {MARQUEE_RECTANGLE_ADD, 1},
                {MARQUEE_RECTANGLE_SUBTRACT, 1},
                {MARQUEE_CIRCLE_ADD, 2},
                {MARQUEE_CIRCLE_SUBTRACT, 2},
                {MARQUEE_CIRCLE_REPLACE, 2},
                {MARQUEE_POLYGON_ADD, 3},
                {MARQUEE_POLYGON_SUBTRACT, 3},
                {MARQUEE_POLYGON_REPLACE, 3},

                {BRUSH, 4},
                {ERASER, 5},
                {BUCKET_FILL, 6},
                {COLOR_SELECT, 7},
                {LINE, 8},

                {CIRCLE_FILL, 9},
                {RECTANGLE_FILL, 9},
                {POLYGON_FILL, 9},

                {RECTANGLE_OUTLINE, 10},
                {CIRCLE_OUTLINE, 10},
                {POLYGON_OUTLINE, 10},

                {GRADIENT, 11}
        };

        _main_spacer_left.set_expand(true);
        _main_spacer_left.set_halign(GTK_ALIGN_START);
        _list_view.set_hexpand(false);
        _list_view.set_halign(GTK_ALIGN_CENTER);
        _main_spacer_right.set_expand(true);
        _main_spacer_right.set_halign(GTK_ALIGN_END);

        _main.push_back(&_main_spacer_left);
        _main.push_back(&_list_view);
        _main.push_back(&_main_spacer_right);

        _scrolled_window.set_child(&_main);
        _scrolled_window.set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
        _scrolled_window.set_propagate_natural_height(true);
        _scrolled_window.set_hexpand(true);

        for (auto* icon : {
            &_filled_shapes_icon,
            &_outline_shapes_icon,
            &_marquee_rectangle_icon,
            &_marquee_circle_icon,
            &_marquee_polygon_icon
        })
            icon->set_has_popover_indicator_visible(true);
    }

    Toolbox::operator Widget*()
    {
        return &_scrolled_window;
    }

    void Toolbox::select(ToolID id)
    {
        for (auto& pair : _id_to_icons)
        {
            bool selected = pair.first == id;
            auto& icon = pair.second;
            icon->set_selection_indicator_visible(selected);
        }

        auto marquee_rectangle_selected = id == MARQUEE_RECTANGLE_REPLACE or
            id == MARQUEE_RECTANGLE_ADD or
            id == MARQUEE_RECTANGLE_SUBTRACT;

        _marquee_rectangle_icon.set_child_selection_indicator_visible(marquee_rectangle_selected);
        _marquee_rectangle_icon.set_has_popover_indicator_visible(not marquee_rectangle_selected);

        auto marquee_circle_selected = id == MARQUEE_CIRCLE_REPLACE or
                id == MARQUEE_CIRCLE_ADD or
                id == MARQUEE_CIRCLE_SUBTRACT;

        _marquee_circle_icon.set_child_selection_indicator_visible(marquee_circle_selected);
        _marquee_circle_icon.set_has_popover_indicator_visible(not marquee_circle_selected);

        auto marquee_polygon_selected = id == MARQUEE_POLYGON_REPLACE or
                id == MARQUEE_POLYGON_ADD or
                id == MARQUEE_POLYGON_SUBTRACT;

        _marquee_polygon_icon.set_child_selection_indicator_visible(marquee_polygon_selected);
        _marquee_polygon_icon.set_has_popover_indicator_visible(not marquee_polygon_selected);

        auto shapes_fill_selected =id == CIRCLE_FILL or
                id == RECTANGLE_FILL or
                id == POLYGON_FILL;

        _filled_shapes_icon.set_child_selection_indicator_visible(shapes_fill_selected);
        _filled_shapes_icon.set_has_popover_indicator_visible(not shapes_fill_selected);

        auto shapes_outline_selected =id == CIRCLE_OUTLINE or
                id == RECTANGLE_OUTLINE or
                id == POLYGON_OUTLINE;

        _outline_shapes_icon.set_child_selection_indicator_visible(shapes_outline_selected);
        _outline_shapes_icon.set_has_popover_indicator_visible(not shapes_outline_selected);

        _list_view.get_selection_model()->select(_id_to_listview_positions.at(id));
    }

    void Toolbox::on_active_tool_changed()
    {
        select(active_state->get_active_tool());
    }

    Toolbox::~Toolbox()
    {
        for (auto& pair : _id_to_icons)
            delete pair.second;
    }
}