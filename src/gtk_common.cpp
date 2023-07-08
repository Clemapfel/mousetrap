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
    }

    void throw_if_gtk_is_uninitialized()
    {
        if (GTK_INITIALIZED)
            return;

        log::fatal(detail::notify_if_gtk_uninitialized::message, MOUSETRAP_DOMAIN);
    }
}
