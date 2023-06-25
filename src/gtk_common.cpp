//
// Created by clem on 6/25/23.
//

#include <mousetrap/gtk_common.hpp>
#include <mousetrap/log.hpp>

namespace mousetrap::detail
{
    void mark_gtk_initialized()
    {
        if (not GTK_INITIALIZED)
            GTK_INITIALIZED = true;
        else
            log::critical("In detail::mark_gtk_initialized: Attempting to initialize the backend even though it is already initialized-", MOUSETRAP_DOMAIN);
    }

    void throw_if_uninitialized()
    {
        if (GTK_INITIALIZED)
            return;

        log::fatal("Attempting to create an object that depends on the mousetrap backend, even though the backend was not yet initialized. Make sure any mousetrap object is only instantiated after `Application` emitted it's `activate` signal.", MOUSETRAP_DOMAIN);
    }
}
