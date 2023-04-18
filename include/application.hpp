//
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gtk_common.hpp>
#include <include/signal_emitter.hpp>
#include <include/window.hpp>
#include <include/action.hpp>
#include <include/menu_model.hpp>

#include <string>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief application id, see https://docs.gtk.org/gio/type_func.Application.id_is_valid.html
    using ApplicationID = std::string;

    #ifndef DOXYGEN
    namespace detail
    {
        struct _ApplicationInternal
        {
            GObject parent_instance;

            GtkApplication* native;
            std::unordered_map<ActionID, detail::ActionInternal*>* actions;

            bool holding;
            bool busy;
        };
        using ApplicationInternal = _ApplicationInternal;
    }
    #endif

    /// @brief object representing an entire application, supplies the main render loop, mapping of actions
    class Application : public SignalEmitter,
        HAS_SIGNAL(Application, activate),
        HAS_SIGNAL(Application, shutdown)
    {
        public:
            /// @brief construct new action
            /// @param id valid application id, see
            Application(const ApplicationID& id);

            /// @brief get id
            /// @return id
            ApplicationID get_id() const;

            /// @brief destroy action, should only be called at the very end of <tt>main</tt>
            virtual ~Application();

            /// @brief start the main render loop
            /// @returns 1 on error, 0 otherwise
            int run();

            /// @brief immediately exit the application, this forces mousetrap::Application::run to return
            void quit();

            /// @brief mark the application such that quitting regularly will be prevented, use mousetrap::Application::release to undo this
            void hold();

            /// @brief undo a previous mousetrap::Application::hold()
            void release();

            /// @brief mark that the application is currently doing a long-running application. Call mousetrap::Application::unmark_as_busy to undo this
            void mark_as_busy();

            /// @brief undo a previous call to mousetrap::Application::mark_as_busy
            void unmark_as_busy();

            /// @brief expose as GObject for signal handling \internal
            explicit operator GObject*() const override;

            /// @brief expose as GApplication \internal
            explicit operator GApplication*() const;

            /// @brief expose as GtkApplication \internal
            explicit operator GtkApplication*() const;

            /// @brief expose as GActionMap \internal
            explicit operator GActionMap*() const;

            /// @brief add an action to application, see the manual page on actions for details
            /// @param action pointer to action, the user is responsible for making sure the action stays in scope
            void add_action(const Action&);

            /// @brief convenience function, creates new actino, set its behavior, then immediately add it to the application
            template<typename Function_t>
            Action* add_new_action(const std::string& action_id, Function_t f);

            /// @brief remove an action from application
            /// @param id
            void remove_action(const ActionID&);

            /// @brief lookup action based on action id
            /// @param id
            /// @return thin wrapper around action, this is a newly created wrapper around the internal action object, not the mousetrap::Action instance initially registered via Application::add_action
            [[nodiscard]] Action get_action(const ActionID&);

            /// @brief check if application has an action with given id registered
            /// @param id
            /// @return true if action with id is registered, false otherwise
            bool has_action(const ActionID&);

            /// @brief set the model used for the applications menubar, this should be called during emission of the <tt>startup<//t> signal
            /// @param model
            void set_menubar(MenuModel*);

        private:
            detail::ApplicationInternal* _internal = nullptr;
    };
}
