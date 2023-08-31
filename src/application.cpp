//
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/application.hpp>
#include <mousetrap/log.hpp>
#include <mousetrap/render_area.hpp>
#include <mousetrap/gtk_common.hpp>

#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        DECLARE_NEW_TYPE(ApplicationInternal, application_internal, APPLICATION_INTERNAL)
        DEFINE_NEW_TYPE_TRIVIAL_INIT(ApplicationInternal, application_internal, APPLICATION_INTERNAL)

        static void application_internal_finalize(GObject* object)
        {
            auto* self = MOUSETRAP_APPLICATION_INTERNAL(object);

            for (auto& pair : (*self->actions))
                g_object_unref(pair.second);

            delete self->actions;
            G_OBJECT_CLASS(application_internal_parent_class)->finalize(object);
        }

        DEFINE_NEW_TYPE_TRIVIAL_CLASS_INIT(ApplicationInternal, application_internal, APPLICATION_INTERNAL)

        static ApplicationInternal* application_internal_new(const std::string& id, int32_t flags)
        {
            log::initialize();

            auto* native = adw_application_new(id.c_str(), (GApplicationFlags) flags);
            g_application_set_resource_base_path(G_APPLICATION(native), nullptr);

            auto* self = (ApplicationInternal*) g_object_new(application_internal_get_type(), nullptr);
            application_internal_init(self);

            self->native = GTK_APPLICATION(native);
            self->actions = new std::map<ActionID, detail::ActionInternal*>();
            self->holding = false;
            self->busy = false;

            return self;
        }
    }

    Application::Application(const std::string& id, bool allow_multiple_instances)
        : CTOR_SIGNAL(Application, activate),
          CTOR_SIGNAL(Application, shutdown)
    {
        if (not g_application_id_is_valid(id.c_str()))
            log::critical("In Application::Application: id " + id + " is not a valid application id", MOUSETRAP_DOMAIN);

        #if GLIB_MINOR_VERSION > 74
        int32_t flags = G_APPLICATION_DEFAULT_FLAGS;
        #else
        int32_t flags = G_APPLICATION_FLAGS_NONE;
        #endif

        if (allow_multiple_instances)
            flags |= G_APPLICATION_NON_UNIQUE;

        _internal = detail::application_internal_new(id, flags);

        g_signal_connect(_internal->native, "startup", G_CALLBACK(detail::mark_gtk_initialized), nullptr);

        #if MOUSETRAP_ENABLE_OPENGL_COMPONENT
        g_signal_connect(_internal->native, "startup", G_CALLBACK(detail::initialize_opengl), nullptr);
        g_signal_connect(_internal->native, "shutdown", G_CALLBACK(detail::shutdown_opengl), nullptr);
        #endif
    }

    Application::Application(detail::ApplicationInternal* internal)
        : CTOR_SIGNAL(Application, activate),
          CTOR_SIGNAL(Application, shutdown)
    {
        _internal = g_object_ref(internal);
    }

    Application::~Application()
    {
        g_object_unref(_internal);
    }

    Application::Application(Application&& other) noexcept
        : CTOR_SIGNAL(Application, activate),
          CTOR_SIGNAL(Application, shutdown)
    {
        _internal = g_object_ref(other._internal);
    }

    Application& Application::operator=(Application&& other) noexcept
    {
        if (&other == this)
            return *this;

        _internal = g_object_ref(other._internal);
        return *this;
    }

    Application::operator NativeObject() const
    {
        return G_OBJECT(_internal->native);
    }

    Application::operator GtkApplication*() const
    {
        return _internal->native;
    }

    NativeObject Application::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    ApplicationID Application::get_id() const
    {
        auto* id = g_application_get_application_id(G_APPLICATION(_internal->native));
        if (id != nullptr)
            return std::string(id);
        else
            return "";
    }

    int Application::run()
    {
        return g_application_run(G_APPLICATION(_internal->native), 0, nullptr);
    }

    void Application::quit()
    {
        g_application_quit(G_APPLICATION(_internal->native));
    }

    void Application::hold()
    {
        if (not _internal->holding)
        {
            g_application_hold(G_APPLICATION(_internal->native));
            _internal->holding = true;
        }
    }

    void Application::release()
    {
        if (not _internal->holding)
            log::warning("In Application::release: Attempting to release application, but it is not currently holding", MOUSETRAP_DOMAIN);
        else
        {
            g_application_release(G_APPLICATION(_internal->native));
            _internal->holding = false;
        }
    }

    void Application::mark_as_busy()
    {
        if (not _internal->busy)
        {
            g_application_mark_busy(G_APPLICATION(_internal->native));
            _internal->busy = true;
        }
    }

    void Application::unmark_as_busy()
    {
        if (not _internal->busy)
            log::warning("In Application::unmark_as_busy: Application is not currently marked busy", MOUSETRAP_DOMAIN);
        else
        {
            g_application_unmark_busy(G_APPLICATION(_internal->native));
            _internal->busy = false;
        }
    }

    bool Application::get_is_marked_as_busy() const
    {
        return _internal->busy;
    }

    bool Application::get_is_holding() const
    {
        return _internal->holding;
    }

    void Application::add_action(const Action& action)
    {
        if (G_ACTION(action.operator NativeObject()) == nullptr)
            log::warning("In Application::add_action: Attempting to add action `" + action.get_id() + "` to application, but the actions behavior was not set yet. Call Action::set_function or Action::set_stateful_function first");

        auto inserted = _internal->actions->insert({action.get_id(), action._internal}).first->second;
        g_action_map_add_action(G_ACTION_MAP(_internal->native), G_ACTION(inserted->g_action));

        auto* app = GTK_APPLICATION(_internal->native);

        auto accels = std::vector<const char*>();
        for (auto& s : inserted->shortcuts)
        {
            if (s != "never")
                accels.push_back(s.c_str());
        }
        accels.push_back(NULL);

        if (not accels.empty())
            gtk_application_set_accels_for_action(app, ("app." + inserted->id).c_str(), accels.data());
    }

    void Application::remove_action(const ActionID& id)
    {
        if (not has_action(id))
            return;

        auto* self = G_ACTION_MAP(_internal->native);
        _internal->actions->erase(id);
        g_action_map_remove_action(self, ("app." + id).c_str());
    }

    bool Application::has_action(const ActionID& id)
    {
        return _internal->actions->find(id) != _internal->actions->end();
    }

    Action Application::get_action(const ActionID& id)
    {
        auto it = _internal->actions->find(id);
        if (it == _internal->actions->end())
        {
            log::critical("In Application::get_action: No action with id `" + id + "` registered");
            return Action(nullptr);
        }
        else
            return Action(it->second);
    }

    void Application::set_current_theme(Theme theme)
    {
        auto* manager = adw_style_manager_get_default();
        if (not ADW_IS_STYLE_MANAGER(manager))
        {
            log::critical("In Application::set_current_theme: Display not yet initialized, make sure to call this function after `activate` was emitted on this application instance", MOUSETRAP_DOMAIN);
            return;
        }

        adw_style_manager_set_color_scheme(manager, theme == Theme::DEFAULT_DARK ? ADW_COLOR_SCHEME_FORCE_DARK : ADW_COLOR_SCHEME_FORCE_LIGHT);
        if (theme == Theme::HIGH_CONTRAST_DARK || theme == Theme::HIGH_CONTRAST_LIGHT)
            if (adw_style_manager_get_high_contrast(manager) == false)
                log::warning("In Application::set_current_theme: High Contrast theme not supported, using default fallback instead.", MOUSETRAP_DOMAIN);
    }

    Theme Application::get_current_theme() const
    {
        auto* manager = adw_style_manager_get_default();
        if (not ADW_IS_STYLE_MANAGER(manager))
            return Theme::DEFAULT_DARK;

        auto dark = adw_style_manager_get_dark(manager);
        auto high_contrast = adw_style_manager_get_high_contrast(manager);

        if (dark)
        {
            if (high_contrast)
                return Theme::HIGH_CONTRAST_DARK;
            else
                return Theme::DEFAULT_DARK;
        }
        else
        {
            if (high_contrast)
                return Theme::HIGH_CONTRAST_LIGHT;
            else
                return Theme::DEFAULT_LIGHT;
        }
    }
}
