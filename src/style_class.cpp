//
// Created by clem on 8/16/23.
//

#include <mousetrap/style_class.hpp>
#include <mousetrap/log.hpp>

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
            delete self->values;
            G_OBJECT_CLASS(style_class_internal_parent_class)->finalize(object);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(StyleClassInternal, style_class_internal, STYLE_CLASS_INTERNAL)

        static StyleClassInternal* style_class_internal_new(const std::string& name)
        {
            auto* self = (StyleClassInternal*) g_object_new(style_class_internal_get_type(), nullptr);
            style_class_internal_init(self);

            self->provider = gtk_css_provider_new();
            self->values = new std::map<std::string, std::string>();
            self->name = new std::string(name);
            return self;
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

    StyleClass StyleManager::get_style_class(const std::string& name) const
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

    // ###

    StyleClass::StyleClass(const std::string& name)
        : _internal(detail::style_class_internal_new(name))
    {
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
        str << *_internal->name << " {\n";
        for (auto& pair : *_internal->values)
            str << pair.first << ": " << pair.second << ";\n";
        str << "}" << std::endl;
        return str.str();
    }

    std::string StyleClass::get_name() const
    {
        return *_internal->name;
    }

    void StyleClass::set_property(const std::string& property_name, const std::string& property_value)
    {
        _internal->values->insert_or_assign(property_name, property_value);
    }

    std::string StyleClass::get_property(const std::string& property_name)
    {
        auto it = _internal->values->find(property_name);
        if (it == _internal->values->end())
        {
            log::critical("In StyleClass::get_property: No property with name `" + property_name + "` registered");
            return "";
        }
        else
            return it->second;
    }
}

