//
// Copyright 2022 Clemens Cords
// Created on 8/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/widget.hpp>
#include <mousetrap/action.hpp>
#include <mousetrap/icon.hpp>

#include <string>
#include <set>
#include <map>

namespace mousetrap
{
    #ifndef DOXYGEN
    class MenuModel;
    namespace detail
    {
        struct _MenuModelInternal
        {
            GObject parent;

            GMenu* native;
            std::map<std::string, GtkWidget*>* id_to_widget;
            std::set<const MenuModel*>* submenus;
            bool has_widget_in_toplevel = false;
        };
        using MenuModelInternal = _MenuModelInternal;
        DEFINE_INTERNAL_MAPPING(MenuModel);
    }
    #endif

    /// @brief menu model, used to specifc behavior of menu bars and popover menus
    /// \signals
    /// \signal_items_changed{MenuModel}
    class MenuModel : public SignalEmitter,
        HAS_SIGNAL(MenuModel, items_changed)
    {
        friend class MenuBar;
        friend class PopoverMenu;

        public:
            /// @brief construct as emtpy
            MenuModel();

            /// @brief construct from internal
            MenuModel(detail::MenuModelInternal*);

            /// @brief destruct
            ~MenuModel();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief add an action to the model, if the action is stateful, the menu item will contain a check box
            /// @param label label to be used
            /// @param action action that triggers when the label is activated
            void add_action(
                const std::string& label,
                const Action& action
            );

            /// @brief add widget to menu, note that widgets can only be display when they are part of a section or submenu, not if they are in the toplevel sectino
            /// @param widget
            void add_widget(const Widget& widget);

            /// @brief formatting for sectsion
            enum SectionFormat
            {
                /// @brief present as nested submenu
                NORMAL,

                /// @brief present as vertical list of round buttons areound each label
                HORIZONTAL_BUTTONS,

                /// @brief present as horizontal list of round buttons, left to right
                HORIZONTAL_BUTTONS_LEFT_TO_RIGHT,

                /// @brief present as horizontal list of round button, right to left
                HORIZONTAL_BUTTONS_RIGHT_TO_LEFT,

                /// @brief present label items as circular buttons
                CIRCULAR_BUTTONS,

                /// @brief present label items as buttons inline with the outer menu
                INLINE_BUTTONS
            };

            /// @brief add a section to a menu. A section is visible in the same scope as the outer menu, it has a title and present its child labels according to the given mousetrap::SectionFormat
            /// @param label label for the section, appears at the start of the section, similar to a heading
            /// @param menu_model model to use for populating the section items
            /// @param format section format to use
            void add_section(
                const std::string& label,
                const MenuModel& menu_model,
                SectionFormat format = NORMAL
            );

            /// @brief add submenu to a menu. A submenut will appear as a single menu item, when clicked, the menu will open another popup to display the menu items contained in the submenu
            /// @param label label for the submenu
            /// @param menu_model model to use for populating the submenu items
            void add_submenu(const std::string& label, const MenuModel& menu_model);

            /// @brief add menu item that is a button with an icon and no label
            /// @param icon icon
            /// @param action acton to trigger
            void add_icon(const Icon&, const Action&);

            /// @brief expose a GObject \for_internal_use_only
            operator NativeObject() const override;

            /// @brief expose as GMenuModel
            operator GMenuModel*() const;

        protected:
            /// @brief get list of registered widgets \for_internal_use_only
            /// @return map with internal id to widget mapping
            [[nodiscard]] std::map<std::string, GtkWidget*> get_widgets() const;

        private:
            static inline size_t current_id = 1;
            detail::MenuModelInternal* _internal = nullptr;
    };
}
