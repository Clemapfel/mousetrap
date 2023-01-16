#include <app/shortcut_information.hpp>

namespace mousetrap
{
    ShortcutInformation::ShortcutInformation()
    {}

    void ShortcutInformation::update()
    {
        _main.clear();

        delete _shortcuts;
        _shortcuts = new ShortcutGroupDisplay(_title, _accelerators_and_descriptions);

        _main.clear();
        _main.push_back(_shortcuts);
    }

    ShortcutInformation::operator Widget*()
    {
        return &_main;
    }

    ShortcutInformation::~ShortcutInformation()
    {
        delete _shortcuts;
    }

    void ShortcutInformation::set_title(const std::string& text)
    {
        _title = text;
    }

    void ShortcutInformation::add_shortcut(const std::string& accelerator, const std::string& description)
    {
        _accelerators_and_descriptions.emplace_back(accelerator, description);
        update();
    }

    void ShortcutInformation::create_from_group(const std::string& group, KeyFile* file)
    {
        auto keys = file->get_keys(group);
        _accelerators_and_descriptions.clear();

        for (auto& key : keys)
            _accelerators_and_descriptions.emplace_back(file->get_value(group, key), file->get_comment_above(group, key));

        update();
    }

    size_t ShortcutInformation::get_n_shortcuts()
    {
        return _accelerators_and_descriptions.size();
    }
}
