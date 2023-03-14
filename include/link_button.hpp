//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/16/23
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class LinkButton : public WidgetImplementation<GtkLinkButton>
    {
        public:
            LinkButton(const std::string& uri, const std::string& label = "");

            void set_uri(const std::string&);
            std::string get_uri() const;

            void set_visited(bool);
            bool get_visited() const;
    };
}
