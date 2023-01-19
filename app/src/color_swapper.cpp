#include <app/color_swapper.hpp>
#include <app/verbose_color_picker.hpp>
#include <app/color_picker.hpp>
#include <app/color_preview.hpp>
#include <app/palette_view.hpp>

namespace mousetrap
{
    ColorSwapper::ColorSwapper()
    {
        _render_area.connect_signal_realize(on_gl_area_realize, this);
        _render_area.connect_signal_resize(on_gl_area_resize, this);
        _render_area.set_cursor(GtkCursorType::POINTER);

        _arrow.set_use_markup(true);
        _arrow.set_halign(GTK_ALIGN_START);
        _arrow.set_valign(GTK_ALIGN_END);

        _arrow_overlay.set_child(&_arrow);
        _arrow_overlay.add_overlay(&_render_area);

        _frame.set_margin(state::margin_unit);
        _frame.set_child(&_arrow_overlay);
        _frame.set_expand(true);

        _main.push_back(&_frame);
        _main.set_expand(false);

        _click_event_controller.connect_signal_click_pressed(on_click_release, this);
        _motion_event_controller.connect_signal_motion_enter(on_motion_enter, this);

        _main.add_controller(&_click_event_controller);
        _main.add_controller(&_motion_event_controller);

        state::actions::color_swapper_swap.set_function([]() {
            auto* instance = (ColorSwapper*) state::color_swapper;
            instance->swap_colors();
        });
        state::add_shortcut_action(state::actions::color_swapper_swap);

        _tooltip.create_from("color_swapper", state::tooltips_file, state::keybindings_file);
        operator Widget*()->set_tooltip_widget(_tooltip);
    }

    ColorSwapper::~ColorSwapper()
    {
        delete _primary_color_shape;
        delete _secondary_color_shape;

        delete _primary_color_shape_frame;
        delete _secondary_color_shape_frame;

        delete _primary_color_shape_transparency_tiling;
        delete _secondary_color_shape_transparency_tiling;
    }

    ColorSwapper::operator Widget*()
    {
        return &_main;
    }

    void ColorSwapper::update()
    {
        if (_primary_color_shape != nullptr)
            _primary_color_shape->set_color(state::primary_color);

        if (_secondary_color_shape != nullptr)
            _secondary_color_shape->set_color(state::secondary_color);

        _render_area.queue_render();
    }

    void ColorSwapper::on_gl_area_realize(Widget* widget, ColorSwapper* instance)
    {
        auto* self = (GLArea*) widget;
        self->make_current();
        instance->initialize_render_area();
        instance->_render_area.queue_render();
    }

    void ColorSwapper::on_gl_area_resize(GLArea* self, int w, int h, ColorSwapper* instance)
    {
        instance->_canvas_size.x = w;
        instance->_canvas_size.y = h;

        auto min = std::min(w, h);
        std::string spacer = std::to_string(0.105 * min);
        std::string arrow_size = std::to_string(0.18 * min);
        instance->_arrow.set_text("<span font_size=\"" + spacer + "pt\"> </span><span font_size=\"" + arrow_size + "pt\"><tt>&#8635;</tt></span>");
        instance->_arrow.set_use_markup(true);
        instance->update();
        instance->_render_area.queue_render();
    }

    void ColorSwapper::initialize_render_area()
    {
        if (_transparency_tiling_shader == nullptr)
        {
            _transparency_tiling_shader = new Shader();
            _transparency_tiling_shader->create_from_file(
                    get_resource_path() + "shaders/transparency_tiling.frag",
                    ShaderType::FRAGMENT
            );
        }

        _primary_color_shape = new Shape();
        _secondary_color_shape = new Shape();
        _primary_color_shape_transparency_tiling = new Shape();
        _secondary_color_shape_transparency_tiling = new Shape();

        Vector2f size = {0.75, 0.75};

        _secondary_color_shape->as_rectangle({0.25, 0.25}, size);
        _primary_color_shape->as_rectangle({0, 0}, size);

        _secondary_color_shape_transparency_tiling->as_rectangle({0.25, 0.25}, size);
        _primary_color_shape_transparency_tiling->as_rectangle({0, 0}, size);

        _primary_color_shape_frame = new Shape;
        {
            auto top_left = _primary_color_shape->get_vertex_position(0);
            auto size = _primary_color_shape->get_size();
            std::vector<Vector2f> vertices = {
                    {top_left.x + 0.001, top_left.y},
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y},
                    {top_left.x + 0.001, top_left.y + size.y}
            };
            _primary_color_shape_frame->as_wireframe(vertices);
            _primary_color_shape_frame->set_color(RGBA(0, 0, 0, 1));
        }

        _secondary_color_shape_frame = new Shape;
        {
            auto top_left = _secondary_color_shape->get_vertex_position(0);
            auto size = _secondary_color_shape->get_size();
            std::vector<Vector2f> vertices = {
                    top_left,
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y - 0.001},
                    {top_left.x, top_left.y + size.y - 0.001}
            };
            _secondary_color_shape_frame->as_wireframe(vertices);
            _secondary_color_shape_frame->set_color(RGBA(0, 0, 0, 1));
        }

        _primary_color_shape->set_color(state::primary_color);
        _secondary_color_shape->set_color(state::secondary_color);

        _render_area.clear_render_tasks();

        auto primary_transparency_render_task = RenderTask(_primary_color_shape_transparency_tiling, _transparency_tiling_shader);
        primary_transparency_render_task.register_vec2("_canvas_size", &_canvas_size);

        auto secondary_transparency_render_task = RenderTask(_secondary_color_shape_transparency_tiling, _transparency_tiling_shader);
        secondary_transparency_render_task.register_vec2("_canvas_size", &_canvas_size);

        _render_area.add_render_task(secondary_transparency_render_task);
        _render_area.add_render_task(_secondary_color_shape);
        _render_area.add_render_task(_secondary_color_shape_frame);
        _render_area.add_render_task(primary_transparency_render_task);
        _render_area.add_render_task(_primary_color_shape);
        _render_area.add_render_task(_primary_color_shape_frame);

        _render_area.queue_render();
    }

    void ColorSwapper::swap_colors()
    {
        auto current_primary = state::primary_color;
        auto current_secondary = state::secondary_color;

        _primary_color_shape->set_color(current_secondary);
        _secondary_color_shape->set_color(current_primary);

        state::primary_color = current_secondary;
        state::secondary_color = current_primary;
        state::preview_color_current = state::primary_color;
        state::preview_color_previous = state::primary_color;

        state::verbose_color_picker->update();
        state::color_picker->update();
        state::palette_view->update();
        state::color_preview->update();
        state::canvas->update();

        _render_area.queue_render();
    }

    void ColorSwapper::on_click_release(ClickEventController* self, gint n_press, gdouble x, gdouble y,
            void* instance)
    {
        ((ColorSwapper*) instance)->swap_colors();
        gtk_widget_grab_focus(((ColorSwapper*) instance)->operator Widget*()->operator GtkWidget*());
    }

    void ColorSwapper::on_global_key_pressed(ColorSwapper* instance)
    {
        instance->swap_colors();
    }

    void ColorSwapper::on_motion_enter(MotionEventController* self, gdouble x, gdouble y, void* instance)
    {
        ((ColorSwapper*) instance)->_render_area.grab_focus();
    }
}

