//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/10/23
//

#pragma once

#include <mousetrap/menu_model.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class MenuBar;
    namespace detail
    {
        struct _MenuBarInternal
        {
            GObject parent;
            GtkPopoverMenuBar* native;
            detail::MenuModelInternal* model;
        };
        using MenuBarInternal = _MenuBarInternal;
        DEFINE_INTERNAL_MAPPING(MenuBar);
    }
    #endif

    /// @brief display menu as a menubar
    /// \signals
    /// \widget_signals{MenuBar}
    /// \bug When trying to display a menu model that has a custom widget added to anything but the top-level submenu, that widget fails to insert
    class MenuBar : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(MenuBar, realize),
        HAS_SIGNAL(MenuBar, unrealize),
        HAS_SIGNAL(MenuBar, destroy),
        HAS_SIGNAL(MenuBar, hide),
        HAS_SIGNAL(MenuBar, show),
        HAS_SIGNAL(MenuBar, map),
        HAS_SIGNAL(MenuBar, unmap)
    {
        public:
            /// @brief create from menu model
            /// @param menu_model
            MenuBar(const MenuModel& menu_model);

            /// @brief create from internal
            MenuBar(detail::MenuBarInternal*);

            /// @brief destructor
            ~MenuBar();

            /// @brief expose internal
            NativeObject get_internal() const override;

        protected:
            void refresh_widgets();

        private:
            detail::MenuBarInternal* _internal = nullptr;
    };
}
