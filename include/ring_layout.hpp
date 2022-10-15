//
// Copyright 2022 Clemens Cords
// Created on 10/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/center_box.hpp>
#include <include/box.hpp>
#include <include/label.hpp>

namespace mousetrap
{
    class RingLayout : public WidgetImplementation<GtkBox>
    {
        public:
            RingLayout();

            void set_top_widget(Widget*);
            void set_top_right_widget(Widget*);
            void set_right_widget(Widget*);
            void set_bottom_right_widget(Widget*);
            void set_bottom_widget(Widget*);
            void set_bottom_left_widget(Widget*);
            void set_left_widget(Widget*);
            void set_top_left_widget(Widget*);
            void set_center_widget(Widget*);

            Widget* get_top_widget();
            Widget* get_top_right_widget();
            Widget* get_right_widget();
            Widget* get_bottom_right_widget();
            Widget* get_bottom_widget();
            Widget* get_bottom_left_widget();
            Widget* get_left_widget();
            Widget* get_top_left_widget();
            Widget* get_center_widget();

        private:
            CenterBox _left_center_right = CenterBox(GTK_ORIENTATION_HORIZONTAL);
            Box _topleft_top_topright = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _bottomleft_bottom_bottomright = Box(GTK_ORIENTATION_HORIZONTAL);

            Box _top = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _top_right = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _right = Box(GTK_ORIENTATION_VERTICAL);
            Box _bottom_right = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _bottom = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _bottom_left = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _left = Box(GTK_ORIENTATION_VERTICAL);
            Box _top_left = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _center = Box(GTK_ORIENTATION_HORIZONTAL);

            Widget* _top_widget = nullptr;
            Widget* _top_right_widget = nullptr;
            Widget* _right_widget = nullptr;
            Widget* _bottom_right_widget = nullptr;
            Widget* _bottom_widget = nullptr;
            Widget* _bottom_left_widget = nullptr;
            Widget* _left_widget = nullptr;
            Widget* _top_left_widget = nullptr;
            Widget* _center_widget = nullptr;

            void update_visibility();
    };
}

namespace mousetrap
{
    RingLayout::RingLayout()
        : WidgetImplementation<GtkBox>(GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)))
    {
        gtk_box_append(get_native(), _topleft_top_topright.operator GtkWidget*());
        gtk_box_append(get_native(), _left_center_right.operator GtkWidget*());
        gtk_box_append(get_native(), _bottomleft_bottom_bottomright.operator GtkWidget*());
        gtk_box_set_homogeneous(get_native(), false);

        _topleft_top_topright.push_back(&_top_left);
        _topleft_top_topright.push_back(&_top);
        _topleft_top_topright.push_back(&_top_right);

        _left_center_right.set_start_widget(&_left);
        _left_center_right.set_center_widget(&_center);
        _left_center_right.set_end_widget(&_right);

        _bottomleft_bottom_bottomright.push_back(&_bottom_left);
        _bottomleft_bottom_bottomright.push_back(&_bottom);
        _bottomleft_bottom_bottomright.push_back(&_bottom_right);

        for (auto* box : std::vector<Widget*>{
                &_left_center_right,
                &_topleft_top_topright,
                &_bottomleft_bottom_bottomright,
                &_top,
                &_top_right,
                &_right,
                &_bottom_right,
                &_bottom,
                &_bottom_left,
                &_left,
                &_top_left,
                &_center})
            box->set_expand(true);
    }

    void RingLayout::update_visibility()
    {
        if (_top_left_widget == nullptr and _top_widget == nullptr and _top_right_widget == nullptr)
            _topleft_top_topright.set_visible(false);
        else
            _topleft_top_topright.set_visible(true);

        if (_bottom_left_widget == nullptr and _bottom_widget == nullptr and _bottom_right_widget == nullptr)
            _bottomleft_bottom_bottomright.set_visible(false);
        else
            _bottomleft_bottom_bottomright.set_visible(true);
    }

    void RingLayout::set_top_widget(Widget* widget)
    {
        _top_widget = widget;
        _top.push_back(widget);
        update_visibility();
    }

    Widget* RingLayout::get_top_widget()
    {
        return _top_widget;
    }

    void RingLayout::set_top_right_widget(Widget* widget)
    {
        _top_right_widget = widget;
        _top_right.push_back(widget);
        update_visibility();
    }

    Widget* RingLayout::get_top_right_widget()
    {
        return _top_right_widget;
    }

    void RingLayout::set_right_widget(Widget* widget)
    {
        _right_widget = widget;
        _right.push_back(widget);
        update_visibility();
    }

    Widget* RingLayout::get_right_widget()
    {
        return _right_widget;
    }

    void RingLayout::set_bottom_right_widget(Widget* widget)
    {
        _bottom_right_widget = widget;
        _bottom_right.push_back(widget);
        update_visibility();
    }

    Widget* RingLayout::get_bottom_right_widget()
    {
        return _bottom_right_widget;
    }

    void RingLayout::set_bottom_widget(Widget* widget)
    {
        _bottom_widget = widget;
        _bottom.push_back(widget);
        update_visibility();
    }

    Widget* RingLayout::get_bottom_widget()
    {
        return _bottom_widget;
    }

    void RingLayout::set_bottom_left_widget(Widget* widget)
    {
        _bottom_left_widget = widget;
        _bottom_left.push_back(widget);
        update_visibility();
    }

    Widget* RingLayout::get_bottom_left_widget()
    {
        return _bottom_left_widget;
    }

    void RingLayout::set_left_widget(Widget* widget)
    {
        _left_widget = widget;
        _left.push_back(widget);
        update_visibility();
    }

    Widget* RingLayout::get_left_widget()
    {
        return _left_widget;
    }

    void RingLayout::set_top_left_widget(Widget* widget)
    {
        _top_left_widget = widget;
        _top_left.push_back(widget);
        update_visibility();
    }

    Widget* RingLayout::get_top_left_widget()
    {
        return _top_left_widget;
    }

    void RingLayout::set_center_widget(Widget* widget)
    {
        _center_widget = widget;
        _center.push_back(widget);
        update_visibility();
    }

    Widget* RingLayout::get_center_widget()
    {
        return _center_widget;
    }
}