//
// Created by clem on 4/12/23.
//

#include <include/header_bar.hpp>

namespace mousetrap
{
    HeaderBar::HeaderBar()
        : WidgetImplementation<GtkHeaderBar>(GTK_HEADER_BAR(gtk_header_bar_new()))
    {
        gtk_header_bar_set_title_widget(get_native(), nullptr);
        // to synchronize with HeaderBar::_title_widget
    }

    void HeaderBar::set_layout(const std::string& layout)
    {
        gtk_header_bar_set_decoration_layout(get_native(), layout.c_str());
    }

    std::string HeaderBar::get_layout() const
    {
        auto* layout = gtk_header_bar_get_decoration_layout(get_native());
        return layout != nullptr ? layout : "";
    }

    void HeaderBar::set_title_widget(const Widget& widget)
    {
        gtk_header_bar_set_title_widget(get_native(), widget.operator NativeWidget());
        _title_widget = &widget;
    }

    Widget* HeaderBar::get_title_widget() const
    {
        return const_cast<Widget*>(_title_widget);
    }

    void HeaderBar::remove_title_widget()
    {
        gtk_header_bar_set_title_widget(get_native(), nullptr);
        _title_widget = nullptr;
    }

    void HeaderBar::set_show_title_buttons(bool b)
    {
        gtk_header_bar_set_show_title_buttons(get_native(), b);
    }

    bool HeaderBar::get_show_title_buttons() const
    {
        return gtk_header_bar_get_show_title_buttons(get_native());
    }

    void HeaderBar::push_back(const Widget& widget)
    {
        gtk_header_bar_pack_end(get_native(), widget.operator NativeWidget());
    }

    void HeaderBar::push_front(const Widget& widget)
    {
        gtk_header_bar_pack_start(get_native(), widget.operator NativeWidget());
    }

    void HeaderBar::remove(const Widget& widget)
    {
        gtk_header_bar_remove(get_native(), widget.operator NativeWidget());
    }
}