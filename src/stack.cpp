//
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#include <include/stack.hpp>
#include <include/log.hpp>

#include <iostream>

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
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(StackInternal, stack_internal, STACK_INTERNAL)

        static StackInternal* stack_internal_new(GtkStack* native)
        {
            auto* self = (StackInternal*) g_object_new(stack_internal_get_type(), nullptr);
            stack_internal_init(self);
            self->native = native;
            self->selection_model = new SelectionModel(gtk_stack_get_pages(native));
            self->children = new std::map<std::string, std::reference_wrapper<const Widget>>();
            return self;
        }
    }
    
    StackSidebar::StackSidebar(const Stack& stack)
        : WidgetImplementation<GtkStackSidebar>(GTK_STACK_SIDEBAR(gtk_stack_sidebar_new()))
    {
        gtk_stack_sidebar_set_stack(get_native(), stack.operator GtkStack*());
    }

    StackSwitcher::StackSwitcher(const Stack& stack)
        : WidgetImplementation<GtkStackSwitcher>(GTK_STACK_SWITCHER(gtk_stack_switcher_new()))
    {
        gtk_stack_switcher_set_stack(get_native(), GTK_STACK(stack.operator GtkStack*()));
    }

    Stack::Stack()
        : WidgetImplementation<GtkStack>(GTK_STACK(gtk_stack_new()))
    {
        _internal = detail::stack_internal_new(get_native());
        detail::attach_ref_to(G_OBJECT(get_native()), _internal);
    }

    Stack::~Stack()
    {}

    SelectionModel* Stack::get_selection_model()
    {
        return _internal->selection_model;
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
            log::critical("In Stack::add_child: Attempting to insert Stack into itself. This would cause an infinite loop");
            gtk_stack_add_titled(get_native(), nullptr, title.c_str(), title.c_str());
            return title;
        }

        auto it = _internal->children->find(title);
        if (it != _internal->children->end())
            log::critical("In Stack::add_child: Child with title `" + title + "` already exist. This may cause the original child to become inaccesible.");

        gtk_stack_add_titled(get_native(), widget.operator NativeWidget(), title.c_str(), title.c_str());
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

        gtk_stack_remove(get_native(), _internal->children->at(id).get().operator GtkWidget *());
        _internal->children->erase(id);
    }

    Stack::ID Stack::get_visible_child()
    {
        return gtk_stack_get_visible_child_name(get_native());
    }

    size_t Stack::get_n_children() const
    {
        return g_list_model_get_n_items(G_LIST_MODEL(gtk_stack_get_pages(get_native())));
    }

    void Stack::set_visible_child(Stack::ID id)
    {
        gtk_stack_set_visible_child_name(get_native(), id.c_str());
    }

    void Stack::set_transition_duration(Time time)
    {
        gtk_stack_set_transition_duration(get_native(), time.as_milliseconds());
    }

    Time Stack::get_transition_duration() const
    {
        return milliseconds(gtk_stack_get_transition_duration(get_native()));
    }

    void Stack::set_transition_type(StackTransitionType type)
    {
        gtk_stack_set_transition_type(get_native(), (GtkStackTransitionType) type);
    }

    StackTransitionType Stack::get_transition_type() const
    {
        return (StackTransitionType) gtk_stack_get_transition_type(get_native());
    }

    void Stack::set_is_horizontally_homogeneous(bool b)
    {
        gtk_stack_set_hhomogeneous(get_native(), b);
    }

    bool Stack::get_is_horizontally_homogenous() const
    {
        return gtk_stack_get_hhomogeneous(get_native());
    }

    void Stack::set_is_vertically_homogeneous(bool b)
    {
        gtk_stack_set_vhomogeneous(get_native(), b);
    }

    bool Stack::get_is_vertically_homogenous() const
    {
        return gtk_stack_get_vhomogeneous(get_native());
    }

    void Stack::set_should_interpolate_size(bool b)
    {
        gtk_stack_set_interpolate_size(get_native(), b);
    }

    bool Stack::get_should_interpolate_size() const
    {
        return gtk_stack_get_interpolate_size(get_native());
    }
}

