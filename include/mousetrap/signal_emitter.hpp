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
        struct InternalMapping
        {
            using value = std::nullptr_t;
        };

        #define DEFINE_INTERNAL_MAPPING(From, To) \
        template<>                         \
        struct InternalMapping<From>       \
        {                                  \
            using value = To;              \
        }; \
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

        template<>
        struct InternalMapping<SignalEmitter>
        {
            using internal = detail::SignalEmitterInternal;
        };
    }
    #endif

    /// @brief garbage collected object \for_internal_use_only
    using NativeObject = GObject*;

    /// @brief object that can emit GLib signals \for_internal_use_only
    class SignalEmitter
    {
        public:
            /// @brief block a signal by id \for_internal_use_only
            /// @param signal_id
            /// @param b true if signal handler should not be invoked, false otherwise
            void set_signal_blocked(const std::string& signal_id, bool b);

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
            NativeObject get_internal() const;

            /// @brief expose as NativeObject, this is the GLib instance that signals are emitted on
            virtual NativeObject get_native() const final;

        protected:
            /// @brief destructor
            ~SignalEmitter();

        private:
            void initialize();
            detail::SignalEmitterInternal* _internal = nullptr;
    };
}

#include "inline/signal_emitter.inl"