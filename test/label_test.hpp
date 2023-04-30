//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/30/23
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class LabelTest : public Widget
    {
        Label center;
        Paned left_center_paned = Paned(Orientation::HORIZONTAL);
        Paned center_right_paned = Paned(Orientation::HORIZONTAL);

        Paned main_paned = Paned(Orientation::VERTICAL);
        Paned main_paned_02 = Paned(Orientation::VERTICAL);

        DropDown wrap_drop_down;
        DropDown ellipsize_drop_down;
        DropDown justify_drop_down;

        Box drop_down_box = Box(Orientation::HORIZONTAL);

        Box main = Box(Orientation::VERTICAL);

        public:
            LabelTest()
            {
                const std::string text = "Lorem ipsum dolor sit amousetrapium, consectetur raditpiscin elit. Nulla at leo tellus. Donec sodales quis enim nec consectetur. Donec consectetur tellus et eros suscipit euismod ut vitae urna. Donec interdum augue orci, in faucibus nunc imperdiet nec. Nullam non lobortis justo. Cras ornare metus non nunc dictum ullamcorper. Vestibulum sodales feugiat sapien quis porta. Sed nec accumsan tellus.";

                center.set_text(text);
                center.set_expand(true);
                center.set_justify_mode(JustifyMode::CENTER);
                center.set_wrap_mode(LabelWrapMode::WORD_OR_CHAR);
                center.set_ellipsize_mode(EllipsizeMode::MIDDLE);

                for (auto pair : std::vector<std::pair<std::string, LabelWrapMode>> {
                    {"NONE",         LabelWrapMode::NONE},
                    {"ONLY_ON_WORD", LabelWrapMode::ONLY_ON_WORD},
                    {"ONY_ON_CHAR",  LabelWrapMode::ONLY_ON_CHAR},
                    {"WORD_OR_CHAR", LabelWrapMode::WORD_OR_CHAR}
                })
                wrap_drop_down.push_back(Label(pair.first), Label("LabelWrapMode::" + pair.first), [value = pair.second](DropDown*, Label* center){
                    center->set_wrap_mode(value);
                }, &center);

                for (auto pair : std::vector<std::pair<std::string, EllipsizeMode>> {
                {"NONE",         EllipsizeMode::NONE},
                {"START", EllipsizeMode::START},
                {"MIDDLE",  EllipsizeMode::MIDDLE},
                {"END", EllipsizeMode::END}
                })
                    ellipsize_drop_down.push_back(Label(pair.first), Label("EllipsizeMode::" + pair.first), [value = pair.second](DropDown*, Label* center){
                        center->set_ellipsize_mode(value);
                    }, &center);

                for (auto pair : std::vector<std::pair<std::string, JustifyMode>> {
                    {"LEFT",         JustifyMode::LEFT},
                    {"CENTER", JustifyMode::CENTER},
                    {"RIGHT",  JustifyMode::RIGHT},
                    {"FILL", JustifyMode::FILL}
                })
                    justify_drop_down.push_back(Label(pair.first), Label("JustifyMode::" + pair.first), [value = pair.second](DropDown*, Label* center){

                        if (value == JustifyMode::CENTER or value == JustifyMode::FILL)
                            center->set_horizontal_alignment(Alignment::CENTER);
                        else if (value == JustifyMode::LEFT)
                            center->set_horizontal_alignment(Alignment::START);
                        else if (value == JustifyMode::RIGHT)
                            center->set_horizontal_alignment(Alignment::END);

                        center->set_justify_mode(value);
                    }, &center);


                for (Widget* w : std::vector<Widget*>{&left_center_paned, &center_right_paned, &main_paned} )
                    w->set_expand(true);

                left_center_paned.set_start_child_shrinkable(false);
                left_center_paned.set_end_child_shrinkable(false);
                center_right_paned.set_start_child_shrinkable(false);
                center_right_paned.set_end_child_shrinkable(false);
                left_center_paned.set_start_child_resizable(true);
                left_center_paned.set_end_child_resizable(true);
                center_right_paned.set_start_child_resizable(true);
                center_right_paned.set_end_child_resizable(true);

                auto sep = [](){
                    auto out = Separator();
                    out.set_expand(true);
                    return out;
                };

                left_center_paned.set_start_child(sep());
                left_center_paned.set_end_child(hbox(center_right_paned));
                center_right_paned.set_start_child(hbox(center));
                center_right_paned.set_end_child(sep());

                drop_down_box.push_back(wrap_drop_down);
                drop_down_box.push_back(ellipsize_drop_down);
                drop_down_box.push_back(justify_drop_down);

                main_paned.set_start_child(left_center_paned);
                main_paned.set_end_child(sep());
                drop_down_box.set_expand_vertically(false);
                main.push_back(drop_down_box);
                main.push_back(main_paned);

                ellipsize_drop_down.set_expand(true);
                wrap_drop_down.set_expand(true);
            }

            operator NativeWidget() const override
            {
                return main.operator NativeWidget();
            }
    };
}
