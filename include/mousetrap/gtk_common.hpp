//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/31/23
//

#pragma once

#include <gtk/gtk.h>
#include <adwaita.h>
#include <string>

#include <iostream> //TODO

namespace mousetrap
{
    /// @brief garbage collected object \for_internal_use_only
    using NativeObject = GObject*;
    inline bool GTK_INITIALIZED = false;
}

namespace mousetrap::detail
{
    void mark_gtk_initialized();
    void throw_if_gtk_is_uninitialized();

    struct notify_if_gtk_uninitialized
    {
        inline notify_if_gtk_uninitialized()
        {
            throw_if_gtk_is_uninitialized();
        }

        static inline std::string message = R"([FATAL]
Attempting to construct a widget, but the GTK4 backend has not yet been initialized. Make sure that, for all widgets, the widgets constructor is called **after** `Application` has emitted its `activate` signal.

A typical `main.cpp` should look like this:
```cpp
#include <mousetrap.hpp>
using namespace mousetrap

int main()
{
    auto app = Application("application.name");
    app.connect_signal_activate([](Application& app) {
        // all initialization and construction of widgets should happen here
    };
    app.run();
}
```
You have most likely attempted to construct a widget outside of the `activate` signal handler.
)";
    };

    template <typename T>
    static void toggle_notify_pointer(T* attachment, GObject* parent, gboolean last_ref)
    {
        if (last_ref)
        {
            g_object_remove_toggle_ref(parent, (GToggleNotify) toggle_notify_pointer<T>, attachment);
            delete attachment;
        }
    }

    template<typename T>
    static void toggle_notify_ref(T* attachment, GObject* parent, gboolean last_ref)
    {
        if (last_ref)
            g_object_remove_toggle_ref(parent, (GToggleNotify) toggle_notify_ref<T>, attachment);

        g_object_unref(G_OBJECT(attachment));
    }

    template<typename T>
    static void destroy_notify_delete(void* in)
    {
        delete (T*) in;
    }

    template<typename T>
    static void destroy_notify_unref(void* in)
    {
        g_object_unref(G_OBJECT(in));
    }

    template<typename T>
    static void attach_pointer_to(GObject* parent, T* attachment)
    {
        if (not G_IS_OBJECT(parent))
            return;

        g_object_add_toggle_ref(parent, (GToggleNotify) toggle_notify_pointer<T>, attachment);
    }

    template<typename T>
    static void detach_pointer_from(GObject* parent, T* attachment)
    {
        if (not G_IS_OBJECT(parent))
            return;

        g_object_remove_toggle_ref(parent, (GToggleNotify) toggle_notify_pointer<T>, attachment);
    }

    template<typename T>
    static void attach_ref_to(GObject* parent, T* attachment)
    {
        if (not G_IS_OBJECT(parent))
            return;

        g_object_add_toggle_ref(parent, (GToggleNotify) toggle_notify_ref<T>, attachment);
    }

    template<typename T>
    static void detach_ref_from(GObject* parent, T* attachment)
    {
        if (not G_IS_OBJECT(parent))
            return;

        g_object_remove_toggle_ref(parent, (GToggleNotify) toggle_notify_ref<T>, attachment);
    }

    template<typename T>
    static void set_data(GObject* object, const std::string& key, T* data)
    {
        if (not G_IS_OBJECT(object))
            return;

        g_object_set_qdata_full(object, g_quark_from_string(key.c_str()), data, nullptr);
    }

    template<typename T>
    static T* get_data(GObject* object, const std::string& key)
    {
        if (not G_IS_OBJECT(object))
            return nullptr;

        return (T*) g_object_get_qdata(object, g_quark_from_string(key.c_str()));
    }

    /// @brief simplified g object declaration
    /// @code
    ///     // in test.hpp
    ///     struct _Test {
    ///         GObject parent; // this is always necessary
    ///     };
    ///
    ///     // in test.cpp
    ///     DECLARE_NEW_TYPE(Test, test, TEST)
    ///     DEFINE_NEW_TYPE_TRIVIAL_FINALIZE(Test, test, TEST)
    ///     DEFINE_NEW_TYPE_TRIVIAL_INIT(Test, test, TEST)
    ///     DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(Test, test, TEST)
    ///     DEFINE_NEW_TYPE_TRIVIAL_NEW(Test, test, TEST)
    /// @endcode
    #define DECLARE_STRUCT_CLASS(CamelCase) struct _##CamelCase##Class { GObjectClass parent_class; };

    #define DECLARE_NEW_TYPE(CamelCase, snake_case, CAPS_CASE) \
            G_DECLARE_FINAL_TYPE (CamelCase, snake_case, MOUSETRAP, CAPS_CASE, GObject) \
            DECLARE_STRUCT_CLASS(CamelCase)            \
            G_DEFINE_TYPE(CamelCase, snake_case, G_TYPE_OBJECT)                         \

    #define DEFINE_NEW_TYPE_TRIVIAL_FINALIZE(CamelCase, snake_case, CAPS_CASE) \
        static void snake_case##_finalize(GObject* object) \
        {                                                                                                        \
            auto* self = MOUSETRAP_##CAPS_CASE(object);                         \
            G_OBJECT_CLASS(snake_case##_parent_class)->finalize(object);       \
        }

    #define DEFINE_NEW_TYPE_TRIVIAL_INIT(CamelCase, snake_case, CAPS_CASE) \
        static void snake_case##_init(CamelCase* self) {}

    #define DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(CamelCase, snake_case, CAPS_CASE) \
        static void snake_case##_class_init(CamelCase##Class* c)                 \
        {                                                                        \
            GObjectClass* gobject_class = G_OBJECT_CLASS(c);                     \
            gobject_class->finalize = snake_case##_finalize; \
        }

    #define DEFINE_NEW_TYPE_TRIVIAL_NEW(CamelCase, snake_case, CAPS_CASE) \
        static CamelCase* snake_case##_new()                                    \
        {                                                                 \
            auto* self = (CamelCase*) g_object_new(snake_case##_get_type(), nullptr); \
            snake_case##_init(self);                                      \
            return self;\
        }

    #define NEW_SIGNAL(id, instance) g_signal_newv(id, G_TYPE_FROM_INSTANCE(instance), (GSignalFlags) (G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS), NULL, NULL, NULL, NULL, G_TYPE_NONE, 0, NULL);
}