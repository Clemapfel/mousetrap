// 
// Copyright 2022 Clemens Cords
// Created on 8/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/colors.hpp>
#include <include/image.hpp>
#include <include/flow_box.hpp>
#include <include/scrolled_window.hpp>

namespace mousetrap
{
    static inline std::vector<HSVA> palette {};
    std::vector<HSVA> sort_by_hue(const std::vector<HSVA>& palette);

    class PaletteView : public Widget
    {
        public:
            PaletteView(GtkOrientation);

            GtkWidget* get_native() {
                return _main->get_native();
            };

        private:
            static inline float _tile_size = 64;

            static void on_child_activated(GtkFlowBox* self, GtkFlowBoxChild* child, PaletteView* instance);
            void reformat();

            FlowBox* _box;
            ScrolledWindow* _main;

            struct ColorTile
            {
                GtkImage* _image;
            };

            std::vector<ColorTile*> _tiles;
    };
}

// ###

namespace mousetrap
{
    PaletteView::PaletteView(GtkOrientation orientation)
    {
        // TODO
        size_t n = 32;
        for (size_t i = 0; i <= n; ++i)
        {
            palette.push_back(RGBA(i / float(n), i / float(n), i / float(n), 1).operator HSVA());
        }
        // TODO

        _box = new FlowBox();
        _box->connect_signal("child-activated", on_child_activated, this);
        _box->set_max_children_per_line(1);
        _box->set_selection_mode(GTK_SELECTION_SINGLE);

        _main = new ScrolledWindow();
        _main->set_policy(GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
        _main->set_placement(GTK_CORNER_BOTTOM_RIGHT);
        _main->set_kinetic_scrolling_enabled(false);
        _main->add(_box);
        _main->set_size_request({_tile_size, 1});

        reformat();
    }

    void PaletteView::reformat()
    {
        auto image = Image();
        image.create(_tile_size, _tile_size, RGBA(0, 0, 0, 1));

        for (auto* t : _tiles)
            delete t;

        _tiles.clear();


        for (auto& color : palette)
        {
            for (size_t x = 1; x < image.get_size().x - 1; ++x)
                for (size_t y = 1; y < image.get_size().y - 1; ++y) // ignore frame
                    image.set_pixel(x, y, color);

            _tiles.push_back(new ColorTile());
            _tiles.back()->_image = GTK_IMAGE(gtk_image_new_from_pixbuf(image.to_pixbuf()));
        }

        for (auto* tile : _tiles)
            _box->add(GTK_WIDGET(tile->_image));
    }

    void PaletteView::on_child_activated(GtkFlowBox* self, GtkFlowBoxChild* child, PaletteView* instance)
    {
        size_t i = gtk_flow_box_child_get_index(child);
        auto color = palette.at(i);

        std::cout << "selected " << i << std::endl;
    }

    std::vector<HSVA> sort_by_hue(const std::vector<HSVA>& palette)
    {
        std::vector<HSVA> grayscale;
        std::vector<HSVA> non_grayscale;

        for (const auto& color : palette)
        {
            if (color.s == 0)
                grayscale.push_back(color);
            else
                non_grayscale.push_back(color);
        }

        std::stable_sort(grayscale.begin(), grayscale.end(), [](HSVA a, HSVA b) -> bool {
            return a.s < b.s;
        });

        std::stable_sort(non_grayscale.begin(), non_grayscale.end(), [](HSVA a, HSVA b) -> bool {
           return a.h < b.h;
        });

        std::vector<HSVA> out;
        out.reserve(palette.size());

        for (auto& color : grayscale)
            out.push_back(color);

        for (auto& color : non_grayscale)
            out.push_back(color);

        return out;
    }

    std::vector<HSVA> sort_by_saturation(const std::vector<HSVA>& palette);
    std::vector<HSVA> sort_by_value(const std::vector<HSVA>& palette);
}