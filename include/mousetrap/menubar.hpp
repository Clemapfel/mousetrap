//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/10/23
//

#pragma once

#include <mousetrap/menu_model.hpp>

namespace mousetrap
{
    /// @brief display menu as a menubar
    /// \signals
    /// \widget_signals{MenuBar}
    class MenuBar : public WidgetImplementation<GtkPopoverMenuBar>
    {
        public:
            /// @brief create from menu model
            /// @param menu_model
            MenuBar(const MenuModel&);

        protected:
            void refresh_widgets();

        private:
            const MenuModel* _model = nullptr;
    };
}
