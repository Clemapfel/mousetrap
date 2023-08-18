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

    /// @brief current state of an animation
    enum AnimationState
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

    /// @brief steady clock that can be used to trigger animations
    class Animation : public SignalEmitter, public detail::notify_if_gtk_uninitialized
    {
        public:
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
            AnimationState get_state() const;

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