//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/24/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_signals.hpp>
#include <app/app_component.hpp>
#include <app/tools.hpp>
#include <app/tooltip.hpp>

#include <deque>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_neighborhood_select);
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_rectangle_replace);
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_rectangle_add);
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_rectangle_subtract);
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_circle_replace);
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_circle_add);
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_circle_subtract);
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_polygon_replace);
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_polygon_add);
        DECLARE_GLOBAL_ACTION(toolbox, select_marquee_polygon_subtract);

        DECLARE_GLOBAL_ACTION(toolbox, select_brush);
        DECLARE_GLOBAL_ACTION(toolbox, select_eraser);
        DECLARE_GLOBAL_ACTION(toolbox, select_eyedropper);
        DECLARE_GLOBAL_ACTION(toolbox, select_bucket_fill);
        DECLARE_GLOBAL_ACTION(toolbox, select_line);

        DECLARE_GLOBAL_ACTION(toolbox, select_rectangle_outline);
        DECLARE_GLOBAL_ACTION(toolbox, select_rectangle_fill);
        DECLARE_GLOBAL_ACTION(toolbox, select_circle_outline);
        DECLARE_GLOBAL_ACTION(toolbox, select_circle_fill);
        DECLARE_GLOBAL_ACTION(toolbox, select_polygon_outline);
        DECLARE_GLOBAL_ACTION(toolbox, select_polygon_fill);
        DECLARE_GLOBAL_ACTION(toolbox, select_gradient_dithered);
        DECLARE_GLOBAL_ACTION(toolbox, select_gradient_smooth);
    }

    class Toolbox : public AppComponent, public signals::ActiveToolChanged
    {
        public:
            Toolbox();
            ~Toolbox();
            operator Widget*() override;

        protected:
            void on_active_tool_changed() override;

        private:
            void select(ToolID);
            ToolID _currently_selected;

            class ToolIcon
            {
                public:
                    ToolIcon(const std::string& image_id, Toolbox*);
                    operator Widget*();

                    void set_selection_indicator_visible(bool);
                    void set_child_selection_indicator_visible(bool);
                    void set_has_popover_indicator_visible(bool);

                    template<typename Function_t>
                    void set_on_click(Function_t f) {
                        _on_click = f;
                    }

                private:
                    Toolbox* _owner;

                    ImageDisplay _has_popover_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "has_popover_indicator" + ".png");
                    ImageDisplay _child_selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "child_selected_horizontal" + ".png");
                    ImageDisplay _selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "selected_indicator" + ".png");

                    ImageDisplay _tool_icon;

                    Overlay _overlay;
                    AspectFrame _main = AspectFrame(1);

                    std::function<void()> _on_click;
                    ClickEventController _click_event_controller;
            };

            class ClickForPopover
            {
                public:
                    ClickForPopover();
                    operator Widget*();

                    Box& get_label_box();
                    Box& get_popover_box();

                private:
                    Box _label_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Popover _popover = Popover();
                    Box _popover_box = Box(GTK_ORIENTATION_HORIZONTAL);

                    ClickEventController _label_click_controller;
                    ClickEventController _popover_click_controller;
            };

            std::map<ToolID, ToolIcon*> _id_to_icons;
            std::map<ToolID, size_t> _id_to_listview_positions;

            ToolIcon _filled_shapes_icon = ToolIcon("shapes_fill", this);
            ClickForPopover _filled_shapes_popover;

            ToolIcon _outline_shapes_icon = ToolIcon("shapes_outline", this);
            ClickForPopover _outline_shapes_popover;

            ToolIcon _marquee_rectangle_icon = ToolIcon("marquee_rectangle", this);
            ClickForPopover _marquee_rectangle_popover;

            ToolIcon _marquee_circle_icon = ToolIcon("marquee_circle", this);
            ClickForPopover _marquee_circle_popover;

            ToolIcon _marquee_polygon_icon = ToolIcon("marquee_polygon", this);
            ClickForPopover _marquee_polygon_popover;

            ListView _list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);

            ScrolledWindow _scrolled_window;

            SeparatorLine _main_spacer_left;
            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
            SeparatorLine _main_spacer_right;

            std::deque<Tooltip> _tooltips;
    };

    namespace state
    {
        inline Toolbox* toolbox = nullptr;
    }
}
