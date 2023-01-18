namespace mousetrap
{
    inline std::string detail::gesture_name_to_xml_code(ShortcutGestureName gesture)
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

    inline std::string detail::sanitize_string(const std::string& accelerator)
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

    inline AcceleratorDisplay::AcceleratorDisplay(const std::string& accelerator, const std::string& description)
            : WidgetImplementation<GtkShortcutsShortcut>([&]() -> GtkShortcutsShortcut*
             {
                 // assemble xml string and use builder because for some reason there's no gtk_shortcuts_shortcut_new

                 std::string description_parsed;
                 for (auto& c : description)
                     if (c != '\n')
                         description_parsed.push_back(c);

                 std::stringstream str;
                 str << "<interface>\n"
                     << "  <object class=\"GtkShortcutsShortcut\" id=\"object\">\n"
                     << "    <property name=\"accelerator\">" << detail::sanitize_string(accelerator) << "</property>\n"
                     << "    <property name=\"title\">" << description_parsed << "</property>\n"
                     << "  </object>\n"
                     << "</interface>" << std::endl;

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

    inline AcceleratorDisplay::AcceleratorDisplay(ShortcutGestureName gesture, const std::string& description)
            : WidgetImplementation<GtkShortcutsShortcut>([&]() -> GtkShortcutsShortcut*
             {
                 std::string description_parsed;
                 for (auto& c : description)
                     if (c != '\n')
                         description_parsed.push_back(c);

                 std::stringstream str;
                 str << "<interface>\n"
                     << "  <object class=\"GtkShortcutsShortcut\" id=\"object\">\n"
                     << "    <property name=\"shortcut-type\">" << detail::gesture_name_to_xml_code(gesture) << "</property>\n"
                     << "    <property name=\"title\">" << description << "</property>\n"
                     << "  </object>\n"
                     << "</interface>" << std::endl;

                 if (builder == nullptr)
                     builder = gtk_builder_new();

                 GError* error = nullptr;
                 gtk_builder_add_from_string(builder, str.str().c_str(), str.str().size(), &error);

                 if (error != nullptr)
                 {
                     std::cerr << "[ERROR] In ShortcutView::ShortcutView(" << detail::gesture_name_to_xml_code(gesture)  << "): "
                               << error->message << std::endl;
                 }

                 return GTK_SHORTCUTS_SHORTCUT(gtk_builder_get_object(builder, "object"));
             }())
    {}

    inline ShortcutGroupDisplay::ShortcutGroupDisplay(const std::string& title, const std::vector<AcceleratorAndDescription>& pairs)
            : WidgetImplementation<GtkShortcutsSection>([&]() -> GtkShortcutsSection* {

        std::stringstream str;

        str << "<interface>\n"
            << "<object class=\"GtkShortcutsSection\" id=\"object\">\n"
            << "<child>"
            << "<object class=\"GtkShortcutsGroup\">\n";

        if (not title.empty())
            str << "<property name=\"title\">" << title << "</property>\n";

        for (auto& pair : pairs)
        {
            if (pair.first == "never")
                continue;

            std::string description_parsed;
            for (auto& c : pair.second)
                if (c != '\n')
                    description_parsed.push_back(c);

            str << "<child>\n"
                << "  <object class=\"GtkShortcutsShortcut\">\n"
                << "    <property name=\"title\" translatable=\"yes\">" << detail::sanitize_string(description_parsed) << "</property>\n"
                << "    <property name=\"accelerator\">" << detail::sanitize_string(pair.first) << "</property>\n"
                << "  </object>\n"
                << "</child>\n";
        }

        str << "</object>\n"
            << "</child>\n"
            << "</object>\n"
            << "</interface>\n";

        if (builder == nullptr)
            builder = gtk_builder_new();

        GError* error = nullptr;
        gtk_builder_add_from_string(builder, str.str().c_str(), str.str().size(), &error);

        if (error != nullptr)
        {
            std::cerr << "[ERROR] In ShortcutGroupDisplay::ShortcutGroupDisplay: " << error->message << std::endl;
            std::cerr << str.str() << std::endl;
        }

        return GTK_SHORTCUTS_SECTION(gtk_builder_get_object(builder, "object"));
    }())
    {}
}