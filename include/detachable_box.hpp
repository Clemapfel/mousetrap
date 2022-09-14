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
    class DetachableBox : public Widget
    {
        public:
            DetachableBox(const std::string& detached_window_title = "");
            virtual ~DetachableBox();
            operator GtkWidget*();

            void detach();
            void attach();

            bool get_child_attached();
            void set_child_attached(bool);

            void set_child(Widget*);

            using OnAttachSignature = void(*)(void*);
            void connect_attach(OnAttachSignature , void*);

            using OnDetachSignature = void(*)(void*);
            void connect_detach(OnDetachSignature, void*);

        private:
            bool _attached = true;

            GtkWindow* _window;
            GtkRevealer* _window_revealer;
            GtkHeaderBar* _titlebar;
            GtkLabel* _titlebar_title;

            GtkRevealer* _anchor;
            GtkWidget* _child = nullptr;

            OnAttachSignature _on_attach = nullptr;
            void* _on_attach_data;

            OnDetachSignature _on_detach = nullptr;
            void* _on_detach_data;

            static gboolean on_window_close(GtkWindow*, DetachableBox* instance);
    };
}

#include <src/detachable_box.inl>
