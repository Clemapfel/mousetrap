//
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/time.hpp>
#include <mousetrap/transition_type.hpp>
#include <mousetrap/selection_model.hpp>
#include <mousetrap/selectable.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    namespace detail
    {
        struct _StackInternal
        {
            GObject parent;
            GtkStack* native;
            SelectionModel* selection_model;
            std::map<std::string, std::reference_wrapper<const Widget>>* children;
        };
        using StackInternal = _StackInternal;
    }
    #endif

    /// @brief displays exactly one of multiple widgets. Use mousetrap::StackSwitcher and mousetrap::StackSidebar to allow a user to switch them through the GUI
    /// \signals
    /// \widget_signals{Stack}
    class Stack : public WidgetImplementation<GtkStack>, public Selectable
    {
        /// @brief id of a stack page, the id is the title of the page and may be used to display the name of that stack page
        using ID = std::string;
            
        public:
            /// @brief construct
            Stack();

            /// @brief destruct
            ~Stack();

            using WidgetImplementation<GtkStack>::operator GtkStack*;

            /// @copydoc mousetrap::Selectable::get_selection_model
            SelectionModel* get_selection_model() override;

            /// @brief add a page to the stack
            /// @param widget may be nullptr to create an empty page
            /// @param title title of the page, usually used to identify pages in widgets allowed user interaction with the stack
            /// @return id of the page, the user is responsible for keeping track of this id to be able to refer to specific pages
            [[nodiscard]] Stack::ID add_child(const Widget& widget, const std::string& title);

            /// @brief get page widget for page with given id
            /// @param id obtained from mousetrap::Stack::add_child
            Widget* get_child(Stack::ID id);

            /// @brief remove a page from the stack
            /// @param id obtained from mousetrap::Stack::add_child
            void remove_child(Stack::ID id);

            /// @brief get number of children
            /// @return n
            size_t get_n_children() const;

            /// @brief get id of page that is currently displayed
            /// @return id
            Stack::ID get_visible_child();

            /// @brief set which page is visible
            /// @param id obtained from mousetrap::Stack::add_child
            void set_visible_child(Stack::ID id);

            /// @brief set the type of animation played when switching between pages of the stack
            /// @param transition_type
            void set_transition_type(StackTransitionType transition_type);

            /// @brief get type of animation played when switching between pages of the stack
            /// @return transition type
            StackTransitionType get_transition_type() const;

            /// @brief set amount of time it takes for the animation to switch between two pages of the stack takes
            /// @param time
            void set_transition_duration(Time time);

            /// @brief get amount of time it takes for the animation to switch between two pages of the stack takes
            /// @return duration
            Time get_transition_duration() const;

            /// @brief set whether the stacks width should be equal to the largest width of any of itss children at all times
            /// @param b true to equalize the width, false otherwise
            void set_is_horizontally_homogeneous(bool);

            /// @brief get whether the stacks width should be equal to the largest width of any of itss children at all times
            /// @return true to equalize the width, false otherwise
            bool get_is_horizontally_homogenous() const;

            /// @brief set whether the stacks height should be equal to the largest height of any of itss children at all times
            /// @param b true to equalize the height, false otherwise
            void set_is_vertically_homogeneous(bool);

            /// @brief set whether the stacks height should be equal to the largest height of any of itss children at all times
            /// @return true to equalize the height, false otherwise
            bool get_is_vertically_homogenous() const;

            /// @brief set whether, when switching between to children, the stack should move from one size to the other linearly, instead of switching immediately
            /// @param b true if sizes should be interpolated, false otherwise
            void set_should_interpolate_size(bool);

            /// @brief get whether, when switching between to children, the stack should move from one size to the other linearly, instead of switching immediately
            /// @return true if sizes should be interpolated, false otherwise
            bool get_should_interpolate_size() const;

        private:
            detail::StackInternal* _internal = nullptr;
    };

    /// @brief widget to choose page of stack, uses page id as label
    /// \signals
    /// \widget_signals{StackSidebar}
    class StackSidebar : public WidgetImplementation<GtkStackSidebar>
    {
        public:
            /// @brief construct
            /// @param stack stack to control
            StackSidebar(const Stack&);
    };

    /// @brief widget to choose page of stack, uses page id as label
    /// \signals
    /// \widget_signals{StackSwitcher}
    class StackSwitcher : public WidgetImplementation<GtkStackSwitcher>
    {
        public:
            /// @brief construct
            /// @param stack stack to control
            StackSwitcher(const Stack&);
    };
}

