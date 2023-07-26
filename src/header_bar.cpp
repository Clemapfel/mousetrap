//
// Created by clem on 4/12/23.
//

#include <mousetrap/header_bar.hpp>

namespace mousetrap
{
    HeaderBar::HeaderBar()
        : Widget(adw_header_bar_new()),
          CTOR_SIGNAL(HeaderBar, realize),
          CTOR_SIGNAL(HeaderBar, unrealize),
          CTOR_SIGNAL(HeaderBar, destroy),
          CTOR_SIGNAL(HeaderBar, hide),
          CTOR_SIGNAL(HeaderBar, show),
          CTOR_SIGNAL(HeaderBar, map),
          CTOR_SIGNAL(HeaderBar, unmap)
    {
        _internal = g_object_ref_sink(ADW_HEADER_BAR(Widget::operator NativeWidget()));
        adw_header_bar_set_title_widget(ADW_HEADER_BAR(Widget::operator NativeWidget()), nullptr);
    }

    HeaderBar::HeaderBar(const std::string& layout)
        : HeaderBar()
    {
        set_layout(layout);
    }
    
    HeaderBar::HeaderBar(detail::HeaderBarInternal* internal) 
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(HeaderBar, realize),
          CTOR_SIGNAL(HeaderBar, unrealize),
          CTOR_SIGNAL(HeaderBar, destroy),
          CTOR_SIGNAL(HeaderBar, hide),
          CTOR_SIGNAL(HeaderBar, show),
          CTOR_SIGNAL(HeaderBar, map),
          CTOR_SIGNAL(HeaderBar, unmap)
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
        adw_header_bar_set_decoration_layout(ADW_HEADER_BAR(operator NativeWidget()), layout.c_str());
    }

    std::string HeaderBar::get_layout() const
    {
        auto* layout = adw_header_bar_get_decoration_layout(ADW_HEADER_BAR(operator NativeWidget()));
        return layout != nullptr ? layout : "";
    }

    void HeaderBar::set_title_widget(const Widget& widget)
    {
        auto* ptr = &widget;
        WARN_IF_SELF_INSERTION(HeaderBar::set_title_widget, this, ptr);
        WARN_IF_PARENT_EXISTS(HeaderBar::set_title_widget, widget);

        adw_header_bar_set_title_widget(ADW_HEADER_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }

    void HeaderBar::remove_title_widget()
    {
        adw_header_bar_set_title_widget(ADW_HEADER_BAR(operator NativeWidget()), nullptr);
    }

    void HeaderBar::set_show_title_buttons(bool b)
    {
        adw_header_bar_set_show_start_title_buttons(ADW_HEADER_BAR(operator NativeWidget()), b);
        adw_header_bar_set_show_end_title_buttons(ADW_HEADER_BAR(operator NativeWidget()), b);
    }

    bool HeaderBar::get_show_title_buttons() const
    {
        return adw_header_bar_get_show_start_title_buttons(ADW_HEADER_BAR(operator NativeWidget())) or adw_header_bar_get_show_end_title_buttons(ADW_HEADER_BAR(operator NativeWidget()));
    }

    void HeaderBar::push_back(const Widget& widget)
    {
        adw_header_bar_pack_end(ADW_HEADER_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }

    void HeaderBar::push_front(const Widget& widget)
    {
        adw_header_bar_pack_start(ADW_HEADER_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }

    void HeaderBar::remove(const Widget& widget)
    {
        adw_header_bar_remove(ADW_HEADER_BAR(operator NativeWidget()), widget.operator NativeWidget());
    }
}