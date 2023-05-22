//
// Copyright 2022 Clemens Cords
// Created on 8/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/menu_model.hpp>
#include <mousetrap/relative_position.hpp>
#include <mousetrap/popover.hpp>
#include <mousetrap/popover_menu.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class PopoverButton;
    namespace detail
    {
        struct _PopoverButtonInternal
        {
            GObject parent;
            GtkMenuButton* native;
            detail::PopoverInternal* popover;
            detail::PopoverMenuInternal* menu;
        };
        using PopoverButtonInternal = _PopoverButtonInternal;
        DEFINE_INTERNAL_MAPPING(PopoverButton);
    }
    #endif

    /// @brief popover menu button, if pressed, shows it's attached popover or popover menu automatically
    class PopoverButton : public Widget,
        HAS_SIGNAL(PopoverButton, activate)
    {
        public:
            /// @brief construct with no child or popover attached
            PopoverButton();

            /// @brief construct from internal
            PopoverButton(detail::PopoverButtonInternal*);

            /// @brief destructor
            ~PopoverButton();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief set the label widget of the button
            /// @param widget can be nullptr
            void set_child(const Widget& widget);

            /// @brief remove child
            void remove_child();

            /// @brief attach a popover to the button, the button will take care of showing / hiding the popover
            /// @param popover Popover or nullptr to remove any popover or popover menu
            void set_popover(Popover& popover);

            /// @brief attach a popover menu to the button, the button will take care of showing / hiden the popover menu
            /// @param popover_menu
            void set_popover_menu(PopoverMenu& popover_menu);

            /// @brief set relative position, applied to any popover attached
            /// @param relative_position
            void set_popover_position(RelativePosition relative_position);

            /// @brief get relative position of any potential popover
            /// @return relative position
            RelativePosition get_popover_position() const;

            /// @brief remove bound PopoverMenu or Popover
            void remove_popover();

            /// @brief set whether a menu dropdown arrow should appear next to the label widget, on by default
            /// @param b true if arrow should appearn, false otherwise
            void set_always_show_arrow(bool);

            /// @brief get whether a menu dropdown arrow is shown next to the label widget
            /// @return true if arrow is visible, false otherwise
            bool get_always_show_arrow() const;

            /// @brief set whether the button has a black outline
            /// @param b true if it should have an outline, false otherwise
            void set_has_frame(bool b);

            /// @brief get whether the button has a black outline
            /// @return true if it has an outline, false otherwise
            bool get_has_frame() const;

            /// @brief show the attached popover, does nothing if popover is already displayed or there is no popover attached
            void popup();

            /// @brief hide the attached popover, does nothing if popover is already hidden or there is no popover attached
            void popdown();

            /// @brief set whether button should be circular
            /// @param b
            void set_is_circular(bool b);

            /// @brief get whether button is circular
            /// @return true if circular, false otherwise
            bool get_is_circular() const;

        private:
            detail::PopoverButtonInternal* _internal = nullptr;
    };
}
