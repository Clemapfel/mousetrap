// 
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/box.hpp>
#include <include/revealer.hpp>
#include <include/signal_component.hpp>

namespace mousetrap
{
    /// \brief box container, call detach() to move child widget into new window. attach() or closing of window moves child back to box
    class DetachableBox : public WidgetImplementation<GtkRevealer>,
        public HasAttachSignal<DetachableBox>,
        public HasDetachSignal<DetachableBox>
    {
        public:
            DetachableBox(const std::string& detached_window_title = "");
            virtual ~DetachableBox();

            void detach();
            void attach();

            bool get_child_attached();
            void set_child_attached(bool);

            void set_child(Widget*);

        private:
            bool _attached = true;

            GtkWindow* _window;
            GtkRevealer* _window_revealer;
            GtkHeaderBar* _titlebar;
            GtkLabel* _titlebar_title;

            GtkRevealer* _anchor;
            GtkWidget* _child = nullptr;

            static gboolean on_window_close(GtkWindow*, DetachableBox* instance);
    };
}
