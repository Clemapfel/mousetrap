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

    std::string shortcut_gesture_name_to_xml_code(ShortcutGestureName);
    std::string accelerator_to_xml_code(const std::string& accelerator);

    class AcceleratorDisplay : public WidgetImplementation<GtkShortcutsShortcut>
    {
        public:
            AcceleratorDisplay(const std::string& accelerator);
            AcceleratorDisplay(ShortcutGestureName);

        private:
            static inline GtkBuilder* builder = nullptr;
    };

    class ShortcutGroupDisplay : public WidgetImplementation<GtkShortcutsGroup>
    {
        public:
            using AcceleratorAndDescription = std::pair<std::string, std::string>;
            ShortcutGroupDisplay(const std::string& title, const std::vector<AcceleratorAndDescription>&);

        private:
            static inline GtkBuilder* builder = nullptr;
    };
}

//

namespace mousetrap
{
    std::string shortcut_gesture_name_to_xml_code(ShortcutGestureName gesture)
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

        return shortcut_type;
    }

    std::string accelerator_to_xml_code(const std::string& accelerator)
    {
        std::stringstream out;
        for (auto& c : accelerator)
        {
            if (c == '<')
                out << "&lt;";
            else if (c == '>')
                out << "&gt;";
            else
                out << c;
        }

        return out.str();
    }

    AcceleratorDisplay::AcceleratorDisplay(const std::string& accelerator)
        : WidgetImplementation<GtkShortcutsShortcut>([&]() -> GtkShortcutsShortcut*
        {
            // assemble xml string and use builder because for some reason there's no gtk_shortcuts_shortcut_new

            std::stringstream str;
            str << "<interface>\n"
                << "<object class=\"GtkShortcutsShortcut\" id=\"object\">\n"
                << "<property name=\"accelerator\">" << accelerator_to_xml_code(accelerator) << "</property>\n"
                << "</object>\n"
                << "</interface>\n"
                << std::endl;

            if (builder == nullptr)
                builder = gtk_builder_new();

            GError* error = nullptr;
            gtk_builder_add_from_string(builder, str.str().c_str(), str.str().size(), &error);

            if (error != nullptr)
            {
                std::cerr << "[ERROR] In ShortcutView::ShortcutView(" << accelerator << "): "
                          << error->message << std::endl;
            }

            return GTK_SHORTCUTS_SHORTCUT(gtk_builder_get_object(builder, "object"));
        }())
    {}

    AcceleratorDisplay::AcceleratorDisplay(ShortcutGestureName gesture)
            : WidgetImplementation<GtkShortcutsShortcut>([&]() -> GtkShortcutsShortcut*
     {

         std::stringstream str;
         str << "<interface>\n"
             << "<object class=\"GtkShortcutsShortcut\" id=\"object\">\n"
             << "<property name=\"shortcut-type\">" << shortcut_gesture_name_to_xml_code(gesture) << "</property>\n"
             << "</object>\n"
             << "</interface>\n"
             << std::endl;

         if (builder == nullptr)
             builder = gtk_builder_new();

         GError* error = nullptr;
         gtk_builder_add_from_string(builder, str.str().c_str(), str.str().size(), &error);

         if (error != nullptr)
         {
             std::cerr << "[ERROR] In ShortcutView::ShortcutView(" << shortcut_gesture_name_to_xml_code(gesture)  << "): "
                       << error->message << std::endl;
         }

         return GTK_SHORTCUTS_SHORTCUT(gtk_builder_get_object(builder, "object"));
     }())
    {}

    ShortcutGroupDisplay::ShortcutGroupDisplay(const std::string& title, const std::vector<AcceleratorAndDescription>& pairs)
        : WidgetImplementation<GtkShortcutsGroup>([&]() -> GtkShortcutsGroup* {

            std::stringstream str;

            str << "<interface>\n"
                << "<object class=\"GtkShortcutsGroup\" id=\"object\">\n"
                << "<property name=\"title\" translatable=\"yes\">" << title << "</property>\n";

            for (auto& pair : pairs)
            {
                str << "<child>\n"
                    << "<object class=\"GtkShortcutsShortcut\">\n"
                    << "<property name=\"title\" translatable=\"yes\">" << pair.second << "</property>"
                    << "<property name=\"accelerator\">" << accelerator_to_xml_code(pair.first) << "</property>"
                    << "</object>\n"
                    << "</child>\n";
            }

            str << "</object>\n"
                << "</interface>\n";

            std::cout << str.str() << std::endl;

        if (builder == nullptr)
                builder = gtk_builder_new();

            GError* error = nullptr;
            gtk_builder_add_from_string(builder, str.str().c_str(), str.str().size(), &error);

            if (error != nullptr)
                std::cerr << "[ERROR] In ShortcutGroupDisplay::ShortcutGroupDisplay: " << error->message << std::endl;

            return GTK_SHORTCUTS_GROUP(gtk_builder_get_object(builder, "object"));
    }())
    {}
}