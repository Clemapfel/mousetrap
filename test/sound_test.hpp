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

        Scale _pitch_scale = Scale(0, 5, 0.01, Orientation::VERTICAL);
        Label _pitch_label = Label("Pitch");
        Box _pitch_box = Box(Orientation::VERTICAL);

        Scale _volume_scale = Scale(0, 5, 0.01, Orientation::VERTICAL);
        Label _volume_label = Label("Volume");
        Box _volume_box = Box(Orientation::VERTICAL);

        CheckButton _looping_button;
        Label _looping_label = Label("Looping");
        Box _looping_box = Box(Orientation::HORIZONTAL);

        Box _main = Box(Orientation::VERTICAL);

    public:
        SoundTest(const std::string& file)
        {
            _buffer.create_from_file(file);

            _pitch_box.push_back(_pitch_scale);
            _pitch_box.push_back(_pitch_label);

            _volume_box.push_back(_volume_scale);
            _volume_box.push_back(_volume_label);

            _looping_box.push_back(_looping_label);
            _looping_box.push_back(_looping_button);

            _play_button.set_child(_play_button_label);
        }

        operator NativeWidget() const override
        {
            return _main;
        }
};