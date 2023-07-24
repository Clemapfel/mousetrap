//
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#include <iostream>
#include <mousetrap/signal_emitter.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(SignalEmitterInternal, signal_emitter_internal, SIGNAL_EMITTER_INTERNAL)

        static void signal_emitter_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_SIGNAL_EMITTER_INTERNAL(object);   
            delete self->signal_handlers;

            G_OBJECT_CLASS(signal_emitter_internal_parent_class)->finalize(object);
        }
        
        DEFINE_NEW_TYPE_TRIVIAL_INIT(SignalEmitterInternal, signal_emitter_internal, SIGNAL_EMITTER_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(SignalEmitterInternal, signal_emitter_internal, SIGNAL_EMITTER_INTERNAL)

        static SignalEmitterInternal* signal_emitter_internal_new()
        {
            auto* self = (SignalEmitterInternal*) g_object_new(signal_emitter_internal_get_type(), nullptr);
            signal_emitter_internal_init(self);

            self->signal_handlers = new std::map<std::string, detail::SignalHandler>();
            return self;
        }
    }

    SignalEmitter::SignalEmitter()
        : _internal(nullptr)
    {}

    SignalEmitter::~SignalEmitter()
    {
        if (_internal != nullptr)
            g_object_unref(_internal);
    }

    NativeObject SignalEmitter::get_internal() const
    {
        const_cast<SignalEmitter*>(this)->initialize();
        return G_OBJECT(_internal);
    }

    void SignalEmitter::initialize()
    {
        _internal = detail::get_data<detail::SignalEmitterInternal>(operator GObject*(), "_signal_emitter_internal");
        if (_internal == nullptr)
        {
            _internal = detail::signal_emitter_internal_new();
            detail::set_data(operator GObject*(), "_signal_emitter_internal", _internal);
        }

        g_object_ref(_internal);
    }
    
    void SignalEmitter::set_signal_blocked(const std::string& signal_id, bool b)
    {
        initialize();

        auto it = _internal->signal_handlers->find(signal_id);
        if (it == _internal->signal_handlers->end())
        {
            log::critical("In SignalEmitter::set_signal_blocked: no signal with id \"" + signal_id + "\" connected.", MOUSETRAP_DOMAIN);
            return;
        }

        if (b)
        {
            g_signal_handler_block(operator GObject*(), it->second.id);
            it->second.is_blocked = true;
        }
        else
        {
            g_signal_handler_unblock(operator GObject*(), it->second.id);
            it->second.is_blocked = false;
        }

    }

    bool SignalEmitter::get_signal_blocked(const std::string& signal_id) const
    {
        const_cast<SignalEmitter*>(this)->initialize();

        auto it = _internal->signal_handlers->find(signal_id);
        if (it == _internal->signal_handlers->end())
            return true;

        return it->second.is_blocked;
    }

    void SignalEmitter::disconnect_signal(const std::string& signal_id)
    {
        initialize();

        auto it = _internal->signal_handlers->find(signal_id);
        if (it == _internal->signal_handlers->end())
            return;

        g_signal_handler_disconnect(operator NativeObject(), it->second.id);
    }

    std::vector<std::string> SignalEmitter::get_all_signal_names()
    {
        initialize();

        std::vector<std::string> out;
        guint n;
        auto* ids = g_signal_list_ids(gtk_file_chooser_get_type(), &n);
        for (size_t i = 0; i < n; ++i)
            out.emplace_back(g_signal_name(ids[i]));

        g_free(ids);
        return out;
    }
}