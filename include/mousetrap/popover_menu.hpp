//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 5/3/23
//

#pragma once

#include <mousetrap/menu_model.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    namespace detail
    {
        struct _PopoverMenuInternal
        {
            GObject parent;
            GtkPopoverMenu* native;
            detail::MenuModelInternal* model;
        };
        using PopoverMenuInternal = _PopoverMenuInternal;
        DEFINE_INTERNAL_MAPPING(PopoverMenu);
    }
    #endif

    /// @brief displays a menu inside a popover that is attached to a popover menu button. If the button is pressed, the popover is shown automatically
    /// \signals
    /// \signal_closed{PopoverMenu}
    /// \widget_signals{PopoverMenu}
    class PopoverMenu : public Widget,
        HAS_SIGNAL(PopoverMenu, closed)
    {
            friend class PopoverButton;

        public:
            /// @brief create from menu model
            /// @param model
            PopoverMenu(const MenuModel& model);

            /// @brief construct from internal
            PopoverMenu(detail::PopoverMenuInternal*);

            /// @brief destructor
            ~PopoverMenu();

            /// @brief expose internal
            NativeObject get_internal() const override;

        protected:
            void refresh_widgets();

        private:
           detail::PopoverMenuInternal* _internal = nullptr;
    };
}