//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/17/23
//

namespace mousetrap
{
    namespace detail
    {
        struct _SignalEmitterInternal
        {
            GObject parent;
            std::map<std::string, detail::SignalHandler>* signal_handlers;
        };
        using SignalEmitterInternal = _SignalEmitterInternal;
    }

    template<typename Function_t>
    void SignalEmitter::connect_signal(const std::string& signal_id, Function_t* function, void* data)
    {
        initialize();
        auto handler_id = g_signal_connect(operator GObject*(), signal_id.c_str(), G_CALLBACK(function), data);
        _internal->signal_handlers->insert_or_assign(signal_id, detail::SignalHandler{handler_id});
    }
}