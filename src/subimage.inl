//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/18/23
//

namespace mousetrap
{
    inline SubImage::Pixel::Pixel(int x, int y, RGBA color)
            : x(x), y(y), color(color)
    {}

    inline SubImage::Pixel::Pixel(Vector2i xy, RGBA color)
            : x(xy.x), y(xy.y), color(color)
    {}

    inline SubImage::SubImage()
    {}

    inline void SubImage::add(Vector2i pos, RGBA color)
    {
        _data.insert(Pixel(pos, color));
    }

    inline void SubImage::add(const std::vector<Vector2i>& positions, RGBA color)
    {
        for (auto& pos : positions)
            add(pos, color);
    }

    inline void SubImage::add(const std::vector<SubImage::Pixel>& pixels)
    {
        for (auto& pixel : pixels)
            _data.insert(pixel);
    }

    inline size_t SubImage::get_n_pixels() const
    {
        return _data.size();
    }

    inline auto SubImage::begin()
    {
        return _data.begin();
    }

    inline auto SubImage::begin() const
    {
        return _data.begin();
    }

    inline auto SubImage::end()
    {
        return _data.end();
    }

    inline auto SubImage::end() const
    {
        return _data.end();
    }

    inline bool SubImage::is_empty() const
    {
        return _data.empty();
    }
}
