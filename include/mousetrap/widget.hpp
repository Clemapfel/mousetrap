//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>

#include <mousetrap/geometry.hpp>
#include <mousetrap/vector.hpp>
#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/signal_component.hpp>
#include <mousetrap/cursor_type.hpp>
#include <mousetrap/alignment.hpp>
#include <mousetrap/image.hpp>

#include <map>
#include <string>
#include <exception>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    /// @brief result of tick callback of widget, determines whether the tick callback should be removed
    enum class TickCallbackResult : bool
    {
        /// @brief tick callback should continue
        CONTINUE = true,

        /// @brief tick callback should be removed
        DISCONTINUE = false
    };

    /// @brief native widget type, non-owning pointer to GtkWidget
    using NativeWidget = GtkWidget*;

    #ifndef DOXYGEN
    struct EventController;
    class Clipboard;
    class Widget;

    namespace detail
    {
        struct _WidgetInternal
        {
            GObject parent;
            GtkWidget* native;
            std::function<TickCallbackResult(GdkFrameClock*)> tick_callback;
            guint tick_callback_id;
            GtkWidget* tooltip_widget;
        };
        using WidgetInternal = _WidgetInternal;
        DEFINE_INTERNAL_MAPPING(Widget);
    }
    #endif

    /// @brief Widget, super class of all renderable entities
    class Widget : public SignalEmitter,
        HAS_SIGNAL(Widget, realize),
        HAS_SIGNAL(Widget, unrealize),
        HAS_SIGNAL(Widget, destroy),
        HAS_SIGNAL(Widget, hide),
        HAS_SIGNAL(Widget, show),
        HAS_SIGNAL(Widget, map),
        HAS_SIGNAL(Widget, unmap)
    {
        template<typename T>
        friend struct WidgetImplementation;

        public:
            /// @brief construct from internal
            Widget(detail::WidgetInternal*);

            /// @brief default dtor
            virtual ~Widget();

            Widget(const Widget&) = delete;
            Widget& operator=(const Widget&) const = delete;
            Widget(Widget&& other) = delete;
            Widget& operator=(Widget&&) = delete;

            /// @brief implement signal emitter
            explicit operator NativeObject() const override;

            /// @copydoc SignalEmitter::get_internal
            NativeObject get_internal() const override;

            /// @brief expose as GtkWidget, this is the only function that needs to be implemented in oder for an object to act as a widget
            virtual operator NativeWidget() const;

            /// @brief activate the widget, if it is activatable. This will also trigger any associated animations or signals
            void activate();

            /// @brief get minimum allocated size
            /// @return width, height
            Vector2f get_size_request();

            /// @brief set minimum allocated size
            /// @param size width, height
            void set_size_request(Vector2f);

            /// @brief check if both widgets point to the same internal object
            /// @param other
            /// @return true if internally managed object is the same, false otherwise
            bool operator==(const Widget& other) const;

            /// @brief check if both widgets point to the same internal object
            /// @param other
            /// @return false if internally managed object is the same, true otherwise
            bool operator!=(const Widget& other) const;

            /// @brief set top margin, minimum distance to any other widget boundary
            /// @param margin
            void set_margin_top(float margin);

            /// @brief get top margin, minimum distance to any other widget boundary
            /// @return margin
            float get_margin_top() const;

            /// @brief set bottom margin, minimum distance to any other widget boundary
            /// @param margin
            void set_margin_bottom(float margin);

            /// @brief set botom margin, minimum distance to any other widget boundary
            /// @return margin
            float get_margin_bottom() const;

            /// @brief set left margin, minimum distance to any other widget boundary
            /// @param margin
            void set_margin_start(float margin);

            /// @brief get left margin, minimum distance to any other widget boundary
            /// @return margin
            float get_margin_start() const;

            /// @brief set right margin, minimum distance to any other widget boundary
            /// @param margin
            void set_margin_end(float margin);

            /// @brief get right margin, minimum distance to any other widget boundary
            /// @return margin
            float get_margin_end() const;

            /// @brief set left and right margin, minimum distance to any other widget boundary
            /// @param margin
            void set_margin_horizontal(float margin);

            /// @brief set top and bottom margin, minimum distance to any other widget boundary
            /// @param margin
            void set_margin_vertical(float margin);

            /// @brief set margin in all directions, minimum distance to any other widget boundary
            /// @param margin
            void set_margin(float margin);

            /// @brief set whether widget should attempt to allocate the maximum allowed space horizontally
            /// @param should_expand true if widget should expand horizontally, false otherwise
            void set_expand_horizontally(bool should_expand);

            /// @brief get whether widget should attempt to allocate the maximum allowed space horizontally
            /// @return true if widget should expand horizontally, false otherwise
            bool get_expand_horizontally() const;

            /// @brief set whether widget should attempt to allocate the maximum allowed space verticall
            /// @param should_expand true if widget should expand vertically, false otherwise
            void set_expand_vertically(bool should_expand);

            /// @brief get whether widget should attempt to allocate the maximum allowed space vertically
            /// @return true if widget should expand vertically, false otherwise
            bool get_expand_vertically() const;

            /// @brief set whether the widget should attempt to allocate the maximum allowed space in both dimensions
            /// @param both true widget should expand, false otherwise
            void set_expand(bool both);

            /// @brief set where the widget should align itself along the x-axis
            /// @param alignment
            void set_horizontal_alignment(Alignment);

            /// @brief get where the widget should align itself along the x-axis
            /// @return alignment
            Alignment get_horizontal_alignment() const;

            /// @brief set where the widget should align itself along the y-axis
            /// @param alignment
            void set_vertical_alignment(Alignment);

            /// @brief get where the widget should align itself along the y-axis
            /// @return alignment
            Alignment get_vertical_alignment() const;

            /// @brief set where the widget should align itself along both axes
            /// @param both alignment
            void set_alignment(Alignment both);

            /// @brief set widget opacity, in [0, 1], if opacity is set to 0, equivalent to mousetrap::Widget::hide()
            /// @param opacity
            void set_opacity(float opacity);

            /// @brief get widget opacity
            /// @return opacity, in [0, 1]
            float get_opacity();

            /// @brief set whether the widget should be hidden
            /// @param b false if hidden, true otherwise
            void set_visible(bool b);

            /// @brief get whether the widget is hidden
            /// @return false if hidden, true otherwise
            bool get_visible();

            /// @brief set text displayed as a tooltip when the user hovers over the widget
            /// @param text
            void set_tooltip_text(const std::string& text);

            /// @brief set arbitrary widget displayed as a tooltip when user hovers over the widget
            /// @param widget
            void set_tooltip_widget(const Widget& widget);

            /// @brief remove tooltip
            void remove_tooltip_widget();

            /// @brief set the shape of cursor displayed when the cursor is above the widget
            /// @param type cursor type
            void set_cursor(CursorType type);

            /// @brief set cursor from custom image
            /// @param image
            /// @param offset where the pixel should be anchored, in pixels
            void set_cursor_from_image(const Image& image, Vector2i offset);

            /// @brief hide the widget, this may set its allocated size to 0
            void hide();

            /// @brief show the widget, this allocates its size
            void show();

            /// @brief add an event controller to the widget, a widget may have an arbitrary number of controllers
            /// @param controller
            void add_controller(const EventController&);

            /// @brief remove an event controller from the widget, if event controller is not registered to widget, does nothing
            /// @param controller controller to remove
            void remove_controller(const EventController&);

            /// @brief set whether the widgets can contain input focus
            /// @param b true if it can contain focus, false otherwise
            void set_is_focusable(bool);

            /// @brief get whether widget can contain focus input focus
            /// @return true if it can contain focus, false otherwise
            bool get_is_focusable() const;

            /// @brief attempt to grab the input focus
            void grab_focus();

            /// @brief get whether the wigdet currently holds input focus
            /// @return true if holding focus, false otherwise
            bool get_has_focus();

            /// @brief set whether the input should attempt to grab the focus when the user clicks on it
            /// @param b true if it should grab focus, false otherwise
            void set_focus_on_click(bool);

            /// @brief get whether the input should attempt to grab the focus when the user clicks on it
            /// @return true if it should grab focus, false otherwise
            bool get_focus_on_click() const;

            /// @brief get whether the widget was realized
            /// @return true if realized, false otherwise
            bool get_is_realized();

            /// @brief get minimum size, this will always be equal to or larger than the size request
            /// @return widget and height, in pixels
            Vector2f get_minimum_size() const;

            /// @brief get natural size, this is the intended size of the widget if no expansion or clipping occurrs
            Vector2f get_natural_size() const;

            /// @brief get allocated size as rectangle
            /// @return rectangle
            Rectangle get_allocation() const;

            /// @brief remove the widget from the container it is in, if any
            void unparent();

            /// @brief set whether the widget is allowed to gather input events. If set to false, the widget may appear "grayed out"
            /// @param b true if interactable, false otherwise
            void set_can_respond_to_input(bool);

            /// @brief get whether the widget is allowed to gather input events
            /// @return true if interactable, false otherwise
            bool get_can_respond_to_input() const;

            /// @brief add a callback that is invoked every frame
            /// @tparam Function_t static function or lambda with signature <tt>(FrameClock, Data_t) -> TickCallbackResult</tt>
            /// @tparam Data_t arbitrary data type
            /// @param function
            /// @param data
            template<typename Function_t, typename Data_t>
            void set_tick_callback(Function_t function, Data_t data);

            /// @brief add a callback that is invoked every frame
            /// @tparam Function_t static function or lambda with signature <tt>(FrameClock) -> TickCallbackResult</tt>
            /// @param function
            template<typename Function_t>
            void set_tick_callback(Function_t function);

            /// @brief remove callback that si invoked every frame
            void remove_tick_callback();

            /// @brief get the widgets associated clipboard
            /// @return clipboard
            Clipboard get_clipboard() const;

            /// @brief set whether a child widget inside this widget should be cutoff when it leaves this widgets boundary
            /// @param b true if child shut be cutoff, true if it should be fully visible
            void set_hide_on_overflow(bool);

            /// @brief get whether a child widget inside this widget should be cutoff when it leaves this widgets boundary
            /// @return true if child shut be cutoff, true if it should be fully visible
            bool get_hide_on_overflow() const;

        protected:
            /// @brief ctor protected. Only inheriting classes should call this
            /// @param widget GtkWidget instance
            Widget(NativeWidget widget);

        private:
            detail::WidgetInternal* _internal = nullptr;

            static gboolean tick_callback_wrapper(GtkWidget*, GdkFrameClock*, detail::WidgetInternal* instance);
            static gboolean on_query_tooltip(GtkWidget*, gint x, gint y, gboolean, GtkTooltip* tooltip, detail::WidgetInternal* instance);
    };

    #define COMPOUND_WIDGET_REQUIRED_FUNCTION as_widget

    #ifndef DOXYGEN
    namespace detail
    {
        struct CompoundWidgetSuper
        {
            virtual Widget& COMPOUND_WIDGET_REQUIRED_FUNCTION() = 0;

            virtual operator Widget&() const final;
            virtual operator const Widget&() const final;
            virtual operator Widget&() final;
            virtual operator const Widget&() final;
        };
    }
    #endif

    /// @brief Class users can inherit from for their own object to be treated as a widget
    class CompoundWidget
    #ifndef DOXYGEN
        : public detail::CompoundWidgetSuper
    #endif
    {
        protected:
            /// @brief Expose the top-level widget. This is the only function objects need to implement in order to be able to be treated as a widget
            /// @return top-level widget of a collection of widgets
            virtual Widget& COMPOUND_WIDGET_REQUIRED_FUNCTION() = 0;
    };
};

#include "inline/widget.inl"
