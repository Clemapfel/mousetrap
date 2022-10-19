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
            bool palette_locked = true;
            
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

            class PaletteControlBar
            {
                public:
                    PaletteControlBar(PaletteView* owner);
                    operator Widget*();
                    
                private:
                    PaletteView* _owner;
                    
                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                    ImageDisplay _palette_locked_icon = ImageDisplay(get_resource_path() + "icons/palette_locked.png");
                    ImageDisplay _palette_not_locked_icon = ImageDisplay(get_resource_path() + "icons/palette_not_locked.png");
                    ToggleButton _palette_locked_toggle_button;

                    static void on_palette_locked_button_toggled(ToggleButton*, PaletteControlBar* instance);

                    MenuButton _menu_button;
                    MenuModel _menu;

                    MenuModel _load_save_menu_section;
                    static void on_menu_load(void*);
                    static void on_menu_load_default(void*);
                    static void on_menu_save(void*);
                    static void on_menu_save_as(void*);
                    static void on_menu_save_as_default(void*);

                    MenuModel _sort_menu_section;
                    static void on_menu_sort_by_default(void* data);
                    static void on_menu_sort_by_hue(void* data);
                    static void on_menu_sort_by_saturation(void* data);
                    static void on_menu_sort_by_value(void* data);
            };

            PaletteControlBar _palette_control_bar = PaletteControlBar(this);

            size_t _previous_selected = 0;
            static void on_flowbox_child_selected(FlowBox*, size_t child_i, PaletteView* instance);

            std::vector<ColorTile*> _color_tiles;
            FlowBox _flow_box = FlowBox(GTK_ORIENTATION_HORIZONTAL);
            ScrolledWindow _scrolled_window;

            Box _palette_view_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Overlay _overlay;
    };
}

//

namespace mousetrap
{
    // COLOR TILE
    
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

    // CONTROL BAR

    void PaletteView::PaletteControlBar::on_palette_locked_button_toggled(ToggleButton* button, PaletteControlBar* instance)
    {
        auto value = button->get_active();
        instance->_owner->palette_locked = value;
        instance->_palette_locked_toggle_button.set_child(value ? &instance->_palette_locked_icon : &instance->_palette_not_locked_icon);
    }

    void PaletteView::PaletteControlBar::on_menu_load(void*)
    {}

    void PaletteView::PaletteControlBar::on_menu_load_default(void*)
    {}

    void PaletteView::PaletteControlBar::on_menu_save(void*)
    {}

    void PaletteView::PaletteControlBar::on_menu_save_as(void*)
    {}

    void PaletteView::PaletteControlBar::on_menu_save_as_default(void*)
    {}

    void PaletteView::PaletteControlBar::on_menu_sort_by_default(void* data)
    {}

    void PaletteView::PaletteControlBar::on_menu_sort_by_hue(void* data)
    {}

    void PaletteView::PaletteControlBar::on_menu_sort_by_saturation(void* data)
    {}

    void PaletteView::PaletteControlBar::on_menu_sort_by_value(void* data)
    {}

    PaletteView::PaletteControlBar::operator Widget*()
    {
        return &_main;
    }

    PaletteView::PaletteControlBar::PaletteControlBar(PaletteView* owner)
        : _owner(owner)
    {
        _palette_locked_toggle_button.set_active(owner->palette_locked);
        _palette_locked_toggle_button.set_child(owner->palette_locked ? &_palette_locked_icon : &_palette_not_locked_icon);
        _palette_locked_toggle_button.connect_signal_toggled(on_palette_locked_button_toggled, this);
        
        state::app->add_action("palette_view.load", on_menu_load, (void*) nullptr);
        state::app->add_action("palette_view.load_default", on_menu_load_default, (void*) nullptr);
        state::app->add_action("palette_view.save", on_menu_save, (void*) nullptr);
        state::app->add_action("palette_view.save_as", on_menu_save_as, (void*) nullptr);
        state::app->add_action("palette_view.save_as_default", on_menu_save_as_default, (void*) nullptr);

        _load_save_menu_section.add_action("Load...", "app.palette_view.load");
        _load_save_menu_section.add_action("Load Default", "app.palette_view.load_default");
        _load_save_menu_section.add_action("Save", "app.palette_view.save");
        _load_save_menu_section.add_action("Save As...", "app.palette_view.save_as");
        _load_save_menu_section.add_action("Save As Default", "app.palette_view.save_as_default");

        state::app->add_action("palette_view.sort_by_default", on_menu_sort_by_default, this);
        state::app->add_action("palette_view.sort_by_hue", on_menu_sort_by_hue, this);
        state::app->add_action("palette_view.sort_by_saturation", on_menu_sort_by_saturation, this);
        state::app->add_action("palette_view.sort_by_value", on_menu_sort_by_value, this);

        _sort_menu_section.add_action("None", "app.palette_view.sort_by_default");
        _sort_menu_section.add_action("Hue", "app.palette_view.sort_by_hue");
        _sort_menu_section.add_action("Saturation", "app.palette_view.sort_by_saturation");
        _sort_menu_section.add_action("Value", "app.palette_view.sort_by_saturation");

        _menu.add_submenu("Sort By", &_sort_menu_section);
        _menu.add_section("Save / Load", &_load_save_menu_section);

        _menu_button.set_model(&_menu);
        
        _main.push_back(&_palette_locked_toggle_button);
        _main.push_back(&_menu_button);
    }
    
    // PALETTE VIEW

    PaletteView::operator Widget*()
    {
        return &_overlay;
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
        _palette_view_box.push_back(&_scrolled_window);

        _scrolled_window.set_hexpand(true);
        _palette_view_box.set_hexpand(true);

        _flow_box.set_selection_mode(GTK_SELECTION_SINGLE);
        _flow_box.connect_signal_child_activated(on_flowbox_child_selected, this);

        _color_tiles.at(_previous_selected)->set_selected(true);
        _flow_box.unselect_all();

        _overlay.set_child(&_palette_view_box);
        auto* control_bar = _palette_control_bar.operator Widget*();
        control_bar->set_expand(false);
        control_bar->set_halign(GTK_ALIGN_END);
        control_bar->set_valign(GTK_ALIGN_START);
        _overlay.add_overlay(control_bar);
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