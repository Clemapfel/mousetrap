//
// Created by clem on 8/16/23.
//

#include <mousetrap/style_manager.hpp>
#include <mousetrap/log.hpp>

#include <cctype>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(StyleClassInternal, style_class_internal, STYLE_CLASS_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(StyleClassInternal, style_class_internal, STYLE_CLASS_INTERNAL)

        static void style_class_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_STYLE_CLASS_INTERNAL(object);
            g_object_unref(self->provider);
            delete self->target_to_properties;
            G_OBJECT_CLASS(style_class_internal_parent_class)->finalize(object);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(StyleClassInternal, style_class_internal, STYLE_CLASS_INTERNAL)

        static StyleClassInternal* style_class_internal_new(const std::string& name)
        {
            auto* self = (StyleClassInternal*) g_object_new(style_class_internal_get_type(), nullptr);
            style_class_internal_init(self);

            self->provider = gtk_css_provider_new();
            self->target_to_properties = new std::map<std::string, std::map<std::string, std::string>>();
            self->name = new std::string(name);
            return self;
        }

        bool validate_css_name(const std::string& name)
        {
            if (name.empty())
            {
                log::critical("In StyleManager::validate_css_name: Name `\"\"` is not a valid identifier", MOUSETRAP_DOMAIN);
                return false;
            }

            if (name.at(0) == '@')
            {
                log::critical("In StyleManager::validate_css_name: Name `" + name + "` is invalid because it starts with `@`", MOUSETRAP_DOMAIN);
                return false;
            }

            for (char c : name)
            {
                if (not (std::isalpha(c) or std::isdigit(c) or c == '-' or c == '_'))
                {
                    log::critical("In StyleManager::validate_css_name: Name `" + name + "` is invalid because contains non-alphabetic characters", MOUSETRAP_DOMAIN);
                    return false;
                }
            }

            return true;
        }
    }
    
    void StyleManager::add_style_class(const StyleClass& style)
    {
        auto it = _style_classes.find(style.get_name());
        auto* internal = detail::MOUSETRAP_STYLE_CLASS_INTERNAL(style.operator NativeObject());
        auto serialized = style.serialize();
        gtk_css_provider_load_from_data(internal->provider, serialized.data(), serialized.size());

        if (it != _style_classes.end())
            remove_style_class(style);

        _style_classes.insert_or_assign(style.get_name(), internal);
        gtk_style_context_add_provider_for_display(
    gdk_display_get_default(),
    GTK_STYLE_PROVIDER(internal->provider),
    GTK_STYLE_PROVIDER_PRIORITY_USER
        );

        g_object_ref(internal);
    }

    void StyleManager::remove_style_class(const StyleClass& style)
    {
        auto* internal = detail::MOUSETRAP_STYLE_CLASS_INTERNAL(style.operator NativeObject());
        gtk_style_context_remove_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(internal->provider)
        );
        g_object_unref(internal);
    }

    StyleClass StyleManager::get_style_class(const std::string& name)
    {
        auto it = _style_classes.find(name);
        if (it == _style_classes.end())
        {
            log::critical("In StyleManager::get_style_class: No style class with name `" + name + "` registered.", MOUSETRAP_DOMAIN);
            return StyleClass(nullptr);
        }
        else
        {
            return StyleClass(it->second);
        }
    }

    void StyleManager::add_css(const std::string& css)
    {
        auto* css_provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(css_provider, css.data(), css.size());

        gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
        );
    }

    std::string StyleManager::color_to_css(RGBA color)
    {
        std::stringstream expression;
        expression << "rgba("
                   << int(std::round(color.r * 255)) << ", "
                   << int(std::round(color.g * 255)) << ", "
                   << int(std::round(color.b * 255)) << ", "
                   << int(std::round(color.a * 255)) << ")"
        ;
        return expression.str();
    }

    std::string StyleManager::color_to_css(HSVA color)
    {
        return StyleManager::color_to_css(color.operator RGBA());
    }

    std::string StyleManager::define_color(const std::string& name, RGBA color)
    {
        std::stringstream expression;
        expression << "@define-color " << name << StyleManager::color_to_css(color) << ";";
        StyleManager::add_css(expression.str());
        return "@" + name;
    }

    std::string StyleManager::define_color(const std::string& name, HSVA color)
    {
        return define_color(name, color.operator RGBA());
    }

    // ###

    StyleClass::StyleClass(const std::string& name)
        : _internal(detail::style_class_internal_new(name))
    {
        detail::validate_css_name(name);
        g_object_ref(_internal);
    }

    StyleClass::StyleClass(detail::StyleClassInternal* internal)
        : _internal(internal)
    {
        g_object_ref(_internal);
    }

    StyleClass::~StyleClass()
    {
        g_object_unref(_internal);
    }

    StyleClass::operator NativeObject() const
    {
        return G_OBJECT(_internal);
    }

    std::string StyleClass::serialize() const
    {
        std::stringstream str;
        
        for (auto& target_to_properties : *_internal->target_to_properties)
        {
            str << "." << *_internal->name << " " << target_to_properties.first << " {\n";
            for (auto& pair : target_to_properties.second)
                str << "    " << pair.first << ": " << pair.second << ";\n";
            str << "}" << std::endl;
        }
        
        return str.str();
    }

    std::string StyleClass::get_name() const
    {
        return *_internal->name;
    }

    void StyleClass::set_property(StyleClassTarget target, const std::string& property, const std::string& css_value)
    {
        auto it = _internal->target_to_properties->find(target);
        if (it == _internal->target_to_properties->end())
            _internal->target_to_properties->insert({target, std::map<std::string, std::string>()});

        (*_internal->target_to_properties)[target].insert_or_assign(property, css_value);
    }

    void StyleClass::set_property(const std::string& property, const std::string& value)
    {
        set_property(STYLE_TARGET_SELF, property, value);
    }

    std::string StyleClass::get_property(StyleClassTarget target, const std::string& property) const
    {
        auto it = _internal->target_to_properties->find(target);
        if (it == _internal->target_to_properties->end())
            return "";

        auto target_it = it->second.find(property);
        if (target_it == it->second.end())
            return "";
        else
            return target_it->second;
    }

    std::string StyleClass::get_property(const std::string& property) const
    {
        return get_property(STYLE_TARGET_SELF, property);
    }
}

