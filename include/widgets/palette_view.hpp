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
            static inline size_t _tile_size = 32 + 16;
            static inline RGBA _selection_frame_color = mousetrap::YELLOW;

            static void on_child_activated(GtkFlowBox* self, GtkFlowBoxChild* child, PaletteView* instance);
            void reformat();

            size_t _selected = 0;
            void select(size_t i);

            FlowBox* _box;
            ScrolledWindow* _main;

            struct ColorTile
            {
                GtkImage* _color_tile;
                GtkImage* _selection_frame;
                Overlay* _overlay;

                ~ColorTile() {
                    delete _color_tile;
                    delete _selection_frame;
                    delete _overlay;
                }
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
            palette.push_back(HSVA(float(rand()) / RAND_MAX, 1, 1, 1));
        }
        // TODO

        _box = new FlowBox();
        _box->connect_signal("child-activated", on_child_activated, this);
        _box->set_max_children_per_line(1);
        _box->set_selection_mode(GTK_SELECTION_SINGLE);

        _main = new ScrolledWindow();
        _main->set_policy(GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        _main->set_placement(GTK_CORNER_BOTTOM_RIGHT);
        _main->set_kinetic_scrolling_enabled(false);
        _main->add(_box);
        _main->set_size_request({_tile_size, 1});

        reformat();
    }

    void PaletteView::reformat()
    {
        size_t size = _tile_size;

        auto color_image = Image();
        color_image.create(size, size, RGBA(0, 0, 0, 0));

        auto selection_frame = Image();
        selection_frame.create(size, size, RGBA(0, 0, 0, 0));

        for (size_t i = 0; i < size - 0; ++i)
        {
            selection_frame.set_pixel(i, 0, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(0, i, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(i, size - 1, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(size - 1, i, RGBA(0, 0, 0, 1));

            color_image.set_pixel(i, 0, RGBA(0, 0, 0, 1));
            color_image.set_pixel(0, i, RGBA(0, 0, 0, 1));
            color_image.set_pixel(i, size - 1, RGBA(0, 0, 0, 1));
            color_image.set_pixel(size - 1, i, RGBA(0, 0, 0, 1));
        }

        for (size_t i = 1; i < size - 1; ++i)
        {
            selection_frame.set_pixel(i, 1, _selection_frame_color);
            selection_frame.set_pixel(1, i, _selection_frame_color);
            selection_frame.set_pixel(i, size - 2, _selection_frame_color);
            selection_frame.set_pixel(size - 2, i, _selection_frame_color);
        }

        for (size_t i = 2; i < size - 2; ++i)
        {
            selection_frame.set_pixel(i, 2, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(2, i, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(i, size-3, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(size-3, i, RGBA(0, 0, 0, 1));
        }

        for (size_t i = 3; i < size - 3; ++i)
        {
            selection_frame.set_pixel(i, 3, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(3, i, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(i, size-4, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(size-4, i, RGBA(0, 0, 0, 1));
        }

        for (auto* t : _tiles)
            delete t;

        _tiles.clear();

        for (auto& color : palette)
        {
            auto as_rgba = color.operator RGBA();

            for (size_t x = 1; x < size - 1; ++x)
                for (size_t y = 1; y < size - 1; ++y)
                    color_image.set_pixel(x, y, as_rgba);

            _tiles.push_back(new ColorTile());
            auto* tile = _tiles.back();

            tile->_color_tile = GTK_IMAGE(gtk_image_new_from_pixbuf(color_image.to_pixbuf()));
            tile->_selection_frame = GTK_IMAGE(gtk_image_new_from_pixbuf(selection_frame.to_pixbuf()));
            gtk_widget_set_opacity(GTK_WIDGET(tile->_selection_frame), 0);

            tile->_overlay = new Overlay();
            tile->_overlay->set_under(GTK_WIDGET(tile->_color_tile));
            tile->_overlay->set_over(GTK_WIDGET(tile->_selection_frame));
        }

        for (auto* tile : _tiles)
            _box->add(tile->_overlay);

        select(_selected);
    }

    void PaletteView::select(size_t i)
    {
        gtk_widget_set_opacity(GTK_WIDGET(_tiles.at(_selected)->_selection_frame), 0);
        gtk_widget_set_opacity(GTK_WIDGET(_tiles.at(i)->_selection_frame), 1);
        _selected = i;
    }

    void PaletteView::on_child_activated(GtkFlowBox* self, GtkFlowBoxChild* child, PaletteView* instance)
    {
        instance->select(gtk_flow_box_child_get_index(child));
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