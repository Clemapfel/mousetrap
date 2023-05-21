//
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/window.hpp>
#include <mousetrap/action.hpp>
#include <mousetrap/menu_model.hpp>

#include <string>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief application id, see https://docs.gtk.org/gio/type_func.Application.id_is_valid.html
    using ApplicationID = std::string;

    #ifndef DOXYGEN
    class Application;
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

        template<>
        struct InternalMapping<Application>
        {
            using value = ApplicationInternal;
        };
    }
    #endif

    /// @brief object representing an entire application, supplies the main render loop, mapping of actions
    /// \signals
    /// \signal_activate{Application}
    /// \signal_shutdown{Application}
    class Application : public SignalEmitter,
        HAS_SIGNAL(Application, activate),
        HAS_SIGNAL(Application, shutdown)
    {
        public:
            /// @brief construct new action
            /// @param id valid application id, see
            Application(const ApplicationID& id);

            /// @brief construct from internal
            /// @param internal
            Application(detail::ApplicationInternal* internal);

            /// @brief destroy action, should only be called at the very end of <tt>main</tt>
            virtual ~Application();

            /// @copydoc SignalEmitter::get_native
            operator NativeObject() const override;

            /// @copydoc SignalEmitter::get_internal
            NativeObject get_internal() const;

            /// @brief get id
            /// @return id
            ApplicationID get_id() const;

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

            /// @brief add an action to application, see the manual page on actions for details
            /// @param action pointer to action, the user is responsible for making sure the action stays in scope
            void add_action(const Action& action);

            /// @brief remove an action from application
            /// @param id
            void remove_action(const ActionID& id);

            /// @brief lookup action based on action id
            /// @param id
            /// @return thin wrapper around action, this is a newly created wrapper around the internal action object, not the mousetrap::Action instance initially registered via Application::add_action
            [[nodiscard]] Action get_action(const ActionID& id);

            /// @brief check if application has an action with given id registered
            /// @param id
            /// @return true if action with id is registered, false otherwise
            bool has_action(const ActionID& id);

            /// @brief set the model used for the applications menubar, this should be called during emission of the <tt>startup</tt> signal
            /// @param model
            void set_menubar(MenuModel* model);

        private:
            detail::ApplicationInternal* _internal = nullptr;
    };
}
