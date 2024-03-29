//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>

#include <mousetrap/widget.hpp>
#include <mousetrap/adjustment.hpp>
#include <mousetrap/orientation.hpp>
#include <mousetrap/relative_position.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class Scale;
    namespace detail
    {
        struct _ScaleInternal
        {
            GObject parent;
            GtkScale* native;
            Adjustment* adjustment;
            std::function<std::string(float)> formatting_function;
        };
        using ScaleInternal = _ScaleInternal;
        DEFINE_INTERNAL_MAPPING(Scale);
    }
    #endif

    /// @brief widget that allows users to choose a value from a range by dragging a slider
    /// \signals
    /// \signal_value_changed{Scale}
    /// \widget_signals{Scale}
    class Scale : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(Scale, value_changed),
        HAS_SIGNAL(Scale, realize),
        HAS_SIGNAL(Scale, unrealize),
        HAS_SIGNAL(Scale, destroy),
        HAS_SIGNAL(Scale, hide),
        HAS_SIGNAL(Scale, show),
        HAS_SIGNAL(Scale, map),
        HAS_SIGNAL(Scale, unmap)
    {
        public:
            /// @brief construct
            /// @param min lower bound of the range
            /// @param max upper bound of the range
            /// @param step minimum step increment
            /// @param orientation orientation of the slider bar
            Scale(float min, float max, float step, Orientation orientation = Orientation::HORIZONTAL);

            /// @brief construct from internal
            Scale(detail::ScaleInternal*);

            /// @brief destructor
            ~Scale();

            /// @brief expose internal
            NativeObject get_internal() const;

            /// @brief get const reference to adjustment
            /// @returns adjustmnet, cannot be modified but its values reflect those of the scale
            Adjustment get_adjustment() const;

            /// @brief get lower bound of the range
            /// @return float
            float get_lower() const;

            /// @brief get upper bound of the range
            /// @return float
            float get_upper() const;

            /// @brief get current value
            /// @return float
            float get_value() const;

            /// @brief get minimum increment that two different slider positions allow for
            /// @return float
            float get_step_increment() const;

            /// @brief set lower bound of the range
            /// @param value float
            void set_lower(float);

            /// @brief set upper bound of the range
            /// @param value float
            void set_upper(float);

            /// @brief set value, changes position of the slider and emits the <tt>value_changed</tt> signal
            /// @param value float
            void set_value(float);

            /// @brief set minimum step increment
            /// @param value float
            void set_step_increment(float);

            /// @brief set whether a string signifying the scales current value should be shown next to the slider button
            /// @param b true if string should be shown, false otherwise
            void set_should_draw_value(bool);

            /// @brief get whether a string signifying the scales current value is shown next to the slider button
            /// @return true if string is being shown, false otherwise
            bool get_should_draw_value() const;

            /// @brief set whether the area of the slider between its origin and the current value should be filled in
            /// @param b true if filled in, false otherwise
            void set_has_origin(bool);

            /// @brief get whether the area of the slider between its origin and the current value is filled in
            /// @return true if filled in, false otherwise
            bool get_has_origin() const;

            /// @brief add a mark to a scale, its position is picked based on the marks value, it optionally also has a label
            /// @param at value the mark should be put at, automatically calcaulated based on the scales range
            /// @param pos relative position
            /// @param label label, or std::string() to omit label
            void add_mark(float at, RelativePosition pos, const std::string& label = "");

            /// @brief clear all marks from the scale
            void clear_marks();

            /// @brief set function that takes the scales values and transforms it into the label shown if mousetrap::Scale::set_should_draw_value was set to true
            /// @tparam Function_t lambda or static function with signature <tt>(float, Data_t) -> std::string</tt>
            /// @tparam Data_t arbitary data
            /// @param function
            /// @param data
            template<typename Function_t, typename Data_t>
            void set_format_value_function(Function_t function, Data_t);

            /// @brief set function that takes the scales values and transforms it into the label shown if mousetrap::Scale::set_should_draw_value was set to true
            /// @tparam Function_t lambda or static function with signature <tt>(float) -> std::string</tt>
            /// @param function
            template<typename Function_t>
            void set_format_value_function(Function_t function);

            /// @brief set the format value function back to the default
            void reset_format_value_function();

            /// @copydoc mousetrap::Orientable::set_orientation
            void set_orientation(Orientation);

            /// @copydoc mousetrap::Orientable::get_orientation
            Orientation get_orientation() const;

        private:
            detail::ScaleInternal* _internal = nullptr;
            static char* on_format_value(GtkScale* scale, double value, detail::ScaleInternal* instance);
    };
}

#include "inline/scale.hpp"
