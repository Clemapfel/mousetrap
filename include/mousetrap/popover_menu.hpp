//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 5/3/23
//

#pragma once

#include <mousetrap/menu_model.hpp>

namespace mousetrap
{
    /// @brief displays a menu inside a popover that is attached to a popover menu button. If the button is pressed, the popover is shown automatically
    /// \signals
    /// \signal_closed{PopoverMenu}
    /// \widget_signals{PopoverMenu}
    class PopoverMenu : public WidgetImplementation<GtkPopoverMenu>,
        HAS_SIGNAL(PopoverMenu, closed)
    {
            friend class PopoverButton;

        public:
            /// @brief create from menu model
            /// @param model
            PopoverMenu(const MenuModel& model);

        protected:
            void refresh_widgets();

        private:
            const MenuModel* _model = nullptr;
    };
}