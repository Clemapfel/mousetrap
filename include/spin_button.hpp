//
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/adjustment.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class SpinButton;
    namespace detail
    {
        struct _SpinButtonInternal
        {
            GObject parent;

            GtkSpinButton* native;
            Adjustment* adjustment;
            std::function<std::string(const SpinButton*, float)> value_to_text_function;
            std::function<float(const SpinButton*, const std::string&)> text_to_value_function;
        };
        using SpinButtonInternal = _SpinButtonInternal;
    }
    #endif

    /// @brief entry with and "increase" and "decrease" button, allows user to pick a value by entering the exact numebr
    class SpinButton : public WidgetImplementation<GtkSpinButton>,
        HAS_SIGNAL(SpinButton, value_changed),
        HAS_SIGNAL(SpinButton, wrapped)
    {
        public:
            /// @brief construct with range
            /// @param min lower bound
            /// @param max upper bound
            /// @param step minimum step increment
            SpinButton(float min, float max, float step);

            /// @brief construct as thin wrapper \internal
            /// @param internal
            SpinButton(detail::SpinButtonInternal*);

            /// @brief get adjustment, modifying it will modify the spin button
            /// @return adjusment
            Adjustment* get_adjustment();

            /// @brief get adjusment, immutable
            /// @return adjustment, const
            const Adjustment* get_adjustment() const;

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
            float get_increment() const;

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
            void set_increment(float);

            /// @brief set number of digits after the decimal point that should be displayed
            /// @param n
            void set_n_digits(size_t);

            /// @brief get number of digits after the decimal point that should be displayed
            /// @return number of digits
            size_t get_n_digits() const;

            /// @brief set whether incrementing while the value is equal to upper bound should wrap around such that the value is now equal to the lower bound, or vice versa
            /// @param b true if wrapping enabled, false otherwise
            void set_should_wrap(bool);

            /// @brief get whether incrementing while the value is equal to upper bound should wrap around such that the value is now equal to the lower bound, or vice versa
            /// @param b
            bool get_should_wrap() const;

            /// @brief set rate at which the step increment increases the longer increment or decrement button is held down
            /// @param rate 0 for default rate, 1 for double the default rate, 2 for triple, etc.
            void set_acceleration_rate(float);

            /// @brief get rate at which the step increment increases the longer increment or decrement button is held down
            /// @return 0 for default rate, 1 for double the default rate, 2 for triple, etc.
            float get_acceleration_rate() const;

            /// @brief set whether, when entering a number manually, the number should be rounded to nearest multiple of the step increment
            /// @param b true if rounding should take place, false otherwise
            void set_should_snap_to_ticks(bool);

            /// @brief get whether, when entering a number manually, the number should be rounded to nearest multiple of the step increment
            /// @return true if rounding should take place, false otherwise
            bool get_should_snap_to_ticks() const;

            /// @brief set whether the text entry only accepts characters `0`, ..., `9` and `.`
            /// @param b false if non-numerical characters should be allowed, true otherwise
            void set_allow_only_numeric(bool);

            /// @brief get whether the text entry only accepts characters `0`, ..., `9` and `.`
            /// @return false if non-numerical characters should be allowed, true otherwise
            bool get_allow_only_numeric() const;

            /// @brief set function that transforms the spin button entry text to a numerical value
            /// @tparam Function_t lambda or static function with signature <tt>(const SpinButton*, const std::string&, Data_t) -> float</tt>
            /// @tparam Data_t arbitrary data
            /// @param function
            /// @param data
            template<typename Function_t, typename Data_t>
            void set_text_to_value_function(Function_t, Data_t);

            /// @brief set function that transforms the spin button entry text to a numerical value
            /// @tparam Function_t lambda or static function with signature <tt>(const SpinButton*, const std::string&) -> float</tt>
            /// @tparam Data_t arbitrary data
            /// @param function
            /// @param data
            template<typename Function_t>
            void set_text_to_value_function(Function_t);

            void reset_text_to_value_function();

            /// @brief set function that transforms the spin button numerical value to a label
            /// @tparam Function_t lambda or static function with signature <tt>(const SpinButton*, float, Data_t) -> std::string</tt>
            /// @tparam Data_t arbitrary data
            /// @param function
            /// @param data
            template<typename Function_t, typename Data_t>
            void set_value_to_text_function(Function_t, Data_t);

            /// @brief set function that transforms the spin button numerical value to a label
            /// @tparam Function_t lambda or static function with signature <tt>(const SpinButton*, float) -> std::string</tt>
            /// @param function
            template<typename Function_t>
            void set_value_to_text_function(Function_t);

            /// @brief set function that transforms the spin button numerical value to a label to default
            void reset_value_to_text_function();

        private:
            detail::SpinButtonInternal* _internal = nullptr;

            static gint on_input(GtkSpinButton*, double* new_value, detail::SpinButtonInternal* instance);
            static bool on_output(GtkSpinButton*, detail::SpinButtonInternal* instance);
    };
}

#include <src/spin_button.inl>