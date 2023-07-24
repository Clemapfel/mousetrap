//
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/gtk_common.hpp>

#include <string>
#include <map>
#include <vector>

namespace mousetrap
{
    namespace detail
    {
        /// @brief template meta function that associates types with their internal types \for_internal_use_only
        template<typename T>
        struct InternalMapping {};

        #define DEFINE_INTERNAL_MAPPING(From) \
        template<>                         \
        struct InternalMapping<From>       \
        {                                  \
            using value = detail::From##Internal;    \
        };
    }

    #ifndef DOXYGEN
    class SignalEmitter;
    namespace detail
    {
        struct SignalHandler
        {
            size_t id;
            bool is_blocked = false;
        };

        struct _SignalEmitterInternal;
        using SignalEmitterInternal = _SignalEmitterInternal;
        DEFINE_INTERNAL_MAPPING(SignalEmitter);
    }
    #endif

    /// @brief object that can emit GLib signals \for_internal_use_only
    class SignalEmitter
    {
        public:
            /// @brief block a signal by id \for_internal_use_only
            /// @param signal_id
            /// @param b true if signal handler should not be invoked, false otherwise
            void set_signal_blocked(const std::string& signal_id, bool b);

            /// @brief get whether a signal is currently blocked
            /// @return bool
            bool get_signal_blocked(const std::string& signal_id) const;

            /// @brief get list of possible signal ids \for_internal_use_only
            /// @return vector of signals names
            std::vector<std::string> get_all_signal_names();

            /// @brief connect static function to signal, does not work with lambdas \for_internal_use_only
            /// @tparam Function_t static function pointer, the user is responsible for asserting that the function has the correct signature
            /// @param signal_id glib id of the signal
            /// @param function pointer to C-function, does not work with lambdas
            /// @param data void-pointer to arbitrary data
            template<typename Function_t>
            void connect_signal(const std::string& signal_id, Function_t* function, void* data = nullptr);

            /// @brief disconnect signal, it may not be invoked until reconnected, if signal is not registered, does nothing. For internal use only
            /// @param signal_id
            void disconnect_signal(const std::string& signal_id);

            /// @brief expose as NativeObject, this is the GLib instance that signals are emitted on \for_internal_use_only
            virtual explicit operator NativeObject() const = 0;

            /// @brief expose internally managed object, \for_internal_use_only
            virtual NativeObject get_internal() const;

        protected:
            /// @brief ctor
            SignalEmitter();

            /// @brief destructor
            ~SignalEmitter();

        private:
            void initialize();
            void connect_signal_implementation(const std::string& signal_id, void* function, void* data);
            detail::SignalEmitterInternal* _internal = nullptr;
    };
}

#include "inline/signal_emitter.inl"