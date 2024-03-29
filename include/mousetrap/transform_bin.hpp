//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 8/16/23
//

#pragma once

#include <mousetrap/widget.hpp>
#include <mousetrap/angle.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class TransformBin;
    namespace detail
    {
        struct _TransformBinInternal;
        using TransformBinInternal = _TransformBinInternal;
        DEFINE_INTERNAL_MAPPING(TransformBin);
    }
    #endif

    /// @brief 1-child container that allows applying a transform to its child
    class TransformBin : public detail::notify_if_gtk_uninitialized,
        public Widget,
        HAS_SIGNAL(TransformBin, realize),
        HAS_SIGNAL(TransformBin, unrealize),
        HAS_SIGNAL(TransformBin, destroy),
        HAS_SIGNAL(TransformBin, hide),
        HAS_SIGNAL(TransformBin, show),
        HAS_SIGNAL(TransformBin, map),
        HAS_SIGNAL(TransformBin, unmap)
    {
        public:
            /// @brief construct
            TransformBin();

            /// @brief construct form internal
            TransformBin(detail::TransformBinInternal*);

            /// @brief destruct
            ~TransformBin();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief set singular child
            void set_child(const Widget&);

            /// @brief remove singular child
            void remove_child();

            /// @brief reset the transform
            void reset();

            /// @brief rotate transform
            /// @param angle
            void rotate(Angle);

            /// @brief translation in 2D
            /// @param offset
            void translate(Vector2f);

            /// @brief scale along x- / y-axis
            /// @param x scale
            /// @param y scale
            void scale(float x, float y);

            /// @brief skew along x- / y-axis
            /// @param x skew
            /// @param y skew
            void skew(float x_skew, float y_skew);

        private:
            detail::TransformBinInternal* _internal = nullptr;
    };
}