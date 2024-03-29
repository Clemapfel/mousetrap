//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/13/23
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/signal_component.hpp>

#include <string>
#include <functional>
#include <deque>

namespace mousetrap
{
    /// @brief id of an action, usually `scope.action_name`
    using ActionID = std::string;

    /// @brief id of a shortcut trigger, Any number of pairwise different modifiers (<tt>\<Control\></tt>, <tt>\<Shift\></tt>, etc.) followed by exactly one key identifier
    /// @see https://docs.gtk.org/gdk4/#constants
    using ShortcutTriggerID = std::string;

    #ifndef DOXYGEN
    class Action;
    class Application;
    namespace detail
    {
        struct _ApplicationInternal;
        struct _ActionInternal
        {
            GObject parent;

            detail::_ApplicationInternal* application;
            ActionID id;
            std::vector<ShortcutTriggerID> shortcuts;

            GApplication* g_app;
            GSimpleAction* g_action;
            GVariant* g_state;

            std::function<void(Action&)> stateless_f;
            std::function<void(Action&)> stateful_f;

            bool enabled;
        };
        using ActionInternal = _ActionInternal;
        DEFINE_INTERNAL_MAPPING(Action);
    }
    #endif

    /// @brief Command with a name, registered to an application. See the manual section on actions for more information
    /// \signals
    /// \signal_activated{Action}
    class Action : public SignalEmitter,
        HAS_SIGNAL(Action, activated)
    {
        friend class Application;

        public:
            /// @brief construct an action with immutable id
            /// @param id string, usually of the form `scope.action_name`
            /// @param application
            Action(const std::string& id, Application& application);

            /// @brief create from internal
            /// @param internal
            Action(detail::ActionInternal* internal);

            /// @brief dtor
            ~Action();

            /// @brief get action id
            /// @return ActionID
            ActionID get_id() const;

            /// @brief copy ctor delete
            Action(const Action&);

            /// @brief move ctor
            Action(Action&&) noexcept;

            /// @brief copy assignment deleted, actions should not be duplicated
            Action& operator=(const Action&) = delete;

            /// @brief move assignment deleted, actions should not be moved
            Action& operator=(Action&&) noexcept = delete;

            /// @brief expose as GObject
            operator NativeObject() const override;

            /// @brief get internal
            NativeObject get_internal() const override;

            /// @brief create action as stateless, given function is executed when action is triggered
            /// @tparam Function_t lambda or static function with signature `(Action&) -> void`
            /// @param f function
            template<typename Function_t>
            void set_function(Function_t f);

            /// @brief create action as stateless, given function is executed when action is triggered
            /// @tparam Function_t lambda or static function with signature `(Action&, Data_t) -> void`
            /// @tparam Data_t arbitrary data type
            /// @param f function
            /// @param data data
            template<typename Function_t, typename Data_t>
            void set_function(Function_t f, Data_t data);

            /// @brief create action as stateful, given function is executed when action is triggered
            /// @tparam Function_t lambda or static function with signature `(Action&, bool) -> bool`
            /// @param f function
            /// @param initial_state state of the action on initialization
            template<typename Function_t>
            void set_stateful_function(Function_t f, bool initial_state = false);

            /// @brief create action as stateful, given function is executed when action is triggered
            /// @tparam Function_t lambda or static function with signature `(Action&, bool, Data_t) -> bool`
            /// @param f function
            /// @param data data
            /// @param initial_state state of the action on initialization
            template<typename Function_t, typename Data_t>
            void set_stateful_function(Function_t f, Data_t data, bool initial_state = false);

            /// @brief set the state of a stateful action, if the action is not stateful, does nothing
            /// @param new_state
            void set_state(bool new_state);

            /// @brief get state of a stateful action, if the action is not stateful, returns false
            /// @return false if state is set to false or the action is not stateful, true otherwise
            bool get_state() const;

            /// @brief trigger the action
            void activate();

            /// @brief add a shortcut trigger for action, warns but does not throw if trigger is malformed
            /// @param trigger
            /// @note shortcuts can be changed only before the action is registered with an application. If already registered, unregister the action and reregister a new action with the same behavior but different shortcuts
            void add_shortcut(const ShortcutTriggerID& trigger);

            /// @brief get shortcuts for action
            /// @return vector of shortcut triggers
            const std::vector<ShortcutTriggerID>& get_shortcuts() const;

            /// @brief reset all shortcuts
            /// @param action
            void clear_shortcuts();

            /// @brief set whether triggering the action will execute the registered function
            /// @param is_enabled
            void set_enabled(bool is_enabled);

            /// @brief get whether the action is currently enabled
            /// @return true if enabled, false otherwise
            bool get_enabled() const;

            /// @brief get whether the action is stateful
            /// @return true if action was set as stateful, false otherwise
            bool get_is_stateful() const;

        private:
            detail::ActionInternal* _internal = nullptr;

            static void on_action_activate(GSimpleAction*, GVariant*, detail::ActionInternal*);
            static void on_action_change_state(GSimpleAction*, GVariant*, detail::ActionInternal*);

            void update_application();
    };
}

#include "inline/action.hpp"
