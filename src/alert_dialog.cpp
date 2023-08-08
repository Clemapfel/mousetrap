//
// Created by clem on 6/26/23.
//

#include <mousetrap/alert_dialog.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap::detail
{
    DECLARE_NEW_TYPE(AlertDialogInternal, alert_dialog_internal, ALERT_DIALOG_INTERNAL)
    DEFINE_NEW_TYPE_TRIVIAL_INIT(AlertDialogInternal,alert_dialog_internal, ALERT_DIALOG_INTERNAL)

    static void alert_dialog_internal_finalize(GObject* object)
    {
        auto* self = MOUSETRAP_ALERT_DIALOG_INTERNAL(object);

        delete self->button_labels;
        delete self->on_selection;

        G_OBJECT_CLASS(alert_dialog_internal_parent_class)->finalize(object);
    }

    DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(AlertDialogInternal,alert_dialog_internal, ALERT_DIALOG_INTERNAL)

    static AlertDialogInternal* alert_dialog_internal_new()
    {
        auto* self = (AlertDialogInternal*) g_object_new(alert_dialog_internal_get_type(), nullptr);
        alert_dialog_internal_init(self);

        auto* window = adw_window_new();
        gtk_window_set_hide_on_close(GTK_WINDOW(window), false);
        self->native = ADW_MESSAGE_DIALOG(adw_message_dialog_new(GTK_WINDOW(window), "", ""));

        adw_message_dialog_set_heading_use_markup(self->native, true);
        adw_message_dialog_set_body_use_markup(self->native, true);
        adw_message_dialog_set_close_response(self->native, "-1");

        gtk_window_set_modal(GTK_WINDOW(self->native), true);

        self->button_labels = new std::vector<std::string>();
        self->on_selection = nullptr;

        detail::attach_ref_to(G_OBJECT(self->native), self);
        return self;
    }
}

namespace mousetrap
{
    void AlertDialog::update_buttons()
    {
        for (size_t i = 0; i < _internal->button_labels->size(); ++i)
            adw_message_dialog_add_response(_internal->native, std::to_string(i).c_str(), _internal->button_labels->at(i).c_str());
    }

    AlertDialog::AlertDialog(const std::vector<std::string>& button_labels, const std::string& message, const std::string& detailed_message)
        : _internal(detail::alert_dialog_internal_new())
    {
        adw_message_dialog_set_heading(_internal->native, message.c_str());
        adw_message_dialog_set_body(_internal->native, detailed_message.c_str());
        g_signal_connect(_internal->native, "response", G_CALLBACK(on_resonse), _internal);

        *_internal->button_labels = button_labels;
        update_buttons();
    }

    AlertDialog::AlertDialog(detail::AlertDialogInternal* internal)
        : _internal(internal)
    {
        g_object_ref(_internal);
    }

    AlertDialog::~AlertDialog()
    {
        g_object_unref(_internal);
    }

    NativeObject AlertDialog::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void AlertDialog::set_button_label(size_t index, const std::string& label)
    {
        if (index >= _internal->button_labels->size()) {
            log::critical("In AlertDialog::set_button_label: Button ID " + std::to_string(index) + " is out of range for an AlertDialog with " + std::to_string(_internal->button_labels->size()) + " buttons.");
            return;
        }

        _internal->button_labels->at(index) = label;
        update_buttons();
    }

    std::string AlertDialog::get_button_label(int index) const
    {
        if (index >= _internal->button_labels->size()) {
            log::critical("In AlertDialog::set_button_label: Button ID " + std::to_string(index) + " is out of range for an AlertDialog with " + std::to_string(_internal->button_labels->size()) + " buttons.");
            return "";
        }

        if (index < 0)
            return "";

        return _internal->button_labels->at(index);
    }

    size_t AlertDialog::get_n_buttons() const
    {
        return _internal->button_labels->size();
    }

    void AlertDialog::add_button(int index, const std::string& label)
    {
        if (index <= 0)
            _internal->button_labels->insert(_internal->button_labels->begin(), label);
        else if (index >= _internal->button_labels->size())
            _internal->button_labels->push_back(label);
        else
            _internal->button_labels->insert(_internal->button_labels->begin() + index, label);

        update_buttons();
    }

    void AlertDialog::remove_button(size_t index)
    {
        if (index >= _internal->button_labels->size())
        {
            log::critical("In AlertDialog::remove_button: Button ID " + std::to_string(index) + " is out of range for an AlertDialog with " + std::to_string(_internal->button_labels->size()) + " buttons");
            return;
        }

        _internal->button_labels->erase(_internal->button_labels->begin() + index);
        update_buttons();
    }

    std::string AlertDialog::get_message() const
    {
        auto* c_str = adw_message_dialog_get_heading(_internal->native);
        return (c_str == nullptr ? "" : std::string(c_str));
    }

    void AlertDialog::set_message(const std::string& message)
    {
        adw_message_dialog_set_heading(_internal->native, message.c_str());
    }

    std::string AlertDialog::get_detailed_description() const
    {
        auto* c_str = adw_message_dialog_get_body(_internal->native);
        return (c_str == nullptr ? "" : std::string(c_str));
    }

    void AlertDialog::set_detailed_description(const std::string& message)
    {
        adw_message_dialog_set_body(_internal->native, message.c_str());
    }

    void AlertDialog::set_is_modal(bool b)
    {
        gtk_window_set_modal(GTK_WINDOW(_internal->native), b);
    }

    bool AlertDialog::get_is_modal() const
    {
        return gtk_window_get_modal(GTK_WINDOW(_internal->native));
    }

    void AlertDialog::present() const
    {
        gtk_window_present(GTK_WINDOW(_internal->native));
    }

    void AlertDialog::on_choose_callback(GObject* source, GAsyncResult* result, void* data)
    {
        GError* error = nullptr;
        const char* id = adw_message_dialog_choose_finish(ADW_MESSAGE_DIALOG(source), result);

        auto* internal = detail::MOUSETRAP_ALERT_DIALOG_INTERNAL(data);
        auto temp = AlertDialog(internal);

        if (error != nullptr)
        {
            if (error->code == 2) // dismissed by user
            {
                if (internal->on_selection != nullptr)
                    (*internal->on_selection)(temp, -1);
            }
            else
                log::critical("In AlertDialog::on_selection: " + std::string(error->message));

            g_error_free(error);
            return;
        }

        if (internal->on_selection != nullptr)
            (*internal->on_selection)(temp, std::stoi(id));
    }

    void AlertDialog::on_resonse(AdwMessageDialog*, gchar* response, detail::AlertDialogInternal*)
    {
        std::cout << response << std::endl;
    }
}
