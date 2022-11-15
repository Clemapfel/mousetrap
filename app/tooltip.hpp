// 
// Copyright 2022 Clemens Cords
// Created on 11/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/shortcut_information.hpp>

namespace mousetrap
{
    class Tooltip
    {
        public:
            Tooltip();
            operator Widget*();

            void set_title(const std::string&);
            void set_description(const std::string&);
            void create_shortcut_information_from(const std::string& group, ConfigFile* file);

        private:
            Viewport _main;
            Box _box = Box(GTK_ORIENTATION_VERTICAL);

            void reformat();

            std::string _title;
            std::string _description;

            Label _title_label;
            Label _description_label;

            Frame _shortcut_frame;
            ShortcutInformation _shortcut_information;
    };
}

//

namespace mousetrap
{
    Tooltip::Tooltip()
    {
        _box.push_back(&_title_label);
        _box.push_back(&_description_label);

        _shortcut_frame.set_child(_shortcut_information);
        _box.push_back(&_shortcut_frame);

        _shortcut_information.set_title("Keybindings");

        _main.set_child(&_box);
        //_main.set_policy(GTK_POLICY_NEVER, GTK_POLICY_NEVER);
        //_main.set_propagate_natural_height(true);
        _main.set_hexpand(false);
    }

    Tooltip::operator Widget*()
    {
        return &_main;
    }

    void Tooltip::set_title(const std::string& title)
    {
        _title = title;
        reformat();
    }

    void Tooltip::set_description(const std::string& description)
    {
        _description = description;
        reformat();
    }

    void Tooltip::create_shortcut_information_from(const std::string& group, ConfigFile* file)
    {
        _shortcut_information.create_from_group(group, file);
        reformat();
    }

    void Tooltip::reformat()
    {
        _title_label.set_text("<b>" + _title + "</b>");
        _description_label.set_text(_description);
        _shortcut_frame.set_visible(_shortcut_information.get_n_shortcuts() != 0);
        _main.set_size_request(_shortcut_information.operator Widget*()->get_preferred_size().natural_size);

        for (auto* label : {&_title_label, &_description_label})
        {
            label->set_margin_top(2 * state::margin_unit);
            label->set_margin_horizontal(2 * state::margin_unit);
            label->set_halign(GTK_ALIGN_START);
            label->set_wrap_mode(LabelWrapMode::WORD_OR_CHAR);
            label->set_justify_mode(JustifyMode::LEFT);
        }
    }
}