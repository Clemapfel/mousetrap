// 
// Copyright 2022 Clemens Cords
// Created on 9/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/box.hpp>
#include <include/revealer.hpp>

namespace mousetrap
{
    /// \brief box container, call detach() to move child widget into new window. attach() or closing of window moves child back to box
    class DetachableBox : public WidgetImplementation<GtkRevealer>
    {
        public:
            DetachableBox(const std::string& detached_window_title = "");
            virtual ~DetachableBox();

            void detach();
            void attach();

            bool get_child_attached();
            void set_child_attached(bool);

            void set_child(Widget*);

            template<typename T>
            using on_attach_function_t = void(DetachableBox*, T);

            template<typename Function_t, typename T>
            void connect_signal_attach(Function_t , T);

            template<typename T>
            using on_detach_function_t = void(DetachableBox*, T);

            template<typename Function_t, typename T>
            void connect_signal_detach(Function_t , T);

        private:
            bool _attached = true;

            GtkWindow* _window;
            GtkRevealer* _window_revealer;
            GtkHeaderBar* _titlebar;
            GtkLabel* _titlebar_title;

            GtkRevealer* _anchor;
            GtkWidget* _child = nullptr;

            std::function<on_attach_function_t<void*>> _on_attach;
            void* _on_attach_data;

            std::function<on_detach_function_t<void*>> _on_detach;
            void* _on_detach_data;

            static gboolean on_window_close(GtkWindow*, DetachableBox* instance);
    };
}

#include <src/detachable_box.inl>
