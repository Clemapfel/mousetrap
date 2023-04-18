//
// Created by clem on 3/28/23.
//

#include <include/log.hpp>
#include <include/time.hpp>
#include <iostream>

namespace mousetrap
{
    namespace detail
    {
        constexpr const char* mousetrap_level_field = "MOUSETRAP_LEVEL";
    }

    std::string log::default_file_formatting_function(const std::string& message, const std::map<std::string, std::string>& fields)
    {
        auto timestamp = get_timestamp_now();

        std::stringstream out;
        out << "[" << timestamp << "]: " << message << "\n";
        for (auto& pair : fields)
            out << "\t" << pair.first << " " << pair.second << "\n";

        return out.str();
    }

    void log::initialize()
    {
        if (_initialized)
            return;
        
        g_log_set_writer_func(log_writer, nullptr, nullptr);
        _initialized = true;
        reset_file_formatting_function();
    }

    template <typename Function_t>
    void log::set_file_formatting_function(Function_t function)
    {
        _log_format_function = [f = function](const std::string& message, const std::map<std::string, std::string>& values) -> std::string {
            return f(message, values);
        };
    }

    void log::reset_file_formatting_function()
    {
        _log_format_function = [](const std::string& message, const std::map<std::string, std::string>& values) -> std::string {
            return log::default_file_formatting_function(message, values);
        };
    }

    bool log::get_surpress_debug(LogDomain domain)
    {
        auto debug_it = _allow_debug.find(domain);
        return debug_it == _allow_debug.end() or debug_it->second == false;
    }

    bool log::get_surpress_info(LogDomain domain)
    {
        auto info_it = _allow_debug.find(domain);
        return info_it == _allow_debug.end() or info_it->second == false;
    }

    GLogWriterOutput log::log_writer(GLogLevelFlags log_level, const GLogField* fields, gsize n_fields, gpointer)
    {
        // see https://www.freedesktop.org/software/systemd/man/systemd.journal-fields.html

        const char* domain = nullptr;
        const char* message = nullptr;
        const char* level = nullptr;

        bool domain_read = false;
        bool message_read = false;
        bool level_read = false;

        std::map<std::string, std::string> values;

        for (size_t i = 0; i < n_fields; ++i)
        {
            if (not level_read and std::string(fields[i].key) == std::string(detail::mousetrap_level_field))
            {
                level = (const char*) fields[i].value;
                level_read = true;
            }

            if (not domain_read and std::string(fields[i].key) == std::string("GLIB_DOMAIN"))
            {
                domain = (const char*) fields[i].value;
                domain_read = true;
            }

            if (not message_read and std::string(fields[i].key) == std::string("MESSAGE"))
            {
                message = (const char*) fields[i].value;
                message_read = true;
            }

            if (std::string(fields[i].key) != std::string("MESSAGE"))
                values.insert({std::string((const char*) fields[i].key), std::string((const char*) fields[i].value)});
        }

        if (_log_file != nullptr)
        {
            auto formatted = _log_format_function(std::string(message), values);
            formatted += "\n";

            GError* error = nullptr;
            gsize written = 0;

            g_output_stream_write_all(
                G_OUTPUT_STREAM(_log_file_stream),
                (void*) formatted.c_str(),
                formatted.size(),
                &written,
                nullptr,
                &error
            );

            if (error != nullptr)
            {
                g_error("[FATAL] In log::log_writer: %s", error->message);
                g_error_free(error);
            }
        }

        // if non-mousetrap message, use default rejection filter
        if (level == nullptr)
            if (g_log_writer_default_would_drop(log_level, domain))
                return G_LOG_WRITER_HANDLED;

        // reject debug unless enabled
        if (level_read and std::string(level) == std::string("DEBUG") and log::get_surpress_debug(domain))
                return G_LOG_WRITER_HANDLED;

        // reject info unless enabled
        auto info_it = _allow_debug.find(domain);
        if (level_read and std::string(level) == std::string("INFO") and log::get_surpress_info(domain))
            return G_LOG_WRITER_HANDLED;

        return g_log_writer_standard_streams(log_level, fields, n_fields, nullptr);
    }

    void log::debug(const std::string& message, LogDomain domain)
    {
        g_log_structured(domain, (GLogLevelFlags) LogLevel::DEBUG,
             detail::mousetrap_level_field, "DEBUG",
             "MESSAGE", message.c_str()
        );
    }

    void log::info(const std::string& message, LogDomain domain)
    {
        g_log_structured(domain, (GLogLevelFlags) LogLevel::INFO,
             detail::mousetrap_level_field, "INFO",
             "MESSAGE", message.c_str()
        );
    }

    void log::warning(const std::string& message, LogDomain domain)
    {
        g_log_structured(domain, (GLogLevelFlags) LogLevel::WARNING,
             detail::mousetrap_level_field, "WARNING",
             "MESSAGE", message.c_str()
        );
    }

    void log::critical(const std::string& message, LogDomain domain)
    {
        g_log_structured(domain, (GLogLevelFlags) LogLevel::CRITICAL,
             detail::mousetrap_level_field, "CRITICAL",
             "MESSAGE", message.c_str()
        );
    }

    void log::fatal(const std::string& message, LogDomain domain)
    {
        g_log_structured(domain, (GLogLevelFlags) LogLevel::FATAL,
             detail::mousetrap_level_field, "FATAL",
             "MESSAGE", message.c_str()
        );
    }

    void log::set_surpress_debug(LogDomain domain, bool b)
    {
        _allow_debug.insert_or_assign(domain, not b);
    }

    void log::set_surpress_info(LogDomain domain, bool b)
    {
        _allow_info.insert_or_assign(domain, not b);
    }

    bool log::set_file(const std::string& path)
    {
        GError* error = nullptr;
        _log_file = g_file_new_for_path(path.c_str());
        _log_file_stream = g_file_append_to(_log_file, GFileCreateFlags::G_FILE_CREATE_NONE, nullptr, &error);

        if (error != nullptr)
        {
            log::critical("In log::set_file: " + std::string(error->message), MOUSETRAP_DOMAIN);
            g_error_free(error);
            return false;
        }

        return true;
    }
}
