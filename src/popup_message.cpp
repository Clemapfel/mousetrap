//
// Created by clem on 8/26/23.
//

#include <mousetrap/popup_message.hpp>

namespace mousetrap
{
    // OVERLAY
    
    PopupMessageOverlay::PopupMessageOverlay(const std::string& str)
        : Widget(gtk_label_new(str.c_str())),
          CTOR_SIGNAL(PopupMessageOverlay, realize),
          CTOR_SIGNAL(PopupMessageOverlay, unrealize),
          CTOR_SIGNAL(PopupMessageOverlay, destroy),
          CTOR_SIGNAL(PopupMessageOverlay, hide),
          CTOR_SIGNAL(PopupMessageOverlay, show),
          CTOR_SIGNAL(PopupMessageOverlay, map),
          CTOR_SIGNAL(PopupMessageOverlay, unmap)
    {
        _internal = ADW_TOAST_OVERLAY(Widget::operator NativeWidget());
        g_object_ref_sink(_internal);
    }

    PopupMessageOverlay::PopupMessageOverlay(detail::PopupMessageOverlayInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(PopupMessageOverlay, realize),
          CTOR_SIGNAL(PopupMessageOverlay, unrealize),
          CTOR_SIGNAL(PopupMessageOverlay, destroy),
          CTOR_SIGNAL(PopupMessageOverlay, hide),
          CTOR_SIGNAL(PopupMessageOverlay, show),
          CTOR_SIGNAL(PopupMessageOverlay, map),
          CTOR_SIGNAL(PopupMessageOverlay, unmap)
    {
        _internal = g_object_ref(internal);
    }

    PopupMessageOverlay::~PopupMessageOverlay()
    {
        g_object_unref(_internal);
    }

    NativeObject PopupMessageOverlay::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void PopupMessageOverlay::set_child(const Widget& widget) 
    {
        adw_toast_overlay_set_child(_internal, widget.operator NativeWidget());
    }

    void PopupMessageOverlay::show_message(const PopupMessage& message) 
    {
        adw_toast_overlay_add_toast(_internal, ADW_TOAST(message.operator NativeObject()));
    }
    
    // MESSAGE

    PopupMessage::PopupMessage(const std::string& title, const std::string& button_label)
        : _internal(adw_toast_new(title.c_str())),
          CTOR_SIGNAL(PopupMessage, dismissed),
          CTOR_SIGNAL(PopupMessage, button_clicked)
    {
        g_object_ref(_internal);
        if (not button_label.empty())
            set_button_label(button_label);
    }

    PopupMessage::PopupMessage(detail::PopupMessageInternal* internal)
        : _internal(internal),
        CTOR_SIGNAL(PopupMessage, dismissed),
        CTOR_SIGNAL(PopupMessage, button_clicked)
    {
        _internal = g_object_ref(internal);
    }

    PopupMessage::~PopupMessage()
    {
        g_object_unref(_internal);
    }

    NativeObject PopupMessage::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void PopupMessage::set_title(const std::string& title)
    {
        adw_toast_set_title(_internal, title.c_str());
    }

    std::string PopupMessage::get_title() const
    {
        auto* title = adw_toast_get_title(_internal);
        return title == nullptr ? "" : title;
    }

    void PopupMessage::set_button_label(const std::string& label)
    {
        adw_toast_set_button_label(_internal, label.c_str());
    }

    std::string PopupMessage::get_button_label() const
    {
        auto* label = adw_toast_get_button_label(_internal);
        return label == nullptr ? "" : label;
    }
}