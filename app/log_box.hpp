//
// Created by clem on 3/8/23.
//

#include <mousetrap.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class LogBox : public AppComponent
    {
        public:
            LogBox();
            operator Widget*();

        private:
            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

            Label _current_save_path_label;
    };
}

namespace mousetrap::state
{
    inline LogBox* log_box = nullptr;
}