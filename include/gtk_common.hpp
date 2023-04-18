//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/31/23
//

#pragma once

#include <gtk/gtk.h>
#include <string>

namespace mousetrap::detail
{
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
        {
            g_object_remove_toggle_ref(parent, (GToggleNotify) toggle_notify_ref<T>, attachment);

            if (G_IS_OBJECT(attachment))
                g_object_unref(G_OBJECT(attachment));
        }
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
        g_object_add_toggle_ref(parent, (GToggleNotify) toggle_notify_pointer<T>, attachment);
    }

    template<typename T>
    static void detach_pointer_from(GObject* parent, T* attachment)
    {
        g_object_remove_toggle_ref(parent, (GToggleNotify) toggle_notify_pointer<T>, attachment);
    }

    template<typename T>
    static void attach_ref_to(GObject* parent, T* attachment)
    {
        g_object_add_toggle_ref(parent, (GToggleNotify) toggle_notify_ref<T>, attachment);
    }

    template<typename T>
    static void detach_ref_from(GObject* parent, T* attachment)
    {
        g_object_remove_toggle_ref(parent, (GToggleNotify) toggle_notify_ref<T>, attachment);
    }

    template<typename T>
    static void add_data(GObject* object, const std::string& key, T* data)
    {
        g_object_set_qdata_full(object, g_quark_from_string(key.c_str()), new T(data), destroy_notify_delete<T>);
    }

    template<typename T>
    static void set_data(GObject* object, const std::string& key, T* data)
    {
        g_object_set_qdata_full(object, g_quark_from_string(key.c_str()), data, nullptr);
    }

    template<typename T>
    static T* get_data(GObject* object, const std::string& key)
    {
        return (T*) g_object_get_qdata(object, g_quark_from_string(key.c_str()));
    }

    /// @brief simplified g object declaration
    /// @example \code
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
    /// \endcode
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
}