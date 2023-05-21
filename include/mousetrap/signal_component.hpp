//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/17/23
//

#pragma once

#include <cstddef>
#include <functional>
#include <mousetrap/gtk_common.hpp>

namespace mousetrap
{
    /// @brief base class of all signal components
    struct SignalComponent
    {
        SignalComponent() = default;
        SignalComponent(SignalComponent&&) = delete;
        SignalComponent& operator=(SignalComponent&&) = delete;
        SignalComponent(const SignalComponent&) = delete;
        SignalComponent& operator=(const SignalComponent&) = delete;
    };

    namespace detail
    {
        template<typename T>
        struct InternalMapping {};
    }

    #define SPLAT(...) __VA_ARGS__

    #define CTOR_SIGNAL(T, signal_name) \
        SIGNAL_CLASS_NAME(signal_name)<T>(this)

    #define HAS_SIGNAL(T, signal_name) \
        public SIGNAL_CLASS_NAME(signal_name)<T>

    #define SIGNAL_INTERNAL_PRIVATE_CLASS_NAME(CamelCase) _##CamelCase
    #define SIGNAL_INTERNAL_CLASS_NAME(CamelCase) CamelCase
    #define SIGNAL_CLASS_NAME(snake_case) has_signal_##snake_case

    #define DECLARE_SIGNAL(CamelCase, snake_case, CAPS_CASE, g_signal_id, return_t) \
    \
    namespace detail \
    { \
        struct SIGNAL_INTERNAL_PRIVATE_CLASS_NAME(CamelCase) \
        { \
            GObject parent; \
            void* instance; \
            std::function<return_t(void*)> function; \
            bool blocked; \
        }; \
        using SIGNAL_INTERNAL_CLASS_NAME(CamelCase) = SIGNAL_INTERNAL_PRIVATE_CLASS_NAME(CamelCase); \
        SIGNAL_INTERNAL_CLASS_NAME(CamelCase)* snake_case##_new(void* instance); \
    } \
    \
    template<typename T> \
    class SIGNAL_CLASS_NAME(snake_case) : protected SignalComponent \
    { \
        private: \
            detail::SIGNAL_INTERNAL_CLASS_NAME(CamelCase)* _internal = nullptr;               \
            T* _instance = nullptr;                                                                        \
            \
            static return_t wrapper(void*, detail::SIGNAL_INTERNAL_CLASS_NAME(CamelCase)* internal) \
            { \
                if (G_IS_OBJECT(internal) and not internal->blocked and internal->function) \
                    return internal->function(internal->instance); \
                else \
                    return return_t(); \
            } \
            \
            void initialize()  \
            { \
                if (_internal == nullptr) \
                { \
                    _internal = detail::snake_case##_new((void*) _instance); \
                    detail::attach_ref_to(_instance->operator GObject*(), _internal);\
                    g_object_ref(_internal);                                                                    \
                }                                                                        \
            }\
        \
        protected: \
            SIGNAL_CLASS_NAME(snake_case)(T* instance) \
                : _instance(instance) \
            {} \
                                                                                    \
            ~SIGNAL_CLASS_NAME(snake_case)()                                        \
            {                                      \
                if (_internal != nullptr)                                           \
                    g_object_unref(_internal);\
            }\
        \
        public: \
            static inline constexpr const char* signal_id = g_signal_id; \
            \
            template<typename Function_t, typename Data_t> \
            void connect_signal_##snake_case(Function_t f, Data_t data) \
            { \
                initialize();                                                                    \
                _internal->function = [f, data](void* instance) -> return_t { \
                    return f((T*) instance, data); \
                }; \
                ((T*) _internal->instance)->connect_signal(signal_id, wrapper, _internal); \
            } \
            \
            template<typename Function_t> \
            void connect_signal_##snake_case(Function_t f) \
            { \
                initialize();                                                                    \
                _internal->function = [f](void* instance) -> return_t { \
                    return f((T*) instance); \
                }; \
                ((T*) _internal->instance)->connect_signal(signal_id, wrapper, _internal); \
            } \
            \
            void set_signal_##snake_case##_blocked(bool b) \
            { \
                initialize();                                                                    \
                _internal->blocked = b; \
            } \
            \
            bool get_signal_##snake_case##_blocked() const \
            { \
                initialize();                                                                    \
                return _internal->blocked; \
            } \
            \
            return_t emit_signal_##snake_case() \
            { \
                initialize();                                                                    \
                return wrapper(nullptr, _internal); \
            } \
            \
            void disconnect_signal_##snake_case() \
            { \
                initialize();                                                                    \
                ((T*) _internal->instance)->disconnect_signal(signal_id); \
            } \
    };

    #define DECLARE_SIGNAL_MANUAL(CamelCase, snake_case, CAPS_CASE, g_signal_id, return_t, arg_list, arg_name_only_list) \
    \
    namespace detail \
    {                                                                                                               \
        struct SIGNAL_INTERNAL_PRIVATE_CLASS_NAME(CamelCase)   : protected SignalComponent                                                 \
        {                                                                                                           \
            GObject parent;                                                                                         \
            void* instance;                                                                                         \
            std::function<return_t(void*, arg_list)> function;                                                             \
            bool blocked;\
        };                                                                                                           \
        using SIGNAL_INTERNAL_CLASS_NAME(CamelCase) = SIGNAL_INTERNAL_PRIVATE_CLASS_NAME(CamelCase); \
        SIGNAL_INTERNAL_CLASS_NAME(CamelCase)* snake_case##_new(void* instance); \
    }                                                                                                               \
                                                                                                                    \
    template<typename T>                                                                                            \
    class SIGNAL_CLASS_NAME(snake_case)                                                                             \
    {                                                                                                               \
        private:                                                                                                    \
            detail::SIGNAL_INTERNAL_CLASS_NAME(CamelCase)* _internal = nullptr;                                     \
            T* _instance = nullptr;                                                                                 \
                                                                                                                    \
            static return_t wrapper(void*, arg_list, detail::SIGNAL_INTERNAL_CLASS_NAME(CamelCase)* internal)  \
            {                                                                                                       \
                if (G_IS_OBJECT(internal) and not internal->blocked and internal->function) \
                    return internal->function(internal->instance, arg_name_only_list);                                        \
                else                                                                                                \
                    return return_t();\
            }                                                                                                       \
                                                                                                                    \
            void initialize()                                                                                       \
            {                                                                                                       \
                if (_internal == nullptr)                                                                           \
                {                                                                    \
                    _internal = detail::snake_case##_new((void*) _instance); \
                    detail::attach_ref_to(_instance->operator GObject*(), _internal);  \
                    g_object_ref(_internal);                                                                    \
                }       \
            }                                                                                                       \
                                                                                                                    \
        protected:                                                                                                  \
            SIGNAL_CLASS_NAME(snake_case)(T* instance)                                                              \
                : _instance(instance)                                                                               \
            {}                                                                                                      \
                                                                                                                    \
            ~SIGNAL_CLASS_NAME(snake_case)()                                                                        \
            {                                                                       \
                if (_internal != nullptr)                                                                           \
                    g_object_unref(_internal);\
            }                                                                                                       \
                                                                                                                    \
        public:                                                                                                     \
            static inline constexpr const char* signal_id = g_signal_id; \
                                                                                                                    \
            template<typename Function_t, typename Data_t>                                                          \
            void connect_signal_##snake_case(Function_t f, Data_t data)                                             \
            {                                                                                                       \
                initialize();                                                                                       \
                _internal->function = [f, data](void* instance, arg_list) -> return_t {                        \
                    return f((T*) instance, arg_name_only_list, data);                                                                                                    \
                };                                                                                                       \
                ((T*) _internal->instance)->connect_signal(signal_id, wrapper, _internal); \
            }                                                                                                            \
                                                                                                                         \
            template<typename Function_t>                                                                                \
            void connect_signal_##snake_case(Function_t f)                                                               \
            {                                                                                                            \
                initialize();                                                                                            \
                _internal->function = [f](void* instance, arg_list) -> return_t {                                        \
                    return f((T*) instance, arg_name_only_list);                                                                                                         \
                };                                                                                                       \
                ((T*) _internal->instance)->connect_signal(signal_id, wrapper, _internal); \
            }                                                                                                            \
                                                                                                                         \
            void set_signal_##snake_case##_blocked(bool b)                                                               \
            {                                                                                                            \
                initialize();                                                                                            \
                _internal->blocked = b;\
            }                                                                                                            \
                                                                                                                         \
            bool get_signal_##snake_case##_blocked() const                                                               \
            {                                                                                                            \
                initialize();                                                                                            \
                return _internal->blocked;\
            }                                                                                                            \
                                                                                                                         \
            return_t emit_signal_##snake_case(arg_list)                                                                  \
            {                                                                                                            \
                initialize();                                                                                            \
                return wrapper(nullptr, arg_name_only_list, _internal);\
            }                                                                                                            \
                                                                                                                         \
            void disconnect_signal_##snake_case()                                                                        \
            {                                                                                                            \
                initialize();                                                                                            \
                ((T*) _internal->instance)->disconnect_signal(signal_id);\
            }\
    }\

    /// @brief an argument of this type should not be interacted with
    using UnusedArgument_t = void*;
    
    /// @see
    DECLARE_SIGNAL(Activate, activate, ACTIVATE, "activate", void);
    /// @class has_signal_activate
    /// @brief \signal_activate_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_activate::connect_signal_activate(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_activate::connect_signal_activate(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_activate::emit_signal_activate()
    /// \signal_emit_brief
    ///
    /// @var has_signal_activate::signal_id
    /// \signal_id{activate}
    ///
    /// @fn void has_signal_activate::set_signal_activate_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_activate::get_signal_activate_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_activate::disconnect_signal_activate()
    /// \signal_disconnect
    ///
    /// @fn has_signal_activate::has_signal_activate
    /// \signal_ctor

    DECLARE_SIGNAL(Startup, startup, STARTUP, "startup", void);
    /// @class has_signal_startup
    /// @brief \signal_startup_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_startup::connect_signal_startup(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_startup::connect_signal_startup(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_startup::emit_signal_startup()
    /// \signal_emit_brief
    ///
    /// @var has_signal_startup::signal_id
    /// \signal_id{https://docs.gtk.org/gio/signal.Application.startup.html}
    ///
    /// @fn void has_signal_startup::set_signal_startup_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_startup::get_signal_startup_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_startup::disconnect_signal_startup()
    /// \signal_disconnect
    ///
    /// @fn has_signal_startup::has_signal_startup
    /// \signal_ctor

    DECLARE_SIGNAL(Shutdown, shutdown, SHUTDOWN, "shutdown", void);
    /// @class has_signal_shutdown
    /// @brief \signal_shutdown_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_shutdown::connect_signal_shutdown(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_shutdown::connect_signal_shutdown(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_shutdown::emit_signal_shutdown()
    /// \signal_emit_brief
    ///
    /// @fn void has_signal_shutdown::set_signal_shutdown_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_shutdown::get_signal_shutdown_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_shutdown::disconnect_signal_shutdown()
    /// \signal_disconnect
    ///
    /// @fn has_signal_shutdown::has_signal_shutdown
    /// \signal_ctor

    DECLARE_SIGNAL(Update, update, UPDATE, "update", void);
    /// @class has_signal_update
    /// @brief \signal_update_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_update::connect_signal_update(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_update::connect_signal_update(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_update::emit_signal_update()
    /// \signal_emit_brief
    ///
    /// @var has_signal_update::signal_id
    /// \signal_id{https://docs.gtk.org/gdk4/signal.FrameClock.update.html}
    ///
    /// @fn void has_signal_update::set_signal_update_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_update::get_signal_update_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_update::disconnect_signal_update()
    /// \signal_disconnect
    ///
    /// @fn has_signal_update::has_signal_update
    /// \signal_ctor

    DECLARE_SIGNAL(Paint, paint, PAINT, "paint", void);
    /// @class has_signal_paint
    /// @brief \signal_paint_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_paint::connect_signal_paint(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_paint::connect_signal_paint(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_paint::emit_signal_paint()
    /// \signal_emit_brief
    ///
    /// @var has_signal_paint::signal_id
    /// \signal_id{https://docs.gtk.org/gdk4/signal.FrameClock.paint.html}
    ///
    /// @fn void has_signal_paint::set_signal_paint_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_paint::get_signal_paint_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_paint::disconnect_signal_paint()
    /// \signal_disconnect
    ///
    /// @fn has_signal_paint::has_signal_paint
    /// \signal_ctor

    DECLARE_SIGNAL(Realize, realize, REALIZE, "realize", void);
    /// @class has_signal_realize
    /// @brief \signal_realize_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_realize::connect_signal_realize(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_realize::connect_signal_realize(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_realize::emit_signal_realize()
    /// \signal_emit_brief
    ///
    /// @var has_signal_realize::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Widget.realize.html}
    ///
    /// @fn void has_signal_realize::set_signal_realize_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_realize::get_signal_realize_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_realize::disconnect_signal_realize()
    /// \signal_disconnect
    ///
    /// @fn has_signal_realize::has_signal_realize
    /// \signal_ctor

    DECLARE_SIGNAL(Unrealize, unrealize, UNREALIZE, "unrealize", void);
    /// @class has_signal_unrealize
    /// @brief \signal_unrealize_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_unrealize::connect_signal_unrealize(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_unrealize::connect_signal_unrealize(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_unrealize::emit_signal_unrealize()
    /// \signal_emit_brief
    ///
    /// @fn void has_signal_unrealize::set_signal_unrealize_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_unrealize::get_signal_unrealize_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_unrealize::disconnect_signal_unrealize()
    /// \signal_disconnect
    ///
    /// @fn has_signal_unrealize::has_signal_unrealize
    /// \signal_ctor

    DECLARE_SIGNAL(Destroy, destroy, DESTROY, "destroy", void);
    /// @class has_signal_destroy
    /// @brief \signal_destroy_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_destroy::connect_signal_destroy(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_destroy::connect_signal_destroy(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_destroy::emit_signal_destroy()
    /// \signal_emit_brief
    ///
    /// @fn void has_signal_destroy::set_signal_destroy_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_destroy::get_signal_destroy_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_destroy::disconnect_signal_destroy()
    /// \signal_disconnect
    ///
    /// @fn has_signal_destroy::has_signal_destroy
    /// \signal_ctor

    DECLARE_SIGNAL(Hide, hide, HIDE, "hide", void);
    /// @class has_signal_hide
    /// @brief \signal_hide_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_hide::connect_signal_hide(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_hide::connect_signal_hide(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_hide::emit_signal_hide()
    /// \signal_emit_brief
    ///
    /// @var has_signal_hide::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Widget.hide.html}
    ///
    /// @fn void has_signal_hide::set_signal_hide_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_hide::get_signal_hide_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_hide::disconnect_signal_hide()
    /// \signal_disconnect
    ///
    /// @fn has_signal_hide::has_signal_hide
    /// \signal_ctor

    DECLARE_SIGNAL(Show, show, SHOW, "show", void);
    /// @class has_signal_show
    /// @brief \signal_show_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_show::connect_signal_show(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_show::connect_signal_show(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_show::emit_signal_show()
    /// \signal_emit_brief
    ///
    /// @var has_signal_show::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Widget.show.html}
    ///
    /// @fn void has_signal_show::set_signal_show_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_show::get_signal_show_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_show::disconnect_signal_show()
    /// \signal_disconnect
    ///
    /// @fn has_signal_show::has_signal_show
    /// \signal_ctor

    DECLARE_SIGNAL(Map, map, MAP, "map", void);
    /// @class has_signal_map
    /// @brief \signal_map_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_map::connect_signal_map(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_map::connect_signal_map(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_map::emit_signal_map()
    /// \signal_emit_brief
    ///
    /// @var has_signal_map::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Widget.map.html}
    ///
    /// @fn void has_signal_map::set_signal_map_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_map::get_signal_map_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_map::disconnect_signal_map()
    /// \signal_disconnect
    ///
    /// @fn has_signal_map::has_signal_map
    /// \signal_ctor

    DECLARE_SIGNAL(Unmap, unmap, UNMAP, "unmap", void);
    /// @class has_signal_unmap
    /// @brief \signal_unmap_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_unmap::connect_signal_unmap(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_unmap::connect_signal_unmap(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_unmap::emit_signal_unmap()
    /// \signal_emit_brief
    ///
    /// @var has_signal_unmap::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Widget.unmap.html}
    ///
    /// @fn void has_signal_unmap::set_signal_unmap_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_unmap::get_signal_unmap_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_unmap::disconnect_signal_unmap()
    /// \signal_disconnect
    ///
    /// @fn has_signal_unmap::has_signal_unmap
    /// \signal_ctor

    /// @brief Result of mousetrap::has_signal_close_request
    enum WindowCloseRequestResult : bool
    {
        /// @brief Allow the default handler to be invoked, this means that the window will close after the signal handler has exited
        ALLOW_CLOSE = false,

        /// @brief Prevent the default handler to be invoked, this means the window will stay open after the signal handler function has exited
        PREVENT_CLOSE = true
    };

    DECLARE_SIGNAL(CloseRequest, close_request, CLOSE_REQUEST, "close-request", WindowCloseRequestResult);
    /// @class has_signal_close_request
    /// @brief \signal_close_request_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_close_request::connect_signal_close_request(Function_t)
    /// \signal_connect{(T*) -> mousetrap::WindowCloseRequestResult}
    ///
    /// @fn void has_signal_close_request::connect_signal_close_request(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> mousetrap::WindowCloseRequestResult}
    ///
    /// @fn void has_signal_close_request::emit_signal_close_request()
    /// \signal_emit_brief
    ///
    /// @var has_signal_close_request::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Window.close-request.html}
    ///
    /// @fn void has_signal_close_request::set_signal_close_request_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_close_request::get_signal_close_request_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_close_request::disconnect_signal_close_request()
    /// \signal_disconnect
    ///
    /// @fn has_signal_close_request::has_signal_close_request
    /// \signal_ctor

    DECLARE_SIGNAL(ActivateDefaultWidget, activate_default_widget, ACTIVATE_DEFAULT_WIDGET, "activate-default", void);
    /// @class has_signal_activate_default_widget
    /// @brief \signal_activate_default_widget_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_activate_default_widget::connect_signal_activate_default_widget(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_activate_default_widget::connect_signal_activate_default_widget(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_activate_default_widget::emit_signal_activate_default_widget()
    /// \signal_emit_brief
    ///
    /// @var has_signal_activate_default_widget::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Window.activate-default.html}
    ///
    /// @fn void has_signal_activate_default_widget::set_signal_activate_default_widget_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_activate_default_widget::get_signal_activate_default_widget_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_activate_default_widget::disconnect_signal_activate_default_widget()
    /// \signal_disconnect
    ///
    /// @fn has_signal_activate_default_widget::has_signal_activate_default_widget
    /// \signal_ctor

    DECLARE_SIGNAL(ActivateFocusedWidget, activate_focused_widget, ACTIVATE_FOCUSED_WIDGET, "activate-focus", void);
    /// @class has_signal_activate_focused_widget
    /// @brief \signal_activate_focused_widget_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_activate_focused_widget::connect_signal_activate_focused_widget(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_activate_focused_widget::connect_signal_activate_focused_widget(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_activate_focused_widget::emit_signal_activate_focused_widget()
    /// \signal_emit_brief
    ///
    /// @var has_signal_activate_focused_widget::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Window.activate-focus.html}
    ///
    /// @fn void has_signal_activate_focused_widget::set_signal_activate_focused_widget_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_activate_focused_widget::get_signal_activate_focused_widget_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_activate_focused_widget::disconnect_signal_activate_focused_widget()
    /// \signal_disconnect
    ///
    /// @fn has_signal_activate_focused_widget::has_signal_activate_focused_widget
    /// \signal_ctor

    DECLARE_SIGNAL(Clicked, clicked, CLICKED, "clicked", void);
    /// @class has_signal_clicked
    /// @brief \signal_clicked_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_clicked::connect_signal_clicked(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_clicked::connect_signal_clicked(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_clicked::emit_signal_clicked()
    /// \signal_emit_brief
    ///
    /// @var has_signal_clicked::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Button.activate.html}
    ///
    /// @fn void has_signal_clicked::set_signal_clicked_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_clicked::get_signal_clicked_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_clicked::disconnect_signal_clicked()
    /// \signal_disconnect
    ///
    /// @fn has_signal_clicked::has_signal_clicked
    /// \signal_ctor

    DECLARE_SIGNAL(Toggled, toggled, TOGGLED, "toggled", void);
    /// @class has_signal_toggled
    /// @brief \signal_toggled_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_toggled::connect_signal_toggled(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_toggled::connect_signal_toggled(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_toggled::emit_signal_toggled()
    /// \signal_emit_brief
    ///
    /// @var has_signal_toggled::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.ToggleButton.toggled.html}
    ///
    /// @fn void has_signal_toggled::set_signal_toggled_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_toggled::get_signal_toggled_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_toggled::disconnect_signal_toggled()
    /// \signal_disconnect
    ///
    /// @fn has_signal_toggled::has_signal_toggled
    /// \signal_ctor

    DECLARE_SIGNAL(TextChanged, text_changed, TEXT_CHANGED, "changed", void);
    /// @class has_signal_text_changed
    /// @brief \signal_text_changed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_text_changed::connect_signal_text_changed(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_text_changed::connect_signal_text_changed(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_text_changed::emit_signal_text_changed()
    /// \signal_emit_brief
    ///
    /// @var has_signal_text_changed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Editable.changed.html}
    ///
    /// @fn void has_signal_text_changed::set_signal_text_changed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_text_changed::get_signal_text_changed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_text_changed::disconnect_signal_text_changed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_text_changed::has_signal_text_changed
    /// \signal_ctor

    DECLARE_SIGNAL(Undo, undo, UNDO, "undo", void);
    /// @class has_signal_undo
    /// @brief \signal_undo_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_undo::connect_signal_undo(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_undo::connect_signal_undo(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_undo::emit_signal_undo()
    /// \signal_emit_brief
    ///
    /// @var has_signal_undo::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.TextBuffer.undo.html}
    ///
    /// @fn void has_signal_undo::set_signal_undo_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_undo::get_signal_undo_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_undo::disconnect_signal_undo()
    /// \signal_disconnect
    ///
    /// @fn has_signal_undo::has_signal_undo
    /// \signal_ctor

    DECLARE_SIGNAL(Redo, redo, REDO, "redo", void);
    /// @class has_signal_redo
    /// @brief \signal_redo_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_redo::connect_signal_redo(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_redo::connect_signal_redo(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_redo::emit_signal_redo()
    /// \signal_emit_brief
    ///
    /// @var has_signal_redo::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.TextBuffer.redo.html}
    ///
    /// @fn void has_signal_redo::set_signal_redo_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_redo::get_signal_redo_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_redo::disconnect_signal_redo()
    /// \signal_disconnect
    ///
    /// @fn has_signal_redo::has_signal_redo
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(SelectionChanged, selection_changed, SELECTION_CHANGED, "selection-changed", void,
        SPLAT(int32_t position, int32_t n_items),
        SPLAT(position, n_items)
    );
    /// @class has_signal_selection_changed
    /// @brief \signal_selection_changed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_selection_changed::connect_signal_selection_changed(Function_t)
    /// \signal_connect{(T*, int32_t position, int32_t n_items) -> void}
    ///
    /// @fn void has_signal_selection_changed::connect_signal_selection_changed(Function_t, Data_t)
    /// \signal_connect_data{(T*, int32_t position, int32_t n_items, Data_t) -> void}
    ///
    /// @fn void has_signal_selection_changed::emit_signal_selection_changed(int32_t position, int32_t n_items)
    /// \signal_emit_brief
    /// @param position index of the element that changed
    /// @param n_items number of items that changed
    ///
    /// @var has_signal_selection_changed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.SelectionModel.selection-changed.html}
    ///
    /// @fn void has_signal_selection_changed::set_signal_selection_changed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_selection_changed::get_signal_selection_changed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_selection_changed::disconnect_signal_selection_changed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_selection_changed::has_signal_selection_changed
    /// \signal_ctor

    using ModifierState = GdkModifierType;
    using KeyValue = guint;
    using KeyCode = guint;

    DECLARE_SIGNAL_MANUAL(KeyPressed, key_pressed, KEY_PRESSED, "key-pressed", bool,
        SPLAT(KeyValue keyval, KeyCode keycode, ModifierState modifier),
        SPLAT(keyval, keycode, modifier)
    );
    /// @class has_signal_key_pressed
    /// @brief \signal_key_pressed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_key_pressed::connect_signal_key_pressed(Function_t)
    /// \signal_connect{(T*, KeyValue key_value, KeyCode key_code, ModifierState modifiers) -> bool}
    ///
    /// @fn void has_signal_key_pressed::connect_signal_key_pressed(Function_t, Data_t)
    /// \signal_connect_data{(T*, KeyValue key_value, KeyCode key_code, ModifierState modifiers, Data_t) -> bool}
    ///
    /// @fn void has_signal_key_pressed::emit_signal_key_pressed(KeyValue keyval, KeyCode keycode, ModifierState modifier)
    /// \signal_emit_brief
    /// @param keyval key identifier, a complete list of keys can be found at https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h
    /// @param keycode os-supplied internal identifier, can usually be ignored. Test against keyval instead
    /// @param modifier modifier state, can be queried to see whether Alt, Control, Shift, etc. are held down during this key event
    ///
    /// @var has_signal_key_pressed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerKey.key-pressed.html}
    ///
    /// @fn void has_signal_key_pressed::set_signal_key_pressed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_key_pressed::get_signal_key_pressed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_key_pressed::disconnect_signal_key_pressed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_key_pressed::has_signal_key_pressed
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(KeyReleased, key_released, KEY_RELEASED, "key-released", void,
        SPLAT(KeyValue keyval, KeyCode keycode, ModifierState modifier),
        SPLAT(keyval, keycode, modifier)
    );
    /// @class has_signal_key_released
    /// @brief \signal_key_released_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_key_released::connect_signal_key_released(Function_t)
    /// \signal_connect{(T*, KeyValue key_value, KeyCode key_code, ModifierState modifiers) -> void}
    ///
    /// @fn void has_signal_key_released::connect_signal_key_released(Function_t, Data_t)
    /// \signal_connect_data{(T*, KeyValue key_value, KeyCode key_code, ModifierState modifiers, Data_t) -> void}
    ///
    /// @fn void has_signal_key_released::emit_signal_key_released(KeyValue keyval, KeyCode keycode, ModifierState modifier)
    /// \signal_emit_brief
    /// @param keyval key identifier, a complete list of keys can be found at https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h
    /// @param keycode os-supplied internal identifier, can usually be ignored. Test against keyval instead
    /// @param modifier modifier state, can be queried to see whether Alt, Control, Shift, etc. are held down during this key event
    ///
    /// @var has_signal_key_released::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerKey.key-released.html}
    ///
    /// @fn void has_signal_key_released::set_signal_key_released_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_key_released::get_signal_key_released_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_key_released::disconnect_signal_key_released()
    /// \signal_disconnect
    ///
    /// @fn has_signal_key_released::has_signal_key_released
    /// \signal_ctor

    /// @see https://docs.gtk.org/gtk4/signal.EventControllerKey.key-pressed.html
    DECLARE_SIGNAL_MANUAL(ModifiersChanged, modifiers_changed, MODIFIERS_CHANGED, "modifiers", bool,
        SPLAT(KeyValue keyval, KeyCode keycode, ModifierState modifier),
        SPLAT(keyval, keycode, modifier)
    );
    /// @class has_signal_modifiers_changed
    /// @brief \signal_modifiers_changed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_modifiers_changed::connect_signal_modifiers_changed(Function_t)
    /// \signal_connect{(T*, KeyValue key_value, KeyCode key_code, ModifierState modifier) -> bool}
    ///
    /// @fn void has_signal_modifiers_changed::connect_signal_modifiers_changed(Function_t, Data_t)
    /// \signal_connect_data{(T* KeyValue key_value, KeyCode key_code, ModifierState modifier, Data_t) -> bool}
    ///
    /// @fn void has_signal_modifiers_changed::emit_signal_modifiers_changed(KeyValue keyval, KeyCode keycode, ModifierState modifier)
    /// \signal_emit_brief
    /// @param keyval key identifier, a complete list of keys can be found at https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h
    /// @param keycode os-supplied internal identifier, can usually be ignored. Test against keyval instead
    /// @param modifier modifier state, can be queried to see whether Alt, Control, Shift, etc. are held down during this key event
    ///
    /// @var has_signal_modifiers_changed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerKey.modifiers.html}
    ///
    /// @fn void has_signal_modifiers_changed::set_signal_modifiers_changed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_modifiers_changed::get_signal_modifiers_changed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_modifiers_changed::disconnect_signal_modifiers_changed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_modifiers_changed::has_signal_modifiers_changed
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(MotionEnter, motion_enter, MOTION_ENTER, "enter", void,
        SPLAT(double x, double y),
        SPLAT(x, y)
    );
    /// @class has_signal_motion_enter
    /// @brief \signal_motion_enter_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_motion_enter::connect_signal_motion_enter(Function_t)
    /// \signal_connect{(T*, double x, double y) -> void}
    ///
    /// @fn void has_signal_motion_enter::connect_signal_motion_enter(Function_t, Data_t)
    /// \signal_connect_data{(T*, double x, double y, Data_t) -> void}
    ///
    /// @fn void has_signal_motion_enter::emit_signal_motion_enter(double x, double y)
    /// \signal_emit_brief
    /// @param x x-coordinate of the cursor, widget-relative position, in pixels
    /// @param y y-coordinate of the cursor, widget-relative position, in pixels
    ///
    /// @var has_signal_motion_enter::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerMotion.enter.html}
    ///
    /// @fn void has_signal_motion_enter::set_signal_motion_enter_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_motion_enter::get_signal_motion_enter_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_motion_enter::disconnect_signal_motion_enter()
    /// \signal_disconnect
    ///
    /// @fn has_signal_motion_enter::has_signal_motion_enter
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Motion, motion, MOTION, "motion", void,
        SPLAT(double x, double y),
        SPLAT(x, y)
    );
    /// @class has_signal_motion
    /// @brief \signal_motion_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_motion::connect_signal_motion(Function_t)
    /// \signal_connect{(T*, double x, double y) -> void}
    ///
    /// @fn void has_signal_motion::connect_signal_motion(Function_t, Data_t)
    /// \signal_connect_data{(T*, double x, double y, Data_t) -> void}
    ///
    /// @fn void has_signal_motion::emit_signal_motion(double x, double y)
    /// \signal_emit_brief
    /// @param x x-coordinate of the cursor, widget-relative position, in pixels
    /// @param y y-coordinate of the cursor, widget-relative position, in pixels
    ///
    /// @var has_signal_motion::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerMotion.motion.html}
    ///
    /// @fn void has_signal_motion::set_signal_motion_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_motion::get_signal_motion_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_motion::disconnect_signal_motion()
    /// \signal_disconnect
    ///
    /// @fn has_signal_motion::has_signal_motion
    /// \signal_ctor

    DECLARE_SIGNAL(MotionLeave, motion_leave, MOTION_LEAVE, "leave", void);
    /// @class has_signal_motion_leave
    /// @brief \signal_motion_leave_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_motion_leave::connect_signal_motion_leave(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_motion_leave::connect_signal_motion_leave(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_motion_leave::emit_signal_motion_leave()
    /// \signal_emit_brief
    ///
    /// @var has_signal_motion_leave::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerMotion.leave.html}
    ///
    /// @fn void has_signal_motion_leave::set_signal_motion_leave_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_motion_leave::get_signal_motion_leave_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_motion_leave::disconnect_signal_motion_leave()
    /// \signal_disconnect
    ///
    /// @fn has_signal_motion_leave::has_signal_motion_leave
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(ClickPressed, click_pressed, CLICK_PRESSED, "pressed", void,
        SPLAT(int32_t n_press, double x, double y),
        SPLAT(n_press, x, y)
    );
    /// @class has_signal_click_pressed
    /// @brief \signal_click_pressed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_click_pressed::connect_signal_click_pressed(Function_t)
    /// \signal_connect{(T*, int32_t n_presses, double x, double y) -> void}
    ///
    /// @fn void has_signal_click_pressed::connect_signal_click_pressed(Function_t, Data_t)
    /// \signal_connect_data{(T*, int32_t n_presses, double x, double y, Data_t) -> void}
    ///
    /// @fn void has_signal_click_pressed::emit_signal_click_pressed(int32_t n_press, double x, double y)
    /// \signal_emit_brief
    /// @param n_press number of clicks in this sequence
    /// @param x x-coordinate of the cursor, widget-relative position, in pixels
    /// @param y y-coordinate of the cursor, widget-relative position, in pixels
    ///
    /// @var has_signal_click_pressed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureClick.pressed.html}
    ///
    /// @fn void has_signal_click_pressed::set_signal_click_pressed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_click_pressed::get_signal_click_pressed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_click_pressed::disconnect_signal_click_pressed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_click_pressed::has_signal_click_pressed
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(ClickReleased, click_released, CLICK_RELEASED, "released", void,
        SPLAT(int32_t n_press, double x, double y),
        SPLAT(n_press, x, y)
    );
    /// @class has_signal_click_released
    /// @brief \signal_click_released_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_click_released::connect_signal_click_released(Function_t)
    /// \signal_connect{(T*, int32_t n_presses, double x, double y) -> void}
    ///
    /// @fn void has_signal_click_released::connect_signal_click_released(Function_t, Data_t)
    /// \signal_connect_data{(T*, int32_t n_presses, double x, double y, Data_t) -> void}
    ///
    /// @fn void has_signal_click_released::emit_signal_click_released(int32_t n_press, double x, double y)
    /// \signal_emit_brief
    /// @param n_press number of clicks in this sequence
    /// @param x x-coordinate of the cursor, widget-relative position, in pixels
    /// @param y y-coordinate of the cursor, widget-relative position, in pixels
    ///
    /// @var has_signal_click_released::signal_id
    /// \signal_id{ https://docs.gtk.org/gtk4/signal.GestureClick.released.html}
    ///
    /// @fn void has_signal_click_released::set_signal_click_released_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_click_released::get_signal_click_released_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_click_released::disconnect_signal_click_released()
    /// \signal_disconnect
    ///
    /// @fn has_signal_click_released::has_signal_click_released
    /// \signal_ctor

    DECLARE_SIGNAL(ClickStopped, click_stopped, CLICK_STOPPED, "stopped", void);
    /// @class has_signal_click_stopped
    /// @brief \signal_click_stopped_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_click_stopped::connect_signal_click_stopped(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_click_stopped::connect_signal_click_stopped(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_click_stopped::emit_signal_click_stopped()
    /// \signal_emit_brief
    ///
    /// @var has_signal_click_stopped::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureClick.stopped.html}
    ///
    /// @fn void has_signal_click_stopped::set_signal_click_stopped_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_click_stopped::get_signal_click_stopped_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_click_stopped::disconnect_signal_click_stopped()
    /// \signal_disconnect
    ///
    /// @fn has_signal_click_stopped::has_signal_click_stopped
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(KineticScrollDecelerate, kinetic_scroll_decelerate, KINETIC_SCROLL_DECELERATE, "decelerate", void,
        SPLAT(double x_velocity, double y_velocity),
        SPLAT(x_velocity, y_velocity)
    );
    /// @class has_signal_kinetic_scroll_decelerate
    /// @brief \signal_kinetic_scroll_decelerate_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_kinetic_scroll_decelerate::connect_signal_kinetic_scroll_decelerate(Function_t)
    /// \signal_connect{(T*, double x_velocity, double y_velocity) -> void}
    ///
    /// @fn void has_signal_kinetic_scroll_decelerate::connect_signal_kinetic_scroll_decelerate(Function_t, Data_t)
    /// \signal_connect_data{(T*, double x_velocity, double y_velocity, Data_t) -> void}
    ///
    /// @fn void has_signal_kinetic_scroll_decelerate::emit_signal_kinetic_scroll_decelerate(double x_velocity, double y_velocity)
    /// \signal_emit_brief
    /// @param x_velocity current x-velocity, if postive, scroll left to right, if negative, scrolls right to left
    /// @param y_velocity current y-velocity, if positive, scrolls top to bottom, if negative, scrolls bottom to top
    ///
    /// @var has_signal_kinetic_scroll_decelerate::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerScroll.decelerate.html}
    ///
    /// @fn void has_signal_kinetic_scroll_decelerate::set_signal_kinetic_scroll_decelerate_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_kinetic_scroll_decelerate::get_signal_kinetic_scroll_decelerate_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_kinetic_scroll_decelerate::disconnect_signal_kinetic_scroll_decelerate()
    /// \signal_disconnect
    ///
    /// @fn has_signal_kinetic_scroll_decelerate::has_signal_kinetic_scroll_decelerate
    /// \signal_ctor

    DECLARE_SIGNAL(ScrollBegin, scroll_begin, SCROLL_BEGIN, "scroll-begin", void);
    /// @class has_signal_scroll_begin
    /// @brief \signal_scroll_begin_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_scroll_begin::connect_signal_scroll_begin(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_scroll_begin::connect_signal_scroll_begin(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_scroll_begin::emit_signal_scroll_begin()
    /// \signal_emit_brief
    ///
    /// @var has_signal_scroll_begin::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerScroll.scroll-begin.html}
    ///
    /// @fn void has_signal_scroll_begin::set_signal_scroll_begin_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_scroll_begin::get_signal_scroll_begin_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_scroll_begin::disconnect_signal_scroll_begin()
    /// \signal_disconnect
    ///
    /// @fn has_signal_scroll_begin::has_signal_scroll_begin
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Scroll, scroll, SCROLL, "scroll", bool,
       SPLAT(double delta_x, double delta_y),
       SPLAT(delta_x, delta_y)
    );
    /// @class has_signal_scroll
    /// @brief \signal_scroll_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_scroll::connect_signal_scroll(Function_t)
    /// \signal_connect{(T*, double delta_x, double delta_y) -> void}
    ///
    /// @fn void has_signal_scroll::connect_signal_scroll(Function_t, Data_t)
    /// \signal_connect_data{(T*, double delta_x, double delta_y, Data_t) -> void}
    ///
    /// @fn void has_signal_scroll::emit_signal_scroll(double delta_x, double delta_y)
    /// \signal_emit_brief
    /// @param delta_x difference of horizontal scroll offsets between this and previous frame. If positive, scroll left to right, if negative, scroll right to left
    /// @param delta_y difference of vertical scroll offsets between this and previous frame. If positive, scroll top to bottom, if negative, scroll bottom to top
    ///
    /// @var has_signal_scroll::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerScroll.scroll.html}
    ///
    /// @fn void has_signal_scroll::set_signal_scroll_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_scroll::get_signal_scroll_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_scroll::disconnect_signal_scroll()
    /// \signal_disconnect
    ///
    /// @fn has_signal_scroll::has_signal_scroll
    /// \signal_ctor

    DECLARE_SIGNAL(ScrollEnd, scroll_end, SCROLL_END, "scroll-end", void);
    /// @class has_signal_scroll_end
    /// @brief \signal_scroll_end_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_scroll_end::connect_signal_scroll_end(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_scroll_end::connect_signal_scroll_end(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_scroll_end::emit_signal_scroll_end()
    /// \signal_emit_brief
    ///
    /// @var has_signal_scroll_end::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerScroll.scroll-begin.html}
    ///
    /// @fn void has_signal_scroll_end::set_signal_scroll_end_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_scroll_end::get_signal_scroll_end_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_scroll_end::disconnect_signal_scroll_end()
    /// \signal_disconnect
    ///
    /// @fn has_signal_scroll_end::has_signal_scroll_end
    /// \signal_ctor

    DECLARE_SIGNAL(FocusGained, focus_gained, FOCUS_GAINED, "enter", void);
    /// @class has_signal_focus_gained
    /// @brief \signal_focus_gained_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_focus_gained::connect_signal_focus_gained(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_focus_gained::connect_signal_focus_gained(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    //
    /// @fn void has_signal_focus_gained::emit_signal_focus_gained()
    /// \signal_emit_brief
    ///
    /// @var has_signal_focus_gained::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerFocus.enter.html}
    ///
    /// @fn void has_signal_focus_gained::set_signal_focus_gained_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_focus_gained::get_signal_focus_gained_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_focus_gained::disconnect_signal_focus_gained()
    /// \signal_disconnect
    ///
    /// @fn has_signal_focus_gained::has_signal_focus_gained
    /// \signal_ctor

    DECLARE_SIGNAL(FocusLost, focus_lost, FOCUS_LOST, "leave", void);
    /// @class has_signal_focus_lost
    /// @brief \signal_focus_lost_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_focus_lost::connect_signal_focus_lost(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_focus_lost::connect_signal_focus_lost(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_focus_lost::emit_signal_focus_lost()
    /// \signal_emit_brief
    ///
    /// @var has_signal_focus_lost::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.EventControllerFocus.leave.html}
    ///
    /// @fn void has_signal_focus_lost::set_signal_focus_lost_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_focus_lost::get_signal_focus_lost_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_focus_lost::disconnect_signal_focus_lost()
    /// \signal_disconnect
    ///
    /// @fn has_signal_focus_lost::has_signal_focus_lost
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(DragBegin, drag_begin, DRAG_BEGIN, "drag-begin", void,
        SPLAT(double start_x, double start_y),
        SPLAT(start_x, start_y)
    );
    /// @class has_signal_drag_begin
    /// @brief \signal_drag_begin_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_drag_begin::connect_signal_drag_begin(Function_t)
    /// \signal_connect{(T*, double start_x, double start_y) -> void}
    ///
    /// @fn void has_signal_drag_begin::connect_signal_drag_begin(Function_t, Data_t)
    /// \signal_connect_data{(T*, double start_x, double start_y, Data_t) -> void}
    ///
    /// @fn void has_signal_drag_begin::emit_signal_drag_begin(double start_x, double start_y)
    /// \signal_emit_brief
    /// @param start_x x-position of the cursor, widget-relative coordinates, in pixels
    /// @param start_y y-position of the cursor, widget-relative coordiantes, in pixels
    ///
    /// @var has_signal_drag_begin::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureDrag.drag-begin.html}
    ///
    /// @fn void has_signal_drag_begin::set_signal_drag_begin_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_drag_begin::get_signal_drag_begin_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_drag_begin::disconnect_signal_drag_begin()
    /// \signal_disconnect
    ///
    /// @fn has_signal_drag_begin::has_signal_drag_begin
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Drag, drag, DRAG, "drag-update", void,
        SPLAT(double offset_x, double offset_y),
        SPLAT(offset_x, offset_y)
    );
    /// @class has_signal_drag
    /// @brief \signal_drag_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_drag::connect_signal_drag(Function_t)
    /// \signal_connect{(T*, double offset_x, double offset_y) -> void}
    ///
    /// @fn void has_signal_drag::connect_signal_drag(Function_t, Data_t)
    /// \signal_connect_data{(T*, double offset_x, double offset_y, Data_t) -> void}
    ///
    /// @fn void has_signal_drag::emit_signal_drag(double offset_x, double offset_y)
    /// \signal_emit_brief
    /// @param offset_x horizontal offset from drag start point, widget-relative coordinates, in pixels
    /// @param offset_y vertical offset from drag start point, widget-relative coordinates, in pixels
    ///
    /// @var has_signal_drag::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureDrag.drag-update.html}
    ///
    /// @fn void has_signal_drag::set_signal_drag_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_drag::get_signal_drag_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_drag::disconnect_signal_drag()
    /// \signal_disconnect
    ///
    /// @fn has_signal_drag::has_signal_drag
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(DragEnd, drag_end, DRAG_END, "drag-end", void,
        SPLAT(double offset_x, double offset_y),
        SPLAT(offset_x, offset_y)
    );
    /// @class has_signal_drag_end
    /// @brief \signal_drag_end_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_drag_end::connect_signal_drag_end(Function_t)
    /// \signal_connect{(T*, double offset_x, double offset_y) -> void}
    ///
    /// @fn void has_signal_drag_end::connect_signal_drag_end(Function_t, Data_t)
    /// \signal_connect_data{(T*, double offset_x, double offset_y, Data_t) -> void}
    ///
    /// @fn void has_signal_drag_end::emit_signal_drag_end(double offset_x, double offset_y)
    /// \signal_emit_brief
    /// @param offset_x horizontal offset from drag start point, widget-relative coordinates, in pixels
    /// @param offset_y vertical offset from drag start point, widget-relative coordinates, in pixels
    ///
    /// @var has_signal_drag_end::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureDrag.drag-end.html}
    ///
    /// @fn void has_signal_drag_end::set_signal_drag_end_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_drag_end::get_signal_drag_end_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_drag_end::disconnect_signal_drag_end()
    /// \signal_disconnect
    ///
    /// @fn has_signal_drag_end::has_signal_drag_end
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(ScaleChanged, scale_changed, SCALE_CHANGED, "scale-changed", void,
        double scale,
        scale
    );
    /// @class has_signal_scale_changed
    /// @brief \signal_scale_changed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_scale_changed::connect_signal_scale_changed(Function_t)
    /// \signal_connect{(T*, double scale) -> void}
    ///
    /// @fn void has_signal_scale_changed::connect_signal_scale_changed(Function_t, Data_t)
    /// \signal_connect_data{(T*, double scale, Data_t) -> void}
    ///
    /// @fn void has_signal_scale_changed::emit_signal_scale_changed(double scale)
    /// \signal_emit_brief
    /// @param scale difference of current scale to scale at the start of the gesture
    ///
    /// @var has_signal_scale_changed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureZoom.scale-changed.html}
    ///
    /// @fn void has_signal_scale_changed::set_signal_scale_changed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_scale_changed::get_signal_scale_changed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_scale_changed::disconnect_signal_scale_changed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_scale_changed::has_signal_scale_changed
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(RotationChanged, rotation_changed, ROTATION_CHANGED, "angle-changed", void,
        SPLAT(double angle_absolute_radians, double angle_delta_radians),
        SPLAT(angle_absolute_radians, angle_delta_radians)
    );
    /// @class has_signal_rotation_changed
    /// @brief \signal_rotation_changed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_rotation_changed::connect_signal_rotation_changed(Function_t)
    /// \signal_connect{(T*, double angle_absolute_radians, double angle_delta_radians) -> void}
    ///
    /// @fn void has_signal_rotation_changed::connect_signal_rotation_changed(Function_t, Data_t)
    /// \signal_connect_data{(T*, double angle_absolute_radians, double angle_delta_radians, Data_t) -> void}
    ///
    /// @fn void has_signal_rotation_changed::emit_signal_rotation_changed(double angle_absolute_radians, double angle_delta_radians)
    /// \signal_emit_brief
    /// @param angle_absolute_radians current angle relative to widget coordinate origin, in radians
    /// @param angle_delta_radians difference between the angle this frame and that of the last frame
    ///
    /// @var has_signal_rotation_changed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureRotate.angle-changed.html}
    ///
    /// @fn void has_signal_rotation_changed::set_signal_rotation_changed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_rotation_changed::get_signal_rotation_changed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_rotation_changed::disconnect_signal_rotation_changed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_rotation_changed::has_signal_rotation_changed
    /// \signal_ctor

    DECLARE_SIGNAL(PropertiesChanged, properties_changed, PROPERTIES_CHANGED, "changed", void);
    /// @class has_signal_properties_changed
    /// @brief \signal_properties_changed_brief
    /// @tparam T instance type
    /// @see mousetrap::has_signal_value_changed
    ///
    /// @fn void has_signal_properties_changed::connect_signal_properties_changed(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_properties_changed::connect_signal_properties_changed(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_properties_changed::emit_signal_properties_changed()
    /// \signal_emit_brief
    ///
    /// @var has_signal_properties_changed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Adjustment.changed.html}
    ///
    /// @fn void has_signal_properties_changed::set_signal_properties_changed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_properties_changed::get_signal_properties_changed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_properties_changed::disconnect_signal_properties_changed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_properties_changed::has_signal_properties_changed
    /// \signal_ctor

    DECLARE_SIGNAL(ValueChanged, value_changed, VALUE_CHANGED, "value-changed", void);
    /// @class has_signal_value_changed
    /// @brief \signal_value_changed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_value_changed::connect_signal_value_changed(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_value_changed::connect_signal_value_changed(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_value_changed::emit_signal_value_changed()
    /// \signal_emit_brief
    ///
    /// @var has_signal_value_changed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Adjustment.value-changed.html}
    ///
    /// @fn void has_signal_value_changed::set_signal_value_changed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_value_changed::get_signal_value_changed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_value_changed::disconnect_signal_value_changed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_value_changed::has_signal_value_changed
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Render, render, RENDER, "render", bool, GdkGLContext* context, context);
    /// @class has_signal_render
    /// @brief \signal_render_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_render::connect_signal_render(Function_t)
    /// \signal_connect{(T*, GdkGLContext* context) -> bool}
    ///
    /// @fn void has_signal_render::connect_signal_render(Function_t, Data_t)
    /// \signal_connect_data{(T*, GdkGLContext* context, Data_t) -> bool}
    ///
    /// @fn void has_signal_render::emit_signal_render()
    /// \signal_emit_brief
    ///
    /// @var has_signal_render::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GLArea.render.html}
    ///
    /// @fn void has_signal_render::set_signal_render_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_render::get_signal_render_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_render::disconnect_signal_render()
    /// \signal_disconnect
    ///
    /// @fn has_signal_render::has_signal_render
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Resize, resize, RESIZE, "resize", void,
        SPLAT(int32_t width, int32_t height),
        SPLAT(width, height)
    );
    /// @class has_signal_resize
    /// @brief \signal_resize_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_resize::connect_signal_resize(Function_t)
    /// \signal_connect{(T*, int32_t width, int32_t height) -> void}
    ///
    /// @fn void has_signal_resize::connect_signal_resize(Function_t, Data_t)
    /// \signal_connect_data{(T*, int32_t width, int32_t height, Data_t) -> void}
    ///
    /// @fn void has_signal_resize::emit_signal_resize(int32_t width, int32_t height)
    /// \signal_emit_brief
    /// @param width absolute width, in pixels
    /// @param height absolute height, in pixels
    ///
    /// @var has_signal_resize::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GLArea.resize.html}
    ///
    /// @fn void has_signal_resize::set_signal_resize_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_resize::get_signal_resize_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_resize::disconnect_signal_resize()
    /// \signal_disconnect
    ///
    /// @fn has_signal_resize::has_signal_resize
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(PageAdded, page_added, PAGE_ADDED, "page-added", void,
        SPLAT(UnusedArgument_t _, uint32_t page_index),
        SPLAT(_, page_index)
    );
    /// @class has_signal_page_added
    /// @brief \signal_page_added_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_page_added::connect_signal_page_added(Function_t)
    /// \signal_connect{(T*, UnusedArgument_t _, uint32_t page_index) -> void}
    ///
    /// @fn void has_signal_page_added::connect_signal_page_added(Function_t, Data_t)
    /// \signal_connect_data{(T*, UnusedArgument_t _, uint32_t page_index, Data_t) -> void}
    ///
    /// @fn void has_signal_page_added::emit_signal_page_added(UnusedArgument_t _, uint32_t page_index)
    /// \signal_emit_brief
    /// @param _ pointer to internal widget, can be ignored
    /// @param page_index index of the new page
    ///
    /// @var has_signal_page_added::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Notebook.page-added.html}
    ///
    /// @fn void has_signal_page_added::set_signal_page_added_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_page_added::get_signal_page_added_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_page_added::disconnect_signal_page_added()
    /// \signal_disconnect
    ///
    /// @fn has_signal_page_added::has_signal_page_added
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(PageRemoved, page_removed, PAGE_REMOVED, "page-removed", void,
        SPLAT(UnusedArgument_t _, uint32_t page_index),
        SPLAT(_, page_index)
    );
    /// @class has_signal_page_removed
    /// @brief \signal_page_removed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_page_removed::connect_signal_page_removed(Function_t)
    /// \signal_connect{(T*, UnusedArgument_t _, uint32_t page_index) -> void}
    ///
    /// @fn void has_signal_page_removed::connect_signal_page_removed(Function_t, Data_t)
    /// \signal_connect_data{(T*, UnusedArgument_t _, uint32_t page_index, Data_t) -> void}
    ///
    /// @fn void has_signal_page_removed::emit_signal_page_removed(UnusedArgument_t _, uint32_t page_index)
    /// \signal_emit_brief
    /// @param _ pointer to internal widget, can be ignored
    /// @param page_index index of the new page
    ///
    /// @var has_signal_page_removed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Notebook.page-removed.html}
    ///
    /// @fn void has_signal_page_removed::set_signal_page_removed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_page_removed::get_signal_page_removed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_page_removed::disconnect_signal_page_removed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_page_removed::has_signal_page_removed
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(PageReordered, page_reordered, PAGE_REORDERED, "page-reordered", void,
        SPLAT(UnusedArgument_t _, uint32_t page_index),
        SPLAT(_, page_index)
    );
    /// @class has_signal_page_reordered
    /// @brief \signal_page_reordered_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_page_reordered::connect_signal_page_reordered(Function_t)
    /// \signal_connect{(T*, UnusedArgument_t _, uint32_t page_index) -> void}
    ///
    /// @fn void has_signal_page_reordered::connect_signal_page_reordered(Function_t, Data_t)
    /// \signal_connect_data{(T*, UnusedArgument_t _, uint32_t page_index, Data_t) -> void}
    ///
    /// @fn void has_signal_page_reordered::emit_signal_page_reordered(UnusedArgument_t _, uint32_t page_index)
    /// \signal_emit_brief
    /// @param _ pointer to internal widget, can be ignored
    /// @param page_index index of the new page
    ///
    /// @var has_signal_page_reordered::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Notebook.page-reordered.html}
    ///
    /// @fn void has_signal_page_reordered::set_signal_page_reordered_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_page_reordered::get_signal_page_reordered_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_page_reordered::disconnect_signal_page_reordered()
    /// \signal_disconnect
    ///
    /// @fn has_signal_page_reordered::has_signal_page_reordered
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(PageSelectionChanged, page_selection_changed, PAGE_SELECTION_CHANGED, "switch-page", void,
        SPLAT(UnusedArgument_t _, uint32_t page_index),
        SPLAT(_, page_index)
    );
    /// @class has_signal_page_selection_changed
    /// @brief \signal_page_selection_changed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_page_selection_changed::connect_signal_page_selection_changed(Function_t)
    /// \signal_connect{(T*, UnusedArgument_t _, uint32_t page_index) -> void}
    ///
    /// @fn void has_signal_page_selection_changed::connect_signal_page_selection_changed(Function_t, Data_t)
    /// \signal_connect_data{(T*, UnusedArgument_t _, uint32_t page_index, Data_t) -> void}
    ///
    /// @fn void has_signal_page_selection_changed::emit_signal_page_selection_changed(GtkWidget* _, uint32_t page_index)
    /// \signal_emit_brief
    /// @param _ pointer to internal widget, can be ignored
    /// @param page_index index of the newly selected page
    ///
    /// @var has_signal_page_selection_changed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Notebook.switch-page.html}
    ///
    /// @fn void has_signal_page_selection_changed::set_signal_page_selection_changed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_page_selection_changed::get_signal_page_selection_changed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_page_selection_changed::disconnect_signal_page_selection_changed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_page_selection_changed::has_signal_page_selection_changed
    /// \signal_ctor

    DECLARE_SIGNAL(Wrapped, wrapped, WRAPPED, "wrapped", void);
    /// @class has_signal_wrapped
    /// @brief \signal_wrapped_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_wrapped::connect_signal_wrapped(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_wrapped::connect_signal_wrapped(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_wrapped::emit_signal_wrapped()
    /// \signal_emit_brief
    ///
    /// @var has_signal_wrapped::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.SpinButton.wrapped.html}
    ///
    /// @fn void has_signal_wrapped::set_signal_wrapped_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_wrapped::get_signal_wrapped_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_wrapped::disconnect_signal_wrapped()
    /// \signal_disconnect
    ///
    /// @fn has_signal_wrapped::has_signal_wrapped
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Response, response, RESPONSE, "response", void, int response, response);
    /// @class has_signal_response
    /// @brief \signal_response_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_response::connect_signal_response(Function_t)
    /// \signal_connect{(T*, int response_id) -> void}
    ///
    /// @fn void has_signal_response::connect_signal_response(Function_t, Data_t)
    /// \signal_connect_data{(T*, int response_id, Data_t) -> void}
    ///
    /// @fn void has_signal_response::emit_signal_response(int response)
    /// \signal_emit_brief
    /// @param response response id as int
    ///
    /// @var has_signal_response::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.NativeDialog.response.html}
    ///
    /// @fn void has_signal_response::set_signal_response_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_response::get_signal_response_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_response::disconnect_signal_response()
    /// \signal_disconnect
    ///
    /// @fn has_signal_response::has_signal_response
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Pressed, pressed, PRESSED, "pressed", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    );
    /// @class has_signal_pressed
    /// @brief \signal_pressed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_pressed::connect_signal_pressed(Function_t)
    /// \signal_connect{(T*, double x, double y) -> void}
    ///
    /// @fn void has_signal_pressed::connect_signal_pressed(Function_t, Data_t)
    /// \signal_connect_data{(T*, double x, double y, Data_t) -> void}
    ///
    /// @fn void has_signal_pressed::emit_signal_pressed(int32_t n_press, double x, double y)
    /// \signal_emit_brief
    /// @param x x-coordinate of the cursor, widget-relative position, in pixels
    /// @param y y-coordinate of the cursor, widget-relative position, in pixels
    ///
    /// @var has_signal_pressed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureClick.pressed.html}
    ///
    /// @fn void has_signal_pressed::set_signal_pressed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_pressed::get_signal_pressed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_pressed::disconnect_signal_pressed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_pressed::has_signal_pressed
    /// \signal_ctor

    DECLARE_SIGNAL(PressCancelled, press_cancelled, PRESS_CANCELLED, "cancelled", void);
    /// @class has_signal_press_cancelled
    /// @brief \signal_press_cancelled_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_press_cancelled::connect_signal_press_cancelled(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_press_cancelled::connect_signal_press_cancelled(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_press_cancelled::emit_signal_press_cancelled()
    /// \signal_emit_brief
    ///
    /// @var has_signal_press_cancelled::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureLongPress.cancelled.html}
    ///
    /// @fn void has_signal_press_cancelled::set_signal_press_cancelled_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_press_cancelled::get_signal_press_cancelled_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_press_cancelled::disconnect_signal_press_cancelled()
    /// \signal_disconnect
    ///
    /// @fn has_signal_press_cancelled::has_signal_press_cancelled
    /// \signal_ctor

    /// @brief Touch-screen pan direction
    enum class PanDirection
    {
        /// @brief pan right to left
        LEFT = GTK_PAN_DIRECTION_LEFT,

        /// @brief pan left to right
        RIGHT = GTK_PAN_DIRECTION_RIGHT,

        /// @brief pan bottom to top
        TOP = GTK_PAN_DIRECTION_UP,

        /// @brief pan top to bottom
        DOWN = GTK_PAN_DIRECTION_DOWN
    };

    DECLARE_SIGNAL_MANUAL(Pan, pan, PAN, "pan", void,
          SPLAT(PanDirection direction, double offset),
          SPLAT(direction, offset)
    );
    /// @class has_signal_pan
    /// @brief \signal_pan_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_pan::connect_signal_pan(Function_t)
    /// \signal_connect{(T*, PanDirection direction, double offset) -> void}
    ///
    /// @fn void has_signal_pan::connect_signal_pan(Function_t, Data_t)
    /// \signal_connect_data{(T*, PanDirection direction, double offset, Data_t) -> void}
    ///
    /// @fn void has_signal_pan::emit_signal_pan(int32_t n_press, double x, double y)
    /// \signal_emit_brief
    /// @param direction pan direction
    /// @param offset offset from starting position, in pixels
    ///
    /// @var has_signal_pan::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureClick.pan.html}
    ///
    /// @fn void has_signal_pan::set_signal_pan_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_pan::get_signal_pan_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_pan::disconnect_signal_pan()
    /// \signal_disconnect
    ///
    /// @fn has_signal_pan::has_signal_pan
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Swipe, swipe, SWIPE, "swipe", void,
                          SPLAT(double x_velocity, double y_velocity),
                          SPLAT(x_velocity, y_velocity)
    );
    /// @class has_signal_swipe
    /// @brief \signal_swipe_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_swipe::connect_signal_swipe(Function_t)
    /// \signal_connect{(T*, double x_velocity, double y_velocity) -> void}
    ///
    /// @fn void has_signal_swipe::connect_signal_swipe(Function_t, Data_t)
    /// \signal_connect_data{(T*, double x_velocity, double y_velocity, Data_t) -> void}
    ///
    /// @fn void has_signal_swipe::emit_signal_swipe(int32_t n_press, double x, double y)
    /// \signal_emit_brief
    /// @param x_velocity speed along the x-axis
    /// @param y_velocity speed along the y-axis
    ///
    /// @var has_signal_swipe::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureClick.swipe.html}
    ///
    /// @fn void has_signal_swipe::set_signal_swipe_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_swipe::get_signal_swipe_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_swipe::disconnect_signal_swipe()
    /// \signal_disconnect
    ///
    /// @fn has_signal_swipe::has_signal_swipe
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(StylusDown, stylus_down, STYLUS_DOWN, "down", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    );
    /// @class has_signal_stylus_down
    /// @brief \signal_stylus_down_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_stylus_down::connect_signal_stylus_down(Function_t)
    /// \signal_connect{(T*, double x, double y) -> void}
    ///
    /// @fn void has_signal_stylus_down::connect_signal_stylus_down(Function_t, Data_t)
    /// \signal_connect_data{(T*, double x, double y, Data_t) -> void}
    ///
    /// @fn void has_signal_stylus_down::emit_signal_stylus_down(double x, double y)
    /// \signal_emit_brief
    /// @param x x-coordinate of the cursor, widget-relative position, in pixels
    /// @param y y-coordinate of the cursor, widget-relative position, in pixels
    ///
    /// @var has_signal_stylus_down::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureClick.stylus_down.htmlhttps://docs.gtk.org/gtk4/signal.GestureStylus.down.html}
    ///
    /// @fn void has_signal_stylus_down::set_signal_stylus_down_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_stylus_down::get_signal_stylus_down_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_stylus_down::disconnect_signal_stylus_down()
    /// \signal_disconnect
    ///
    /// @fn has_signal_stylus_down::has_signal_stylus_down
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(StylusUp, stylus_up, STYLUS_UP, "up", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    );
    /// @class has_signal_stylus_up
    /// @brief \signal_stylus_up_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_stylus_up::connect_signal_stylus_up(Function_t)
    /// \signal_connect{(T*, double x, double y) -> void}
    ///
    /// @fn void has_signal_stylus_up::connect_signal_stylus_up(Function_t, Data_t)
    /// \signal_connect_data{(T*, double x, double y, Data_t) -> void}
    ///
    /// @fn void has_signal_stylus_up::emit_signal_stylus_up(double x, double y)
    /// \signal_emit_brief
    /// @param x x-coordinate of the cursor, widget-relative position, in pixels
    /// @param y y-coordinate of the cursor, widget-relative position, in pixels
    ///
    /// @var has_signal_stylus_up::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureClick.stylus_up.htmlhttps://docs.gtk.org/gtk4/signal.GestureStylus.up.html}
    ///
    /// @fn void has_signal_stylus_up::set_signal_stylus_up_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_stylus_up::get_signal_stylus_up_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_stylus_up::disconnect_signal_stylus_up()
    /// \signal_disconnect
    ///
    /// @fn has_signal_stylus_up::has_signal_stylus_up
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Proximity, proximity, PROXIMITY, "proximity", void,
                          SPLAT(double x, double y),
                          SPLAT(x, y)
    );
    /// @class has_signal_proximity
    /// @brief \signal_proximity_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_proximity::connect_signal_proximity(Function_t)
    /// \signal_connect{(T*, double x, double y) -> void}
    ///
    /// @fn void has_signal_proximity::connect_signal_proximity(Function_t, Data_t)
    /// \signal_connect_data{(T*, double x, double y, Data_t) -> void}
    ///
    /// @fn void has_signal_proximity::emit_signal_proximity(double x, double y)
    /// \signal_emit_brief
    /// @param x x-coordinate of the cursor, widget-relative position, in pixels
    /// @param y y-coordinate of the cursor, widget-relative position, in pixels
    ///
    /// @var has_signal_proximity::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureClick.proximity.htmlhttps://docs.gtk.org/gtk4/signal.GestureStylus.up.html}
    ///
    /// @fn void has_signal_proximity::set_signal_proximity_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_proximity::get_signal_proximity_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_proximity::disconnect_signal_proximity()
    /// \signal_disconnect
    ///
    /// @fn has_signal_proximity::has_signal_proximity
    /// \signal_ctor

    /// @brief type of scroll, used by mousetrap::ScrolledWindow `scroll_child` signal
    enum class ScrollType
    {
        /// @brief no scrolling
        NONE = GTK_SCROLL_NONE,

        /// @brief jumped to new location
        JUMP = GTK_SCROLL_JUMP,

        /// @brief scroll step backwards
        STEP_BACKWARD = GTK_SCROLL_STEP_BACKWARD,

        /// @brief scroll step forwards
        STEP_FORWARD = GTK_SCROLL_STEP_FORWARD,

        /// @brief scroll step up
        STEP_UP = GTK_SCROLL_STEP_UP,

        /// @brief scroll step down
        STEP_DOWN = GTK_SCROLL_STEP_DOWN,

        /// @brief scroll step left
        STEP_LEFT = GTK_SCROLL_STEP_LEFT,

        /// @brief scroll step right
        STEP_RIGHT = GTK_SCROLL_STEP_RIGHT,

        /// @brief page step backwards
        PAGE_BACKWARD = GTK_SCROLL_PAGE_BACKWARD,

        /// @brief page step forwards
        PAGE_FORWARD = GTK_SCROLL_PAGE_FORWARD,

        /// @brief page step up
        PAGE_UP = GTK_SCROLL_PAGE_UP,

        /// @brief page step down
        PAGE_DOWN = GTK_SCROLL_PAGE_DOWN,

        /// @brief page step left
        PAGE_LEFT = GTK_SCROLL_PAGE_LEFT,

        /// @brief page step right
        PAGE_RIGHT = GTK_SCROLL_PAGE_RIGHT,

        /// @brief jump to start
        SCROLL_START = GTK_SCROLL_START,

        /// @brief jump to end
        SCROLL_END = GTK_SCROLL_END
    };

    DECLARE_SIGNAL_MANUAL(ScrollChild, scroll_child, SCROLL_CHILD, "scroll-child", void,
          SPLAT(ScrollType scroll_type, bool is_horizontal),
          SPLAT(scroll_type, is_horizontal)
    );
    /// @class has_signal_scroll_child
    /// @brief \signal_scroll_child_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_scroll_child::connect_signal_scroll_child(Function_t)
    /// \signal_connect{(T*, ScrollType scroll_type, bool is_horizontal) -> void}
    ///
    /// @fn void has_signal_scroll_child::connect_signal_scroll_child(Function_t, Data_t)
    /// \signal_connect_data{(T*, ScrollType scroll_type, bool is_horizontal, Data_t) -> void}
    ///
    /// @fn void has_signal_scroll_child::emit_signal_scroll_child(ScrollType scroll_type, bool is_horizontal)
    /// \signal_emit_brief
    /// @param scroll_type type of scroll action
    /// @param is_horizontal whether the scroll action was invoked horizontally or vertically
    ///
    /// @var has_signal_scroll_child::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.GestureClick.scroll_child.html}
    ///
    /// @fn void has_signal_scroll_child::set_signal_scroll_child_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_scroll_child::get_signal_scroll_child_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_scroll_child::disconnect_signal_scroll_child()
    /// \signal_disconnect
    ///
    /// @fn has_signal_scroll_child::has_signal_scroll_child
    /// \signal_ctor

    DECLARE_SIGNAL(Closed, closed, CLOSED, "closed", void);
    /// @class has_signal_closed
    /// @brief \signal_closed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_closed::connect_signal_closed(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_closed::connect_signal_closed(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_closed::emit_signal_closed()
    /// \signal_emit_brief
    ///
    /// @var has_signal_closed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/signal.Popover.closedd.html}
    ///
    /// @fn void has_signal_closed::set_signal_closed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_closed::get_signal_closed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_closed::disconnect_signal_closed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_closed::has_signal_closed
    /// \signal_ctor

    DECLARE_SIGNAL(Play, play, PLAY, "play", void);
    /// @class has_signal_play
    /// @brief \signal_play_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_play::connect_signal_play(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_play::connect_signal_play(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_play::emit_signal_play()
    /// \signal_emit_brief
    ///
    /// @var has_signal_play::signal_id
    /// \signal_id{play}
    ///
    /// @fn void has_signal_play::set_signal_play_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_play::get_signal_play_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_play::disconnect_signal_play()
    /// \signal_disconnect
    ///
    /// @fn has_signal_play::has_signal_play
    /// \signal_ctor

    DECLARE_SIGNAL(Stop, stop, STOP, "stop", void);
    /// @class has_signal_stop
    /// @brief \signal_stop_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_stop::connect_signal_stop(Function_t)
    /// \signal_connect{(T*) -> void}
    ///
    /// @fn void has_signal_stop::connect_signal_stop(Function_t, Data_t)
    /// \signal_connect_data{(T*, Data_t) -> void}
    ///
    /// @fn void has_signal_stop::emit_signal_stop()
    /// \signal_emit_brief
    ///
    /// @var has_signal_stop::signal_id
    /// \signal_id{stop}
    ///
    /// @fn void has_signal_stop::set_signal_stop_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_stop::get_signal_stop_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_stop::disconnect_signal_stop()
    /// \signal_disconnect
    ///
    /// @fn has_signal_stop::has_signal_stop
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(ItemsChanged, items_changed, ITEMS_CHANGED, "items-changed", void,
                          SPLAT(int32_t position, int32_t n_removed, int32_t n_added),
                          SPLAT(position, n_removed, n_added)
    );
    /// @class has_signal_items_changed
    /// @brief \signal_items_changed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_items_changed::connect_signal_items_changed(Function_t)
    /// \signal_connect{(T*, int32_t position, int32_t n_removed, int32_t n_added) -> void}
    ///
    /// @fn void has_signal_items_changed::connect_signal_items_changed(Function_t, Data_t)
    /// \signal_connect_data{(T*, int32_t position, int32_t n_removed, int32_t n_added, Data_t) -> void}
    ///
    /// @fn void has_signal_items_changed::emit_signal_items_changed(int32_t position, int32_t n_removed, int32_t n_added)
    /// \signal_emit_brief
    /// @param position position of item that changed
    /// @param n_removed number of items removed
    /// @param n_added number of items added
    ///
    /// @var has_signal_items_changed::signal_id
    /// \signal_id{https://docs.gtk.org/gio/signal.MenuModel.items-changed.html}
    ///
    /// @fn void has_signal_items_changed::set_signal_items_changed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_items_changed::get_signal_items_changed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_items_changed::disconnect_signal_items_changed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_items_changed::has_signal_items_changed
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Revealed, revealed, REVEALED, "notify::reveal-child", void, UnusedArgument_t _, _);
    /// @class has_signal_revealed
    /// @brief \signal_revealed_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_revealed::connect_signal_revealed(Function_t)
    /// \signal_connect{(T*, UnusedArgument_t) -> void}
    ///
    /// @fn void has_signal_revealed::connect_signal_revealed(Function_t, Data_t)
    /// \signal_connect_data{(T*, UnusedArgument_t, Data_t) -> void}
    ///
    /// @fn void has_signal_revealed::emit_signal_revealed(UnusedArgument_t)
    /// \signal_emit_brief
    /// @param _ unused parameter, should be ignored
    ///
    /// @var has_signal_revealed::signal_id
    /// \signal_id{https://docs.gtk.org/gtk4/property.Revealer.reveal-child.html}
    ///
    /// @fn void has_signal_revealed::set_signal_revealed_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_revealed::get_signal_revealed_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_revealed::disconnect_signal_revealed()
    /// \signal_disconnect
    ///
    /// @fn has_signal_revealed::has_signal_revealed
    /// \signal_ctor

    DECLARE_SIGNAL_MANUAL(Activated, activated, ACTIVATED, "activate", void, UnusedArgument_t _, _);
    /// @class has_signal_activated
    /// @brief \signal_activated_brief
    /// @tparam T instance type
    ///
    /// @fn void has_signal_activated::connect_signal_activated(Function_t)
    /// \signal_connect{(T*, UnusedArgument_t) -> void}
    ///
    /// @fn void has_signal_activated::connect_signal_activated(Function_t, Data_t)
    /// \signal_connect_data{(T*, UnusedArgument_t, Data_t) -> void}
    ///
    /// @fn void has_signal_activated::emit_signal_activated(UnusedArgument_t)
    /// \signal_emit_brief
    /// @param _ unused parameter, should be ignored
    ///
    /// @var has_signal_activated::signal_id
    /// \signal_id{https://docs.gtk.org/gio/signal.SimpleAction.activate.html}
    ///
    /// @fn void has_signal_activated::set_signal_activated_blocked(bool)
    /// \signal_set_blocked
    ///
    /// @fn bool has_signal_activated::get_signal_activated_blocked() const
    /// \signal_get_blocked
    ///
    /// @fn void has_signal_activated::disconnect_signal_activated()
    /// \signal_disconnect
    ///
    /// @fn has_signal_activated::has_signal_activated
    /// \signal_ctor
}
