//
// Created by clem on 3/20/23.
//

#include <mousetrap/notebook.hpp>
#include <mousetrap/log.hpp>
#include <mousetrap/selection_model.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(NotebookInternal, notebook_internal, NOTEBOOK_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(NotebookInternal, notebook_internal, NOTEBOOK_INTERNAL)

        static void notebook_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_NOTEBOOK_INTERNAL(object);
            G_OBJECT_CLASS(notebook_internal_parent_class)->finalize(object);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(NotebookInternal, notebook_internal, NOTEBOOK_INTERNAL)

        static NotebookInternal* notebook_internal_new(GtkNotebook* native)
        {
            auto* self = (NotebookInternal*) g_object_new(notebook_internal_get_type(), nullptr);
            notebook_internal_init(self);

            self->popup_enabled = false;
            self->tabs_reorderable = false;
            self->native = native;
            return self;
        }
    }
    
    Notebook::Notebook()
        : Widget(gtk_notebook_new()),
          CTOR_SIGNAL(Notebook, page_added),
          CTOR_SIGNAL(Notebook, page_reordered),
          CTOR_SIGNAL(Notebook, page_removed),
          CTOR_SIGNAL(Notebook, page_selection_changed),
          CTOR_SIGNAL(Notebook, realize),
          CTOR_SIGNAL(Notebook, unrealize),
          CTOR_SIGNAL(Notebook, destroy),
          CTOR_SIGNAL(Notebook, hide),
          CTOR_SIGNAL(Notebook, show),
          CTOR_SIGNAL(Notebook, map),
          CTOR_SIGNAL(Notebook, unmap)
    {
        gtk_notebook_popup_disable(GTK_NOTEBOOK(operator NativeWidget()));
        _internal = detail::notebook_internal_new(GTK_NOTEBOOK(operator NativeWidget()));
        detail::attach_ref_to(G_OBJECT(GTK_NOTEBOOK(operator NativeWidget())), _internal);
        g_object_ref(_internal);
    }
    
    Notebook::Notebook(detail::NotebookInternal* internal)
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(Notebook, page_added),
          CTOR_SIGNAL(Notebook, page_reordered),
          CTOR_SIGNAL(Notebook, page_removed),
          CTOR_SIGNAL(Notebook, page_selection_changed),
          CTOR_SIGNAL(Notebook, realize),
          CTOR_SIGNAL(Notebook, unrealize),
          CTOR_SIGNAL(Notebook, destroy),
          CTOR_SIGNAL(Notebook, hide),
          CTOR_SIGNAL(Notebook, show),
          CTOR_SIGNAL(Notebook, map),
          CTOR_SIGNAL(Notebook, unmap)
    {
        _internal = g_object_ref(internal);
    }

    Notebook::~Notebook()
    {
        g_object_unref(_internal);
    }

    NativeObject Notebook::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    uint64_t Notebook::push_front(const Widget& child_widget, const Widget& label_widget)
    {
        if ((child_widget.operator GtkWidget*() == this->operator GtkWidget*()) or
            (label_widget.operator GtkWidget*() ==  this->operator GtkWidget*()))
        {
            log::critical("In Notebook::push_front: Attempting to insert Notebook into itself. This would cause an infinite loop");
            return 0;
        }

        auto out = gtk_notebook_prepend_page(
            GTK_NOTEBOOK(operator NativeWidget()),
            child_widget.operator GtkWidget*(),
            label_widget.operator GtkWidget*()
        );

        if (out == -1)
        {
            log::critical("In Notebook::push_front: Failed to insert page", MOUSETRAP_DOMAIN);
        }

        gtk_notebook_set_tab_reorderable(
            GTK_NOTEBOOK(operator NativeWidget()),
            child_widget.operator GtkWidget*(),
            _internal->tabs_reorderable
        );

        return out;
    }

    uint64_t Notebook::push_back(const Widget& child_widget, const Widget& label_widget)
    {
        if ((child_widget.operator GtkWidget*() == this->operator GtkWidget*()) or
            (label_widget.operator GtkWidget*() == this->operator GtkWidget*()))
        {
            log::critical("In Notebook::push_back: Attempting to insert Notebook into itself. This would cause an infinite loop");
            return 0;
        }

        auto out = gtk_notebook_append_page(
            GTK_NOTEBOOK(operator NativeWidget()),
            child_widget.operator GtkWidget*(),
            label_widget.operator GtkWidget*()
        );

        if (out == -1)
            std::cerr << "[ERROR] In Notebook::push_back: Failed to insert page" << std::endl;

        gtk_notebook_set_tab_reorderable(
            GTK_NOTEBOOK(operator NativeWidget()),
            child_widget.operator GtkWidget *(),
            _internal->tabs_reorderable
        );

        return out;
    }

    uint64_t Notebook::insert(uint64_t new_position, const Widget& child_widget, const Widget& label_widget)
    {
        if ((child_widget.operator GtkWidget*() == this->operator GtkWidget*()) or
        (label_widget.operator GtkWidget*() == this->operator GtkWidget*()))
        {
            log::critical("In Notebook::insert: Attempting to insert Notebook into itself. This would cause an infinite loop");
            return 0;
        }

        int pos = new_position >= get_n_pages() ? -1 : new_position;

        auto out = gtk_notebook_insert_page(
            GTK_NOTEBOOK(operator NativeWidget()),
            child_widget.operator GtkWidget*(),
            label_widget.operator GtkWidget*(),
            pos
        );

        if (out == -1)
            log::critical("In Notebook::insert: Failed to insert page", MOUSETRAP_DOMAIN);

        gtk_notebook_set_tab_reorderable(
            GTK_NOTEBOOK(operator NativeWidget()),
            child_widget.operator GtkWidget*(),
            _internal->tabs_reorderable
        );

        return out;
    }

    void Notebook::move_page_to(uint64_t current_position, uint64_t new_position)
    {
        auto* pages = gtk_notebook_get_pages(GTK_NOTEBOOK(operator NativeWidget()));

        auto n_pages = g_list_model_get_n_items(pages);
        if (current_position >= n_pages)
        {
            log::critical("In Notebook::move_page_to: Index " + std::to_string(current_position) + " is out of bounds for a Notebook with " + std::to_string(n_pages) + " pages.", MOUSETRAP_DOMAIN);
            g_object_unref(pages);
            return;
        }

        if (new_position >= n_pages)
        {
            log::critical("In Notebook::move_page_to: Index " + std::to_string(new_position) + " is out of bounds for a Notebook with " + std::to_string(n_pages) + " pages.", MOUSETRAP_DOMAIN);
            g_object_unref(pages);
            return;
        }

        auto* page = GTK_NOTEBOOK_PAGE(g_list_model_get_item(pages, current_position));
        gtk_notebook_reorder_child(GTK_NOTEBOOK(operator NativeWidget()), gtk_notebook_page_get_child(page), new_position);

        g_object_unref(pages);
    }

    void Notebook::remove(uint64_t position)
    {
        int pos = position >= get_n_pages() ? -1 : position;
        gtk_notebook_remove_page(GTK_NOTEBOOK(operator NativeWidget()), pos);
    }

    void Notebook::next_page()
    {
        gtk_notebook_next_page(GTK_NOTEBOOK(operator NativeWidget()));
    }

    void Notebook::previous_page()
    {
        gtk_notebook_prev_page(GTK_NOTEBOOK(operator NativeWidget()));
    }

    void Notebook::goto_page(uint64_t i)
    {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(operator NativeWidget()), i >= get_n_pages() ? -1 : i);
    }

    int64_t Notebook::get_current_page() const
    {
        return gtk_notebook_get_current_page(GTK_NOTEBOOK(operator NativeWidget()));
    }

    uint64_t Notebook::get_n_pages() const
    {
        return gtk_notebook_get_n_pages(GTK_NOTEBOOK(operator NativeWidget()));
    }

    bool Notebook::get_is_scrollable() const
    {
        return gtk_notebook_get_scrollable(GTK_NOTEBOOK(operator NativeWidget()));
    }

    void Notebook::set_is_scrollable(bool b)
    {
        gtk_notebook_set_scrollable(GTK_NOTEBOOK(operator NativeWidget()), b);
    }

    bool Notebook::get_has_border() const
    {
        return gtk_notebook_get_show_border(GTK_NOTEBOOK(operator NativeWidget()));
    }

    void Notebook::set_has_border(bool b)
    {
        gtk_notebook_set_show_border(GTK_NOTEBOOK(operator NativeWidget()), b);
    }

    void Notebook::set_tabs_visible(bool b)
    {
        gtk_notebook_set_show_tabs(GTK_NOTEBOOK(operator NativeWidget()), b);
    }

    bool Notebook::get_tabs_visible() const
    {
        return gtk_notebook_get_show_tabs(GTK_NOTEBOOK(operator NativeWidget()));
    }

    void Notebook::set_tab_position(RelativePosition position)
    {
        gtk_notebook_set_tab_pos(GTK_NOTEBOOK(operator NativeWidget()), (GtkPositionType) position);
    }

    RelativePosition Notebook::get_tab_position() const
    {
        return (RelativePosition) gtk_notebook_get_tab_pos(GTK_NOTEBOOK(operator NativeWidget()));
    }

    void Notebook::set_quick_change_menu_enabled(bool b)
    {
        if (b)
            gtk_notebook_popup_enable(GTK_NOTEBOOK(operator NativeWidget()));
        else
            gtk_notebook_popup_disable(GTK_NOTEBOOK(operator NativeWidget()));

        _internal->popup_enabled = b;
    }

    bool Notebook::get_quick_change_menu_enabled() const
    {
        return _internal->popup_enabled;
    }

    void Notebook::set_tabs_reorderable(bool b)
    {
        auto* pages = gtk_notebook_get_pages(GTK_NOTEBOOK(operator NativeWidget()));
        for (uint64_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(pages)); ++i)
            gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(operator NativeWidget()),
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