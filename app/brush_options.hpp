// 
// Copyright 2022 Clemens Cords
// Created on 11/21/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/global_state.hpp>
#include <app/app_component.hpp>
#include <app/canvas.hpp>
#include <app/bubble_log_area.hpp>
#include <app/file_chooser_dialog.hpp>

namespace mousetrap
{
    class BrushOptions : public AppComponent
    {
        public:
            BrushOptions();
            ~BrushOptions();

            void update();
            operator Widget*();

            void reload_default_brushes();
            void select_brush(size_t);

        private:
            Box _main = Box(GTK_ORIENTATION_VERTICAL);
            Box _content_box = Box(GTK_ORIENTATION_VERTICAL);

            size_t max_brush_size = state::settings_file->get_value_as<size_t>("brush_options", "maximum_brush_size");

            // settings
            MenuModel _menu;
            MenuModel _tile_size_submenu;
            PopoverMenu _popover_menu = PopoverMenu(&_menu);

            size_t _tile_size = state::settings_file->get_value_as<size_t>("brush_options", "brush_icon_tile_size");

            Box _tile_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _tile_size_label = Label("Tile Size (px): ");
            SpinButton _tile_size_spin_button = SpinButton(2, 256, 1);
            static void on_tile_size_spin_button_value_changed(SpinButton*, BrushOptions* instance);

            MenuButton _menu_button;
            Label _menu_button_label = Label("Brush Options");

            // opacity
            float _opacity = state::settings_file->get_value_as<float>("brush_options", "brush_opacity");

            Box _opacity_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _opacity_label = Label("Opacity");
            Scale _opacity_scale = Scale(0, 1, 0.001);
            SpinButton _opacity_spin_button = SpinButton(0, 1, 0.001);

            static void on_opacity_scale_value_changed(Scale*, BrushOptions*);
            static void on_opacity_spin_button_value_changed(SpinButton*, BrushOptions*);

            // size
            size_t _size = state::settings_file->get_value_as<size_t>("brush_options", "brush_size");

            Box _size_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Label _size_label = Label("Size");
            Scale _size_scale = Scale(1, max_brush_size, 1);
            SpinButton _size_spin_button = SpinButton(1, max_brush_size, 1);

            static void on_size_scale_value_changed(Scale*, BrushOptions*);
            static void on_size_spin_button_value_changed(SpinButton*, BrushOptions*);

            // shapes
            Label _brush_shape_label = Label("Shape");

            class BrushShapeIcon
            {
                public:
                    BrushShapeIcon(BrushOptions* owner);
                    ~BrushShapeIcon();

                    void set_brush(Brush*);
                    Brush* get_brush();
                    operator Widget*();

                    void update_tile_size();
                    Vector2ui get_texture_size();

                private:
                    BrushOptions* _owner;
                    Brush* _brush;

                    Frame _frame;
                    AspectFrame _aspect_frame = AspectFrame(1);

                    Overlay _overlay;

                    GLArea _area;
                    Shape* _shape = nullptr;
                    Shape* _background = nullptr;
                    Texture* _texture = nullptr;

                    std::string _path;
                    Vector2ui _size;
                    Label _size_label;

                    static void on_area_realize(Widget*, BrushShapeIcon* instance);
            };

            std::vector<BrushShapeIcon*> _brush_shapes;
            GridView _brush_shape_view = GridView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);
            ScrolledWindow _brush_shape_window;
            Frame _brush_shape_window_frame;

            void add_brush(Brush*);
            static void on_brush_shape_selection_changed(SelectionModel*, size_t first_item_position, size_t, BrushOptions* instance);

            ShortcutController _shortcut_controller = ShortcutController(state::app);
            Action _increase_brushsize_action = Action("brush_options.increase_brush_size");
            Action _decrease_brushsize_action = Action("brush_options.decrease_brush_size");

            Action _reload_default_brushes_action = Action("brush_options.reload_default_brushes");

            Action _add_brush_action = Action("brush_options.add_brush");
            OpenFileDialog _add_brush_dialog = OpenFileDialog("Select Brush Image...");

            Tooltip _tooltip;
    };


    namespace state
    {
        void reload_brushes();
    }
}

//

namespace mousetrap
{
    BrushOptions::BrushOptions()
    {
        _opacity_label.set_hexpand(false);
        _opacity_scale.set_hexpand(true);
        _opacity_spin_button.set_hexpand(false);

        _opacity_box.push_back(&_opacity_scale);
        _opacity_box.push_back(&_opacity_spin_button);
        _opacity_scale.connect_signal_value_changed(on_opacity_scale_value_changed, this);
        _opacity_spin_button.connect_signal_value_changed(on_opacity_spin_button_value_changed, this);
        _opacity_scale.set_value(_opacity);
        _opacity_spin_button.set_value(_opacity);
        _opacity_spin_button.set_focusable(true);
        _opacity_spin_button.set_focus_on_click(true);

        _size_label.set_hexpand(false);
        _size_scale.set_hexpand(true);
        _size_spin_button.set_hexpand(false);

        _size_box.push_back(&_size_scale);
        _size_box.push_back(&_size_spin_button);
        _size_scale.connect_signal_value_changed(on_size_scale_value_changed, this);
        _size_spin_button.connect_signal_value_changed(on_size_spin_button_value_changed, this);
        _size_scale.set_value(_size);
        _size_spin_button.set_value(_size);
        _size_spin_button.set_focusable(true);
        _size_spin_button.set_focus_on_click(true);

        for (auto* label : {&_opacity_label, &_size_label, &_brush_shape_label})
            label->set_halign(GTK_ALIGN_START);

        for (auto* box : {&_opacity_box, &_size_box})
            box->set_margin_start(state::margin_unit);

        for (auto* brush : state::brushes)
        {
            auto* icon = _brush_shapes.emplace_back(new BrushShapeIcon(this));
            icon->set_brush(brush);

            std::stringstream name;
            name << brush->get_name() << " ("
                 << brush->get_base_image().get_size().x << "x"
                 << brush->get_base_image().get_size().y << ")";

            icon->operator Widget*()->set_tooltip_text(name.str());
            _brush_shape_view.push_back(icon->operator Widget*());
        }

        _brush_shape_view.get_selection_model()->connect_signal_selection_changed(on_brush_shape_selection_changed, this);
        _brush_shape_window.set_child(&_brush_shape_view);
        _brush_shape_window.set_propagate_natural_width(true);
        _brush_shape_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

        _brush_shape_window_frame.set_label_widget(nullptr);
        _brush_shape_window_frame.set_child(&_brush_shape_window);
        _brush_shape_window_frame.set_margin_horizontal(state::margin_unit);
        _brush_shape_window_frame.set_margin_vertical(state::margin_unit);

        _tile_size_spin_button.set_margin_start(state::margin_unit);
        _tile_size_spin_button.set_value(_tile_size);
        _tile_size_spin_button.connect_signal_value_changed(on_tile_size_spin_button_value_changed, this);
        _tile_size_box.push_back(&_tile_size_label);
        _tile_size_box.push_back(&_tile_size_spin_button);
        _tile_size_box.set_margin(state::margin_unit);

        _reload_default_brushes_action.set_do_function([](BrushOptions* instance) {
            state::reload_brushes();
            instance->reload_default_brushes();
            ((BubbleLogArea*) state::bubble_log)->send_message("Loaded brushes from `" + get_resource_path() + "brushes`");
        }, this);
        state::app->add_action(_reload_default_brushes_action);

        auto filter = FileFilter("Image File");
        filter.add_allow_all_supported_image_formats();
        _add_brush_dialog.get_file_chooser().add_filter(filter, true);
        _add_brush_dialog.set_on_cancel_pressed([](OpenFileDialog*, BrushOptions* instance){
            instance->_add_brush_dialog.close();
        }, this);

        _add_brush_dialog.set_on_accept_pressed([](OpenFileDialog* dialog, BrushOptions* instance){

            auto path = dialog->get_current_name();
            auto image = Image();

            if (not image.create_from_file(path))
            {
                ((BubbleLogArea*) state::bubble_log)->send_message("Unable to open file `" + path  + "`", InfoMessageType::ERROR);
                dialog->close();
                return;
            }

            if (image.get_size().x > instance->max_brush_size or image.get_size().y > instance->max_brush_size)
            {
                std::stringstream message;
                message << "Unable to add brush from `" << path << "`: Image is larger than " << instance->max_brush_size << "x" << instance->max_brush_size;
                ((BubbleLogArea*) state::bubble_log)->send_message(message.str(), InfoMessageType::ERROR);
                dialog->close();
                return;
            }

            std::stringstream name;
            auto file = FileDescriptor(path);
            for (size_t i = 0; i < file.get_name().size() - file.get_extension().size(); ++i)
                name << file.get_name().at(i);

            auto brush = new Brush();
            brush->create_from_image(image, name.str());
            state::brushes.emplace_back(brush);
            instance->add_brush(brush);
            instance->select_brush(state::brushes.size() - 1);
            ((BubbleLogArea*) state::bubble_log)->send_message("New Brush `" + brush->get_name() + "` added");
            dialog->close();

        }, this);

        _add_brush_action.set_do_function([](BrushOptions* instance){
            instance->_add_brush_dialog.show();
        }, this);
        state::app->add_action(_add_brush_action);

        auto settings_section = MenuModel();

        auto tile_size_menu = MenuModel();
        tile_size_menu.add_widget(&_tile_size_box);
        settings_section.add_submenu("Preview Size...", &tile_size_menu);

        auto brush_section = MenuModel();
        brush_section.add_action("Add...", "brush_options.add_brush");
        brush_section.add_action("Load Defaults", "brush_options.reload_default_brushes");

        _menu.add_section("Settings", &settings_section);
        _menu.add_section("Brushes", &brush_section);

        auto* popover = new PopoverMenu(&_menu);

        _menu_button_label.set_size_request({32, 32});
        _menu_button.set_child(&_menu_button_label);
        _menu_button.set_popover(popover);

        _content_box.push_back(&_opacity_label);
        _content_box.push_back(&_opacity_box);
        _content_box.push_back(&_size_label);
        _content_box.push_back(&_size_box);
        _content_box.push_back(&_brush_shape_label);
        _content_box.push_back(&_brush_shape_window_frame);

        _content_box.set_margin_top(state::margin_unit);
        _content_box.set_margin_horizontal(state::margin_unit);

        _main.push_back(&_menu_button);
        _main.push_back(&_content_box);

        select_brush(0);

        _increase_brushsize_action.set_do_function([](BrushOptions* instance){
            instance->_size_spin_button.set_value(instance->_size_spin_button.get_value() + 1);
        }, this);
        _increase_brushsize_action.add_shortcut(state::keybindings_file->get_value("brush_options", "increase_brush_size"));
        state::app->add_action(_increase_brushsize_action);
        _shortcut_controller.add_action("brush_options.increase_brush_size");

        _decrease_brushsize_action.set_do_function([](BrushOptions* instance){
            instance->_size_spin_button.set_value(instance->_size_spin_button.get_value() - 1);
        }, this);
        _decrease_brushsize_action.add_shortcut(state::keybindings_file->get_value("brush_options", "decrease_brush_size"));
        state::app->add_action(_decrease_brushsize_action);
        _shortcut_controller.add_action("brush_options.decrease_brush_size");

        _tooltip.create_from("brush_options", state::tooltips_file, state::keybindings_file);
        operator Widget*()->set_tooltip_widget(_tooltip.operator Widget*());

        auto size_tooltip = state::tooltips_file->get_value("brush_options", "brush_size");
        _size_box.set_tooltip_text(size_tooltip);
        _size_label.set_tooltip_text(size_tooltip);

        auto opacity_tooltip = state::tooltips_file->get_value("brush_options", "brush_opacity");
        _opacity_box.set_tooltip_text(opacity_tooltip);
        _opacity_label.set_tooltip_text(opacity_tooltip);

        auto shape_tooltip = state::tooltips_file->get_value("brush_options", "brush_shape");
        _brush_shape_window_frame.set_tooltip_text(shape_tooltip);
        _brush_shape_label.set_tooltip_text(shape_tooltip);
    }

    BrushOptions::~BrushOptions()
    {
        for (auto* icon : _brush_shapes)
            delete icon;
    }

    void BrushOptions::update()
    {}

    BrushOptions::operator Widget*()
    {
        return &_main;
    }

    void BrushOptions::add_brush(Brush* brush)
    {
        auto* icon = _brush_shapes.emplace_back(new BrushShapeIcon(this));
        icon->set_brush(brush);

        std::stringstream name;
        name << brush->get_name() << " ("
             << brush->get_base_image().get_size().x << "x"
             << brush->get_base_image().get_size().y << ")";

        icon->operator Widget*()->set_tooltip_text(name.str());
        _brush_shape_view.push_back(icon->operator Widget*());
    }

    void BrushOptions::on_opacity_scale_value_changed(Scale* scale, BrushOptions* instance)
    {
        instance->_opacity = scale->get_value();
        instance->_opacity_spin_button.set_signal_value_changed_blocked(true);
        instance->_opacity_spin_button.set_value(instance->_opacity);
        instance->_opacity_spin_button.set_signal_value_changed_blocked(false);

        state::brush_opacity = instance->_opacity;
        state::update_canvas();
    }

    void BrushOptions::on_opacity_spin_button_value_changed(SpinButton* scale, BrushOptions* instance)
    {
        instance->_opacity = scale->get_value();
        instance->_opacity_scale.set_signal_value_changed_blocked(true);
        instance->_opacity_scale.set_value(instance->_opacity);
        instance->_opacity_scale.set_signal_value_changed_blocked(false);

        state::brush_opacity = instance->_opacity;
        state::update_canvas();
    }

    void BrushOptions::on_size_scale_value_changed(Scale* scale, BrushOptions* instance)
    {
        auto value = scale->get_value();

        instance->_size = value;
        instance->_size_spin_button.set_signal_value_changed_blocked(true);
        instance->_size_spin_button.set_value(instance->_size);
        instance->_size_spin_button.set_signal_value_changed_blocked(false);

        state::brush_size = instance->_size;
        state::current_brush->set_size(state::brush_size);
        state::update_canvas();
    }

    void BrushOptions::on_size_spin_button_value_changed(SpinButton* scale, BrushOptions* instance)
    {
        auto value = scale->get_value();

        instance->_size = value;
        instance->_size_scale.set_signal_value_changed_blocked(true);
        instance->_size_scale.set_value(instance->_size);
        instance->_size_scale.set_signal_value_changed_blocked(false);

        state::brush_size = instance->_size;
        state::current_brush->set_size(state::brush_size);
        state::update_canvas();
    }

    void BrushOptions::on_tile_size_spin_button_value_changed(SpinButton* scale, BrushOptions* instance)
    {
        instance->_tile_size = scale->get_value();
        for (auto* tile : instance->_brush_shapes)
            tile->update_tile_size();
    };

    BrushOptions::BrushShapeIcon::BrushShapeIcon(BrushOptions* owner)
        : _owner(owner)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.set_expand(true);

        _frame.set_child(&_area);
        _frame.set_label_widget(nullptr);

        _aspect_frame.set_child(&_frame);

        _overlay.set_child(&_aspect_frame);
        _size_label.set_align(GTK_ALIGN_END);
        _overlay.add_overlay(&_size_label);

        update_tile_size();
    }

    BrushOptions::BrushShapeIcon::~BrushShapeIcon()
    {
        if (_shape != nullptr)
            delete _shape;
    }

    void BrushOptions::BrushShapeIcon::set_brush(Brush* brush)
    {
        _brush = brush;
        _size = _brush->get_base_image().get_size();

        std::stringstream label_text;
        label_text << "<span font_scale=\"subscript\" bgcolor=\"black\" bgalpha=\"50%\" fgcolor=\"white\">";
        label_text << std::to_string(_brush->get_base_size()) << "</span>";
        _size_label.set_text(label_text.str());

        if (_texture != nullptr)
            _texture->create_from_image(_brush->get_base_image());

        if (_shape != nullptr)
            _shape->set_texture(_texture);

        _area.queue_render();
    }

    BrushOptions::BrushShapeIcon::operator Widget*()
    {
        return &_overlay;
    }

    void BrushOptions::BrushShapeIcon::on_area_realize(Widget* widget, BrushShapeIcon* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        static const float margin = 0.05;

        instance->_background = new Shape();
        instance->_background->as_rectangle({0, 0}, {1, 1});
        instance->_background->set_color(RGBA(0, 0, 0, 1));

        instance->_texture = new Texture();
        instance->_shape = new Shape();
        instance->_shape->as_rectangle({0, 0}, {1, 1});
        instance->_shape->set_texture(instance->_texture);

        area->add_render_task(instance->_background);
        area->add_render_task(instance->_shape);

        instance->set_brush(instance->_brush);
    }

    Vector2ui BrushOptions::BrushShapeIcon::get_texture_size()
    {
        return _brush->get_texture()->get_size();
    }

    void BrushOptions::BrushShapeIcon::update_tile_size()
    {
        _frame.set_size_request(Vector2f(_owner->_tile_size));
    }

    Brush* BrushOptions::BrushShapeIcon::get_brush()
    {
        return _brush;
    }

    void BrushOptions::on_brush_shape_selection_changed(SelectionModel*, size_t first_item_position, size_t,
                                                        BrushOptions* instance)
    {
        state::current_brush = instance->_brush_shapes.at(first_item_position)->get_brush();

        state::update_canvas();

        instance->_size_spin_button.set_signal_value_changed_blocked(true);
        instance->_size_scale.set_signal_value_changed_blocked(true);

        auto size = state::current_brush->get_base_size();

        instance->_size_spin_button.set_value(size);
        instance->_size_scale.set_value(size);

        instance->_size_spin_button.set_signal_value_changed_blocked(false);
        instance->_size_scale.set_signal_value_changed_blocked(false);
    }

    void state::reload_brushes()
    {
        state::brushes.clear();

        auto files = get_all_files_in_directory(get_resource_path() + "brushes", false, false);
        std::sort(files.begin(), files.end(), [](FileDescriptor& a, FileDescriptor& b) -> bool {
            return a.get_name() < b.get_name();
        });

        for (auto& file : files)
        {
            auto* brush = new Brush();
            if (brush->create_from_file(file.get_path()))
                state::brushes.emplace_back(brush);
        }

        if (state::brushes.empty())
        {
            auto default_brush_image = Image();
            default_brush_image.create(1, 1, RGBA(1, 1, 1, 1));
            state::brushes.emplace_back(new Brush())->create_from_image(default_brush_image, "default");
        }

        state::current_brush = state::brushes.at(0);

        if (state::brush_options != nullptr)
            ((BrushOptions*) state::brush_options)->reload_default_brushes();
    }

    void BrushOptions::reload_default_brushes()
    {
        auto current_brush_name = state::current_brush->get_name();
        auto current_brush_size = _size;

        _brush_shape_view.get_selection_model()->set_all_signals_blocked(true);
        _brush_shape_view.clear();

        for (auto* ptr : _brush_shapes)
            delete ptr;

        _brush_shapes.clear();

        for (auto* brush : state::brushes)
            add_brush(brush);

        size_t to_select = 0;
        for (size_t i = 0; i < _brush_shapes.size(); ++i)
            if (_brush_shapes.at(i)->get_brush()->get_name() == current_brush_name)
                to_select = i;

        select_brush(to_select);
        _brush_shape_view.get_selection_model()->set_all_signals_blocked(false);
    }

    void BrushOptions::select_brush(size_t i)
    {
        on_brush_shape_selection_changed(_brush_shape_view.get_selection_model(), i, 0, this);
    }
}