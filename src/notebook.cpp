//
// Created by clem on 3/20/23.
//

#include <include/notebook.hpp>
#include <include/log.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(NotebookInternal, notebook_internal, NOTEBOOK_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_FINALIZE(NotebookInternal, notebook_internal, NOTEBOOK_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(NotebookInternal, notebook_internal, NOTEBOOK_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(NotebookInternal, notebook_internal, NOTEBOOK_INTERNAL)

        static NotebookInternal* notebook_internal_new()
        {
            auto* self = (NotebookInternal*) g_object_new(notebook_internal_get_type(), nullptr);
            notebook_internal_init(self);

            self->popup_enabled = false;
            self->tabs_reorderable = false;
            return self;
        }
    }
    
    Notebook::Notebook()
        : WidgetImplementation<GtkNotebook>(GTK_NOTEBOOK(gtk_notebook_new())),
          CTOR_SIGNAL(Notebook, page_added),
          CTOR_SIGNAL(Notebook, page_reordered),
          CTOR_SIGNAL(Notebook, page_removed),
          CTOR_SIGNAL(Notebook, page_selection_changed)
    {
        gtk_notebook_popup_disable(get_native());
        _internal = detail::notebook_internal_new();
        detail::attach_ref_to(G_OBJECT(get_native()), _internal);
    }

    size_t Notebook::push_front(Widget* child_widget, Widget* label_widget)
    {
        if ((child_widget != nullptr and child_widget->operator GtkWidget*() == this->operator GtkWidget*()) or
            (label_widget != nullptr and label_widget->operator GtkWidget*() == this->operator GtkWidget*()))
        {
            log::critical("In Notebook::push_front: Attempting to insert Notebook into itself. This would cause an infinite loop");
            return 0;
        }

        auto out = gtk_notebook_prepend_page(
            get_native(),
            child_widget != nullptr ? child_widget->operator GtkWidget *() : nullptr,
            label_widget != nullptr ? label_widget->operator GtkWidget*() : nullptr
        );

        if (out == -1)
        {
            log::critical("In Notebook::push_front: Failed to insert page", MOUSETRAP_DOMAIN);
        }

        gtk_notebook_set_tab_reorderable(
            get_native(),
            child_widget != nullptr ? child_widget->operator GtkWidget *() : nullptr,
            _internal->tabs_reorderable
        );

        return out;
    }

    size_t Notebook::push_back(Widget* child_widget, Widget* label_widget)
    {
        if ((child_widget != nullptr and child_widget->operator GtkWidget*() == this->operator GtkWidget*()) or
        (label_widget != nullptr and label_widget->operator GtkWidget*() == this->operator GtkWidget*()))
        {
            log::critical("In Notebook::push_back: Attempting to insert Notebook into itself. This would cause an infinite loop");
            return 0;
        }

        auto out = gtk_notebook_append_page(
            get_native(),
            child_widget != nullptr ? child_widget->operator GtkWidget *() : nullptr,
            label_widget != nullptr ? label_widget->operator GtkWidget*() : nullptr);

        if (out == -1)
            std::cerr << "[ERROR] In Notebook::push_back: Failed to insert page" << std::endl;

        gtk_notebook_set_tab_reorderable(
            get_native(),
            child_widget != nullptr ? child_widget->operator GtkWidget *() : nullptr,
            _internal->tabs_reorderable
        );

        return out;
    }

    size_t Notebook::insert(size_t new_position, Widget* child_widget, Widget* label_widget)
    {
        if ((child_widget != nullptr and child_widget->operator GtkWidget*() == this->operator GtkWidget*()) or
        (label_widget != nullptr and label_widget->operator GtkWidget*() == this->operator GtkWidget*()))
        {
            log::critical("In Notebook::insert: Attempting to insert Notebook into itself. This would cause an infinite loop");
            return 0;
        }

        int pos = new_position >= get_n_pages() ? -1 : new_position;

        auto out = gtk_notebook_insert_page(
            get_native(),
            child_widget != nullptr ? child_widget->operator GtkWidget *() : nullptr,
            label_widget != nullptr ? label_widget->operator GtkWidget*() : nullptr,
            pos
        );

        if (out == -1)
            log::critical("In Notebook::insert: Failed to insert page", MOUSETRAP_DOMAIN);

        gtk_notebook_set_tab_reorderable(
            get_native(),
            child_widget != nullptr ? child_widget->operator GtkWidget *() : nullptr,
            _internal->tabs_reorderable
        );

        return out;
    }

    void Notebook::remove(size_t position)
    {
        int pos = position >= get_n_pages() ? -1 : position;
        gtk_notebook_remove_page(get_native(), pos);
    }

    void Notebook::next_page()
    {
        gtk_notebook_next_page(get_native());
    }

    void Notebook::previous_page()
    {
        gtk_notebook_prev_page(get_native());
    }

    void Notebook::goto_page(size_t i)
    {
        gtk_notebook_set_current_page(get_native(), i >= get_n_pages() ? -1 : i);
    }

    int32_t Notebook::get_current_page() const
    {
        return gtk_notebook_get_current_page(get_native());
    }

    size_t Notebook::get_n_pages() const
    {
        return gtk_notebook_get_n_pages(get_native());
    }

    bool Notebook::get_is_scrollable() const
    {
        return gtk_notebook_get_scrollable(get_native());
    }

    void Notebook::set_is_scrollable(bool b)
    {
        gtk_notebook_set_scrollable(get_native(), b);
    }

    bool Notebook::get_has_border() const
    {
        return gtk_notebook_get_show_border(get_native());
    }

    void Notebook::set_has_border(bool b)
    {
        gtk_notebook_set_show_border(get_native(), b);
    }

    void Notebook::set_tabs_visible(bool b)
    {
        gtk_notebook_set_show_tabs(get_native(), b);
    }

    bool Notebook::get_tabs_visible() const
    {
        return gtk_notebook_get_show_tabs(get_native());
    }

    void Notebook::set_tab_position(RelativePosition position)
    {
        gtk_notebook_set_tab_pos(get_native(), (GtkPositionType) position);
    }

    RelativePosition Notebook::get_tab_position() const
    {
        return (RelativePosition) gtk_notebook_get_tab_pos(get_native());
    }

    void Notebook::set_quick_change_menu_enabled(bool b)
    {
        if (b)
            gtk_notebook_popup_enable(get_native());
        else
            gtk_notebook_popup_disable(get_native());

        _internal->popup_enabled = b;
    }

    bool Notebook::get_quick_change_menu_enabled() const
    {
        return _internal->popup_enabled;
    }

    void Notebook::set_tabs_reorderable(bool b)
    {
        auto* pages = gtk_notebook_get_pages(get_native());
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(pages)); ++i)
            gtk_notebook_set_tab_reorderable(get_native(),
                gtk_notebook_page_get_child(GTK_NOTEBOOK_PAGE(g_list_model_get_item(G_LIST_MODEL(pages), i))),
                b
            );

        _internal->tabs_reorderable = b;
    }

    bool Notebook::get_tabs_reorderable() const
    {
        return _internal->tabs_reorderable;
    }
}