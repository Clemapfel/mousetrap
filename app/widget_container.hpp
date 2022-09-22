// 
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/box.hpp>
#include <include/label.hpp>
#include <include/revealer.hpp>
#include <include/scrolled_window.hpp>
#include <include/detachable_box.hpp>
#include <include/expander.hpp>
#include <include/separator_line.hpp>
#include <include/button.hpp>

#include <app/app_component.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
    class WidgetContainer : public AppComponent
    {
        public:
            WidgetContainer(const std::string& title);
            operator Widget*();
            void update();

            void set_child(AppComponent*);

        private:
            Box _header_bar_hbox = Box(GTK_ORIENTATION_HORIZONTAL);
            SeparatorLine _header_bar_hseparator = SeparatorLine(GTK_ORIENTATION_HORIZONTAL);

            Label _title_label;
            Label _detach_label;
            Button _detach_button;

            Expander _expander;
            DetachableBox _child_box;

            SeparatorLine _child_separator_top = SeparatorLine(GTK_ORIENTATION_VERTICAL);
            SeparatorLine _child_separator_bottom = SeparatorLine(GTK_ORIENTATION_VERTICAL);

            AppComponent* _child = nullptr;
            Box _child_vbox = Box(GTK_ORIENTATION_VERTICAL);
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            static void on_detach_triggered(Button*, WidgetContainer*);
            static void on_attach(DetachableBox*, WidgetContainer*);
            static void on_expander_activate(Expander*, WidgetContainer* data);
    };
}

// ###

namespace mousetrap
{
    void WidgetContainer::on_detach_triggered(Button*, WidgetContainer* instance)
    {
        if (instance->_expander.get_expanded() == true)
            instance->_expander.set_expanded(false);

        instance->_detach_label.set_opacity(0.3);
        instance->_child_box.set_child_attached(not instance->_child_box.get_child_attached());

        if (instance->_child != nullptr)
            instance->_child->operator Widget*()->set_visible(true);

        instance->_main.set_visible(false);
    }

    void WidgetContainer::on_attach(DetachableBox*, WidgetContainer* instance)
    {
        if (instance->_expander.get_expanded() == false)
            instance->_expander.set_expanded(true);

        instance->_detach_label.set_opacity(1);

        if (instance->_child != nullptr)
            instance->_child->operator Widget*()->set_visible(true);

        instance->_main.set_visible(true);
    }

    void WidgetContainer::on_expander_activate(Expander* expander, WidgetContainer* instance)
    {
        if (not instance->_expander.get_expanded() and not instance->_child_box.get_child_attached())
        {
            instance->_child_box.attach();
        }
    }

    WidgetContainer::WidgetContainer(const std::string& title)
        : _title_label(" <b><span size=\"80%\">" + title + "</span></b>"),
          _detach_label("<span size=\"80%\">" + std::string("&#128470;") + "</span>"),
          _child_box(title),
          _expander(title)
    {
        _title_label.set_use_markup(true);
        _title_label.set_margin_end(0.5 * state::margin_unit);
        _title_label.set_hexpand(true);

        auto* scrolled_window = new ScrolledWindow();
        scrolled_window->set_child(&_title_label);
        scrolled_window->set_size_request({0, 0});
        gtk_scrolled_window_set_min_content_width(scrolled_window->operator GtkScrolledWindow*(), 0);

        _detach_label.set_use_markup(true);
        _header_bar_hseparator.set_size_request({2, 0});

        _detach_button.set_child(&_detach_label);
        _detach_button.set_tooltip_text("Detach / Attach");
        _detach_button.set_halign(GTK_ALIGN_END);
        _detach_button.set_has_frame(false);

        _header_bar_hbox.push_back(scrolled_window);
        _header_bar_hbox.push_back(&_header_bar_hseparator);
        _header_bar_hbox.push_back(&_detach_button);

        _child_separator_top.set_size_request({0, 2});
        _child_separator_bottom.set_size_request({0, 2});

        _child_box.set_hexpand(true);
        //_child_box->set_margin_top(state::margin_unit);

        _child_vbox.push_back(&_child_separator_top);
        _child_vbox.push_back(&_child_box);

        _expander.set_resize_toplevel(true);
        _expander.set_label_widget(&_header_bar_hbox);
        _expander.set_child(&_child_vbox);

        _main.push_back(&_expander);
        _main.push_back(&_child_separator_bottom);

        _expander.connect_signal("activate", on_expander_activate, this);
        _detach_button.connect_signal("clicked", on_detach_triggered, this);
        _child_box.connect_signal_attach(on_attach, this);
    }

    WidgetContainer::operator Widget*()
    {
        return &_main;
    }

    void WidgetContainer::set_child(AppComponent* child)
    {
        _child_box.set_child(child->operator Widget*());
        _child = child;
    }

    void WidgetContainer::update()
    {
        _child->update();
    }
}