#include <app/tooltip.hpp>
#include <app/project_state.hpp>

namespace mousetrap
{
    Tooltip::Tooltip()
    {
        _box.push_back(&_title_label);
        _box.push_back(&_description_label);
        _box.push_back(_shortcut_information);

        _shortcut_information.set_title("Keybindings");
        _shortcut_information.operator Widget*()->set_valign(GTK_ALIGN_END);

        _description_label.set_wrap_mode(LabelWrapMode::WORD_OR_CHAR);

        _window.set_child(&_box);
        _window.set_expand(true);

        for (auto* label : {&_title_label, &_description_label})
        {
            label->set_margin_top(2 * state::margin_unit);
            label->set_margin_horizontal(2 * state::margin_unit);
            label->set_align(GTK_ALIGN_START);
            label->set_justify_mode(JustifyMode::LEFT);
        }
    }

    Tooltip::operator Widget*()
    {
        return &_window;
    }

    void Tooltip::create_from(const std::string& group_name, KeyFile* tooltip_file, KeyFile* keybindings_file)
    {
        if (tooltip_file != nullptr and tooltip_file->has_group(group_name))
        {
            if (tooltip_file->has_key(group_name, "title"))
                set_title(tooltip_file->get_value(group_name, "title"));

            if (tooltip_file->has_key(group_name, "description"))
                set_description(tooltip_file->get_value(group_name, "description"));
        }

        if (tooltip_file != nullptr and keybindings_file->has_group(group_name))
            create_shortcut_information_from(group_name, keybindings_file);
    }

    void Tooltip::set_title(const std::string& title)
    {
        _title = title;
        _title_label.set_text("<b>" + _title + "</b>");
        reformat();
    }

    void Tooltip::set_description(const std::string& description)
    {
        _description = description;
        _description_label.set_text(_description);
        reformat();
    }

    void Tooltip::create_shortcut_information_from(const std::string& group, KeyFile* file)
    {
        _shortcut_information.create_from_group(group, file);
        reformat();
    }

    void Tooltip::add_shortcut(const std::string& shortcut, const std::string& description)
    {
        _shortcut_information.add_shortcut(shortcut, description);
        reformat();
    }

    void Tooltip::reformat()
    {
        _window.set_size_request({_shortcut_information.operator Widget*()->get_preferred_size().natural_size.x, 0});
        _description_label.set_margin_bottom(_shortcut_information.get_n_shortcuts() > 0 ? 0 : 2 * state::margin_unit);
    }
}