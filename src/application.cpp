//
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#include <mousetrap/application.hpp>
#include <mousetrap/log.hpp>
#include <mousetrap/render_area.hpp>

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

        static ApplicationInternal* application_internal_new(const std::string& id)
        {
            log::initialize();

            auto* native = gtk_application_new(id.c_str(), (GApplicationFlags) gint32(0));

            auto* self = (ApplicationInternal*) g_object_new(application_internal_get_type(), nullptr);
            application_internal_init(self);

            self->native = native;
            self->actions = new std::unordered_map<ActionID, detail::ActionInternal*>();
            self->holding = false;
            self->busy = false;

            return self;
        }
    }

    Application::Application(const std::string& id)
        : CTOR_SIGNAL(Application, activate),
          CTOR_SIGNAL(Application, shutdown)
    {
        if (not g_application_id_is_valid(id.c_str()))
            log::critical("In Application::Application: id " + id + " is not a valid application id", MOUSETRAP_DOMAIN);

        _internal = detail::application_internal_new(id);
        connect_signal("startup", detail::initialize_opengl);
    }

    Application::Application(detail::ApplicationInternal* internal)
        : CTOR_SIGNAL(Application, activate),
          CTOR_SIGNAL(Application, shutdown)
    {
        _internal = g_object_ref(internal);
        connect_signal("startup", detail::initialize_opengl);
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
            g_application_hold(G_APPLICATION(_internal->native));
    }

    void Application::release()
    {
        if (not _internal->holding)
            log::warning("In Application::release: Attempting to release application, but it is not currently holding");
        else
            g_application_release(G_APPLICATION(_internal->native));
    }

    void Application::mark_as_busy()
    {
        if (not _internal->busy)
            g_application_mark_busy(G_APPLICATION(_internal->native));
    }

    void Application::unmark_as_busy()
    {
        if (not _internal->busy)
            log::warning("In Application::unmark_as_busy: Application is not currently marked busy");
        else
            g_application_unmark_busy(G_APPLICATION(_internal->native));
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
}
