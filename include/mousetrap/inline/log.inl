//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/31/23
//

namespace mousetrap::detail
{
    #define WARN_IF_SELF_INSERTION(scope, owner, child) \
        if (owner != nullptr and child != nullptr and (owner->operator GtkWidget*() == child->operator GtkWidget*())) \
        {                                               \
            log::critical("In " + std::string(#scope) + ": Attempting to insert widget into itself. This would cause an infinite loop", MOUSETRAP_DOMAIN); \
            return; \
        }

    #define WARN_IF_PARENT_EXISTS(scope, child) \
        if (gtk_widget_get_parent(child.operator NativeWidget()) != nullptr) \
        { \
            log::critical("In " + std::string(#scope) + ": Attemping to insert widget into a container, but that widget already has a parent.", MOUSETRAP_DOMAIN);\
            return; \
        }
}
