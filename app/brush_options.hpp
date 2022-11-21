// 
// Copyright 2022 Clemens Cords
// Created on 11/21/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/global_state.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class BrushOptions : public AppComponent
    {
        public:
            BrushOptions();
            void update();
            operator Widget*();

        private:
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            // opacity
            float _opacity = 1;

            Box _opacity_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _opacity_label = Label("Opacity");
            Scale _opacity_scale = Scale(0, 1, 1.f / state::color_quantization);
            SpinButton _opacity_spin_button = SpinButton(0, 1, 1.f / state::color_quantization);

            static void on_opacity_scale_value_changed(Scale*, BrushOptions*);
            static void on_opacity_spin_button_value_changed(SpinButton*, BrushOptions*);

            // size
            size_t _size = 1;

            Box _size_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _size_label = Label("Size");
            Scale _size_scale = Scale(0, 256, 1);
            SpinButton _size_spin_button = SpinButton(0, 4096, 1);

            static void on_size_scale_value_changed(Scale*, BrushOptions*);
            static void on_size_spin_button_value_changed(SpinButton*, BrushOptions*);


    };
}

//

namespace mousetrap
{
    BrushOptions::BrushOptions()
    {
        _opacity_label.set_hexpand(false);
        _opacity_scale.set_hexpand(true);
        _opacity_spin_button.set_hexpand(false);

        _opacity_label.set_halign(GTK_ALIGN_START);
        _opacity_box.push_back(&_opacity_scale);
        _opacity_box.push_back(&_opacity_spin_button);
        _opacity_scale.connect_signal_value_changed(on_opacity_scale_value_changed, this);
        _opacity_spin_button.connect_signal_value_changed(on_opacity_spin_button_value_changed, this);

        _size_label.set_hexpand(false);
        _size_scale.set_hexpand(true);
        _size_spin_button.set_hexpand(false);

        _size_label.set_halign(GTK_ALIGN_START);
        _size_box.push_back(&_size_scale);
        _size_box.push_back(&_size_spin_button);
        _size_scale.connect_signal_value_changed(on_size_scale_value_changed, this);
        _size_spin_button.connect_signal_value_changed(on_size_spin_button_value_changed, this);

        for (auto* box : {&_opacity_box, &_size_box})
            box->set_margin_start(state::margin_unit);

        _main.push_back(&_opacity_label);
        _main.push_back(&_opacity_box);
        _main.push_back(&_size_label);
        _main.push_back(&_size_box);
    }

    void BrushOptions::update()
    {}

    BrushOptions::operator Widget*()
    {
        return &_main;
    }

    void BrushOptions::on_opacity_scale_value_changed(Scale* scale, BrushOptions* instance)
    {
        instance->_opacity = scale->get_value();
        instance->_opacity_spin_button.set_signal_value_changed_blocked(true);
        instance->_opacity_spin_button.set_value(instance->_opacity);
        instance->_opacity_spin_button.set_signal_value_changed_blocked(false);
    }

    void BrushOptions::on_opacity_spin_button_value_changed(SpinButton* scale, BrushOptions* instance)
    {
        instance->_opacity = scale->get_value();
        instance->_opacity_scale.set_signal_value_changed_blocked(true);
        instance->_opacity_scale.set_value(instance->_opacity);
        instance->_opacity_scale.set_signal_value_changed_blocked(false);
    }

    void BrushOptions::on_size_scale_value_changed(Scale* scale, BrushOptions* instance)
    {
        instance->_size = scale->get_value();
        instance->_size_spin_button.set_signal_value_changed_blocked(true);
        instance->_size_spin_button.set_value(instance->_size);
        instance->_size_spin_button.set_signal_value_changed_blocked(false);
    }

    void BrushOptions::on_size_spin_button_value_changed(SpinButton* scale, BrushOptions* instance)
    {
        instance->_size = scale->get_value();
        instance->_size_scale.set_signal_value_changed_blocked(true);
        instance->_size_scale.set_value(instance->_size);
        instance->_size_scale.set_signal_value_changed_blocked(false);
    }
}