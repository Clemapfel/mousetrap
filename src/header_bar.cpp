//
// Created by clem on 4/12/23.
//

#include <mousetrap/header_bar.hpp>

namespace mousetrap
{
    HeaderBar::HeaderBar()
        : Widget(gtk_header_bar_new())
    {
        _internal = g_object_ref_sink(GTK_HEADER_BAR(Widget::operator NativeWidget()));
        gtk_header_bar_set_title_widget(GTK_HEADER_BAR(Widget::operator NativeWidget()), nullptr);
    }

    HeaderBar::HeaderBar(const std::string& layout)
        : HeaderBar()
    {
        set_layout(layout);
    }
    
    HeaderBar::HeaderBar(detail::HeaderBarInternal* internal) 
        : Widget(GTK_WIDGET(internal))
    {
        _internal = g_object_ref(internal);
    }
    
    HeaderBar::~HeaderBar() 
    {
        g_object_unref(_internal);
    }

    NativeObject HeaderBar::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    void HeaderBar::set_layout(const std::string& layout)
    {
        gtk_header_bar_set_decoration_layout(GTK_HEADER_BAR(operator NativeWidget()), layout.c_str());
    }

    std::string HeaderBar::get_layout() const
    {
        auto* layout = gtk_header_bar_get_decoration_layout(GTK_HEADER_BAR(operator NativeWidget()));
        return layout != nullptr ? layout : "";
    }

    void HeaderBar::set_title_widget(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(HeaderBar::set_title_widget, this, ptr);
        gtk_header_bar_set_title_widget(GTK_HEADER_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }

    void HeaderBar::remove_title_widget()
    {
        gtk_header_bar_set_title_widget(GTK_HEADER_BAR(operator NativeWidget()), nullptr);
    }

    void HeaderBar::set_show_title_buttons(bool b)
    {
        gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(operator NativeWidget()), b);
    }

    bool HeaderBar::get_show_title_buttons() const
    {
        return gtk_header_bar_get_show_title_buttons(GTK_HEADER_BAR(operator NativeWidget()));
    }

    void HeaderBar::push_back(const Widget& widget)
    {
        gtk_header_bar_pack_end(GTK_HEADER_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }

    void HeaderBar::push_front(const Widget& widget)
    {
        gtk_header_bar_pack_start(GTK_HEADER_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }

    void HeaderBar::remove(const Widget& widget)
    {
        gtk_header_bar_remove(GTK_HEADER_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }
}