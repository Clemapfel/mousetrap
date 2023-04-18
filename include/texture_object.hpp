//
// Copyright 2022 Clemens Cords
// Created on 9/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    /// @brief object that can be bound to a texture unit
    struct TextureObject
    {
        /// @brief bind for rendering, usually to GL_TEXTURE0
        virtual void bind() const = 0;

        /// @brief unbind from rendering
        virtual void unbind() const = 0;
    };
}
