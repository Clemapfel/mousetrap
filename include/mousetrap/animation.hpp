//
// Copyright 2022 Clemens Cords
// Created on 8/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/time.hpp>
#include <mousetrap/widget.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class Animation;
    namespace detail
    {
        struct _AnimationInternal
        {
            GObject parent;
            AdwTimedAnimation* native;
            std::function<void(Animation&, double)>* on_tick_callback;
            std::function<void(Animation&)>* on_done_callback;
        };
        using AnimationInternal = _AnimationInternal;
        DEFINE_INTERNAL_MAPPING(Animation);
    }
    #endif

    /// @brief steady clock that can be used to trigger animations
    class Animation : public SignalEmitter, public detail::notify_if_gtk_uninitialized
    {
        public:
            /// @brief current state of an animation
            enum State
            {
                /// @brief default state of the animation, not yet started
                IDLE = ADW_ANIMATION_IDLE,
                /// @brief animation currently paused
                PAUSED = ADW_ANIMATION_PAUSED,
                /// @brief animation actively playing
                PLAYING = ADW_ANIMATION_PLAYING,
                /// @brief animation is done playing
                DONE = ADW_ANIMATION_FINISHED,
            };

            /// @brief tweening mode
            enum TimingFunction
            {
                /// @brief linear tweening
                LINEAR = ADW_LINEAR,

                /// @brief ease in cubic 
                EXPONENTIAL_EASE_IN = ADW_EASE_IN_EXPO,
                /// @brief ease out cubic
                EXPONENTIAL_EASE_OUT = ADW_EASE_OUT_EXPO,
                /// @brief symmetrical cubic
                EXPONENTIAL_SIGMOID = ADW_EASE_IN_OUT_EXPO,

                /// @brief ease in sine
                SINE_EASE_IN = ADW_EASE_IN_SINE,
                /// @brief ease out sine
                SINE_EASE_OUT = ADW_EASE_OUT_SINE,
                /// @brief symmetrical sine
                SINE_SIGMOID = ADW_EASE_IN_OUT_SINE,

                /// @brief ease in circular
                CIRCULAR_EASE_IN = ADW_EASE_IN_CIRC,
                /// @brief ease out circular
                CIRCULAR_EASE_OUT = ADW_EASE_OUT_CIRC,
                /// @brief symmetrical circular
                CIRCULAR_SIGMOID = ADW_EASE_IN_OUT_CIRC,

                /// @brief undershoot, the cubic
                OVERSHOOT_EASE_IN = ADW_EASE_IN_BACK,
                /// @brief cubic, then overshoot
                OVERSHOOT_EASE_OUT = ADW_EASE_OUT_BACK,
                /// @brief under- / over-shoot symmetrical
                OVERSHOOT_SIGMOID = ADW_EASE_IN_OUT_BACK,

                /// @brief ease in elastic
                ELASTIC_EASE_IN = ADW_EASE_IN_ELASTIC,
                /// @brief ease out elastic
                ELASTIC_EASE_OUT = ADW_EASE_OUT_ELASTIC,
                /// @brief symmetrical elastic
                ELASTIC_SIGMOID = ADW_EASE_IN_OUT_ELASTIC,

                /// @brief ease in bounce
                BOUNCE_EASE_IN = ADW_EASE_IN_BOUNCE,
                /// @brief ease out bounce
                BOUNCE_EASE_OUT = ADW_EASE_OUT_BOUNCE,
                /// @brief symmetrical bounce
                BOUNCE_SIGMOID = ADW_EASE_IN_OUT_BOUNCE
            };

            /// @brief construct
            /// @param target widget to animate
            /// @param duration
            Animation(const Widget& target, Time duration);

            /// @brief create from internal
            Animation(detail::AnimationInternal*);

            /// @brief destructor
            ~Animation();

            /// @brief expose internal
            operator NativeObject() const;

            /// @brief expose internal
            NativeObject get_internal();

            /// @brief get state
            /// @return state
            State get_state() const;

            /// @brief move to state PLAYING
            void play();

            /// @brief move to state PAUSED
            void pause();

            /// @brief move to state IDLE
            void reset();

            /// @brief set duration, millisecond precision
            /// @param time
            void set_duration(Time);

            /// @brief get duration, millisecond precision
            /// @return time
            Time get_duration() const;

            /// @brief set initial value, 0 by default
            /// @param value
            void set_lower(double);

            /// @brief get start value, 0 by default
            /// @return value
            double get_lower() const;

            /// @brief set end value, 1 by default
            /// @param value
            void set_upper(double);

            /// @brief get end value, 1 by default
            /// @return value
            double get_upper() const;

            /// @brief get current value, in [lower, upper]
            double get_value() const;

            /// @brief set number of repeats, 1 by default
            /// @param n
            void set_repeat_count(uint64_t n);

            /// @brief get number of repeats, 1 by default
            /// @return n
            uint64_t get_repeat_count() const;

            /// @brief set whether animation plays in reverse
            /// @param b
            void set_is_reversed(bool);

            /// @brief get whether animation is playing in reverse
            /// @return b
            bool get_is_reversed() const;

            /// @brief choose tweening mode, governs the curse of the `value` during animation
            /// @param mode
            void set_timing_function(TimingFunction);

            /// @brief get tweening mode, governs the curser of the `value` during animation
            /// @return mode
            TimingFunction get_timing_function() const;

            /// @brief register a callback called once per frame
            /// @param f function with signature `(Animation&, double, Data_t) -> void`
            /// @param data arbitrary data
            template<typename Function_t, typename Data_t>
            void on_tick(Function_t f, Data_t data);

            /// @brief register a callback called once per frame
            /// @param f function with signature `(Animation&, double) -> void`
            /// @param data arbitrary data
            template<typename Function_t>
            void on_tick(Function_t f);

            /// @brief register a callback called once per frame
            /// @param f function with signature `(Animation&, Data_t) -> void`
            /// @param data arbitrary data
            template<typename Function_t, typename Data_t>
            void on_done(Function_t f, Data_t data);

            /// @brief register a callback called once per frame
            /// @param f function with signature `(Animation&) -> void`
            /// @param data arbitrary data
            template<typename Function_t>
            void on_done(Function_t f);

        private:
            detail::AnimationInternal* _internal = nullptr;
    };

    template <typename Function_t, typename Data_t>
    void Animation::on_tick(Function_t f_in, Data_t data_in)
    {
        if (*_internal->on_tick_callback)
            delete _internal->on_tick_callback;

        (*_internal->on_tick_callback) = [f = f_in, data = data_in](Animation& self, double value){
            f(self, value, data);
        };
    }

    template <typename Function_t>
    void Animation::on_tick(Function_t f_in)
    {
        if (*_internal->on_tick_callback)
            delete _internal->on_tick_callback;

        (*_internal->on_tick_callback) = [f = f_in](Animation& self, double value){
            f(self, value);
        };
    }

    template <typename Function_t, typename Data_t>
    void Animation::on_done(Function_t f_in, Data_t data_in)
    {
        if (*_internal->on_done_callback)
            delete _internal->on_done_callback;

        (*_internal->on_done_callback) = [f = f_in, data = data_in](Animation& self){
            f(self, data);
        };
    }

    template <typename Function_t>
    void Animation::on_done(Function_t f_in)
    {
        if (*_internal->on_done_callback)
            delete _internal->on_done_callback;

        (*_internal->on_done_callback) = [f = f_in](Animation& self){
            f(self);
        };
    }
}