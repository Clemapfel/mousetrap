//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/28/23
//

#pragma once

#include <include/gtk_common.hpp>

#include <string>
#include <map>
#include <functional>
#include <sstream>

namespace mousetrap
{
    /// @brief log levels, changes formatting and behavior
    enum class LogLevel
    {
        /// @brief error, exits application
        FATAL = G_LOG_LEVEL_ERROR,

        /// @brief critical, signals that an error occurred but does not exit runtime
        CRITICAL = G_LOG_LEVEL_CRITICAL,

        /// @brief warning
        WARNING = G_LOG_LEVEL_WARNING,

        /// @brief info
        INFO = G_LOG_LEVEL_INFO,

        /// @brief debug
        DEBUG = G_LOG_LEVEL_DEBUG
    };

    /// @brief log domain
    using LogDomain = const char*;

    /// @brief domain for errors inside mousetrap \internal
    constexpr const char* MOUSETRAP_DOMAIN = "mousetrap";

    /// @brief default user domain if no domain is specified
    constexpr const char* USER_DOMAIN = "debug";

    /// @brief uninstantiatable singleton, provides global logging functionality
    class log
    {
        public:
            /// @brief ctor deleted, singleton instance that cannot be instantiated
            log() = delete;

            /// @brief initiailze the logging suite, automatically called during construction of mousetrap::Application
            static void initialize();

            /// @brief print a debug message, not printed unless set_surpress_debug is set to false
            /// @param message message
            /// @param domain logging domain, optional
            static void debug(const std::string& message, LogDomain = USER_DOMAIN);

            /// @brief print a info message, usually reserved to benign logging, printed unless set_surpress_info is set to true
            /// @param message message
            /// @param domain logging domain, optional
            static void info(const std::string& message, LogDomain = USER_DOMAIN);

            /// @brief print a warning message, appropriate to inform user of unexpected behavior that cannot cause an error
            /// @param message message
            /// @param domain logging domain, optional
            static void warning(const std::string& message, LogDomain = USER_DOMAIN);

            /// @brief inform user of an error, this function does not interrupt runtime
            /// @param message message
            /// @param domain logging domain, optional
            static void critical(const std::string& message, LogDomain = USER_DOMAIN);

            /// @brief inform user of a critical error, this function will exit runtime
            /// @param message message
            /// @param domain logging domain, optional
            static void fatal(const std::string& message, LogDomain = USER_DOMAIN);

            /// @brief surpress debug level log message for given log domain. true by default
            /// @param domain logging domain
            /// @param b true if debug messages should be surpressed, false otherwise
            static void set_surpress_debug(LogDomain, bool);

            /// @brief surpress info level log message for given log domain. true by default
            /// @param domain logging domain
            /// @param b true if info messages should be surpressed, false otherwise
            static void set_surpress_info(LogDomain, bool);

            /// @brief get whether debug level log messages are printed for given domain
            /// @param domain logging domain
            /// @return true if debug messages should be surpressed, false otherwise
            static bool get_surpress_debug(LogDomain);

            /// @brief get whether info level log messages are printed for given domain
            /// @param domain logging domain
            /// @return true if info messages should be surpressed, false otherwise
            static bool get_surpress_info(LogDomain);

            /// @brief specify a log file, any message regardless of priority will be appended to it. File will not be overwritten, if it does not exist, it will be created
            /// @param path absolute path
            /// @return true if file was succesfully opened for streaming, false otherwise
            static bool set_file(const std::string& path);

            /// @brief set formatting function, this functino transforms the log message into a string which will be appended to the log file. This function is not applied to messages printed to cout or cerr
            /// @tparam Function_t lambda with signatures (const std::string& message, const std::map<std::string, std::string>& fields) -> std::string
            /// @param function
            template<typename Function_t>
            static void set_file_formatting_function(Function_t function);

            /// @brief reset formatting function to default
            static void reset_file_formatting_function();

        private:
            static inline bool _initialized = false;
            static inline bool _forward_to_file = false;
            static GLogWriterOutput log_writer(GLogLevelFlags log_level, const GLogField* fields, gsize n_fields, gpointer);

            static inline std::map<LogDomain, bool> _allow_debug;
            static inline std::map<LogDomain, bool> _allow_info;

            static std::string default_file_formatting_function(const std::string& message, const std::map<std::string, std::string>& fields);

            static inline std::function<std::string(const std::string& message, const std::map<std::string, std::string>& fields)> _log_format_function;

            static inline GFile* _log_file = nullptr;
            static inline GFileOutputStream* _log_file_stream = nullptr;
    };
}

#include <src/log.inl>