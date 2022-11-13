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

    class ShortcutView : public WidgetImplementation<GtkShortcutsShortcut>
    {
        public:
            ShortcutView(const std::string& accelerator, const std::string& title);
            ShortcutView(ShortcutGestureName, const std::string& title);

        private:
            static inline GtkBuilder* builder = nullptr;
    };
}

//

namespace mousetrap
{
    ShortcutView::ShortcutView(const std::string& accelerator, const std::string& title)
        : WidgetImplementation<GtkShortcutsShortcut>([&]() -> GtkShortcutsShortcut*
        {
            // assemble xml string and use builder because for some reason there's no gtk_shortcuts_shortcut_new

            std::stringstream str;
            str << "<interface>\n"
                << "<object class=\"GtkShortcutsShortcut\" id=\"object\">\n"
                << "<property name=\"accelerator\">";

                for (auto& c : accelerator)
                {
                    if (c == '<')
                        str << "&lt;";
                    else if (c == '>')
                        str << "&gt;";
                    else
                        str << c;
                }

            str << "</property>\n"
                << "<property name=\"title\" translatable=\"yes\">"
                << title << "</property>\n"
                << "</object>\n"
                << "</interface>\n"
                << std::endl;

            if (builder == nullptr)
                builder = gtk_builder_new();

            GError* error = nullptr;
            gtk_builder_add_from_string(builder, str.str().c_str(), str.str().size(), &error);

            if (error != nullptr)
            {
                std::cerr << "[ERROR] In ShortcutView::ShortcutView(" << accelerator << ", " << title << "): "
                          << error->message << std::endl;
            }

            return GTK_SHORTCUTS_SHORTCUT(gtk_builder_get_object(builder, "object"));
        }())
    {}

    ShortcutView::ShortcutView(ShortcutGestureName gesture, const std::string& title)
            : WidgetImplementation<GtkShortcutsShortcut>([&]() -> GtkShortcutsShortcut*
     {
         std::string shortcut_type;

         if (gesture == GESTURE_SWIPE_LEFT)
             shortcut_type = "gesture-swipe-left";
         else if (gesture == GESTURE_SWIPE_RIGHT)
             shortcut_type = "gesture-swipe-right";
         else if (gesture == GESTURE_TWO_FINGER_SWIPE_RIGHT)
             shortcut_type = "gesture-two-finger-swipe-right";
         else if (gesture == GESTURE_TWO_FINGER_SWIPE_LEFT)
             shortcut_type = "gesture-two-finger-swipe-left";
         else if (gesture == GESTURE_STRETCH)
             shortcut_type = "gesture-stretch";
         else if (gesture == GESTURE_PINCH)
             shortcut_type = "gesture-pinch";
         else if (gesture == GESTURE_ROTATE_CLOCKWISE)
             shortcut_type = "gesture-rotate-clockwise";
         else if (gesture == GESTURE_ROTATE_COUNTERCLOCKWISE)
             shortcut_type = "gesture-rotate-counterclockwise";

         std::stringstream str;
         str << "<interface>\n"
             << "<object class=\"GtkShortcutsShortcut\" id=\"object\">\n"
             << "<property name=\"shortcut-type\">" << shortcut_type << "</property>\n"
             << "<property name=\"title\" translatable=\"yes\">" << title << "</property>\n"
             << "</object>\n"
             << "</interface>\n"
             << std::endl;

         if (builder == nullptr)
             builder = gtk_builder_new();

         GError* error = nullptr;
         gtk_builder_add_from_string(builder, str.str().c_str(), str.str().size(), &error);

         if (error != nullptr)
         {
             std::cerr << "[ERROR] In ShortcutView::ShortcutView(" << shortcut_type << ", " << title << "): "
                       << error->message << std::endl;
         }

         return GTK_SHORTCUTS_SHORTCUT(gtk_builder_get_object(builder, "object"));
     }())
    {}
}