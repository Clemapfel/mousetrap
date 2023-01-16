// 
// Copyright 2022 Clemens Cords
// Created on 11/13/22 by clem (mail@clemens-cords.com)
//

#pragma once
#include <include/widget.hpp>

namespace mousetrap
{
    enum ShortcutGestureName
    {
        GESTURE_SWIPE_LEFT = GTK_SHORTCUT_GESTURE_SWIPE_LEFT,
        GESTURE_SWIPE_RIGHT = GTK_SHORTCUT_GESTURE_SWIPE_RIGHT,
        GESTURE_TWO_FINGER_SWIPE_RIGHT = GTK_SHORTCUT_GESTURE_TWO_FINGER_SWIPE_RIGHT,
        GESTURE_TWO_FINGER_SWIPE_LEFT = GTK_SHORTCUT_GESTURE_TWO_FINGER_SWIPE_LEFT,
        GESTURE_STRETCH = GTK_SHORTCUT_GESTURE_STRETCH,
        GESTURE_PINCH = GTK_SHORTCUT_GESTURE_PINCH,
        GESTURE_ROTATE_CLOCKWISE = GTK_SHORTCUT_GESTURE_ROTATE_CLOCKWISE,
        GESTURE_ROTATE_COUNTERCLOCKWISE = GTK_SHORTCUT_GESTURE_ROTATE_COUNTERCLOCKWISE
    };

    namespace detail
    {
        std::string gesture_name_to_xml_code(ShortcutGestureName);
        std::string sanitize_string(const std::string& accelerator);
    }

    class AcceleratorDisplay : public WidgetImplementation<GtkShortcutsShortcut>
    {
        public:
            AcceleratorDisplay(const std::string& accelerator, const std::string& description);
            AcceleratorDisplay(ShortcutGestureName, const std::string& description);

        private:
            static inline GtkBuilder* builder = nullptr;
    };

    class ShortcutGroupDisplay : public WidgetImplementation<GtkShortcutsSection>
    {
        public:
            using AcceleratorAndDescription = std::pair<std::string, std::string>;
            ShortcutGroupDisplay(const std::string& title, const std::vector<AcceleratorAndDescription>&);

        private:
            static inline GtkBuilder* builder = nullptr;
    };
}

#include <src/shortcut_viewer.inl>
