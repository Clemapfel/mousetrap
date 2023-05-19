# Chapter 9: Native Rendering

In this chapter we will learn:
+ How to use `RenderArea`
+ How to draw any shape
+ How to bind a shape, transform, and shader for rendering
+ How to render to a (multi-sampled) texture
---

<details><summary><b><tt>main.cpp</tt> for this chapter</b></summary>

Here is a `main.cpp` that allows use to run any code snippet mentioned in this chapter:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("example.app");
    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);
        
        auto render_area = RenderArea();
        render_area.set_expand(true);
        render_area.set_size_request({400, 300})
        
        // snippet code here
        
        auto aspect_frame = AspectFrame(4:3);
        aspect_frame.set_chlid(render_area);
        window.set_child(aspect_frame);
        window.present();
    });
    
    return app.run();
}
```
</details>

---

In the [chapter on widgets](04_widgets.md), we learned that, so far, we can only create new widgets by combining already pre-defined widgets as a compound widget. We could create a widget that has a `Scale`, but we can't render our own scale with, for example, a square knob. In this chapter, this will change. By using the native rendering interface mousetrap provides, we are free to create any shape we want and assemble completely new widgets pixel-by-pixel, line-by-line.

## RendeArea

This central widget of this chapter is `RenderArea`. At first, it may seem very simple:

```cpp
auto area = RenderArea();
```

This widget will render as a transparent area, it's invisible for now. `RenderArea` provides no renderable element, instead, we will have to create **shapes** and bind them for rendering.

## Vertices

A drawable shape is made up of **vertices**. In mousetrap, a vertex is represented by the \a{Vertex} struct, which has three properties:

+ `position`: vertex position in 3D space
+ `texture_coordinates`: texture coordinates in 2d texture space
+ `color`: color in RGBA

### Coordinate Spaces

In this section, we will be using two different coordinate systems. **3D space**, **texture space**.

#### 3D Spaces

A vertices `position` is a point in 3D space, which is the left-handed coordinate system familiar from traditional geometry:

\image html https://learnopengl.com/img/getting-started/coordinate_systems_right_handed.png

<center>(source: [learnopengl.com](https://learnopengl.com/Getting-started/Coordinate-Systems))</center>

 We can simplify 3d space by assuming the the the z-coordinate to be '0' from now on. This reduces the 3d space to a 2D plane. We call this plane **gl space**, named after OpenGL, which is the native rendering framework used by `RenderArea`. 
 
In see that in gl space, the y-coordinate is positive when move "up", negative when moving "down". The x coordinate is negative when going "left", positive when going "right". 

The coordinates are **relative**, meaning they are normalized to `[-1, 1]`. This means that a shapes coordinates are **independent of the size of the viewport**. No matter if our `RenderArea` is of size 500x200 or 200x500, a shapes vertex coordinates do not change. The shape is dynamically resized depending on the viewport size, a distance of 0.25 along the x-axis will be 250px if the viewport is 1000px wide, or 25px if the viewport is 100px wide.

Some more example:
 
| Conceptual Position | 2D Coordinate |
|---------------------|---------------|
| top left            | `(-1, +1)`    |
| top                 | `( 0, +1)`    |
| top right           | `(+1, +1)`    |
| left                | `(-1,  0)`    |
| center (origin)     | `( 0,  0)`    |
| right               | `(+1,  0)`    |
| bottom left         | `(-1, -1)`    |
| bottom              | `( 0, -1)`    |
| bottom right        | `(+1, -1)`    |

#### Texture Space

A vertices `texture_coordinate` use **texture space**, which uses a different coordinate system. It is anchored at the top-left of the texture, with the y-coordinate increasing in value when going down and decreasing in value when going up. The x-coordinate increase in value when going right, and decreases when going left. Coordinates are normalized to `[0, 1]`.

Some examples:

| Conceptual Position | 2D Coordinate |
|---------------------|---------------|
| top left (origin)   | `(0.0, 0.0)`  |
| top                 | `(0.5, 0.0)`  |
| top right           | `(1.0, 0.0)`  |
| left                | `(0.0, 0.5)`  |
| center              | `(0.5, 0.5)`  |
| right               | `(1.0, 0.5)`  |
| bottom left         | `(0.0, 1.0)`  |
| bottom              | `(0.5, 1.0)`  |
| bottom right        | `(1.0, 1.0)`  |

This coordinate system is identical to relative widget space. Note that, because this coordinate system is a float in `[0, 1]`, we cann query a specific pixel of a texture using the texture-coordinates. We are reliant on the graphics card to map a specific pixel position to our texture coordinate.

#### Converting between coordinates

`RenderArea` offers two static functions that allow us to convert between absolute widget space and gl space. Recall that for a widget of size 400x300, the top left corner will be (0, 0), and the bottom right corner will be (400, 300). For example, `MotionEventController`s `motion` signal provides use with coordinates in widget space. 

To convert from widget to gl space, we use `RenderArea::to_gl_coordinates`. For the inverse, we use `RenderArea::from_gl_coordinates`.

For example, to translate the position returned by `MotionEventController` to gl coordinates used by `Shape`, we can do the following:

```cpp
auto motion_controller = MotionEventController();
motion_controller.connect_signal_motion([](MotionEventController*, double widget_x, double widget_y){
    auto widget_coordinates = Vector2f(widget_x, widget_y);
    auto gl_coordinates = RenderArea::to_gl_coordinates(widget_coordinates);
    std::cout << "Current Positio in GL Space: " << gl_coordinates.x << " " < gl_coordinates.y << std::endl;
})
```

Note that we do not need an instance of `RenderArea`, as `to_gl_coordinates` and `from_gl_coordinates` are static functions.

---

## Shapes

### Rendering Shapes

Before we learn about the different types of shapes, we need to learn how to render them. Consider the simplest shape, which is a point. A point is always exactly 1 pixel on screen.

We construct a point shape using `mousetrap::Shape` like so: 

```cpp
auto point = Shape();
point.as_point({0, 0});
```
Where {0, 0} is the center of the viewport.

Mousetrap also offers the following, more syntactically convenient form:

```cpp
auto point = Shape::Point({0, 0});
```
Which is exactly equivalent.

To render this point, we first need our `RenderArea` instance, so we will create that:

```cpp
auto render_area = RenderArea();
```

Then we need to bind our shape for rendering. This is done using a **render task**. A render task takes all objects needed for rendering, and groups them together. When a frame is painted, `RenderArea` will iterate through all of its render tasks and draw each task, in order.

\a{RenderTask} can be constructed manually, or we can use the convenience function `RenderArea::add_render_task`:

Putting it all together:

```cpp
auto point = Shape::Point({0, 0});
auto render_area = RenderArea();

// create task and register it
render_area.add_render_task(RenderTask(task));
```

\image html render_area_point_hello_world.png

\how_to_generate_this_image_begin
```cpp
auto render_area = RenderArea();
auto point = Shape::Point({0, 0});
render_area.add_render_task(RenderTask(point));

auto aspect_frame = AspectFrame(1);
aspect_frame.set_child(render_area);
aspect_frame.set_size_request({150, 150});

auto base = Separator();
auto overlay = Overlay();
overlay.set_child(base);
overlay.add_overlay(aspect_frame, true);

window.set_child(overlay);
```
\how_to_generate_this_image_end

Where we created a `RenderTask` using only `point`, then immediately registered it with our `RenderArea`.

### Shape Types

Mousetrap offers a wide variety of 2d shapes. All shapes are instances of \a{Shape}, which offers the following pre-defined shape types:

#### Point

A `Point` is always exactly one pixel in size. Its constructor takes on `Vector2f`:

```cpp
auto point = Shape::Point({0, 0});
```

\image html render_area_shape_point.png

#### Points

A `Points` is number of points. Instead of taking a single `Vector2f`, its constructor takes `std::vector<Vector2f>`:

```cpp
auto points = Shape::Points({
    {-0.5,  0.5}, 
    {+0.5,  0.5}, 
    { 0.0, -0.5}
});
```

\image html render_area_shape_points.png

#### Line

A `Line` draws a straight line between two vertices. The line is always exactly 1 pixel thick. If we want a line of arbitrary thickness, we should use a `Rectangle`.

```cpp
auto line = Shape::Line({-0.5, 0.5}, {0.5, -0.5});
```

\image html render_area_shape_line.png

#### Lines

`Lines` is a series of *unconnected* lines. Its constructor takes a `std::vector<std::pair<Vector2f, Vector2f>>`, or a vector of vertex-cordinate pairs. Each pair represent one line.

```cpp
auto lines = Shape::Lines({
    {{-0.5, 0.5}, {0.5, -0.5}}, // first line
    {{0.5, 0.5}, {-0.5, -0.5}}  // second line
});
```

\image html render_area_shape_lines.png

#### LineStrip

A `LineStrip` should not be confused with `Lines`. `LineStrip` is a *connected* series of lines. It takes `std::vector<Vector2f>`. A line will be drawn between each successive pair of coordinates. 

In general, if the supplied vector is `{a1, a2, a3, ..., an}` then `LineStrip` will render as a series of lines with coordaintes `{a1, a2}, {a2, a3}, ..., {a(n-1), a(n)}`

```cpp
auto line_strip = Shape::LineStrip({
    {-0.5, +0.5},  // a1
    {+0.5, +0.5},  // a2
    {+0.5, -0.5},  // a3
    {-0.5, -0.5}   // a4
});
```

\image html render_area_shape_line_strip.png

#### Wireframe

`Wireframe` is similar to a `LineStrip`, except that it also connects the last and first vertex. So for a supplied vector `{a1, a2, a3, ..., an}`, the series of lines will be `{a1, a2}, {a2, a3}, ..., {a(n-1), a(n)}, {a(n), a1}`, the last vertex-coordinate pair is what distinguishes it from a `LineStrip`. As such, `Wireframe` is sometimes also called a **line loop**.

```cpp
auto wireframe = Shape::Wireframe({
    {-0.5, +0.5}, 
    {+0.5, +0.5}, 
    {+0.5, -0.5}, 
    {-0.5, -0.5}
});
```

\image html render_area_shape_wireframe.png

#### Triangle

A `Triangle` is constructed using three `Vector2f`, one for each of its vertices:

```cpp
auto triangle = Shape::Triangle({-0.5, 0.5}, {+0.5, 0.5}, {0, -0.5});
```

\image html render_area_shape_triangle.png

#### Rectangle

A `Rectangle` is perhaps the most common shape. It has four vertices and two constructors that either take the top-left corner and a size, or 4 vertex positions in clockwise order:

```cpp
auto rectangle = Shape::Rectangle(
    {-0.5, +0.5},  // top-left
    {+0.5, +0.5},  // top-right
    {+0.5, -0.5},  // bottom-right
    {-0.5, -0.5}   // bottom-left
);

// equivalent to:
auto rectangle = Shape::Rectangle(
    {-0.5, 0.5}, // top-left
    {1, 1}       // {width, height}
);
```

\image html render_area_shape_rectangle.png

#### Circle

A `Circle` is constructed from a center point and radius. We need to specify the number of outer vertices used for the circle, this number will determine how "smooth" the outline is. For example, a circle with 3 outer vertices is a triangle, a circle with 4 is a square, a circle with 5 is a pentagon, etc. As the number of outer vertices increase, the shape approaches a mathematical circle.

```cpp
auto circle = Shape::Circle(
    {0, 0},     // center
    0.5,        // radius
    32          // n outer vertices
);
```

\image html render_area_shape_circle.png

#### Ellipse

A `Ellipse` is a more generalized from of a `Circle`. It's x- and y-radius determine the shape of the ellipse:

```cpp
auto ellipse = Shape::Ellipse(
    {0, 0},     // center
    0.6,        // x-radius
    0.5,        // y-radius
    32          // n outer vertices
);
```

\image html render_area_shape_ellipse.png

#### Polygon

The most general form of convex shapes, `Polygon` is constructed using a vector of vertices. Its constructo takes a vector of `Vector2f`, which will be sorted clockwise, then their outer hull will be calculated which results in the final convex polygon:

```cpp
auto polygon = Shape::Polygon({
    {0, 0.75}, 
    {0.75, 0.25}, 
    {0.5, -0.75}, 
    {-0.5, -0.5},
    {-0.75, 0}
});
```

\image html render_area_shape_polygon.png

#### Rectangular Frame

A `RectangularFrame` takes the top-left vertex, the outer size and the x- and y-width, which will be the thickness of the frame horizontally and vertically:

```cpp
auto rectangular_frame = Shape::RectangularFrame(
    {-0.5, 0.5},  // top left
    {1, 1},       // outer {width, height}
    0.15,         // horizontal thickness
    0.15          // vertical thickness
);
```
\image html render_area_shape_rectangular_frame.png

#### Circular Ring

The round form of a rectangular frame, we have `CircularRing`, which takes a center, the radius of the outer perimeter, as well as the thickness. Like `Circle` and `Ellipse`, we have to specify the number of outer vertices, which decides the smoothness of the ring:

```cpp
auto circular_ring = Shape::CircularRing(
    {0, 0},     // center
    0.5,        // outer radius
    0.15,       // thickness
    32          // n outer vertices
);
```
\image html render_area_shape_circular_ring.png

#### Elliptical Ring

A generalization of `CircularRing`, `EllipticalRing` has an ellipse as its outer shape. Its thickness along the horizontal and vertical dimension are supplied separately, making it far more flexible than `CircularRing`.

```cpp
auto elliptical_ring = Shape::EllipticalRing(
    {0, 0},     // center
    0.6,        // x-radius
    0.5,        // y-radius
    0.15,       // x-thickness
    0.15,       // y-thickness
    32          // n outer vertices
);
```
\image html render_area_shape_elliptical_ring.png

#### Outline

Lastly, we have a very special shape, \link mousetrap::Shape::Outline `Outline`\endlink. `Outline` takes as the only argument to its constructor **another shape**. It will then generate a number of lines that trace the conceptual perimeter of the shape. 

As the name suggest, this is useful for generating outlines of another shape. By rendering the `Outline` on top of the original shape, we can achieve a similar effect ot how `Frame` is used for widgets.

\image html render_area_shape_triangle_outline.png "Shape::Outline(triangle)"
\image html render_area_shape_rectangle_outline.png "Shape::Outline(rectangle)"
\image html render_area_shape_circle_outline.png "Shape::Outline(circle)"
\image html render_area_shape_ellipse_outline.png "Shape::Outline(ellipse)"
\image html render_area_shape_polygon_outline.png "Shape::Outline(polygon)"
\image html render_area_shape_rectangular_frame_outline.png "Shape::Outline(rectangular_frame)"
\image html render_area_shape_circular_ring_outline.png "Shape::Outline(circular_ring)"
\image html render_area_shape_elliptical_ring_outline.png "Shape::Outline(elliptical_ring)"

Rendering these white outline on top of a white shape would make them invisible, of course. To achieve the desired effect, we need to make the outline another color, which brings us to the additional properties of all `Shape`s.

### Shape Properties

Shapes are made up of vertices whose property we have discussed before. In addition to this, the \a{Shape} as a whole has some properties of its own:

#### Centroid

The **centroid** of a shape is the intuitive "center of mass". In mathematical terms, it is the component-wise mean of all vertex coordinates. In practice, for many shapes such as rectangles, triangle, circle and ellipses, the centroid will be the center of the shape, as it is defined in common language. 

We can access the centroid of a shape using `Shape::get_centroid`. To move a shape a certain distance, we move its centroid that distance by calling `Shape::set_centroid`, which will automatically move all other vertices of the shape by the same amount.

#### Color

To chang the color of a shape, we use `Shape::set_color`. This makes all vertices of the shape the same color. By default, a shapes color will be `RBGA(1, 1, 1, 1)`, white.

#### Visibility

We can prevent a shape from being rendered by calling `Shape::set_is_visible(false)`. This is different from making all vertices of a shape have an opacity of `0`. `is_visible` directly hooks into the shapes render function and prevents it from being called. 

#### Bounding Box

We can access the [axis aligned bounding box](https://en.wikipedia.org/wiki/Bounding_volume) of a shape with `Shape::get_bounding_box`, which returns a \a{Rectangle}. Using this, we can query the top-left coordinate and size of the bounding box.

---

## RenderArea Size

The `resize` signal of `RenderArea` is emitted when the `RenderArea` widgets allocated size on screen changes. At first, this may not seem very useful, we can query the size using `Widget::get_allocation` anyway. In actuality, it will usually be necessary to connect to this signal in almost any case where we want to draw a shape to a `RenderArea` of variable size. 

Consider the following example:

```cpp
auto render_area = RenderArea();
auto circle = Shape::Circle({0, 0}, 0.5);
render_area.add_render_task(circle);

render_area.set_size_request({300, 300});
window.set_child(render_area);
```

\image html render_area_circle_unstretched.png

Everything appears as expected. If we now manually resize the window to a non-square resolution:

\image html render_area_circle_stretched.png

The circle appears as an ellipse, despite being a `Shape::Circle`. This is due to the fact the the vertices of this circle use the gl coordinate system, which is **relative**. Relative coordinate systems do not respect the size and aspect ratio of the viewport, the distance between the left edge of the widget and the right edge of the widget will always be `2`, thus stretching the viewport stretches all shapes.

The easiest way to prevent this is by putting the `RenderArea` into an `AspectFrame`, which will force the viewport to allows have the correct ratio, `1` in this case. If we want the `RenderArea` to be freely resizable, however, we will need to do some simple geometry:

```cpp
auto render_area = RenderArea();
static auto circle = Shape::Ellipse({0, 0}, 0.5, 0.5, 32); 
// recall that an ellipse for whom (x-radius == y-radius) is a circle
render_area.add_render_task(circle);

render_area.connect_signal_resize([](RenderArea*, int32_t width, int32_t height)
{
    // calculate y-to-x-ratio
    float new_ratio = float(height) / width;

    // multiply x-radius to normalize it. Then reformat the `circle` shape
    circle.as_ellipse({0, 0}, 0.5 * new_ratio,  0.5, 32);
});

render_area.set_size_request({200, 200});
window.set_child(render_area);
```

Here, we connected to signal `resize`, which provides us with the current width and height of the `RenderArea`, in pixels. By changing `Shape::Circle` to `Shape::Ellipse`, we can modify the "circles" x-radius by multiplying it with the current aspect ratio of the viewport.

When we now rescale the window, the shape will be automatically reformated and appear as a proper circle, regardless of `RenderArea`s allocated size:

\image html render_area_circle_normalized.png

While this isn't that much effort, if we need our `RenderArea` to always have a specific size or aspect ratio, an `AspectFrame` may be an easier solution.

---

## Textures

We've learned before that we can us `ImageDisplay` to render an image on screen. This works for most purposes, though it is somewhat limited. `ImageDisplay` has a number of dsiadvantages:

+ image data is not updated every frame that it is rendered
+ uploading data is costly
+ scaling the image will always trigger linear interpolation
+ the image is always shown in full, on a rectangle

If we need the additional flexiblity, we should use \a{Texture}, which represents an image living in the graphics card memory.

We create a texture from an \a{Image} like so:

```cpp
auto image = Image();
image.load_from_file(// ...

auto texture = Texture();
texture.create_from_image(image);
```

Once `create_from_image` is called, the image data is uploaded to the graphics cards' RAM, so we can safely discard the `Image` instance. 

To display this texture, we need to bind it to a shape, then render that shape:

```cpp
auto texture_shape = Shape::Rectangle({-1, 1}, {2, 2});
texture_shape.set_texture(texture);
```

Where this shape is a rectangle that is the size of the entire viewport. 

\todo figure 
\todo texture coordinate mapping

#### Scale Mode

Similar to `Image::as_scaled`, we have options on how we want the texture to behave when scaled to a size other than its native resolution (the resolution of the image we created the texture from). Mousetrap offers the following texture scale modes, which are represented by the enum \a{TextureScaleMode}:

+ `NEAREST`: nearest-neighbor scaling
+ `LINEAR`: linear scaling

Which are roughly equivalent to `Image`s `InterpolationType::NEAREST` and `InterpolationType::BILINEAR`, except much, much more performant. Rescaling a texture is essentially free when done by the graphics card, as opposed to the CPU.

#### Wrap Mode

Wrap mode governs how the texture behaves when a vertices texture coordinates are outside of `[0, 1]`. Mousetrap offers the following wrap modes, which are all part of the enum \a{TextureWrapMode}:

+ `ZERO`: Pixels will appear as `RGBA(0, 0, 0, 0)`
+ `ONE`: Pixels will appear as `RGBA(1, 1, 1, 1)`
+ `REPEAT`: \todo figure
+ `MIRROR`: \todo figure
+ `STRETCH`: \todo figure

With this an by being able to modify the vertex coordinates for every shapes vertices, we have much more control over how image data is displayed on screen. Modifying textures is also viable for real-time applications, simple keep an `Image` in RAM that is modified by the application, then `create_from_image` every frame the image changes to update the `Texture` rendered on screen.

---

## Intermission

For most users, what we have learned so far will be enough to create new widgets from scratch. Using render tasks and textures, we have full control over rendering any shape or image. Using signal `resize` or `AspectFrame`, we can create complex images at any resolution. To animate these images, we can react to user-behavior by adding one or more of the [event controllers from the previous chapter](05_events.md) to `RenderArea`, then modifying a shapes property from within any of the signals provided by these event controllers. 

The rest of this chapter is intended users familiar with OpenGL. We will learn how to integrate native OpenGL into mousetrap, write our own shaders, and how to perform advanced rendering techniques, such as rendering to a texture and applying multi-sample anti-aliasing. This requires external knowledge not supplied by this manual, which is why we recommend users unfamiliar with these topics to simply [skip to the last chapter](10_closing_statements.md), as the tools already provided are powerful enough to create an application for almost any task.

---

---

## Render Task

So far, we registered render tasks using `render_area.add_render_task(RenderTask(shape))`. This is shortform for the following:

```cpp
auto render_task = RenderTask(
    shape,        // const Shape&
    shader,       // const Shader*
    transform,    // const Transform*
    blend_mode    // mousetrap::BlendMode
)
render_area.add_render_task(render_task);
```

Using these four components, `RenderTask` gathers all objects necessary to render a shape to the screen. All of these except the `Shape` are optional, if not specified a default value will be used for them. This is what allows less experienced users to fully ignore shaders, transforms and blendmodes.

In this section, we'll go through each render task component, learning how we can hook our own OpenGL behavior into mousetraps rendering architecture.

---

## Shaders

Mousetrap offers two types of shaders, **fragment** and **vertex** shaders. These shaders are written in GLSL. 

### Compiling Shaders

To create a shader, we first instantiate \a{Shader}, which is a class representing a **shader program** (a compiled vertex- and fragment shader). We can then compile code for a fragment shader like so:

```cpp
// create default shader
auto shader = Shader();

// replace default fragment shader with custom fragment shader
shader.create_from_string(ShaderType::FRAGMENT, R"(
    #version 330
    
    in vec4 _vertex_color;
    in vec2 _texture_coordinates;
    in vec3 _vertex_position;
    
    out vec4 _fragment_color;
    
    void main()
    {
        vec2 pos = _vertex_position.xy;
        _fragment_color = vec4(pos.y, dot(pos.x, pos.y), pos.x, 1);
    }
)");
```

We can then supply a pointer to the shader as the 2nd argument to `RenderArea::add_render_task`, which automatically binds it for rendering:

```cpp
// create rectangle shape that occupise 100% of the viewport
auto shape = Shape::Rectangle({-1, -1}, {2, 2});

// create fragment shader
auto shader = Shader();
shader.create_from_string(ShaderType::FRAGMENT, R"(
    (...)
)");

// bind shape and shader for rendering
render_area.add_render_task(RenderTask(
    shape, 
    &shader
);
```

\image html shader_hello_world.png

\how_to_generate_this_image_begin
```cpp
auto render_area = RenderArea();
static auto shape = Shape::Rectangle({-1, -1}, {2, 2});
static auto shader = Shader();
shader.create_from_string(ShaderType::FRAGMENT, R"(
    #version 130

    in vec4 _vertex_color;
    in vec2 _texture_coordinates;
    in vec3 _vertex_position;

    out vec4 _fragment_color;

    void main()
    {
        vec2 pos = _vertex_position.xy;
        _fragment_color = vec4(pos.y, dot(pos.x, pos.y), pos.x, 1);
    }
)");

render_area.add_render_task(RenderTask(shape, &shader));

auto aspect_frame = AspectFrame(1);
aspect_frame.set_child(render_area);
window.set_child(aspect_frame);
```
\how_to_generate_this_image_end

We see that the first argument to `Shader::create_from_string` is the **shader type**, which is either `ShaderType::FRAGMENT` or `ShaderType::VERTEX`. If we do not call `create_from_string` for either or both of these, the default shader program will be used. It may be instructive to see what these default shaders behavior is:

### Default Vertex Shader

This is the default vertex shader is the following:

```glsl
#version 330

layout (location = 0) in vec3 _vertex_position_in;
layout (location = 1) in vec4 _vertex_color_in;
layout (location = 2) in vec2 _vertex_texture_coordinates_in;

uniform mat4 _transform;

out vec4 _vertex_color;
out vec2 _texture_coordinates;
out vec3 _vertex_position;

void main()
{
    gl_Position = _transform * vec4(_vertex_position_in, 1.0);
    _vertex_color = _vertex_color_in;
    _vertex_position = _vertex_position_in;
    _texture_coordinates = _vertex_texture_coordinates_in;
}
```

We see that it requires OpenGL 3.3, which is the reason that mousetrap also requires that version as a dependency. This shader simply forwards the corresponding shape attributes to the fragment shader.

The current vertices position is supplied via `_vertex_position_in`, the vertices texture coordinates as `_vertex_color_in` and the vertices texture coordinates as `_vertex_texture_coordinates`.

The output variables are `_vertex_color`, `_texture_coordinates` and `_vertex_position`, which should be assigned with the interpolated result of the vertex shader. The shader has furthermore access to the uniform `_transform`, which holds the transform of the current render task.

### Default Fragment Shader

```glsl
#version 330

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform int _texture_set;
uniform sampler2D _texture;

void main()
{
    if (_texture_set != 1)
        _fragment_color = _vertex_color;
    else
        _fragment_color = texture2D(_texture, _texture_coordinates) * _vertex_color;
}
```

The fragment shader is handed `_vertex_color`, `_texture_coordinate` and `_vertex_position`, which we recognize as the output of the vertex shader. The output of the fragment shader is `_fragment_color`.

The default fragment shader respects two uniforms, `_texture`, which is the texture of the shape we are currently rendering and `_texture_set`, which is `1` if `Shape::set_texture` was called before, `0` otherwise.

Users aiming to experiment with shaders should take care to not modify the name or `location` of any of the `in` or `out` variables of either shader.

### Binding Uniforms

Both the vertex and fragment shader make use of uniforms. We've seen that `_transform`, `_texture` and `_texture_set` are assigned automatically. Mousetrap offers a convenient way to add additional uniforms, however.

To bind a uniform, we first need a CPU-side value that should be uploaded to the graphics card. Let's say we want our fragment shader to color the shape a certain color, which is specified by a CPU-side program. This fragment shader would look like this:

```glsl
#version 330

in vec4 _vertex_color;
in vec2 _texture_coordinates;
in vec3 _vertex_position;

out vec4 _fragment_color;

uniform vec4 _color_rgba; // new uniform

void main()
{
    _fragment_color = _color_rgba;
}
```

To set the value of `_color_rgba`, we use `RenderTask::set_uniform_rgba`. This is one of the many `set_uniform_*` functions, which allow us to bind C++ values to OpenGL Shader Uniforms:

The following types can be assigned this way:

| C++ Type      | `RenderTask` function   | GLSL Uniform Type |
|---------------|-------------------------|-------------------|
| `float`       | `set_uniform_float`     | `float`           |
| `int32_t`     | `set_uniform_int`       | `int`             |
 | `uint32_t`    | `set_uniform_uint`      | `uint`            |
 | `Vector2f`    | `set_uniform_vec2`      | `vec2`            |
| `Vector3f`    | `set_uniform_vec3`      | `vec3`            |
| `Vector4f`    | `set_uniform_vec4`      | `vec4`            |
| `GLTransform` | `set_uniform_transform` | `mat4x4`          |
| `RGBA`        | `set_uniform_rgba`      | `vec4`            |
| `HSVA`        | `set_uniform_hsva`      | `vec4`            |

\todo Add `set_uniform_texture` so users do not have to use texture locations

With this, we can set our custom `_color_rgba` uniform value like this:

```cpp
// create shader
auto shader = Shader();
shader.create_from_string(ShaderType::FRAGMENT, R"(
    #version 330

    in vec4 _vertex_color;
    in vec2 _texture_coordinates;
    in vec3 _vertex_position;

    out vec4 _fragment_color;

    uniform vec4 _color_rgba;

    void main()
    {
        _fragment_color = _color_rgba;
    }
)");

// create shape
auto shape = Shape::Rectangle({-1, -1}, {2, 2});

// create task
auto task = RenderTask(shape, &shader);

// register uniform with task
task.set_uniform_rgba("_color_rgba", RGBA(1, 0, 1, 1));

// register task with render area
render_area.add_render_task(task);
```

\image html shader_rbga_uniform.png

\how_to_generate_this_image_begin
```cpp
auto render_area = RenderArea();

static auto shader = Shader();
shader.create_from_string(ShaderType::FRAGMENT, R"(
    #version 330

    in vec4 _vertex_color;
    in vec2 _texture_coordinates;
    in vec3 _vertex_position;

    out vec4 _fragment_color;

    uniform vec4 _color_rgba;

    void main()
    {
        _fragment_color = _color_rgba;
    }
)");

static auto shape = Shape::Rectangle({-1, -1}, {2, 2});
static auto task = RenderTask(shape, &shader);
task.set_uniform_rgba("_color_rgba", RGBA(1, 0, 1, 1));
render_area.add_render_task(task);

auto aspect_frame = AspectFrame(1);
aspect_frame.set_child(render_area);
window.set_child(aspect_frame);
```
\how_to_generate_this_image_end

In summary, while we do not have control over the `in` and `out` variables of either shader type, we have full control of the uniforms, giving us all the flexibility we need to accomplish complex shader tasks.

---

## Transforms

As mentioned before, \a{GLTransform} is the C++-side object that represents spatial transforms. It is called `GLTransform` because it **uses the GL coordinate system**. Applying a `GLTransform` to a vector in widget- or texture-space will produce incorrect results. They should only be applied to the `vertex_position` attribute of a `Shape`s vertices.

Internally, a `GLTransform` is a 4x4 matrix. At any time, we can directly access this matrix as the public member `GLTransform::transform`. 

When constructed, the matrix will be the identity transform:
```
1 0 0 0
0 1 0 0
0 0 1 0
0 0 0 1
```
No matter the current state of the transform, we can reset it back to the identity matrix by calling `GLTransform::reset`.

`GLTransform` has the basic spatial transform already programmed in, so we usually do not need to modify the internal matrix ourself. It provides the following transforms:

+ `GLTransform::translate` for translation in 3d space
+ `GLTransform::scale` for scaling
+ `GLTransform::rotate` for rotation around a point

We can combine two transforms using `GLTransform::combine`. If we wish to apply the transfrom CPU-side to a `Vector2f` or `Vector3f`, we can use `GLTransform::apply_to`.

While we could apply the transform to each vertex of a `Shape`, then render the shape, it is much more performant to do this kind of math GPU-side. To do this, we register the transform with a `RenderTask`. Afterwards, the transform will be forwarded to the vertex shaders `_transform` uniform, which is then applied to the shapes vertices automatically:

```cpp
auto shape = // ...
auto transform = GLTransform();
transform.translate({-0.5, 0.1});
transform.rotate(degrees(180), {0,0);

auto task = RenderTask(
   shape,     // shape
   nullptr,   // set null to use the default shader
   transform  // use our transform instead of identity
);
```

---

## Blend Mode

As the last component of a render task, we have **blend mode**. This governs how two colors behave when rendered on top of each other. We call the color currently in the frame buffer `destination`, while the newly added color is called `origin`. 
Mousetrap offers the following blend modes, which are part of the enum \a{BlendMode}:

| `BlendMode`        | Resulting Color                      |
|--------------------|--------------------------------------|
| `NONE`             | `origin + 0 * destination`           | 
| `NORMAL`           | traditional alpha-blending           |
 | `ADD`              | `origin.rgba + destination.rgba`     |
| `SUBTRACT`         | `origin.rgba - destination.rgba`     |
| `REVERSE_SUBTRACT` | `destination.rgba - origin.rgba`     | 
| `MULTIPLY`         | `origin.rgba * destination.rgba`     |
| `MIN`              | `min(origin.rgba, destination.rgba)` |
 | `MAX`              | `max(origin.rgba, destination.rgba)` | 

These are the familiar blend modes from graphics editors such as Photoshop or GIMP.

If left unspecified, `RenderTask` will use `BlendMode::NORMAL`, which represents traditional alpha blending. Using this blend mode, the opacity of `destination` and `origin` is treated as their emission, and then weighted mean is calculated producing the final result. This mimics how mixing colored light behaves in the real world.

---

## Render
