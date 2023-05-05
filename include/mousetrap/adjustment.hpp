//
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/signal_component.hpp>

#ifdef DOXYGEN
    #include "../../docs/doxygen.inl"
#endif

namespace mousetrap
{
    #ifndef DOXYGEN
    namespace detail
    {
        struct _AdjustmentInternal
        {
            GtkAdjustment* native;
        };
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

            /// @brief create from gtk adjustment \internal
            /// @param native
            Adjustment(GtkAdjustment* native);

            /// @brief create adjustment
            /// @param current current value, clamped to [lower, upper]
            /// @param lower lower bound of the range
            /// @param upper upper bound of the range
            /// @param increment minimum step increment
            Adjustment(float current, float lower, float upper, float increment);

            /// @brief dtor
            ~Adjustment();

            /// @brief expose a gobject, \internal
            operator GObject*() const override;

            /// @brief copy ctor delete
            Adjustment(const Adjustment&) = delete;

            /// @brief copy assignment deleted
            Adjustment& operator=(const Adjustment&) = delete;

            /// @brief move ctor, safely transfers ownership of the internal adjustment
            /// @param other rvalue reference to other
            Adjustment(Adjustment&&) noexcept;

            /// @brief move assignment, safely transfers ownership of the internal adjustment
            /// @param other rvalue reference to other
            /// @return reference to self after assignment
            Adjustment& operator=(Adjustment&&) noexcept;

            /// @brief expose gtk adjustment \internal
            explicit operator GtkAdjustment*() const;

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
            float get_increment() const;

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
            void set_increment(float new_value);

        private:
            GtkAdjustment* _native;
    };
}
