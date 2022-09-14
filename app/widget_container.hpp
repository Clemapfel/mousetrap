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

namespace mousetrap
{
    class WidgetContainer : public Widget
    {
        public:
            WidgetContainer(const std::string& title);
            operator GtkWidget*() override;
            void update();

            void set_child(Widget*);

        private:
            Box* _header_bar_hbox;
            SeparatorLine* _header_bar_hseparator;

            Label* _title_label;
            Label* _detach_label;
            Button* _detach_button;

            Expander* _expander;
            DetachableBox* _child_box;

            SeparatorLine* _child_separator_top;
            SeparatorLine* _child_separator_bottom;

            Widget* _child = nullptr;
            Box* _child_vbox;

            Box* _main;

            static void on_detach_triggered(GtkButton*, void* data);
            static void on_attach(void*);
            static void on_expander_activate(GtkExpander*, void* data);
    };
}

// ###

namespace mousetrap
{
    void WidgetContainer::on_detach_triggered(GtkButton*, void* data)
    {
        auto* instance = (WidgetContainer*) data;

        if (instance->_expander->get_expanded() == true)
            instance->_expander->set_expanded(false);

        instance->_detach_label->set_opacity(0.3);
        instance->_child_box->set_child_attached(not instance->_child_box->get_child_attached());

        if (instance->_child != nullptr)
            instance->_child->set_visible(true);

        instance->_main->set_visible(false);
    }

    void WidgetContainer::on_attach(void* data)
    {
        auto* instance = (WidgetContainer*) data;

        if (instance->_expander->get_expanded() == false)
            instance->_expander->set_expanded(true);

        instance->_detach_label->set_opacity(1);

        if (instance->_child != nullptr)
            instance->_child->set_visible(true);

        instance->_main->set_visible(true);
    }

    void WidgetContainer::on_expander_activate(GtkExpander* expander, void* data)
    {
        auto* instance = (WidgetContainer*) data;

        if (not instance->_expander->get_expanded() and not instance->_child_box->get_child_attached())
        {
            instance->_child_box->attach();
        }

        //instance->_child_box->set_visible(not instance->_expander->get_expanded());
    }

    WidgetContainer::WidgetContainer(const std::string& title)
    {
        std::string size_string = "80%";

        _title_label = new Label(" <b><span size=\"" + size_string + "\">" + title + "</span></b>");
        _title_label->set_use_markup(true);
        _title_label->set_margin_end(0.5 * state::margin_unit);
        _title_label->set_hexpand(true);

        _detach_label = new Label("<span size=\"" + size_string + "\">" + "&#128470;" + "</span>");
        _detach_label->set_use_markup(true);

        _header_bar_hseparator = new SeparatorLine(GTK_ORIENTATION_HORIZONTAL);
        _header_bar_hseparator->set_size_request({2, 0});

        _detach_button = new Button();
        _detach_button->set_child(_detach_label);
        _detach_button->set_tooltip_text("Detach / Attach");
        _detach_button->set_halign(GTK_ALIGN_END);
        _detach_button->set_has_frame(false);

        _header_bar_hbox = new Box(GTK_ORIENTATION_HORIZONTAL);
        _header_bar_hbox->push_back(_title_label);
        _header_bar_hbox->push_back(_header_bar_hseparator);
        _header_bar_hbox->push_back(_detach_button);

        _child_separator_top = new SeparatorLine(GTK_ORIENTATION_VERTICAL);
        _child_separator_bottom = new SeparatorLine(GTK_ORIENTATION_VERTICAL);

        _child_separator_top->set_size_request({0, 2});
        _child_separator_bottom->set_size_request({0, 2});

        _child_box = new DetachableBox(title);
        _child_box->set_hexpand(true);
        //_child_box->set_margin_top(state::margin_unit);

        _child_vbox = new Box(GTK_ORIENTATION_VERTICAL);
        _child_vbox->push_back(_child_separator_top);
        _child_vbox->push_back(_child_box);

        _expander = new Expander(title);
        _expander->set_resize_toplevel(true);
        _expander->set_label_widget(_header_bar_hbox);
        _expander->set_child(_child_vbox);

        _main = new Box(GTK_ORIENTATION_VERTICAL);
        _main->push_back(_expander);
        _main->push_back(_child_separator_bottom);

        _expander->connect_signal("activate", on_expander_activate, this);
        _detach_button->connect_signal("clicked", on_detach_triggered, this);
        _child_box->connect_attach(on_attach, this);
    }

    WidgetContainer::operator GtkWidget*()
    {
        return _main->operator GtkWidget*();
    }

    void WidgetContainer::set_child(Widget* child)
    {
        _child_box->set_child(child);
        _child = child;
    }

    void WidgetContainer::update()
    {
        if (_child != nullptr)
            _child->update();
    }
}