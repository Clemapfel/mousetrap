//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/16/23
//

namespace mousetrap
{
    LinkButton::LinkButton(const std::string& uri, const std::string& label)
            : WidgetImplementation<GtkLinkButton>(GTK_LINK_BUTTON(gtk_link_button_new_with_label(uri.c_str(), label.c_str())))
    {}

    void LinkButton::set_uri(const std::string& uri)
    {
        gtk_link_button_set_uri(get_native(), uri.c_str());
    }

    const std::string& LinkButton::get_uri() const
    {
        return gtk_link_button_get_uri(get_native());
    }

    bool LinkButton::get_visited() const
    {
        return gtk_link_button_get_visited(get_native());
    }

    void LinkButton::set_visited(bool b)
    {
        gtk_link_button_set_visited(get_native(), b);
    }
}
