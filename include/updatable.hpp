// 
// Copyright 2022 Clemens Cords
// Created on 8/5/22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace mousetrap
{
    struct Updatable
    {
        virtual void update() = 0;
        virtual bool is_initialized() const = 0;
    };
}