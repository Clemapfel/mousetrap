// 
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    gboolean DetachableBox::on_window_close(GtkWindow*, DetachableBox* instance)
    {
        instance->attach();
        return false;
    }

    DetachableBox::DetachableBox(const std::string& window_title)
    {
        _titlebar_title = GTK_LABEL(gtk_label_new(window_title.c_str()));
        gtk_label_set_ellipsize(_titlebar_title, PANGO_ELLIPSIZE_START);

        _titlebar = GTK_HEADER_BAR(gtk_header_bar_new());
        gtk_header_bar_set_decoration_layout(_titlebar, "title:close");
        gtk_header_bar_set_title_widget(_titlebar, GTK_WIDGET(_titlebar_title));

        _window = GTK_WINDOW(gtk_window_new());
        g_object_ref(_window);

        gtk_window_set_titlebar(_window, GTK_WIDGET(_titlebar));
        gtk_window_set_decorated(_window, true);
        gtk_widget_set_visible(GTK_WIDGET(_window), false);

        g_signal_connect(_window, "close-request", G_CALLBACK(on_window_close), this);

        _anchor = GTK_REVEALER(gtk_revealer_new());
        gtk_revealer_set_transition_type(_anchor, GTK_REVEALER_TRANSITION_TYPE_CROSSFADE);

        _window_revealer = GTK_REVEALER(gtk_revealer_new());
        gtk_revealer_set_transition_type(_window_revealer, GTK_REVEALER_TRANSITION_TYPE_CROSSFADE);
        gtk_window_set_child(_window, GTK_WIDGET(_window_revealer));
    }

    DetachableBox::~DetachableBox()
    {
        if (_child != nullptr)
            g_object_unref(_child);

        g_object_unref(_window);
    }

    DetachableBox::operator GtkWidget*()
    {
        return GTK_WIDGET(_anchor);
    }

    void DetachableBox::set_child(Widget* child)
    {
        if (_child != nullptr)
            g_object_unref(_child);

        _child = child->operator GtkWidget*();
        _child = g_object_ref(_child);

        if (_attached)
            attach();
        else
            detach();
    }

    void DetachableBox::attach()
    {
        _attached = true;

        gtk_revealer_set_reveal_child(_window_revealer, false);
        gtk_revealer_set_child(_window_revealer, nullptr);
        gtk_widget_set_visible(GTK_WIDGET(_window), false);

        gtk_revealer_set_child(_anchor, _child);
        gtk_revealer_set_reveal_child(_anchor, true);

        if (_on_attach != nullptr)
            _on_attach(_on_attach_data);
    }

    void DetachableBox::detach()
    {
        _attached = false;

        gtk_revealer_set_reveal_child(_anchor, false);
        gtk_revealer_set_child(_anchor, nullptr);

        gtk_widget_set_visible(GTK_WIDGET(_window), true);
        gtk_window_present(_window);
        gtk_revealer_set_child(_window_revealer, GTK_WIDGET(_child));
        gtk_revealer_set_reveal_child(_window_revealer, true);

        if (_on_detach != nullptr)
            _on_detach(_on_detach_data);
    }

    bool DetachableBox::get_child_attached()
    {
        return _attached;
    }

    void DetachableBox::set_child_attached(bool b)
    {
        if (_attached and not b)
            detach();
        else if (not _attached and b)
            attach();
    }

    void DetachableBox::connect_attach(OnAttachSignature f, void* data)
    {
        _on_attach = f;
        _on_attach_data = data;
    }

    void DetachableBox::connect_detach(OnDetachSignature f, void* data)
    {
        _on_detach = f;
        _on_detach_data = data;
    }
}