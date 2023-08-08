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

        self->native = gtk_alert_dialog_new("");
        gtk_alert_dialog_set_modal(self->native, true);

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
        std::vector<const char*> c_str_labels;
        for (auto& label : *_internal->button_labels)
            c_str_labels.push_back(label.c_str());

        c_str_labels.push_back(nullptr);
        gtk_alert_dialog_set_buttons(_internal->native, c_str_labels.data());
    }

    AlertDialog::AlertDialog(const std::vector<std::string>& button_labels, const std::string& message, const std::string& detailed_message)
    : _internal(detail::alert_dialog_internal_new())
    {
        g_object_ref(_internal);
        gtk_alert_dialog_set_message(_internal->native, message.c_str());
        gtk_alert_dialog_set_detail(_internal->native, detailed_message.c_str());

        *_internal->button_labels = button_labels;
        update_buttons();
    }

    AlertDialog::AlertDialog(detail::AlertDialogInternal* internal)
    : _internal(internal)
    {
        g_object_ref(internal);
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
        auto* c_str = gtk_alert_dialog_get_message(_internal->native);
        return (c_str == nullptr ? "" : std::string(c_str));
    }

    void AlertDialog::set_message(const std::string& message)
    {
        gtk_alert_dialog_set_message(_internal->native, message.c_str());
    }

    std::string AlertDialog::get_detailed_description() const
    {
        auto* c_str = gtk_alert_dialog_get_detail(_internal->native);
        return (c_str == nullptr ? "" : std::string(c_str));
    }

    void AlertDialog::set_detailed_description(const std::string& message)
    {
        gtk_alert_dialog_set_detail(_internal->native, message.c_str());
    }

    void AlertDialog::set_is_modal(bool b)
    {
        gtk_alert_dialog_set_modal(_internal->native, b);
    }

    bool AlertDialog::get_is_modal() const
    {
        return gtk_alert_dialog_get_modal(_internal->native);
    }

    void AlertDialog::present() const
    {
        gtk_alert_dialog_choose(_internal->native, GTK_WINDOW(gtk_window_new()), nullptr, on_choose_callback, _internal);
    }

    void AlertDialog::on_choose_callback(GObject* source, GAsyncResult* result, void* data)
    {
        GError* error = nullptr;
        int id = gtk_alert_dialog_choose_finish(GTK_ALERT_DIALOG(source), result, &error);

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
            (*internal->on_selection)(temp, id);
    }
}
