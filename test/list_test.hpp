#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class ListTest : public Widget
    {
        private:
            Box main = Box(Orientation::HORIZONTAL);

            ListView linear = ListView(Orientation::VERTICAL);
            ListView nested = ListView(Orientation::VERTICAL);

            Paned paned = Paned(Orientation::HORIZONTAL);

        public:
            operator NativeWidget() const override
            {
                return main;
            }

            ListTest()
            {
                auto label = [](const std::string& string) -> Label {
                    auto out = Label(string);
                    //out.set_horizontal_alignment(Alignment::START);
                    return out;
                };

                linear.push_back(label("Label 01"));
                linear.push_back(label("Label 02"));
                linear.push_back(label("Label 03"));

                nested.push_back(label("Outer 01"));
                auto it = nested.push_back(label("Outer 02"));
                nested.push_back(label("Outer 03"));

                nested.push_back(label("Inner 01"), it);
                auto it2 = nested.push_back(label("Inner 02"), it);
                nested.push_back(label("Inner 03"), it);

                nested.push_back(label("Inner Inner 01"), it2);
                nested.push_back(label("Inner Inner 02"), it2);
                nested.push_back(label("Inner Inner 03"), it2);

                for (auto* list : {
                    &linear,
                    &nested
                })
                    list->set_margin(10);

                paned.set_start_child(linear);
                paned.set_end_child(nested);
                paned.set_expand(true);

                main.push_back(paned);
            }
    };
}