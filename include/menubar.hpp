//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/10/23
//

#pragma once

#include <include/menu_model.hpp>

namespace mousetrap
{
    /// @brief display menu as a menubar
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
