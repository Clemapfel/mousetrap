//
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/stack.hpp>
#include <mousetrap/log.hpp>

#include <iostream>
#include <map>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(StackInternal, stack_internal, STACK_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(StackInternal, stack_internal, STACK_INTERNAL)

        static void stack_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_STACK_INTERNAL(object);
            G_OBJECT_CLASS(stack_internal_parent_class)->finalize(object);
            delete self->children;
            g_object_unref(self->selection_model);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(StackInternal, stack_internal, STACK_INTERNAL)

        static StackInternal* stack_internal_new(GtkStack* native)
        {
            auto* self = (StackInternal*) g_object_new(stack_internal_get_type(), nullptr);
            stack_internal_init(self);
            self->native = native;
            self->selection_model = gtk_stack_get_pages(native);
            self->children = new std::map<std::string, std::reference_wrapper<const Widget>>();
            return self;
        }
    }
    
    StackSidebar::StackSidebar(const Stack& stack)
        : Widget(gtk_stack_sidebar_new()),
          CTOR_SIGNAL(StackSidebar, realize),
          CTOR_SIGNAL(StackSidebar, unrealize),
          CTOR_SIGNAL(StackSidebar, destroy),
          CTOR_SIGNAL(StackSidebar, hide),
          CTOR_SIGNAL(StackSidebar, show),
          CTOR_SIGNAL(StackSidebar, map),
          CTOR_SIGNAL(StackSidebar, unmap)
    {
        _internal = GTK_STACK_SIDEBAR(Widget::operator NativeObject());
        g_object_ref(_internal);
        gtk_stack_sidebar_set_stack(_internal, stack.operator GtkStack*());
    }
    
    StackSidebar::StackSidebar(detail::StackSidebarInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(StackSidebar, realize),
          CTOR_SIGNAL(StackSidebar, unrealize),
          CTOR_SIGNAL(StackSidebar, destroy),
          CTOR_SIGNAL(StackSidebar, hide),
          CTOR_SIGNAL(StackSidebar, show),
          CTOR_SIGNAL(StackSidebar, map),
          CTOR_SIGNAL(StackSidebar, unmap)
    {
        _internal = g_object_ref(internal);
    }
    
    StackSidebar::~StackSidebar()
    {
        g_object_unref(_internal);
    }

    NativeObject StackSidebar::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    // ###

    StackSwitcher::StackSwitcher(const Stack& stack)
        : Widget(gtk_stack_switcher_new()),
          CTOR_SIGNAL(StackSwitcher, realize),
          CTOR_SIGNAL(StackSwitcher, unrealize),
          CTOR_SIGNAL(StackSwitcher, destroy),
          CTOR_SIGNAL(StackSwitcher, hide),
          CTOR_SIGNAL(StackSwitcher, show),
          CTOR_SIGNAL(StackSwitcher, map),
          CTOR_SIGNAL(StackSwitcher, unmap)
    {
        _internal = GTK_STACK_SWITCHER(Widget::operator NativeObject());
        g_object_ref(_internal);
        gtk_stack_switcher_set_stack(_internal, stack.operator GtkStack*());
    }

    StackSwitcher::StackSwitcher(detail::StackSwitcherInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(StackSwitcher, realize),
          CTOR_SIGNAL(StackSwitcher, unrealize),
          CTOR_SIGNAL(StackSwitcher, destroy),
          CTOR_SIGNAL(StackSwitcher, hide),
          CTOR_SIGNAL(StackSwitcher, show),
          CTOR_SIGNAL(StackSwitcher, map),
          CTOR_SIGNAL(StackSwitcher, unmap)
    {
        _internal = g_object_ref(internal);
    }

    StackSwitcher::~StackSwitcher()
    {
        g_object_unref(_internal);
    }

    NativeObject StackSwitcher::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    // ###

    Stack::Stack()
        : Widget(gtk_stack_new()),
          CTOR_SIGNAL(Stack, realize),
          CTOR_SIGNAL(Stack, unrealize),
          CTOR_SIGNAL(Stack, destroy),
          CTOR_SIGNAL(Stack, hide),
          CTOR_SIGNAL(Stack, show),
          CTOR_SIGNAL(Stack, map),
          CTOR_SIGNAL(Stack, unmap)
    {
        _internal = detail::stack_internal_new(GTK_STACK(operator NativeWidget()));
        detail::attach_ref_to(G_OBJECT(_internal->native), _internal);
        g_object_ref(_internal);
    }
    
    Stack::Stack(detail::StackInternal* internal)
        : Widget(GTK_WIDGET(internal->native)),
          CTOR_SIGNAL(Stack, realize),
          CTOR_SIGNAL(Stack, unrealize),
          CTOR_SIGNAL(Stack, destroy),
          CTOR_SIGNAL(Stack, hide),
          CTOR_SIGNAL(Stack, show),
          CTOR_SIGNAL(Stack, map),
          CTOR_SIGNAL(Stack, unmap)
    {
        _internal = g_object_ref(internal);
    }

    Stack::~Stack()
    {
        g_object_unref(_internal);
    }

    NativeObject Stack::get_internal() const 
    {
        return G_OBJECT(_internal);
    }

    SelectionModel Stack::get_selection_model()
    {
        return SelectionModel(_internal->selection_model);
    }

    Widget* Stack::get_child(Stack::ID id)
    {
        auto it = _internal->children->find(id);
        if (it == _internal->children->end())
            return nullptr;
        else
            return const_cast<Widget*>(&(it->second.get()));
    }

    Stack::ID Stack::add_child(const Widget& widget, const std::string& title)
    {
        auto* ptr = &widget;
        if (widget.operator GtkWidget*() == this->operator GtkWidget*())
        {
            log::critical("In Stack::add_child: Attempting to insert Stack into itself. This would cause an infinite loop", MOUSETRAP_DOMAIN);
            gtk_stack_add_titled(_internal->native, nullptr, title.c_str(), title.c_str());
            return title;
        }

        auto it = _internal->children->find(title);
        if (it != _internal->children->end())
            log::critical("In Stack::add_child: Child with title `" + title + "` already exist. This may cause the original child to become inaccesible.", MOUSETRAP_DOMAIN);

        gtk_stack_add_titled(_internal->native, widget.operator NativeWidget(), title.c_str(), title.c_str());
        _internal->children->insert({title, std::ref(widget)});
        return title;
    }

    void Stack::remove_child(Stack::ID id)
    {
        if (_internal->children->find(id) == _internal->children->end())
        {
            std::stringstream str;
            str << "In Stack::remove_child: No child with ID `" << id << "`";
            log::critical(str.str(), MOUSETRAP_DOMAIN);
            return;
        }

        gtk_stack_remove(_internal->native, _internal->children->at(id).get().operator GtkWidget *());
        _internal->children->erase(id);
    }

    Stack::ID Stack::get_visible_child()
    {
        return gtk_stack_get_visible_child_name(_internal->native);
    }

    size_t Stack::get_n_children() const
    {
        return g_list_model_get_n_items(G_LIST_MODEL(gtk_stack_get_pages(_internal->native)));
    }

    Stack::ID Stack::get_child_at(size_t index) const
    {
        auto* pages = G_LIST_MODEL(gtk_stack_get_pages(_internal->native));

        size_t n_pages = g_list_model_get_n_items(pages);
        if (index >= n_pages)
        {
            log::critical("In Stack::get_child_at: Index " + std::to_string(index) + " is out of bounds for a Stack with " + std::to_string(n_pages) + " pages.");
            return "";
        }

        return gtk_stack_page_get_name(GTK_STACK_PAGE(g_list_model_get_item(G_LIST_MODEL(pages), index)));
    }

    void Stack::set_visible_child(Stack::ID id)
    {
        gtk_stack_set_visible_child_name(_internal->native, id.c_str());
    }

    void Stack::set_transition_duration(Time time)
    {
        gtk_stack_set_transition_duration(_internal->native, time.as_milliseconds());
    }

    Time Stack::get_transition_duration() const
    {
        return milliseconds(gtk_stack_get_transition_duration(_internal->native));
    }

    void Stack::set_transition_type(StackTransitionType type)
    {
        gtk_stack_set_transition_type(_internal->native, (GtkStackTransitionType) type);
    }

    StackTransitionType Stack::get_transition_type() const
    {
        return (StackTransitionType) gtk_stack_get_transition_type(_internal->native);
    }

    void Stack::set_is_horizontally_homogeneous(bool b)
    {
        gtk_stack_set_hhomogeneous(_internal->native, b);
    }

    bool Stack::get_is_horizontally_homogenous() const
    {
        return gtk_stack_get_hhomogeneous(_internal->native);
    }

    void Stack::set_is_vertically_homogeneous(bool b)
    {
        gtk_stack_set_vhomogeneous(_internal->native, b);
    }

    bool Stack::get_is_vertically_homogeneous() const
    {
        return gtk_stack_get_vhomogeneous(_internal->native);
    }

    void Stack::set_should_interpolate_size(bool b)
    {
        gtk_stack_set_interpolate_size(_internal->native, b);
    }

    bool Stack::get_should_interpolate_size() const
    {
        return gtk_stack_get_interpolate_size(_internal->native);
    }

    Stack::operator GtkStack*() const
    {
        return _internal->native;
    }
}

