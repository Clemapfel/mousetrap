// 
// Copyright 2022 Clemens Cords
// Created on 9/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/texture.hpp>

namespace mousetrap
{
    struct Layer
    {
        std::string name;

        using PixelData = struct
        {
            Image image;
            Texture texture;
        };

        std::deque<PixelData> frames = std::deque<PixelData>();

        bool is_locked = false;
        bool is_visible = true;
        float opacity = 1;
        BlendMode blend_mode = NORMAL;
    };

    namespace state
    {
        std::deque<Layer> layers;
        Vector2ui layer_resolution;
        size_t n_frames = 0;

        Layer* new_layer(const std::string& name, Layer* after = nullptr);
        void new_frame(size_t index);

        void delete_layer(Layer*);
        void delete_frame(size_t index);
    }
}

/// ###

namespace mousetrap
{
    Layer* state::new_layer(const std::string& name, Layer* after)
    {
        if (n_frames == 0)
            n_frames = 1;

        auto after_pos = state::layers.begin();
        for (; after != nullptr and after_pos != state::layers.end(); ++after_pos)
            if (&(*after_pos) == after)
                break;

        auto it = layers.insert(after_pos, Layer{name});

        for (size_t i = 0; i < n_frames; ++i)
        {
            it->frames.emplace_back();
            it->frames.back().image.create(state::layer_resolution.x, state::layer_resolution.y, RGBA(1, 1, 1, 0));
            it->frames.back().texture.create_from_image(it->frames.back().image);
        }

        return &(*it);
    }

    void state::new_frame(size_t index)
    {
        if (index > n_frames)
        {
            std::cout << "[ERROR] In state::new_frame: Attempting to insert frame at position " << index << " but there are only " << n_frames << " frames" << std::endl;
            return;
        }

        for (auto& layer : state::layers)
        {
            auto it = layer.frames.emplace(layer.frames.begin() + index);
            it->image.create(state::layer_resolution.x, state::layer_resolution.y, RGBA(1, 1, 1, 0));
            it->texture.create_from_image(it->image);
        }
    }

    void state::delete_layer(Layer* ptr)
    {
        for (auto it = state::layers.begin(); it != state::layers.end(); ++it)
        {
            if (&(*it) == ptr)
            {
                state::layers.erase(it);
                return;
            }
        }

        std::cout << "[ERROR] In state::delete_layer: Attempting to delete Layer " << ptr->name << " but it is not in the global state." << std::endl;
    }

    void state::delete_frame(size_t index)
    {
        if (index > n_frames)
        {
            std::cout << "[ERROR] In state::delete_frame: Attempting to delete frame with index " << index
                      << " but there are only " << n_frames << " frames." << std::endl;
            return;
        }

        for (auto& layer : layers)
            layer.frames.erase(layer.frames.begin() + index);

        n_frames -= 1;
    }


    /*

    Layer* state::new_layer(RGBA color)
    {
        auto image = Image();
        image.create(state::layer_resolution.x, state::layer_resolution.y, color);
        return new_layer(image);
    }

    Layer* state::new_layer(const Image& image)
    {
        auto out = new Layer();

        //float width = state::layer_resolution.x;
        //float height = state::layer_resolution.y;

        auto image_size = Vector2ui(image.get_size().x, image.get_size().y);

        if (image_size == state::layer_resolution)
            ((Texture*) out->texture)->create_from_image(image);
        else
        {
            auto image_padded = Image();
            image_padded.create(state::layer_resolution.x, state::layer_resolution.y, RGBA(0, 0, 0, 0));

            for (size_t x = 0; x < std::min<size_t>(state::layer_resolution.x, image.get_size().x); ++x)
                for (size_t y = 0; y < std::min<size_t>(state::layer_resolution.y, image.get_size().y); ++y)
                    image_padded.set_pixel(x, y, image.get_pixel(x, y));

            ((Texture*) out->texture)->create_from_image(image_padded);
        }

        return out;
    }

    Layer* state::new_layer(Layer* layer)
    {
        return new Layer(*layer);
    }
     */


}