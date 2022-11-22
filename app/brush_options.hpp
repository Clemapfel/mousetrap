// 
// Copyright 2022 Clemens Cords
// Created on 11/21/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/global_state.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class BrushOptions : public AppComponent
    {
        public:
            BrushOptions();
            ~BrushOptions();

            void update();
            operator Widget*();

        private:
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

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
            Scale _size_scale = Scale(0, 256, 1);
            SpinButton _size_spin_button = SpinButton(0, 10e4-1, 1);

            static void on_size_scale_value_changed(Scale*, BrushOptions*);
            static void on_size_spin_button_value_changed(SpinButton*, BrushOptions*);

            // shapes
            Label _brush_shape_label = Label("Shape");

            class BrushShapeIcon
            {
                public:
                    BrushShapeIcon(BrushOptions* owner);
                    ~BrushShapeIcon();

                    bool load_from(const std::string& path);
                    operator Widget*();

                    void update_tile_size();
                    Vector2ui get_texture_size();
                    Texture* get_texture();

                private:
                    BrushOptions* _owner;

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

            static void on_brush_shape_selection_changed(SelectionModel*, size_t first_item_position, size_t, BrushOptions* instance);
    };
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

        float spin_button_max = std::max<float>(
            _opacity_spin_button.get_preferred_size().natural_size.x,
            _size_spin_button.get_preferred_size().natural_size.x
        );

        for (auto* label : {&_opacity_label, &_size_label, &_brush_shape_label})
            label->set_halign(GTK_ALIGN_START);

        for (auto* box : {&_opacity_box, &_size_box})
            box->set_margin_start(state::margin_unit);

        auto files = get_all_files_in_directory(get_resource_path() + "brushes", false, false);
        std::sort(files.begin(), files.end(), [](FileDescriptor& a, FileDescriptor& b) -> bool {
            return a.get_name() < b.get_name();
        });

        for (auto& file : files)
        {
            auto* icon = _brush_shapes.emplace_back(new BrushShapeIcon(this));
            icon->load_from(file.get_path());

            auto name = file.get_name();
            name = std::string(name.begin(), name.end() - file.get_extension().size());
            icon->operator Widget*()->set_tooltip_text(name);
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

        auto* settings_section = new MenuModel();
        auto* tile_size_menu = new MenuModel();
        tile_size_menu->add_widget(&_tile_size_box);
        settings_section->add_submenu("Tile Size...", tile_size_menu);
        _menu.add_section("Settings", settings_section);
        auto* popover = new PopoverMenu(&_menu);

        _menu_button_label.set_size_request({32, 32});
        _menu_button.set_child(&_menu_button_label);
        _menu_button.set_popover(popover);

        _main.push_back(&_menu_button);
        _main.push_back(&_opacity_label);
        _main.push_back(&_opacity_box);
        _main.push_back(&_size_label);
        _main.push_back(&_size_box);
        _main.push_back(&_brush_shape_label);
        _main.push_back(&_brush_shape_window_frame);
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

    void BrushOptions::on_opacity_scale_value_changed(Scale* scale, BrushOptions* instance)
    {
        instance->_opacity = scale->get_value();
        instance->_opacity_spin_button.set_signal_value_changed_blocked(true);
        instance->_opacity_spin_button.set_value(instance->_opacity);
        instance->_opacity_spin_button.set_signal_value_changed_blocked(false);
    }

    void BrushOptions::on_opacity_spin_button_value_changed(SpinButton* scale, BrushOptions* instance)
    {
        instance->_opacity = scale->get_value();
        instance->_opacity_scale.set_signal_value_changed_blocked(true);
        instance->_opacity_scale.set_value(instance->_opacity);
        instance->_opacity_scale.set_signal_value_changed_blocked(false);
    }

    void BrushOptions::on_size_scale_value_changed(Scale* scale, BrushOptions* instance)
    {
        instance->_size = scale->get_value();
        instance->_size_spin_button.set_signal_value_changed_blocked(true);
        instance->_size_spin_button.set_value(instance->_size);
        instance->_size_spin_button.set_signal_value_changed_blocked(false);
    }

    void BrushOptions::on_size_spin_button_value_changed(SpinButton* scale, BrushOptions* instance)
    {
        instance->_size = scale->get_value();
        instance->_size_scale.set_signal_value_changed_blocked(true);
        instance->_size_scale.set_value(instance->_size);
        instance->_size_scale.set_signal_value_changed_blocked(false);
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
        delete _shape;
    }

    bool BrushOptions::BrushShapeIcon::load_from(const std::string& path)
    {
        _path = path;

        auto image = Image();
        if (not image.create_from_file(_path))
        {
            std::cerr << "[WARNING] Unable to loa brush at `" << path << "`: Image.create_from_file failed" << std::endl;
            return false;
        }

        if (image.get_size().x != image.get_size().y)
        {
            std::cerr << "[WARNING] Unable to load brush at `" << path << "`: Brush textures have to be square"
                      << std::endl;

            _path = "";
            return false;
        }

        _texture = new Texture();

        const auto size = image.get_size();
        static bool once = true;
        for (size_t x = 0; x < size.x; ++x)
        {
            for (size_t y = 0; y < size.y; ++y)
            {
                RGBA color = image.get_pixel(x, y);

                auto as_hsva = color.operator HSVA();
                color.r = as_hsva.v;
                color.g = as_hsva.v;
                color.b = as_hsva.v;

                if (as_hsva.v < 0.05 or color.a < 0.05)
                    color.a = 0;
                else
                    color.a = 1;

                image.set_pixel(x, y, color);
            }
        }

        _texture->create_from_image(image);
        _size = _texture->get_size();

        auto w = _size.x;
        auto h = _size.y;

        std::stringstream label_text;
        label_text << "<span font_scale=\"subscript\" bgcolor=\"black\" bgalpha=\"50%\" fgcolor=\"white\">";
        label_text << std::to_string(w) << "</span>";
        _size_label.set_text(label_text.str());

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

        instance->_shape = new Shape();
        instance->_shape->as_rectangle({0, 0}, {1, 1});
        instance->_shape->set_texture(instance->_texture);

        area->add_render_task(instance->_background);
        area->add_render_task(instance->_shape);
    }

    Vector2ui BrushOptions::BrushShapeIcon::get_texture_size()
    {
        return _texture->get_size();
    }

    void BrushOptions::BrushShapeIcon::update_tile_size()
    {
        _frame.set_size_request(Vector2f(_owner->_tile_size));
    }

    Texture* BrushOptions::BrushShapeIcon::get_texture()
    {
        return _texture;
    }

    void BrushOptions::on_brush_shape_selection_changed(SelectionModel*, size_t first_item_position, size_t,
                                                        BrushOptions* instance)
    {
        state::brush_texture = instance->_brush_shapes.at(first_item_position)->get_texture();
    }
}