//
// Created by clem on 8/26/23.
//

#include <mousetrap/popup_message.hpp>

namespace mousetrap
{
    // OVERLAY
    
    PopupMessageOverlay::PopupMessageOverlay()
        : Widget(adw_toast_overlay_new()),
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

    void PopupMessageOverlay::remove_child()
    {
        adw_toast_overlay_set_child(_internal, nullptr);
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

        #if ADW_MINOR_VERSION >= 4
            adw_toast_set_use_markup(_internal, true);
        #endif

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

    PopupMessage::operator NativeObject() const
    {
        return get_internal();
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

    void PopupMessage::set_button_action(const Action& action)
    {
        adw_toast_set_action_name(_internal, (("app.") + action.get_id()).c_str());
    }

    std::string PopupMessage::get_button_action_id() const
    {
        auto* id = adw_toast_get_action_name(_internal);
        if (id == nullptr)
            return "";

        auto as_string = std::string(id);
        return std::string(as_string.begin() + 4, as_string.end());
    }

    void PopupMessage::set_is_high_priority(bool b)
    {
        if (b)
            adw_toast_set_priority(_internal, ADW_TOAST_PRIORITY_HIGH);
        else
            adw_toast_set_priority(_internal, ADW_TOAST_PRIORITY_NORMAL);
    }

    bool PopupMessage::get_is_high_priority() const
    {
        return adw_toast_get_priority(_internal) == ADW_TOAST_PRIORITY_HIGH;
    }

    void PopupMessage::set_timeout(Time duration)
    {
        adw_toast_set_timeout(_internal, duration.as_microseconds());
    }

    Time PopupMessage::get_timeout() const
    {
        return microseconds(adw_toast_get_timeout(_internal));
    }
}