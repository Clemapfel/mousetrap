// 
// Copyright 2022 Clemens Cords
// Created on 10/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/colors.hpp>
#include <include/flow_box.hpp>
#include <include/scrolled_window.hpp>
#include <include/overlay.hpp>
#include <include/image.hpp>
#include <include/image_display.hpp>
#include <include/menu_button.hpp>
#include <include/label.hpp>
#include <include/box.hpp>
#include <include/gl_area.hpp>
#include <include/aspect_frame.hpp>
#include <include/get_resource_path.hpp>
#include <include/scrollbar.hpp>
#include <include/list_view.hpp>

#include <app/app_component.hpp>
#include <app/palette.hpp>
#include <app/global_state.hpp>

namespace mousetrap
{
    enum class PaletteSortMode
    {
        NONE,
        BY_HUE,
        BY_VALUE,
        BY_SATURATION
    };

    class PaletteView : public AppComponent
    {
        public:
            PaletteView();
            operator Widget*() override;
            void update() override;

            void create_from(Palette&);
            void select(HSVA);

        private:
            class ColorTile
            {
                public:
                    ColorTile(PaletteView* owner, HSVA color);
                    operator Widget*();

                    void set_color(HSVA color);
                    void set_size(size_t);
                    HSVA get_color();
                    void set_selected(bool);

                private:
                    static inline const Vector2f default_size = Vector2f(48);

                    PaletteView* _owner;
                    HSVA _color;

                    GLArea _color_area;
                    GLArea _selection_frame_area;

                    Shape* _color_shape = nullptr;
                    Shape* _frame_shape = nullptr;

                    static inline Texture* is_selected_overlay_texture = nullptr;
                    Shape* _is_selected_shape = nullptr;

                    static void on_color_area_realize(Widget* widget, ColorTile* instance);
                    static void on_selection_frame_realize(Widget* widget, ColorTile* instance);

                    AspectFrame _aspect_frame = AspectFrame(1, 1);
                    Overlay _overlay;
            };

            size_t _previous_selected = 0;
            static void on_flowbox_child_selected(FlowBox*, size_t child_i, PaletteView* instance);

            std::vector<ColorTile*> _color_tiles;
            FlowBox _flow_box = FlowBox(GTK_ORIENTATION_HORIZONTAL);
            ScrolledWindow _scrolled_window;

            Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
    };
}

//

namespace mousetrap
{
    PaletteView::ColorTile::ColorTile(PaletteView* owner, HSVA color)
        : _owner(owner), _color(color)
    {
        _color_area.connect_signal_realize(on_color_area_realize, this);
        _selection_frame_area.connect_signal_realize(on_selection_frame_realize, this);

        _color_area.set_size_request(default_size);
        _selection_frame_area.set_size_request(default_size);

        _overlay.set_child(&_color_area);
        _overlay.add_overlay(&_selection_frame_area);
        _aspect_frame.set_child(&_overlay);

        operator Widget*()->set_align(GTK_ALIGN_CENTER);
        operator Widget*()->set_cursor(GtkCursorType::POINTER);
    }

    void PaletteView::ColorTile::set_color(HSVA color)
    {
        _color = color;
        if (_color_shape != nullptr)
            _color_shape->set_color(color);

        _color_area.queue_render();
    }

    void PaletteView::ColorTile::set_size(size_t size)
    {
        _aspect_frame.set_size_request({size, size});
    }

    HSVA PaletteView::ColorTile::get_color()
    {
        return _color;
    }

    void PaletteView::ColorTile::set_selected(bool b)
    {
        if (_is_selected_shape != nullptr)
            _is_selected_shape->set_visible(b);

        _selection_frame_area.queue_render();
    }

    PaletteView::ColorTile::operator Widget*()
    {
        return &_aspect_frame;
    }

    void PaletteView::ColorTile::on_color_area_realize(Widget* widget, ColorTile* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_color_shape = new Shape();
        instance->_color_shape->as_rectangle({0, 0}, {1, 1});
        instance->_color_shape->set_color(instance->_color);

        instance->_frame_shape = new Shape();
        float eps = 0.001;
        instance->_frame_shape->as_wireframe({
             {eps, eps},
             {eps + (1 - eps), eps},
             {(1 - eps), (1 - eps)},
             {eps, (1 - eps)}
        });
        instance->_frame_shape->set_color(RGBA(0, 0, 0, 1));

        area->add_render_task(instance->_color_shape);
        area->add_render_task(instance->_frame_shape);

        area->queue_render();
    }

    void PaletteView::ColorTile::on_selection_frame_realize(Widget* widget, ColorTile* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (is_selected_overlay_texture == nullptr)
        {
            is_selected_overlay_texture = new Texture();
            is_selected_overlay_texture->create_from_file(get_resource_path() + "icons/" + "selected_indicator" + ".png");
        }

        instance->_is_selected_shape = new Shape();
        instance->_is_selected_shape->as_rectangle({0, 0}, {1, 1});
        instance->_is_selected_shape->set_texture(is_selected_overlay_texture);
        instance->_is_selected_shape->set_visible(false);

        area->add_render_task(instance->_is_selected_shape);
        area->queue_render();
    }

    PaletteView::operator Widget*()
    {
        return &_main;
    }

    void PaletteView::select(HSVA color)
    {
        for (auto* tile : _color_tiles)
            tile->set_selected(tile->get_color() == color);
    }

    void PaletteView::create_from(Palette& palette)
    {
        for (auto* tile : _color_tiles)
            delete tile;

        _color_tiles.clear();

        for (auto& color : palette.get_colors())
            _color_tiles.emplace_back(new ColorTile(this, color));
    }

    void PaletteView::on_flowbox_child_selected(FlowBox* flowbox, size_t child_i, PaletteView* instance)
    {
        instance->_color_tiles.at(instance->_previous_selected)->set_selected(false);
        instance->_color_tiles.at(child_i)->set_selected(true);
        instance->_previous_selected = child_i;

        state::primary_color = instance->_color_tiles.at(child_i)->get_color();
        state::preview_color_current = state::primary_color;
        state::preview_color_previous = state::primary_color;

        state::update_color_picker();
        state::update_verbose_color_picker();
        state::update_color_swapper();
        state::update_color_preview();
    }

    PaletteView::PaletteView()
    {
        create_from(state::palettes.at(state::active_palette_id));

        for (auto* tile : _color_tiles)
            _flow_box.push_back(tile->operator Widget*());

        _scrolled_window.set_child(&_flow_box);
        _scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
        _main.push_back(&_scrolled_window);

        _scrolled_window.set_hexpand(true);
        _main.set_hexpand(true);

        _flow_box.set_selection_mode(GTK_SELECTION_SINGLE);
        _flow_box.connect_signal_child_activated(on_flowbox_child_selected, this);

        _color_tiles.at(_previous_selected)->set_selected(true);
        _flow_box.unselect_all();
    }

    void PaletteView::update()
    {
        auto color = state::primary_color;
        _flow_box.unselect_all();

        for (size_t i = 0; i < _color_tiles.size(); ++i)
        {
            auto* tile = _color_tiles.at(i);
            bool should_select = tile->get_color() == color;
            tile->set_selected(should_select);

            if (should_select)
                _flow_box.select(i);
        }
    }
}