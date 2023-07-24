//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/17/23
//

#include <map>

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
        this->connect_signal_implementation(signal_id, (void*) function, (void*) data);
    }
}