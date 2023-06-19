//
// Created by clem on 4/10/23.
//

#include <mousetrap/menubar.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(MenuBarInternal, menu_bar_internal, MENU_BAR_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(MenuBarInternal, menu_bar_internal, MENU_BAR_INTERNAL)

        static void menu_bar_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_MENU_BAR_INTERNAL(object);
            G_OBJECT_CLASS(menu_bar_internal_parent_class)->finalize(object);
            g_object_unref(self->model);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(MenuBarInternal, menu_bar_internal, MENU_BAR_INTERNAL)

        static MenuBarInternal* menu_bar_internal_new(GtkPopoverMenuBar* native, detail::MenuModelInternal* model)
        {
            auto* self = (MenuBarInternal*) g_object_new(menu_bar_internal_get_type(), nullptr);
            menu_bar_internal_init(self);

            self->native = native;
            self->model = model;

            g_object_ref(self->model);
            return self;
        }
    }
    
    MenuBar::MenuBar(const MenuModel& model)
        : Widget(gtk_popover_menu_bar_new_from_model(model.operator GMenuModel*())),
          CTOR_SIGNAL(MenuBar, realize),
          CTOR_SIGNAL(MenuBar, unrealize),
          CTOR_SIGNAL(MenuBar, destroy),
          CTOR_SIGNAL(MenuBar, hide),
          CTOR_SIGNAL(MenuBar, show),
          CTOR_SIGNAL(MenuBar, map),
          CTOR_SIGNAL(MenuBar, unmap)
    {
        _internal = detail::menu_bar_internal_new(GTK_POPOVER_MENU_BAR(Widget::operator NativeWidget()), (detail::MenuModelInternal*) model.get_internal());
        g_object_ref(_internal);
        refresh_widgets();
    }
    
    MenuBar::MenuBar(detail::MenuBarInternal* internal)
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(MenuBar, realize),
          CTOR_SIGNAL(MenuBar, unrealize),
          CTOR_SIGNAL(MenuBar, destroy),
          CTOR_SIGNAL(MenuBar, hide),
          CTOR_SIGNAL(MenuBar, show),
          CTOR_SIGNAL(MenuBar, map),
          CTOR_SIGNAL(MenuBar, unmap)
    {
        _internal = g_object_ref(internal);
    }

    MenuBar::~MenuBar()
    {
        g_object_unref(_internal);
    }

    NativeObject MenuBar::get_internal() const
    {
        return G_OBJECT(_internal);
    }
    
    void MenuBar::refresh_widgets()
    {
        auto model = MenuModel(_internal->model);

        if (model._internal->has_widget_in_toplevel)
            log::warning("In MenuBar::refresh_widgets: MenuModel contains a widget added with MenuModel::add_widget. These cannot be displayed, instead add the widget to a new menu model and add it as a submenu to the MenuBars menu model.", MOUSETRAP_DOMAIN);
            // TODO clarify this

        for (auto& pair : model.get_widgets())
        {
            if (gtk_popover_menu_bar_add_child(GTK_POPOVER_MENU_BAR(Widget::operator NativeWidget()), pair.second, pair.first.c_str()) != TRUE)
            {
                continue;

                std::stringstream str;
                str << "In PopoverMenuBar::refresh_widgets: Failed to add Widget of type " << G_STRINGIFY(pair.second) << " to submenu." << std::endl;
                log::critical(str.str(), MOUSETRAP_DOMAIN);
            }
        }
    }
}
