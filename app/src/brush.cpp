#include <app/brush.hpp>

namespace mousetrap
{
    Brush::~Brush()
    {}

    void Brush::as_circle()
    {
        _shape = BrushShape::CIRCLE;
        _name = "Circle";
        _base_image.create(1, 1);
        update_image();
    }

    void Brush::as_square()
    {
        _shape = BrushShape::SQUARE;
        _name = "Square";
        _base_image.create(1, 1);
        update_image();
    }

    void Brush::as_ellipse_horizontal()
    {
        _shape = BrushShape::ELLIPSE_HORIZONTAL;
        _name = "Horizontal Ellipse";
        _base_image.create(1, 1);
        update_image();
    }

    void Brush::as_ellipse_vertical()
    {
        _shape = BrushShape::ELLIPSE_VERTICAL;
        _name = "Vertical Ellipse";
        _base_image.create(1, 1);
        update_image();
    }

    void Brush::as_rectangle_horizontal()
    {
        _shape = BrushShape::RECTANGLE_HORIZONTAL;
        _name = "Horizontal Rectangle";
        _base_image.create(1, 1);
        update_image();
    }

    void Brush::as_rectangle_vertical()
    {
        _shape = BrushShape::RECTANGLE_VERTICAL;
        _name = "Vertical Rectangle";
        _base_image.create(1, 1);
        update_image();
    }

    void Brush::as_custom(Image& base_image)
    {
        static size_t i = 0;
        _name = "Custom Brush #" + std::to_string(i++);
        _shape = BrushShape::CUSTOM;
        auto size = base_image.get_size();
        size.x = std::max(size.x, size.y);
        size.y = std::max(size.x, size.y);

        if (size.x == 0)
            size.x = 1;

        if (size.y == 0)
            size.y = 1;

        _size = size.x;

        _base_image.create(size.x, size.y, RGBA(0, 0, 0, 0));

        for (size_t x = 0; x < size.x; ++x)
        {
            for (size_t y = 0; y < size.y; ++y)
            {
                if (x >= base_image.get_size().x or y >= base_image.get_size().y)
                    continue; // padded to square dimensions

                auto base_px = base_image.get_pixel(x, y);
                if (base_px.a > 0.00001)
                    _base_image.set_pixel(x, y, RGBA(1, 1, 1, 1));
            }
        }

        update_image();
    }

    void Brush::update_image()
    {
        auto color = RGBA(1, 1, 1, 1).operator HSVA();
        switch (_shape)
        {
            case BrushShape::CIRCLE:
                _image = generate_circle_filled(_size, _size, color);
                break;
            case BrushShape::SQUARE:
                _image = generate_rectangle_filled(_size, _size, color);
                break;
            case BrushShape::ELLIPSE_HORIZONTAL:
                _image = generate_circle_filled(_size, _size / 3.f, color);
                break;
            case BrushShape::ELLIPSE_VERTICAL:
                _image = generate_circle_filled(_size / 3.f, _size, color);
                break;
            case BrushShape::RECTANGLE_HORIZONTAL:
                _image = generate_rectangle_filled(_size, _size / 3.f, color);
                break;
            case BrushShape::RECTANGLE_VERTICAL:
                _image = generate_rectangle_filled(_size / 3.f, _size, color);
                break;
            case BrushShape::CUSTOM:
                _image = _base_image.as_scaled(_size, _size, GDK_INTERP_NEAREST);
                break;
        }

        // prevent downscaling resulting in completely empty picture
        for (size_t x = 0; x < _image.get_size().x; ++x)
            for (size_t y = 0; y < _image.get_size().y; ++y)
                if (_image.get_pixel(x, y).a > 0)
                    goto end;

        _image.set_pixel(_image.get_size().x / 2, _image.get_size().y / 2, RGBA(1, 1, 1, 1));
        end:;
    }

    const Image& Brush::get_image() const
    {
        return _image;
    }

    std::string Brush::get_name() const
    {
        return _name;
    }

    void Brush::set_size(size_t size)
    {
        _size = size;
        update_image();
    }

    size_t Brush::get_size() const
    {
        return _size;
    }

    BrushShape Brush::get_shape() const
    {
        return _shape;
    }

    std::string brush_shape_to_string(BrushShape shape)
    {
        switch(shape)
        {
            case BrushShape::CIRCLE:
                return "CIRCLE";
            case BrushShape::SQUARE:
                return "SQUARE";
            case BrushShape::ELLIPSE_HORIZONTAL:
                return "ELLIPSE_HORIZONTAL";
            case BrushShape::ELLIPSE_VERTICAL:
                return "ELLIPSE_VERTICAL";
            case BrushShape::RECTANGLE_HORIZONTAL:
                return "RECTANGLE_HORIZONTAL";
            case BrushShape::RECTANGLE_VERTICAL:
                return "RECTANGLE_VERTICAL";
            case BrushShape::CUSTOM:
            default:
                return "CUSTOM";
        }
    }

    BrushShape brush_shape_from_string(const std::string& in)
    {
        if (in == "CIRCLE" or in == "circle")
            return BrushShape::CIRCLE;
        else if (in == "SQUARE" or in == "square")
            return BrushShape::SQUARE;
        else if (in == "ELLIPSE_VERTICAL" or in == "ellipse_vertical")
            return BrushShape::ELLIPSE_VERTICAL;
        else if (in == "ELLIPSE_HORIZONTAL" or in == "ellipse_horizontal")
            return BrushShape::ELLIPSE_HORIZONTAL;
        else if (in == "RECTANGLE_HORIZONTAL" or in == "rectangle_horizontal")
            return BrushShape::RECTANGLE_HORIZONTAL;
        else if (in == "RECTANGLE_VERTICAL" or in == "rectangle_vertical")
            return BrushShape::RECTANGLE_VERTICAL;
        else if (in == "CUSTOM" or in == "custom")
            return BrushShape::CUSTOM;
        else
        {
            std::cerr << "[ERROR] In brush_shape_from_string: Input \"" << in << "\" not recognized. Returning BrushShape::CUSTOM" << std::endl;
            return BrushShape::CUSTOM;
        }
    }
}