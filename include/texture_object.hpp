// 
// Copyright 2022 Clemens Cords
// Created on 9/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    struct TextureObject
    {
        virtual void bind() const = 0;
        virtual void unbind() const = 0;
    };
}