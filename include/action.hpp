//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/13/23
//

#pragma once

#include <include/gtk_common.hpp>
#include <include/signal_emitter.hpp>
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
    namespace detail
    {
        struct _ActionInternal
        {
            GObject parent;

            ActionID id;
            std::vector<ShortcutTriggerID> shortcuts;

            GApplication* g_app;
            GSimpleAction* g_action;
            GVariant* g_state;

            std::function<void()> stateless_f;
            std::function<void()> stateful_f;

            bool enabled;
        };

        using ActionInternal = _ActionInternal;
    }
    #endif

    /// @brief Command with a name, registered to an application. See the manual section on actions for more information
    class Action : public SignalEmitter
    {
        friend class Application;

        public:
            /// @brief construct an action with immutable id
            /// @param id string, usually of the form `scope.action_name`
            /// @param application
            Action(const std::string& id);

            /// @brief dtor
            ~Action();

            /// @brief get action id
            /// @return ActionID
            ActionID get_id() const;

            /// @brief copy ctor deleted, actions should not be duplicated
            Action(const Action&) = delete;

            /// @brief move ctor deleted, actions should not be moved
            Action(Action&&) noexcept = delete;

            /// @brief copy assignment deleted, actions should not be duplicated
            Action& operator=(const Action&) = delete;

            /// @brief move assignment deleted, actions should not be moved
            Action& operator=(Action&&) noexcept = delete;

            /// @brief create action as stateless, given function is executed when action is triggered
            /// @tparam Function_t lambda or static function with signature `() -> void`
            /// @param f function
            template<typename Function_t>
            void set_function(Function_t f);

            /// @brief create action as stateless, given function is executed when action is triggered
            /// @tparam Function_t lambda or static function with signature `(Data_t) -> void`
            /// @tparam Data_t arbitrary data type
            /// @param f function
            /// @param data data
            template<typename Function_t, typename Data_t>
            void set_function(Function_t f, Data_t data);

            /// @brief create action as stateful, given function is executed when action is triggered
            /// @tparam Function_t lambda or static function with signature `(bool) -> bool`
            /// @param f function
            /// @param initial_state state of the action on initialization
            template<typename Function_t>
            void set_stateful_function(Function_t f, bool initial_state = false);

            /// @brief create action as stateful, given function is executed when action is triggered
            /// @tparam Function_t lambda or static function with signature `(bool, Data_t) -> bool`
            /// @param f function
            /// @param data data
            /// @param initial_state state of the action on initialization
            template<typename Function_t, typename Data_t>
            void set_stateful_function(Function_t f, Data_t data, bool initial_state = false);

            /// @brief set the state of a stateful action, if the action is not stateful, does nothing
            /// @param new_state
            void set_state(bool);

            /// @brief get state of a stateful action, if the action is not stateful, returns false
            /// @return false if state is set to false or the action is not stateful, true otherwise
            bool get_state() const;

            /// @brief trigger the action
            void activate() const;

            /// @brief add a shortcut trigger for action, warns but does not throw if trigger is malformed
            /// @param trigger
            /// @note shortcuts can be changed only before the action is registered with an application. If already registered, unregister the action and reregister a new action with the same behavior but different shortcuts
            void add_shortcut(const ShortcutTriggerID&);

            /// @brief get shortcuts for action
            /// @return vector of shortcut triggers
            const std::vector<ShortcutTriggerID>& get_shortcuts() const;

            /// @brief cast to GAction \internal
            explicit operator GAction*() const;

            /// @copydoc SignalEmitter::operator GObject*() const
            operator GObject*() const override;

            /// @brief set whether triggering the action will execute the registered function
            /// @param is_enabled
            void set_enabled(bool);

            /// @brief get whether the action is currently enabled
            /// @return true if enabled, false otherwise
            bool get_enabled() const;

            /// @brief get whether the action is stateful
            /// @return true if action was set as stateful, false otherwise
            bool get_is_stateful() const;

        protected:
            /// @brief init from floating internal
            /// @param internal
            Action(detail::ActionInternal*);

        private:
            detail::ActionInternal* _internal = nullptr;

            static void on_action_activate(GSimpleAction*, GVariant*, detail::ActionInternal*);
            static void on_action_change_state(GSimpleAction*, GVariant*, detail::ActionInternal*);
    };
}

#include <src/action.inl>
