//
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/signal_component.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.hpp"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    class Adjustment;
    namespace detail
    {
        using AdjustmentInternal = GtkAdjustment;
        DEFINE_INTERNAL_MAPPING(Adjustment)
    }
    #endif

    /// @brief adjustment, represents a continous range of values
    /// \signals
    /// \signal_value_changed{Adjustment}
    /// \signal_properties_changed{Adjustment}
    class Adjustment : public SignalEmitter,
        HAS_SIGNAL(Adjustment, value_changed),
        HAS_SIGNAL(Adjustment, properties_changed)
    {
        public:
            /// @brief default ctor
            Adjustment();

            /// @brief default dtor
            ~Adjustment();

            /// @brief create from gtk adjustment \for_internal_use_only
            /// @param native
            Adjustment(detail::AdjustmentInternal* native);

            /// @brief create adjustment
            /// @param current current value, clamped to [lower, upper]
            /// @param lower lower bound of the range
            /// @param upper upper bound of the range
            /// @param increment minimum step increment
            Adjustment(float current, float lower, float upper, float increment);

            /// @copydoc SignalEmitter::get_native
            operator NativeObject() const override;

            /// @copydoc SignalEmitter::get_internal
            NativeObject get_internal() const override;

            /// @brief get lower bound
            /// @return float
            float get_lower() const;

            /// @brief get upper bound
            /// @return float
            float get_upper() const;

            /// @brief get current value
            /// @return float
            float get_value() const;

            /// @brief get minimum step increment
            /// @return float
            float get_step_increment() const;

            /// @brief set lower bounds, clamped to upper bound
            /// @param new_value
            void set_lower(float new_value);

            /// @brief set upper bound, clamped to lower bound
            /// @param new_value
            void set_upper(float new_value);

            /// @brief set current value, clamped to [lower, upper]
            /// @param new_value
            void set_value(float new_value);

            /// @brief set minimum step increment
            /// @param new_value
            void set_step_increment(float new_value);

        private:
            detail::AdjustmentInternal* _internal = nullptr;
    };
}
