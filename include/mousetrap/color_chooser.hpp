//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 6/23/23
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/color.hpp>
#include <mousetrap/signal_component.hpp>

#include <functional>

namespace mousetrap
{
    #if GTK_MINOR_VERSION >= 10

    #ifndef DOXYGEN
    class ColorChooser;
    namespace detail
    {
        struct _ColorChooserInternal
        {
            GObject parent;

            GtkColorDialog* native;
            std::function<void(ColorChooser&, RGBA)>* on_accept;
            std::function<void(ColorChooser&)>* on_cancel;
            GdkRGBA* last_color;
        };
        using ColorChooserInternal = _ColorChooserInternal;
        DEFINE_INTERNAL_MAPPING(ColorChooser);
    }
    #endif

    /// @brief dialog that allows useres to choose a color
    /// @note this object is only available for GTK4.10+
    class ColorChooser : public detail::notify_if_gtk_uninitialized
    {
        public:
            /// @brief create the dialog
            /// @param title title to use for the window
            /// @param modal whether the dialog should pause all other windows
            ColorChooser(const std::string& title = "");

            /// @brief construct from internal
            ColorChooser(detail::ColorChooserInternal*);

            /// @brief dtor
            ~ColorChooser();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @brief set callback called when the user selects a color
            /// @param function Function with signature `(ColorChooser&, RGBA, Data_t) -> void`
            template<typename Function_t, typename Data_t>
            void on_accept(Function_t, Data_t);

            /// @brief set callback called when the user selects a color
            /// @param function Function with signature `(ColorChooser&, RGBA) -> void`
            template<typename Function_t>
            void on_accept(Function_t);

            /// @brief set callback called when the user selects a color
            /// @param function Function with signature `(ColorChooser&, Data_t) -> void`
            template<typename Function_t, typename Data_t>
            void on_cancel(Function_t, Data_t);

            /// @brief set callback called when the user selects a color
            /// @param function Function with signature `(ColorChooser&) -> void`
            template<typename Function_t>
            void on_cancel(Function_t);

            /// @brief get current color, this will be the last selected color
            /// @return rgba
            RGBA get_color() const;

            /// @brief show the dialog to the user
            void present();

            /// @brief set whether all other windows should be paused while this dialog is open
            /// @param modal
            void set_is_modal(bool);

            /// @brief get whether all other windows should be paused while this dialog is open
            /// @return model
            bool get_is_modal() const;

            /// @brief set the dialog windows title
            /// @param title
            void set_title(const std::string&);

            /// @brief get window title
            /// @return title
            std::string get_title() const;

        private:
            detail::ColorChooserInternal* _internal = nullptr;
            static void on_color_selected_callback(GObject* source, GAsyncResult* result, void* data);
    };

    template<typename Function_t, typename Data_t>
    void ColorChooser::on_accept(Function_t f_in, Data_t data_in)
    {
        delete _internal->on_accept;
        _internal->on_accept = new std::function<void(ColorChooser&, RGBA)>([f = f_in, data = data_in](ColorChooser& self, RGBA rgba){
           f(self, rgba, data);
        });
    }

    template<typename Function_t>
    void ColorChooser::on_accept(Function_t f_in)
    {
        delete _internal->on_accept;
        _internal->on_accept = new std::function<void(ColorChooser&, RGBA)>([f = f_in](ColorChooser& self, RGBA rgba){
            f(self, rgba);
        });
    }

    template<typename Function_t, typename Data_t>
    void ColorChooser::on_cancel(Function_t f_in, Data_t data_in)
    {
        delete _internal->on_cancel;
        _internal->on_cancel = new std::function<void(ColorChooser&)>([f = f_in, data = data_in](ColorChooser& self){
            f(self, data);
        });
    }

    template<typename Function_t>
    void ColorChooser::on_cancel(Function_t f_in)
    {
        delete _internal->on_cancel;
        _internal->on_cancel = new std::function<void(ColorChooser&)>([f = f_in](ColorChooser& self){
            f(self);
        });
    }

    #endif
}