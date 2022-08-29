// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
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

#include <vector>

#include <app/global_state.hpp>
#include <app/keybindings.hpp>

namespace mousetrap
{
    enum class PaletteSortMode
    {
        NONE,
        BY_HUE,
        BY_VALUE,
        BY_SATURATION
    };

    class PaletteView : public Widget
    {
        public:
            PaletteView();
            virtual ~PaletteView();

            operator GtkWidget*() override;
            void update() override;

            void set_tile_size(size_t);
            void select_color(HSVA);

            void set_selection_frame_color(RGBA);
            void set_sort_mode(PaletteSortMode);

        private:
            static inline size_t _tile_size = 32 + 16;
            static inline RGBA _selection_frame_color = mousetrap::YELLOW;

            static void on_child_activated(GtkFlowBox* self, GtkFlowBoxChild* child, PaletteView* instance);

            static gboolean on_key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, void* instance);
            KeyEventController* _key_event_controller;

            size_t _selected_index = 0;
            void select(size_t);
            void select_none();

            std::string _selected_palette_id = "";
            PaletteSortMode _sort_mode = PaletteSortMode::NONE;

            FlowBox* _box;
            ScrolledWindow* _scrolled_window;
            Label* _menu_button_label;
            MenuButton* _menu_button;
            Box* _main;

            struct ColorTile
            {
                ColorTile() = default;

                HSVA _color;
                ImageDisplay* _color_tile = nullptr;
                ImageDisplay* _selection_frame = nullptr;
                Overlay* _overlay = nullptr;
            };

            std::vector<ColorTile*> _tiles;
    };
}

// ###

namespace mousetrap
{
    PaletteView::operator GtkWidget*()
    {
        return _main->operator GtkWidget*();
    }

    PaletteView::PaletteView()
    {
        _box = new FlowBox();
        _box->connect_signal("child-activated", on_child_activated, this);
        _box->set_selection_mode(GTK_SELECTION_SINGLE);
        _box->set_column_spacing(0);
        _box->set_row_spacing(0);

        _scrolled_window = new ScrolledWindow();
        _scrolled_window->set_policy(GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        _scrolled_window->set_placement(GTK_CORNER_BOTTOM_RIGHT);
        _scrolled_window->set_kinetic_scrolling_enabled(false);
        _scrolled_window->set_child(_box);
        _scrolled_window->set_size_request({_tile_size, 1});
        _scrolled_window->set_vexpand(true);

        _menu_button = new MenuButton();
        _menu_button_label = new Label("Palette >");
        _menu_button->set_child(_menu_button_label);
        _menu_button->set_vexpand(false);

        _main = new Box(GTK_ORIENTATION_VERTICAL);
        _main->push_back(_menu_button);
        _main->push_back(_scrolled_window);

        _key_event_controller = new KeyEventController();
        _key_event_controller->connect_key_pressed(on_key_pressed, this);
        _main->add_controller(_key_event_controller);

        update();
    }

    PaletteView::~PaletteView()
    {
        delete _box;
        delete _main;

        for (auto* tile : _tiles)
            delete tile;
    }

    void PaletteView::update()
    {
        for (auto* tile : _tiles)
            delete tile;
        
        _tiles.clear();
        _box->clear();

        auto color_image = Image();
        color_image.create(_tile_size, _tile_size, RGBA(0, 0, 0, 0));

        auto selection_frame = Image();
        selection_frame.create(_tile_size, _tile_size, RGBA(0, 0, 0, 0));

        for (size_t i = 0; i < _tile_size - 0; ++i)
        {
            selection_frame.set_pixel(i, 0, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(0, i, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(i, _tile_size - 1, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(_tile_size - 1, i, RGBA(0, 0, 0, 1));

            color_image.set_pixel(i, 0, RGBA(0, 0, 0, 1));
            color_image.set_pixel(0, i, RGBA(0, 0, 0, 1));
            color_image.set_pixel(i, _tile_size - 1, RGBA(0, 0, 0, 1));
            color_image.set_pixel(_tile_size - 1, i, RGBA(0, 0, 0, 1));
        }

        for (size_t i = 1; i < _tile_size - 1; ++i)
        {
            selection_frame.set_pixel(i, 1, _selection_frame_color);
            selection_frame.set_pixel(1, i, _selection_frame_color);
            selection_frame.set_pixel(i, _tile_size - 2, _selection_frame_color);
            selection_frame.set_pixel(_tile_size - 2, i, _selection_frame_color);
        }

        for (size_t i = 2; i < _tile_size - 2; ++i)
        {
            selection_frame.set_pixel(i, 2, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(2, i, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(i, _tile_size-3, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(_tile_size-3, i, RGBA(0, 0, 0, 1));
        }

        for (size_t i = 3; _tile_size >= 16 and i < _tile_size - 3; ++i)
        {
            selection_frame.set_pixel(i, 3, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(3, i, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(i, _tile_size-4, RGBA(0, 0, 0, 1));
            selection_frame.set_pixel(_tile_size-4, i, RGBA(0, 0, 0, 1));
        }

        std::vector<HSVA> palette;

        _menu_button_label->set_text(state::active_palette_id + " >");

        if (_sort_mode == PaletteSortMode::NONE)
            palette = state::palettes.at(state::active_palette_id).get_colors();
        else if (_sort_mode == PaletteSortMode::BY_HUE)
            palette = state::palettes.at(state::active_palette_id).get_colors_by_hue();
        else if (_sort_mode == PaletteSortMode::BY_SATURATION)
            palette = state::palettes.at(state::active_palette_id).get_colors_by_saturation();
        else if (_sort_mode == PaletteSortMode::BY_VALUE)
            palette = state::palettes.at(state::active_palette_id).get_colors_by_value();

        if (palette.empty())
        {
            std::cerr << "[ERROR] In PaletteView::update: Palette with id \"" << state::active_palette_id
                      << "\" does not have any colors." << std::endl;

            palette = {HSVA(0, 0, 0, 1)};
        }

        for (auto& color : palette)
        {
            auto as_rgba = color.operator RGBA();

            for (size_t x = 1; x < _tile_size - 1; ++x)
                for (size_t y = 1; y < _tile_size - 1; ++y)
                    color_image.set_pixel(x, y, as_rgba);

            _tiles.push_back(new ColorTile());
            auto& tile = _tiles.back();

            tile->_color = color;
            tile->_color_tile = new ImageDisplay(color_image);
            tile->_selection_frame = new ImageDisplay(selection_frame);
            tile->_selection_frame->set_visible(false);

            tile->_overlay = new Overlay();
            tile->_overlay->set_child(tile->_color_tile);
            tile->_overlay->add_overlay(tile->_selection_frame);
            tile->_overlay->set_size_request({_tile_size, _tile_size});
        }

        for (auto& tile : _tiles)
            _box->push_back(tile->_overlay);

        _box->set_homogeneous(false);

        select(_selected_index);
        _selected_palette_id = state::active_palette_id;

        _box->show();

        std::stringstream tooltip_str;
        tooltip_str << "<u><b>Palette View</b></u>\n"
                    << "<tt>+/-</tt>: Increase / Decrease Tile Size\n"
                    << "<tt>1-9</tt>: Select Color";

        _main->set_tooltip_text(tooltip_str.str());
    }

    void PaletteView::set_tile_size(size_t size)
    {
        if (size == 0)
        {
            std::cerr << "[WARNING] In PaletteView::set_tile_size: Ignoring size request of 0x0" << std::endl;
            return;
        }

        if (size != _tile_size)
        {
            _tile_size = size;
            update();
        }
    }

    void PaletteView::select(size_t i)
    {
        if (i >= _tiles.size())
        {
            std::cerr << "[ERROR] In PaletteView::select: index " << i << " out of range for a view with " << _tiles.size() << " tiles" << std::endl;
            return;
        }

        _tiles.at(_selected_index)->_selection_frame->set_visible(false);
        _tiles.at(i)->_selection_frame->set_visible(true);

        _box->set_all_signals_blocked(true);
        gtk_widget_activate(GTK_WIDGET(_box->get_child_at_index(i)));
        _box->set_all_signals_blocked(false);
        _selected_index = i;

        state::primary_color = _tiles.at(i)->_color;
        state::color_picker->update();
        state::primary_secondary_color_swapper->update();
        state::verbose_color_picker->update();
    }

    void PaletteView::select_none()
    {
        _tiles.at(_selected_index)->_selection_frame->set_visible(false);
    }

    void PaletteView::on_child_activated(GtkFlowBox* self, GtkFlowBoxChild* child, PaletteView* instance)
    {
        instance->select(gtk_flow_box_child_get_index(child));
    }

    void PaletteView::select_color(HSVA color)
    {
        for (size_t i = 0; i < _tiles.size(); ++i)
        {
            if (_tiles.at(i)->_color == color)
            {
                select(i);
                return;
            }
        }

        select_none();
    }

    void PaletteView::set_selection_frame_color(RGBA color)
    {
        _selection_frame_color = color;
        update();
    }

    void PaletteView::set_sort_mode(PaletteSortMode mode)
    {
        _sort_mode = mode;
        update();
    }

    gboolean PaletteView::on_key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state,
                                void* data)
    {
        PaletteView* instance = reinterpret_cast<PaletteView*>(data);

        if (keyval == get_keybinding("palette_view_select_1"))
            instance->select(0);
        else if (keyval == get_keybinding("palette_view_select_2"))
            instance->select(1);
        else if (keyval == get_keybinding("palette_view_select_3"))
            instance->select(2);
        else if (keyval == get_keybinding("palette_view_select_4"))
            instance->select(3);
        else if (keyval == get_keybinding("palette_view_select_5"))
            instance->select(4);
        else if (keyval == get_keybinding("palette_view_select_6"))
            instance->select(5);
        else if (keyval == get_keybinding("palette_view_select_7"))
            instance->select(6);
        else if (keyval == get_keybinding("palette_view_select_8"))
            instance->select(7);
        else if (keyval == get_keybinding("palette_view_select_9"))
            instance->select(8);

        if (keyval == GDK_KEY_plus)
            instance->set_tile_size(instance->_tile_size + 8);
        else if (keyval == GDK_KEY_minus)
            instance->set_tile_size(instance->_tile_size - 8);

        return true;
    }

    /*
    gboolean PaletteView::on_enter_notify_event(GtkWidget* self, GdkEventCrossing* event, PaletteView* instance)
    {
        if (event->type == GDK_ENTER_NOTIFY)
            instance->_keypress_mode_active = true;

        else if (event->type == GDK_LEAVE_NOTIFY)
            instance->_keypress_mode_active = false;

        return true;
    }

    gboolean PaletteView::on_key_press_event(GtkWidget* self, GdkEventKey* event, PaletteView* instance)
    {
        if (not instance->_keypress_mode_active)
        {
            if (event->keyval == get_keybinding("palette_view_increase_tile_size"))
            {
                instance->set_tile_size(_tile_size + 4);
            }
            else if (event->keyval == get_keybinding("palette_view_decrease_tile_size"))
            {
                if (instance->_tile_size > 8)
                    instance->set_tile_size(_tile_size - 4);
            }
        }

        if (event->keyval == get_keybinding("palette_view_select_1"))
            instance->select(0);
        else if (event->keyval == get_keybinding("palette_view_select_2"))
            instance->select(1);
        else if (event->keyval == get_keybinding("palette_view_select_3"))
            instance->select(2);
        else if (event->keyval == get_keybinding("palette_view_select_4"))
            instance->select(3);
        else if (event->keyval == get_keybinding("palette_view_select_5"))
            instance->select(4);
        else if (event->keyval == get_keybinding("palette_view_select_6"))
            instance->select(5);
        else if (event->keyval == get_keybinding("palette_view_select_7"))
            instance->select(6);
        else if (event->keyval == get_keybinding("palette_view_select_8"))
            instance->select(7);
        else if (event->keyval == get_keybinding("palette_view_select_9"))
            instance->select(8);

        return true;
    }
     */
}
