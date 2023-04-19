//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/19/23
//

#pragma once

#include <mousetrap.hpp>

using namespace mousetrap;

class SoundTest : public Widget
{
    private:
        SoundBuffer _buffer;
        Sound _playback;

        ToggleButton _play_button;
        Label _play_button_label = Label("&#9654;");

        Scale _pitch_scale = Scale(0, 2, 0.01, Orientation::VERTICAL);
        Label _pitch_label = Label("Pitch");
        Box _pitch_box = Box(Orientation::VERTICAL);

        Scale _volume_scale = Scale(0, 2, 0.01, Orientation::VERTICAL);
        Label _volume_label = Label("Volume");
        Box _volume_box = Box(Orientation::VERTICAL);

        CheckButton _looping_button;
        Label _looping_label = Label("Looping");
        Box _looping_box = Box(Orientation::HORIZONTAL);

        Box _main = Box(Orientation::HORIZONTAL);

    public:
        SoundTest(const std::string& file)
        {
            _buffer.create_from_file(file);
            _playback.create_from_buffer(_buffer);

            _pitch_box.push_back(_pitch_scale);
            _pitch_box.push_back(_pitch_label);
            _pitch_scale.connect_signal_value_changed([](Scale* scale, SoundTest* instance){
                instance->_playback.set_pitch(scale->get_value());
            }, this);

            _volume_box.push_back(_volume_scale);
            _volume_box.push_back(_volume_label);
            _volume_scale.connect_signal_value_changed([](Scale* scale, SoundTest* instance){
                instance->_playback.set_volume(scale->get_value());
            }, this);

            _looping_box.push_back(_looping_label);
            _looping_box.push_back(_looping_button);
            _looping_button.connect_signal_toggled([](CheckButton* button, SoundTest* instance){
                instance->_playback.set_should_loop(button->get_active());
            }, this);

            for (auto* scale : {
                &_pitch_scale,
                &_volume_scale
            })
            {
                scale->set_expand(true);
                scale->set_should_draw_value(true);
                scale->set_value(1);
            }

            _play_button.set_child(_play_button_label);
            _play_button.connect_signal_toggled([](ToggleButton* button, SoundTest* instance){
                if (button->get_active())
                    instance->_playback.play();
                else
                    instance->_playback.pause();
            }, this);

            for (auto* box : {
                &_looping_box,
                &_volume_box,
                &_pitch_box
            })
                box->set_margin(10);

            _main.push_back(_play_button);
            _main.push_back(_looping_box);
            _main.push_back(_volume_box);
            _main.push_back(_pitch_box);
        }

        operator NativeWidget() const override
        {
            return _main;
        }
};