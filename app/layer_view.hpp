//
// Copyright 2022 Clemens Cords
// Created on 9/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/column_view.hpp>
#include <include/box.hpp>
#include <include/gl_area.hpp>
#include <include/scale.hpp>
#include <include/label.hpp>
#include <include/toggle_button.hpp>
#include <include/image_display.hpp>
#include <include/dropdown.hpp>
#include <include/get_resource_path.hpp>
#include <include/check_button.hpp>
#include <include/aspect_frame.hpp>

#include <app/global_state.hpp>
#include <app/layer.hpp>

namespace mousetrap
{
    class LayerView : public Widget
    {
        public:
            LayerView();
            operator GtkWidget*() override;

        private:
            static inline Shader* _noop_shader = nullptr;
            static inline Shader* _transparency_tiling_shader = nullptr;

            static inline size_t _layer_thumbnail_size = 48;

            struct LayerViewRow
            {
                LayerViewRow(Layer*, LayerView* parent);

                LayerView* parent;
                Layer* layer;

                GLArea* texture_area;
                Shape* transparency_tiling_shape;
                Shape* layer_texture_shape;
                AspectFrame* texture_area_frame;

                Entry* name;
                FocusEventController* name_focus_controller;

                Box* visible_button_box;
                Label* visible_button_label;
                CheckButton* visible_button;
                static inline const char* layer_hidden_tooltip = "Layer is Hidden";
                static inline const char* layer_visible_tooltip = "Layer is Visible";

                Box* locked_button_box;
                Label* locked_button_label;
                CheckButton* locked_button;
                static inline const char* layer_locked_tooltip = "Layer is Locked";
                static inline const char* layer_unlocked_tooltip = "Layer is Unlocked";

                Scale* opacity_scale;
                DropDown* blend_mode_dropdown;
                std::vector<Label*> blend_mode_dropdown_label_items;
                std::vector<Label*> blend_mode_dropdown_list_items;

                std::vector<SeparatorLine*> separators;

                ListView* main;

                Vector2f* canvas_size;
                static void on_texture_area_realize(GtkGLArea*, LayerViewRow*);
                static void on_texture_area_resize(GtkGLArea*, int, int, LayerViewRow*);

                static void on_is_visible_toggle(GtkCheckButton*, LayerViewRow*);
                static void on_is_locked_toggle(GtkCheckButton*, LayerViewRow*);

                static void on_opacity_select(GtkScale*, LayerViewRow*);

                static void on_layer_name_activate(GtkEntry*, LayerViewRow*);
                static void on_layer_name_focus_lost(GtkEventControllerFocus* self, void*);

                using on_blendmode_selected_data = struct {LayerViewRow* instance; BlendMode mode;};
                static void on_blendmode_selected(void*);
            };

            struct LayerControlBar : public Widget
            {
                LayerControlBar(LayerView* parent_instance);
                operator GtkWidget*() override;

                LayerView* parent;
                Box* main;

                Button* move_up_button;
                ImageDisplay* move_up_label;
                static void on_move_up_pressed(GtkButton*, LayerControlBar*);

                Button* move_down_button;
                ImageDisplay* move_down_label;
                static void on_move_down_pressed(GtkButton*, LayerControlBar*);

                Button* create_button;
                ImageDisplay* create_label;
                static void on_create_pressed(GtkButton*, LayerControlBar*);

                Button* delete_button;
                ImageDisplay* delete_label;
                static void on_delete_pressed(GtkButton*, LayerControlBar*);

                Button* duplicate_button;
                ImageDisplay* duplicate_label;
                static void on_duplicate_pressed(GtkButton*, LayerControlBar*);

                Button* merge_down_button;
                ImageDisplay* merge_down_label;
                static void on_merge_down_pressed(GtkButton*, LayerControlBar*);

                Button* flatten_selected_button;
                ImageDisplay* flatten_selected_label;
                static void on_flatten_selected_pressed(GtkButton*, LayerControlBar*);
            };

            LayerControlBar* _control_bar;

            ScrolledWindow* _row_view_scrolled_window;
            ListView* _row_view;
            std::deque<LayerViewRow*> _rows;

            KeyEventController* _shortcut_controller;
            static gboolean on_shortcut_controller_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, void* data);
            static void on_row_view_selection_changed(GtkSelectionModel* self, guint position, guint n_items, LayerView* instance);

            Box* _main;
            bool initial_update_happened = false;
    };
}

// ###

namespace mousetrap
{
    LayerView::LayerViewRow::LayerViewRow(Layer* layer, LayerView* parent)
        : layer(layer), parent(parent)
    {
        texture_area = new GLArea();
        texture_area->connect_signal("realize", on_texture_area_realize, this);
        texture_area->connect_signal("resize", on_texture_area_resize, this);

        texture_area_frame = new AspectFrame(state::layer_resolution.x / float(state::layer_resolution.y));
        texture_area_frame->set_child(texture_area);

        name = new Entry();
        gtk_editable_set_width_chars(GTK_EDITABLE(name->operator GtkWidget*()), std::string("Layer_9999").size());
        name->set_has_frame(false);
        name->set_text(layer->name);
        name->connect_signal("activate", on_layer_name_activate, this);

        name_focus_controller = new FocusEventController();
        name_focus_controller->connect_leave(on_layer_name_focus_lost, this);
        name->add_controller(name_focus_controller);

        visible_button = new CheckButton();
        visible_button->set_is_checked(layer->is_visible);

        visible_button_label = new Label("");
        visible_button_label->set_use_markup(true);
        visible_button_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        visible_button_box->push_back(visible_button_label);
        visible_button_box->push_back(visible_button);

        locked_button = new CheckButton();
        locked_button->set_is_checked(layer->is_locked);

        locked_button_label = new Label("&#128274;");
        locked_button_label->set_use_markup(true);
        locked_button_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        locked_button_box->push_back(locked_button_label);
        locked_button_box->push_back(locked_button);

        visible_button->connect_signal("toggled", on_is_visible_toggle, this);
        locked_button->connect_signal("toggled", on_is_locked_toggle, this);

        opacity_scale = new Scale(0, 1, 0.01);
        opacity_scale->set_value(layer->opacity);
        opacity_scale->connect_signal("value-changed", on_opacity_select, this);

        blend_mode_dropdown = new DropDown();

        auto add_dropdown_item = [&](BlendMode mode) -> void {

            std::string label;
            std::string list_item;
            std::string label_tooltip;
            std::string list_tooltip;

            if (mode == BlendMode::NORMAL)
            {
                list_item = "Normal";
                label = "Normal";
                label_tooltip = "Blend Mode: Alpha";
                list_tooltip = "<b>Alpha Blending</b>\n<tt>color.rgb = source.a * source.rgb + (1-source.a) * destination.rgb</tt>";
            }
            else if (mode == BlendMode::ADD)
            {
                list_item = "Add";
                label = "Add";
                label_tooltip = "Blend Mode: Additive";
                list_tooltip = "<b>Additive Blending</b>\n<tt>color = source.rgba + destination.rgba</tt>";
            }
            else if (mode == BlendMode::SUBTRACT)
            {
                list_item = "Subtract";
                label = "Subtract";
                label_tooltip = "Blend Mode: Subtractive";
                list_tooltip = "<b>Subtractive Blending</b>\n<tt>color = destination.rgba - source.rgba</tt>";
            }
            else if (mode == BlendMode::MULTIPLY)
            {
                list_item = "Multiply";
                label = "Multiply";
                label_tooltip = "Blend Mode: Multiplicative";
                list_tooltip = "<b>Multiplicative Blending</b>\n<tt>color = source.rgba * destination.rgba</tt>";
            }
            else if (mode == BlendMode::MIN)
            {
                list_item = "Minimum";
                label = "Min";
                label_tooltip = "Blend Mode: Minimum";
                list_tooltip = "<b>Minimum Blending</b>\n<tt>color = min(source.rgba, destination.rgba)</tt>";
            }
            else if (mode == BlendMode::MAX)
            {
                list_item = "Maximum";
                label = "Max";
                label_tooltip = "Blend Mode: Maximum";
                list_tooltip = "<b>Maximum Blending</b>\n<tt>color = max(source.rgba, destination.rgba)</tt>";
            }

            blend_mode_dropdown_list_items.emplace_back(new Label(list_item));
            blend_mode_dropdown_list_items.back()->set_use_markup(true);
            blend_mode_dropdown_list_items.back()->set_tooltip_text(list_tooltip);
            blend_mode_dropdown_list_items.back()->set_halign(GTK_ALIGN_START);

            blend_mode_dropdown_label_items.emplace_back(new Label("<span size=\"small\">" + label + "</span>"));
            blend_mode_dropdown_label_items.back()->set_use_markup(true);
            blend_mode_dropdown_label_items.back()->set_tooltip_text(label_tooltip);

            blend_mode_dropdown->push_back(
                blend_mode_dropdown_list_items.back(),
                blend_mode_dropdown_label_items.back(),
                on_blendmode_selected,
                new on_blendmode_selected_data{this, mode}
            );
        };

        for (auto mode : {BlendMode::NORMAL, BlendMode::ADD, BlendMode::SUBTRACT, BlendMode::MULTIPLY, BlendMode::MIN, BlendMode::MAX})
            add_dropdown_item(mode);

        float row_margin = 1 * state::margin_unit;

        visible_button->set_margin_start(0.5 * state::margin_unit);
        visible_button->set_margin_end(0.5 * state::margin_unit);
        visible_button_box->set_tooltip_text(layer->is_visible ? layer_visible_tooltip : layer_hidden_tooltip);
        visible_button_box->set_margin_top(row_margin);
        visible_button_box->set_margin_bottom(row_margin);

        locked_button->set_margin_start(0.5 * state::margin_unit);
        locked_button_box->set_tooltip_text(layer->is_locked ? layer_locked_tooltip : layer_unlocked_tooltip);
        locked_button_box->set_margin_top(row_margin);
        locked_button_box->set_margin_bottom(row_margin);

        texture_area_frame->set_vexpand(true);
        texture_area_frame->set_size_request({(float(_layer_thumbnail_size) / state::layer_resolution.y) * state::layer_resolution.x, _layer_thumbnail_size});
        texture_area_frame->set_tooltip_text("Layer Preview");

        name->set_margin_top(row_margin);
        name->set_margin_bottom(row_margin);
        name->set_tooltip_text("Layer Name");

        //opacity_scale->set_margin_start(state::margin_unit);
        //opacity_scale->set_margin_end(0.5 * state::margin_unit);
        opacity_scale->set_size_request({100, 0});
        opacity_scale->set_tooltip_text("Layer Opacity");
        opacity_scale->set_hexpand(true);
        opacity_scale->set_margin_top(row_margin);
        opacity_scale->set_margin_bottom(row_margin);

        blend_mode_dropdown->set_margin_top(row_margin);
        blend_mode_dropdown->set_margin_bottom(row_margin);

        main = new ListView(GTK_ORIENTATION_HORIZONTAL);
        main->set_show_separators(true);

        auto add_separator = [&]() {

            separators.emplace_back(new SeparatorLine());
            separators.back()->set_vexpand(true);
            separators.back()->set_size_request({2, 0});
            main->push_back(separators.back());
        };

        main->push_back(visible_button_box);
        main->push_back(texture_area_frame);
        main->push_back(name);
        main->push_back(opacity_scale);
        main->push_back(blend_mode_dropdown);
        main->push_back(locked_button_box);
    }

    void LayerView::LayerViewRow::on_texture_area_realize(GtkGLArea* area, LayerViewRow* instance)
    {
        gtk_gl_area_make_current(area);

        if (LayerView::_noop_shader == nullptr)
            LayerView::_noop_shader = new Shader();

        if (LayerView::_transparency_tiling_shader == nullptr)
        {
            LayerView::_transparency_tiling_shader = new Shader();
            LayerView::_transparency_tiling_shader->create_from_file(
                get_resource_path() + "shaders/transparency_tiling.frag",
                ShaderType::FRAGMENT
            );
        }

        instance->canvas_size = new Vector2f(1, 1);
        instance->layer_texture_shape = new Shape();
        instance->layer_texture_shape->as_rectangle({0, 0}, {1, 1});
        instance->layer_texture_shape->set_texture(instance->layer->texture);

        instance->transparency_tiling_shape = new Shape();
        instance->transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        auto task = RenderTask(instance->transparency_tiling_shape, LayerView::_transparency_tiling_shader);
        task.register_vec2("_canvas_size", instance->canvas_size);

        instance->texture_area->add_render_task(task);
        instance->texture_area->add_render_task(instance->layer_texture_shape, LayerView::_noop_shader);

        gtk_gl_area_queue_render(area);
    }

    void LayerView::LayerViewRow::on_texture_area_resize(GtkGLArea* area, int w, int h, LayerViewRow* instance)
    {
        gtk_gl_area_make_current(area);
        instance->canvas_size->x = w;
        instance->canvas_size->y = h;
        gtk_gl_area_queue_render(area);
    }

    void LayerView::LayerViewRow::on_is_visible_toggle(GtkCheckButton* button, LayerViewRow* instance)
    {
        static const float hidden_opacity = 0.2;

        if (gtk_check_button_get_active(button))
        {
            instance->layer->is_visible = true;
            instance->locked_button_box->set_opacity(1);
            instance->texture_area_frame->set_opacity(1);
            instance->name->set_opacity(1);
            instance->opacity_scale->set_opacity(1);
            instance->blend_mode_dropdown->set_opacity(1);
            instance->visible_button_box->set_tooltip_text(layer_visible_tooltip);
        }
        else
        {
            instance->layer->is_visible = false;
            instance->locked_button_box->set_opacity(hidden_opacity);
            instance->texture_area_frame->set_opacity(hidden_opacity);
            instance->name->set_opacity(hidden_opacity);
            instance->opacity_scale->set_opacity(hidden_opacity);
            instance->blend_mode_dropdown->set_opacity(hidden_opacity);
            instance->visible_button_box->set_tooltip_text(layer_hidden_tooltip);
        }
    }

    void LayerView::LayerViewRow::on_is_locked_toggle(GtkCheckButton* button, LayerViewRow* instance)
    {
        static const char* unlocked_lock = "&#128275;";
        static const char* locked_lock = "&#128274;";

        if (gtk_check_button_get_active(button))
        {
            instance->layer->is_locked = true;
            instance->opacity_scale->set_visible(false);
            instance->blend_mode_dropdown->set_visible(false);
            instance->locked_button_label->set_text(locked_lock);
            instance->locked_button_box->set_tooltip_text(layer_locked_tooltip);
            instance->locked_button_label->set_use_markup(true);
        }
        else
        {
            instance->layer->is_locked = false;
            instance->opacity_scale->set_visible(true);
            instance->blend_mode_dropdown->set_visible(true);
            instance->locked_button_label->set_text(locked_lock);
            instance->locked_button_box->set_tooltip_text(layer_unlocked_tooltip);
            instance->locked_button_label->set_use_markup(true);
        }
    }

    void LayerView::LayerViewRow::on_blendmode_selected(void* data)
    {
        auto* instance = ((on_blendmode_selected_data*) data)->instance;
        auto mode = ((on_blendmode_selected_data*) data)->mode;

        instance->layer->blend_mode = mode;
    }

    void LayerView::LayerViewRow::on_opacity_select(GtkScale* scale, LayerViewRow* instance)
    {
        auto value = gtk_range_get_value(GTK_RANGE(scale));
        instance->layer->opacity = value;
        instance->layer_texture_shape->set_color(RGBA(1, 1, 1, value));
        instance->texture_area->queue_render();
    }

    void LayerView::LayerViewRow::on_layer_name_activate(GtkEntry* entry, LayerViewRow* instance)
    {
        auto* buffer = gtk_entry_get_buffer(entry);
        instance->layer->name = gtk_entry_buffer_get_text(buffer);
        gtk_editable_set_text(GTK_EDITABLE(entry), instance->layer->name.c_str());
        std::cout << instance->layer->name << std::endl;
    }

    void LayerView::LayerViewRow::on_layer_name_focus_lost(GtkEventControllerFocus* self, void* data)
    {
        auto* instance = (LayerViewRow*) data;
        instance->name->set_text(instance->layer->name);
    }
    
    LayerView::LayerControlBar::LayerControlBar(LayerView* parent_instance)
    {
        parent = parent_instance;
        
        static auto new_label = [](const std::string& id) -> ImageDisplay*
        {
            auto* out = new ImageDisplay(get_resource_path() + "icons/" + id + ".png");
            out->set_size_request({32, 32});
            return out;
        };

        move_up_button = new Button();
        move_up_label = new_label("layer_move_up");
        move_up_button->set_child(move_up_label);
        move_up_button->connect_signal("clicked", on_move_up_pressed, this);
        move_up_button->set_tooltip_text("Move Layer Up");

        move_down_button = new Button();
        move_down_label = new_label("layer_move_down");
        move_down_button->set_child(move_down_label);
        move_down_button->connect_signal("clicked", on_move_down_pressed, this);
        move_down_button->set_tooltip_text("Move Layer Down");

        create_button = new Button();
        create_label = new_label("layer_create");
        create_button->set_child(create_label);
        create_button->connect_signal("clicked", on_create_pressed, this);
        create_button->set_tooltip_text("New Layer");

        delete_button = new Button();
        delete_label = new_label("layer_delete");
        delete_button->set_child(delete_label);
        delete_button->connect_signal("clicked", on_delete_pressed, this);
        delete_button->set_tooltip_text("Delete Layer(s)");

        duplicate_button = new Button();
        duplicate_label = new_label("layer_duplicate");
        duplicate_button->set_child(duplicate_label);
        duplicate_button->connect_signal("clicked", on_duplicate_pressed, this);
        duplicate_button->set_tooltip_text("Duplicate Layer");

        merge_down_button = new Button();
        merge_down_label = new_label("layer_merge_down");
        merge_down_button->set_child(merge_down_label);
        merge_down_button->connect_signal("clicked", on_merge_down_pressed, this);
        merge_down_button->set_tooltip_text("Merge Layer Down");

        flatten_selected_button = new Button();
        flatten_selected_label = new_label("layer_flatten_selected");
        flatten_selected_button->set_child(flatten_selected_label);
        flatten_selected_button->connect_signal("clicked", on_flatten_selected_pressed, this);
        flatten_selected_button->set_tooltip_text("Flatten Selected Layers");

        for (auto* button : {move_up_button, move_down_button, create_button, delete_button, duplicate_button, merge_down_button, flatten_selected_button})
        {
            button->set_expand(true);
            button->set_cursor(GtkCursorType::POINTER);
        }

        main = new Box(GTK_ORIENTATION_HORIZONTAL);
        main->push_back(create_button);
        main->push_back(duplicate_button);
        main->push_back(move_up_button);
        main->push_back(move_down_button);
        main->push_back(merge_down_button);
        main->push_back(flatten_selected_button);
        main->push_back(delete_button);
    }

    LayerView::LayerControlBar::operator GtkWidget*()
    {
        return main->operator GtkWidget*();
    }

    void LayerView::LayerControlBar::on_move_up_pressed(GtkButton*, LayerControlBar* instance)
    {
        auto* selection = instance->parent->_row_view->get_selection_model();

        std::vector<size_t> selected;
        auto bitset = gtk_selection_model_get_selection(selection);
        for (size_t i = 0; i < gtk_bitset_get_size(bitset); ++i)
            selected.push_back(gtk_bitset_get_nth(bitset, i));

        assert(selected.size() == 1);

        auto i = selected.at(0);
        std::cout << "moved layer " << state::layers.at(i)->name << " up" << std::endl;
    }

    void LayerView::LayerControlBar::on_move_down_pressed(GtkButton*, LayerControlBar* instance)
    {
        auto* selection = instance->parent->_row_view->get_selection_model();

        std::vector<size_t> selected;
        auto bitset = gtk_selection_model_get_selection(selection);
        for (size_t i = 0; i < gtk_bitset_get_size(bitset); ++i)
            selected.push_back(gtk_bitset_get_nth(bitset, i));

        assert(selected.size() == 1);

        auto i = selected.at(0);
        std::cout << "moved layer " << state::layers.at(i)->name << " down" << std::endl;
    }

    void LayerView::LayerControlBar::on_create_pressed(GtkButton*, LayerControlBar* instance)
    {
        std::cout << "created new layer" << std::endl;
    }

    void LayerView::LayerControlBar::on_delete_pressed(GtkButton*, LayerControlBar* instance)
    {
        auto* selection = instance->parent->_row_view->get_selection_model();

        std::vector<size_t> selected;
        auto bitset = gtk_selection_model_get_selection(selection);
        for (size_t i = 0; i < gtk_bitset_get_size(bitset); ++i)
            selected.push_back(gtk_bitset_get_nth(bitset, i));

        for (auto i : selected)
            std::cout << "deleted layer " << state::layers.at(i)->name << std::endl;
    }

    void LayerView::LayerControlBar::on_duplicate_pressed(GtkButton*, LayerControlBar* instance)
    {
        auto* selection = instance->parent->_row_view->get_selection_model();

        std::vector<size_t> selected;
        auto bitset = gtk_selection_model_get_selection(selection);
        for (size_t i = 0; i < gtk_bitset_get_size(bitset); ++i)
            selected.push_back(gtk_bitset_get_nth(bitset, i));

        assert(selected.size() == 1);

        auto i = selected.at(0);
        std::cout << "duplicated layer " << state::layers.at(i)->name<< std::endl;
    }

    void LayerView::LayerControlBar::on_merge_down_pressed(GtkButton*, LayerControlBar* instance)
    {
        auto* selection = instance->parent->_row_view->get_selection_model();

        std::vector<size_t> selected;
        auto bitset = gtk_selection_model_get_selection(selection);
        for (size_t i = 0; i < gtk_bitset_get_size(bitset); ++i)
            selected.push_back(gtk_bitset_get_nth(bitset, i));

        assert(selected.size() == 1);

        auto i = selected.at(0);
        std::cout << "merged layer " << state::layers.at(i)->name << " onto " << state::layers.at(i+1)->name << std::endl;
    }

    void LayerView::LayerControlBar::on_flatten_selected_pressed(GtkButton*, LayerControlBar* instance)
    {
        auto* selection = instance->parent->_row_view->get_selection_model();

        std::vector<size_t> selected;
        auto bitset = gtk_selection_model_get_selection(selection);
        for (size_t i = 0; i < gtk_bitset_get_size(bitset); ++i)
            selected.push_back(gtk_bitset_get_nth(bitset, i));

        std::cout << "Flattening layers ";
        for (auto i : selected)
            std::cout << state::layers.at(i)->name << " ";

        std::cout << std::endl;
    }

    void LayerView::on_row_view_selection_changed(
        GtkSelectionModel* self,
        guint position,
        guint n_items,
        LayerView* instance)
    {
        size_t temp;
        std::vector<size_t> selected;

        auto bitset = gtk_selection_model_get_selection(self);
        for (size_t i = 0; i < gtk_bitset_get_size(bitset); ++i)
            selected.push_back(gtk_bitset_get_nth(bitset, i));

        static auto set_active = [](Button* button, bool b) {

            button->set_all_signals_blocked(not b);
            button->set_opacity(b ? 1 : 0.2);
            button->set_cursor(b ? GtkCursorType::POINTER : GtkCursorType::NOT_ALLOWED);
        };

        set_active(instance->_control_bar->move_up_button,
            selected.size() == 1 and selected.back() != 0
        );

        set_active(instance->_control_bar->move_down_button,
            selected.size() == 1 and selected.back() != state::layers.size() - 1
        );

        set_active(instance->_control_bar->merge_down_button,
            selected.size() == 1 and selected.back() != state::layers.size() - 1
        );

        set_active(instance->_control_bar->duplicate_button, selected.size() == 1);

        set_active(instance->_control_bar->flatten_selected_button, selected.size() > 1);
        set_active(instance->_control_bar->delete_button, selected.size() >= 1);
    }

    gboolean LayerView::on_shortcut_controller_pressed(
        GtkEventControllerKey* self, guint keyval, guint keycode,
        GdkModifierType modifier, void* data)
    {
        auto* instance = (LayerView*) data;

        bool shift = modifier & GdkModifierType::GDK_SHIFT_MASK;
        bool alt = modifier & GdkModifierType::GDK_ALT_MASK;
        bool control = modifier & GdkModifierType::GDK_CONTROL_MASK;

        // Ctrl + A: Select All
        if (keyval == GDK_KEY_a) // TODO and control and not alt and not shift)
        {
            auto* selection = instance->_row_view->get_selection_model();
            gtk_selection_model_select_all(selection);
        }

        if (keyval == GDK_KEY_Delete)
        {
            instance->_control_bar->on_delete_pressed(
                GTK_BUTTON(instance->_control_bar->delete_button->operator GtkWidget*()),
                instance->_control_bar
            );
        }

        return false;
    }

    LayerView::LayerView()
    {
        _row_view = new ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_MULTIPLE);
        _row_view->set_show_separators(true);
        _row_view->set_hexpand(true);

        auto image = Image();
        image.create_from_file(get_resource_path() + "mole.png");
        state::add_layer(image);
        state::add_layer(RGBA(1, 0, 1, 1));
        state::add_layer(RGBA(0, 0, 1, 1));
        state::add_layer(RGBA(0, 1, 1, 1));

        for (auto* layer : state::layers)
        {
            _rows.emplace_back(new LayerViewRow(layer, this));
            _rows.back()->main->set_hexpand(true);
            _row_view->push_back(_rows.back()->main);
        }

        auto* selection_model = _row_view->get_selection_model();
        g_signal_connect(selection_model, "selection-changed", G_CALLBACK(on_row_view_selection_changed), this);

        _row_view_scrolled_window = new ScrolledWindow();
        _row_view_scrolled_window->set_child(_row_view);
        _row_view_scrolled_window->set_vexpand(true);
        gtk_scrolled_window_set_propagate_natural_width(
            GTK_SCROLLED_WINDOW(_row_view_scrolled_window->operator GtkWidget*()),
            true
        );

        _shortcut_controller = new KeyEventController();
        _shortcut_controller->connect_key_pressed(on_shortcut_controller_pressed, this);
        _row_view_scrolled_window->add_controller(_shortcut_controller);

        _control_bar = new LayerControlBar(this);
        _control_bar->set_hexpand(true);
        _control_bar->set_vexpand(false);
        on_row_view_selection_changed(_row_view->get_selection_model(), 0, 0, this);

        _main = new Box(GTK_ORIENTATION_VERTICAL);;
        _main->push_back(_row_view_scrolled_window);
        _main->push_back(_control_bar);
    }

    LayerView::operator GtkWidget*()
    {
        return _main->operator GtkWidget*();
    }
}