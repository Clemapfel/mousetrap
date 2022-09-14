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
            Box* _header_bar_box;
            Label* _title_label;

            SeparatorLine* _separator;

            Label* _detach_label;
            Button* _detach_button;

            Expander* _expander;
            DetachableBox* _child_box;

            Widget* _child = nullptr;

            Box* _main;

            static void on_reveal_triggered(GtkExpander*, void* data);
            static void on_detach_triggered(GtkButton*, void* data);
    };
}

// ###

namespace mousetrap
{
    void WidgetContainer::on_reveal_triggered(GtkExpander* expander, void* data)
    {
        bool state = gtk_expander_get_expanded(expander);
        auto* instance = (WidgetContainer*) data;

        if (not instance->_child_box->get_child_attached() and state == false)
        {
            instance->_child_box->attach();
            instance->_expander->set_expanded(true);
        }
        else
            instance->_expander->set_expanded(state);
    }

    void WidgetContainer::on_detach_triggered(GtkButton*, void* data)
    {
        auto* instance = (WidgetContainer*) data;

        if (not instance->_expander->get_expanded())
            instance->_expander->set_expanded(false);

        if (instance->_child_box->get_child_attached())
        {
            instance->_child_box->detach();
        }
        else
        {
            instance->_child_box->attach();
        }
    }

    WidgetContainer::WidgetContainer(const std::string& title)
    {
        _title_label = new Label(title);
        _title_label->set_use_markup(true);
        _title_label->set_margin_end(0.5 * state::margin_unit);
        
        _detach_label = new Label("&#128470;");
        _detach_label->set_use_markup(true);

        _separator = new SeparatorLine();
        
        _detach_button = new Button;
        _detach_button->set_child(_detach_label);
        _detach_button->connect_signal("clicked", on_detach_triggered, this);
        _detach_button->set_tooltip_text("Detach / Attach");
        _detach_button->set_halign(GTK_ALIGN_END);

        _header_bar_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _header_bar_box->push_back(_title_label);
        _header_bar_box->push_back(_separator);
        _header_bar_box->push_back(_detach_button);

        _expander = new Expander(title);
        _expander->set_label_widget(_header_bar_box);

        _child_box = new DetachableBox(title);
        _child_box->set_halign(GTK_ALIGN_START);
        _expander->set_child(_child_box);
        _expander->connect_signal("activate", on_reveal_triggered, this);

        _main = new Box(GTK_ORIENTATION_VERTICAL);
        _main->push_back(_expander);

        auto* separator = new Button();
        separator->set_vexpand(true);
        _main->push_back(separator);
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