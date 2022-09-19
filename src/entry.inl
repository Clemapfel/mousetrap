// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    Entry::Entry()
        : WidgetImplementation<GtkEntry>(GTK_ENTRY(gtk_entry_new()))
    {}

    void Entry::set_has_frame(bool b)
    {
        gtk_entry_set_has_frame(get_native(), b);
    }

    std::string Entry::get_text() const
    {
        auto* buffer = gtk_entry_get_buffer(get_native());
        return gtk_entry_buffer_get_text(buffer);
    }

    void Entry::set_text(const std::string& text)
    {
        auto* buffer = gtk_entry_get_buffer(get_native());
        gtk_entry_buffer_set_text(buffer, text.c_str(), text.size());
        gtk_entry_buffer_emit_inserted_text(buffer, 0, text.c_str(), text.size());
        gtk_entry_set_buffer(get_native(), buffer);
    }

    void Entry::set_n_chars(size_t n)
    {
        gtk_entry_set_max_length(get_native(), n);
    }

    template<typename Function_t, typename T>
    void Entry::connect_signal_activate(Function_t function, T data)
    {
        auto temp =  std::function<on_activate_function_t<T>>(function);
        _on_activate_f = std::function<on_activate_function_t<void*>>(*((std::function<on_activate_function_t<void*>>*) &temp));
        _on_activate_data = data;

        connect_signal("activate", on_activate_wrapper, this);
    }

    void Entry::on_activate_wrapper(GtkEntry*, Entry* instance)
    {
        if (instance->_on_activate_f == nullptr)
            return;

        (instance->_on_activate_f)(instance, instance->_on_activate_data);
    }
}
