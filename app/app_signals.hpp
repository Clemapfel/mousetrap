//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/19/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap::signals
{
    #define DECLARE_APP_SIGNAL_COMPONENT(camel_case_name, snake_case_name) \
    struct camel_case_name                                                 \
    {                                                                      \
            public:                                                        \
                void signal_##snake_case_name()                            \
                {                                                          \
                    if (!_blocked) {                                       \
                        on_##snake_case_name();                            \
                    }                                                      \
                }                                                          \
                                                                           \
                void set_##snake_case_name##_blocked(bool b)               \
                {                                                          \
                    _blocked = b;                                          \
                }                                                          \
                                                                           \
            protected:                                                     \
                virtual void on_##snake_case_name() {}                   \
                                                                           \
            private:                                                       \
                bool _blocked = false;                                     \
        };

    DECLARE_APP_SIGNAL_COMPONENT(BrushSelectionChanged, brush_selection_changed)
    DECLARE_APP_SIGNAL_COMPONENT(BrushSetUpdated, brush_set_updated)

    DECLARE_APP_SIGNAL_COMPONENT(ColorSelectionChanged, color_selection_changed)
    DECLARE_APP_SIGNAL_COMPONENT(PaletteUpdated, palette_updated)
    DECLARE_APP_SIGNAL_COMPONENT(PaletteSortModeChanged, palette_sort_mode_changed)
    DECLARE_APP_SIGNAL_COMPONENT(PaletteEditingToggled, palette_editing_toggled)
    DECLARE_APP_SIGNAL_COMPONENT(SelectionChanged, selection_changed)
    DECLARE_APP_SIGNAL_COMPONENT(PlaybackToggled, playback_toggled)
    DECLARE_APP_SIGNAL_COMPONENT(OnionSkinVisibilityToggled, onionskin_visibility_toggled)
    DECLARE_APP_SIGNAL_COMPONENT(OnionSkinLayerCountChanged, onionskin_layer_count_changed)
    DECLARE_APP_SIGNAL_COMPONENT(LayerFrameSelectionChanged, layer_frame_selection_changed)
    DECLARE_APP_SIGNAL_COMPONENT(LayerImageUpdated, layer_image_updated)
    DECLARE_APP_SIGNAL_COMPONENT(LayerCountChanged, layer_count_changed)
    DECLARE_APP_SIGNAL_COMPONENT(LayerPropertiesChanged, layer_properties_changed)
    DECLARE_APP_SIGNAL_COMPONENT(ActiveToolChanged, active_tool_changed)
};
