//
// Copyright 2022 Clemens Cords
// Created on 8/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gtk_common.hpp>
#include <include/signal_emitter.hpp>
#include <include/signal_component.hpp>

#ifdef DOXYGEN
    #include "../docs/doxygen.inl"
#endif

namespace mousetrap
{
    namespace detail
    {
        struct _AdjustmentInternal
        {
            GtkAdjustment* native;
        };
    }

    /// @brief adjustment, represents a continous range of values
    class Adjustment : public SignalEmitter,
        HAS_SIGNAL(Adjustment, value_changed),
        HAS_SIGNAL(Adjustment, properties_changed)
    {
        public:
            /// @brief default ctor
            Adjustment();

            /// @brief create from gtk adjustment \internal
            /// @param adjustment
            Adjustment(GtkAdjustment*);

            /// @brief create adjustment
            /// @param current current value, clamped to [lower, upper]
            /// @param lower lower bound of the range
            /// @param upper upper bound of the range
            /// @param increment minimum step increment
            Adjustment(float current, float lower, float upper, float increment);

            /// @brief dtor
            ~Adjustment();

            /// @copydoc SignalEmitter::operator GObject*() const
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
            void set_lower(float);

            /// @brief set upper bound, clamped to lower bound
            /// @param new_value
            void set_upper(float);

            /// @brief set current value, clamped to [lower, upper]
            /// @param new_value
            void set_value(float);

            /// @brief set minimum step increment
            /// @param new_value
            void set_increment(float);

        private:
            GtkAdjustment* _native;
    };
}
