//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/19/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    #define DECLARE_APP_SIGNAL_COMPONENT(camel_case_name, snake_case_name) \
    struct Has##camel_case_name##Signal                                    \
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
                virtual void on_##snake_case_name() = 0;                   \
                                                                           \
            private:                                                       \
                bool _blocked = false;                                     \
        };

    DECLARE_APP_SIGNAL_COMPONENT(BrushSelectionUpdated, brush_selection_updated)
    DECLARE_APP_SIGNAL_COMPONENT(LayerFrameSelectionUpdated, layer_frame_selection_updated)
    DECLARE_APP_SIGNAL_COMPONENT(ColorSelectionUpdated, color_selection_updated)
    DECLARE_APP_SIGNAL_COMPONENT(PaletteUpdated, palette_updated)
    DECLARE_APP_SIGNAL_COMPONENT(PaletteSortModeUpdated, palette_sort_mode_updated)
    DECLARE_APP_SIGNAL_COMPONENT(SelectionUpdated, selection_updated)
    DECLARE_APP_SIGNAL_COMPONENT(PlaybackActiveUpdated, playback_active_updated)
    DECLARE_APP_SIGNAL_COMPONENT(OnionSkinVisibilityUpdated, onionskin_visibility_updated)
    DECLARE_APP_SIGNAL_COMPONENT(OnionSkinLayerCountUpdated, onionskin_layer_count_updated)
    DECLARE_APP_SIGNAL_COMPONENT(ImageUpdate, image_updated)
};
