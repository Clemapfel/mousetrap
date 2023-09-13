//
// Created by clem on 8/18/23.
//

#include <mousetrap/animation.hpp>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(AnimationInternal, animation_internal, ANIMATION_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(AnimationInternal, animation_internal, ANIMATION_INTERNAL)

        static void animation_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_ANIMATION_INTERNAL(object);

            g_object_unref(self->native);
            delete self->on_tick_callback;
            delete self->on_done_callback;

            G_OBJECT_CLASS(animation_internal_parent_class)->finalize(object);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(AnimationInternal, animation_internal, ANIMATION_INTERNAL)

        void animation_on_tick_wrapper(double value, void* data)
        {
            auto* instance = detail::MOUSETRAP_ANIMATION_INTERNAL(data);
            auto temp = Animation(instance);
            if (*instance->on_tick_callback)
                (*instance->on_tick_callback)(temp, value);
        }

        void animation_on_done_wrapper(AdwAnimation*, void* data)
        {
            auto* instance = detail::MOUSETRAP_ANIMATION_INTERNAL(data);
            auto temp = Animation(instance);
            if (*instance->on_done_callback)
                (*instance->on_done_callback)(temp);
        }

        static AnimationInternal* animation_internal_new(GtkWidget* widget, size_t time_ms)
        {
            log::initialize();

            auto* self = (AnimationInternal*) g_object_new(animation_internal_get_type(), nullptr);
            animation_internal_init(self);

            self->native = ADW_TIMED_ANIMATION(adw_timed_animation_new(widget, 0, 1, time_ms, adw_callback_animation_target_new(
                animation_on_tick_wrapper, self, (GDestroyNotify) nullptr
            )));
            g_signal_connect(self->native, "done", G_CALLBACK(animation_on_done_wrapper), self);
            g_object_ref(self->native);

            self->on_tick_callback = new std::function<void(Animation&, double)>();
            self->on_done_callback = new std::function<void(Animation&)>();
            return self;
        }
    }
    
    Animation::Animation(const Widget& target, Time duration)
        : _internal(detail::animation_internal_new(target.operator NativeWidget(), duration.as_milliseconds()))
    {
        g_object_ref(_internal);
        set_timing_function(Animation::TimingFunction::LINEAR);
    }

    Animation::Animation(detail::AnimationInternal* internal)
        : _internal(internal)
    {
        g_object_ref(_internal);
    }

    Animation::~Animation()
    {
        g_object_unref(_internal);
    }

    NativeObject Animation::get_internal()
    {
        return G_OBJECT(_internal);
    }

    Animation::operator NativeObject() const
    {
        return G_OBJECT(_internal->native);
    }

    void Animation::play()
    {
        auto state = adw_animation_get_state(ADW_ANIMATION(_internal->native));
        if (state == ADW_ANIMATION_PAUSED)
            adw_animation_resume(ADW_ANIMATION(_internal->native));
        else
            adw_animation_play(ADW_ANIMATION(_internal->native));
    }

    void Animation::pause()
    {
        adw_animation_pause(ADW_ANIMATION(_internal->native));
    }

    void Animation::reset()
    {
        adw_animation_reset(ADW_ANIMATION(_internal->native));
    }

    Animation::State Animation::get_state() const
    {
        return (Animation::State) adw_animation_get_state(ADW_ANIMATION(_internal->native));
    }

    Time Animation::get_duration() const
    {
        return milliseconds(adw_timed_animation_get_duration(_internal->native));
    }

    void Animation::set_duration(Time time)
    {
        adw_timed_animation_set_duration(_internal->native, time.as_milliseconds());
    }

    void Animation::set_lower(double value)
    {
        adw_timed_animation_set_value_from(_internal->native, value);
    }

    double Animation::get_lower() const
    {
        return adw_timed_animation_get_value_from(_internal->native);
    }

    void Animation::set_upper(double value)
    {
        adw_timed_animation_set_value_to(_internal->native, value);
    }

    double Animation::get_upper() const
    {
        return adw_timed_animation_get_value_to(_internal->native);
    }

    double Animation::get_value() const
    {
        return adw_animation_get_value(ADW_ANIMATION(_internal->native));
    }

    void Animation::set_repeat_count(uint64_t n)
    {
        adw_timed_animation_set_repeat_count(_internal->native, n);
    }

    uint64_t Animation::get_repeat_count() const
    {
        return adw_timed_animation_get_repeat_count(_internal->native);
    }

    void Animation::set_is_reversed(bool b)
    {
        adw_timed_animation_set_reverse(_internal->native, b);
    }

    bool Animation::get_is_reversed() const
    {
        return adw_timed_animation_get_reverse(_internal->native);
    }

    Animation::TimingFunction Animation::get_timing_function() const
    {
        return (Animation::TimingFunction) adw_timed_animation_get_easing(_internal->native);
    }

    void Animation::set_timing_function(Animation::TimingFunction mode)
    {
        adw_timed_animation_set_easing(_internal->native, (AdwEasing) mode);
    }
}
