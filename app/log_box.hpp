//
// Created by clem on 3/8/23.
//

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/app_signals.hpp>

namespace mousetrap
{
    class LogBox : public AppComponent,
        public signals::CursorPositionChanged,
        public signals::SavePathChanged
    {
        public:
            LogBox();
            operator Widget*();

        protected:
            void on_cursor_position_changed() override;
            void on_save_path_changed() override;

        private:
            void set_cursor_position(Vector2i);
            void set_save_path(const std::string&);

            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

            SeparatorLine _spacer;
            Label _cursor_position_label = Label("");
            Label _current_save_path_label = Label("");
    };
}

namespace mousetrap::state
{
    inline LogBox* log_box = nullptr;
}