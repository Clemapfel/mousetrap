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
auto task = RenderTask(point);
render_area.add_render_task(task);
```

\image html render_area_point_hello_world.png

\how_to_generate_this_image_begin
```cpp
auto render_area = RenderArea();
auto point = Shape::Point({0, 0});
render_area.add_render_task(point);

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

Lastly, we have a very special shape, `Outline`. `Outline` takes as its only argument to its constructor **another shape**. It will then generate a number of lines such that the outline is along the perimeter of the supplied shape. As the name suggest, this is useful for generating outlines of another shape. By rendering the `Outline` on top of the original shape, we can achieve a similar effects as `Frame` does for widgets.

\image html render_area_shape_triangle_outline.png "Shape::Outline(triangle)"
\image html render_area_shape_rectangle_outline.png "Shape::Outline(rectangle)"
\image html render_area_shape_circle_outline.png "Shape::Outline(circle)"
\image html render_area_shape_ellipse_outline.png "Shape::Outline(ellipse)"
\image html render_area_shape_polygon_outline.png "Shape::Outline(polygon)"
\image html render_area_shape_rectangular_frame_outline.png "Shape::Outline(rectangular_frame)"
\image html render_area_shape_circular_ring_outline.png "Shape::Outline(circular_ring)"
\image html render_area_shape_elliptical_ring_outline.png "Shape::Outline(elliptical_ring)"

Rendering these white outline on top of a white shape would make them invisible. To achieve the desired effect, we need to change some properties of the `Outline` shape.

### Shape Properties

Shapes are made up of vertices whose property we have discussed before. In addition to this, the \a{Shape} itself has some properties of its own:

#### Centroid

The **centroid** of a shape is the intuitive "center of mass". In mathematical terms, it is the average of all vertex coordinates. In practice, for many shapes such as rectangles, triangle, circle and ellipses, the centroid will be the center of the shape.

We can access the centroid of a shape using `Shape::get_centroid`. To move a shape a certain distance, we move its centroid that distance, which will move all other vertices accordingly. As such, the centroid is the **origin of transformations**. One such transformation is translation, which we can accomplish using `Shape::set_centroid`.

#### Color

To chang the color of a shape, we use `Shape::set_color`. This changes all vertices of the shape to the same color. By default, a shapes color will be `RBGA(1, 1, 1, 1)`, white.

#### Visibility

We can prevent a shape from being rendered (even if its render tasks is bound), by calling `Shape::set_is_visible(false)`. This is different from making all vertices of a shape have an opacity of `0`. `is_visible` directly hooks into the shapes render function and prevents it from being called. 

#### Bounding Box

We can access the [axis aligned bounding box](https://en.wikipedia.org/wiki/Bounding_volume) of a shape with `Shape::get_bounding_box`, which returns a \{Rectangle}. Mousetrap also provides `Shape::get_size` and `Shape::get_top_left`, which return the `size` and  `top_left` property of the bounding box directly.

---

## RenderArea Size

The `resize` signal of `RenderArea` is emitted anytime the render area change sizes on screen, usually when its allocated size changes. While at first seeming redundant (since we can query the size of a widget using `get_allocation` or `get_preferred_size` anyway), it will usually be necessary to connect to this signal for any application where shapes should **maintain a certain aspect ratio**. Consider the following example:

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

The circle appears as an ellipse, despite being a `Shape::Circle`. This is due to the fact the the vertices of this circle use the OpenGL coordinate system, which is **relative**. Regardless of the ratio of width-to-height, y-coordaintes are always in [-1, 1], with the same being true for x-coordinates. Once the viewport (the RenderAreas allocate area, in this case), becomse non-square, a circle will stretch.

The easiest way to prevent this is by putting the `RenderArea` into an `AspectFrame`, which will force the viewport to allows have the correct ratio, `1` in this case. If we want the `RenderArea` to be freely resizable, however, we will need to do some simple geometry.

```cpp
auto render_area = RenderArea();
static auto circle = Shape::Ellipse({0, 0}, 0.5, 0.5, 32); // an ellipse for whom x-radius == y-radius is a circle
render_area.add_render_task(circle);

render_area.connect_signal_resize([](RenderArea*, int32_t width, int32_t height){
    float new_ratio = float(height) / width;
    circle.as_ellipse({0, 0}, 0.5 * new_ratio,  0.5, 32);
});

render_area.set_size_request({200, 200});
window.set_child(render_area);
```

Here, we connected to signal `resize`, which provides us with two argument, the current width and height of the `RenderArea`s viewport, in pixels. By changing `Shape::Circle` to `Shape::Ellipse`, we can modify the "circles" x-radius by multiplying it with the current aspect ratio of the viewport:

```cpp
// calculate y-to-x-ratio
float new_ratio = float(height) / width;

// multiply x-radius to normalize it. Then reformat the `circle` shape
circle.as_ellipse({0, 0}, 0.5 * new_ratio,  0.5, 32);
```

When we now rescale the window, the shape will be automatically reformated, making it a proper circle at any aspect ratio:

\image html render_area_circle_normalized.png

Using signal `resize` smartly this way, we drop the need to put a `RenderArea` into an aspect frame. If our specific problem only requires one specific viewport ratio, however, using the `AspectFrame` is usually less work than connecting to `resize`.

---

## Textures

In the chapter on widget, we learned that we can use `ImageDisplay` to render an image on screen. This works for most purposes, though it is somewhat limited, as we can only render an image in a square area, and we have no way of manipulating the image once the widget is created. This can be fixed using \a{Texture}, which manages image data in the graphics card memory. 

We create a texture from an \a{Image} like so:

```cpp
auto image = Image();
image.load_from_file(// ...

auto texture = Texture();
texture.create_from_image(image);
```

Once `create_from_image` is called, the image data is uploaded to the graphics cards ram, so we can safely discard the `Image` instance. 

To display this texture, we need to bind it to a shape, then render that shape:

```cpp
auto texture_shape = Shape::Rectangle({-1, 1}, {2, 2});
texture_shape.set_texture(texture);
```

Where the shape is a rectangle that is always the size of the entire viewport. 

Note that the shape for whom `Shape::set_texture` was called only holds a weak pointer to the texture, we have to make sure that the `Texture` instance stays in memory ourself. This is because we want to create as few texture as possible, uploading a texture is a very costly operation, so if multiple objects should have the same texture, we should only ever hold one copy of that texture in memory.

Once `set_texture` is called, if we render the shape, the texture is displayed:

\todo figure

If the shapes vertices color is anything except `RGBA(1, 1, 1, 1)`, each pixel of the texture will be multiplied with that color.

#### Scale Mode

Similar to `Image::as_scaled`, we have options on how we want the texture to behave when scaled to a size other than its native resolution (the resolution of the image we created the texture from). Mousetrap offers the following texture scale modes, which are represented by the enum \a{TextureScaleMode}:

+ `NEAREST`: nearest-neighbor scaling
+ `LINEAR`: linear scaling

Which are roughly equivalent to `Image`s `InterpolationType::NEAREST` and `InterpolationType::BILINEAR`.

#### Wrap Mode

Wrap mode governs how the texture behaves when a vertices texture coordinate are outside of `[0, 1]`. Mousetrap offers the following wrap modes, which are all part of the enum \a{TextureWrapMode}:

\todo figures

---

## Intermission

For most users, what we have learned so far will be enough to create new widgets from scratch. Using render tasks and textures, we have full control over rendering any shape. Using signal `resize` or `AspectFrame`, we can create complex images at any resolution. To animate these images, we can react to user-behavior by adding one or more of the [event controllers from the previous chapter](05_events.md) to `RenderArea`, then modifying a shapes property from within any of the signals provided by these event controllers. 

The rest of this chapter is intended users familiar with OpenGL. We will learn how to integrate native OpenGL into mousetrap, write our own shaders, and how to perform advanced rendering techniques, such as rendering to a texture and applying multi-sample anti-aliasing. This requires external knowledge not supplied by this manual, which is why we recommend users unfamiliar with these topics to simply [skip to the last chapter](10_closing_statements.md), as the tools already provided are powerful enough to create an application for almost any task.

---

---

## Render Task

So far, we registered render tasks using `render_area.add_render_task(shape)`. This is shortform for the following:

```cpp
auto render_task = RenderTask(
    shape,        // const Shape&
    shader,       // const Shader*
    transform,    // const Transform*
    blend_mode    // mousetrap::BlendMode
)
```

Using these four components, `RenderTask` gathers all object necessary to render a shape to the screen. All of these except the `Shape` are optional, if not specified a default value will be used for them, which allowed use to simply render a shape without thinking about any of the other components. 

In this section, we will want to think about them, however. We've already learned about `Shape`, next are `Shader`s.

## Shaders

Mousetrap offers two entry points for shaders, **fragment** and **vertex** shaders. These shaders are written in GLSL. While users can write any behavior they want inside the body of these shaders, the in- and out- variables, as well as certain uniforms are set.

### Compiling Shaders

To create a shader, we first instantiate \a{Shader}, then compile code like so:

```cpp
auto shader = Shader();
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
render_area.add_render_task(
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

        _fragment_color = vec4(
            pos.y,
            dot(pos.x, pos.y),
            pos.x,
            1
        );
    }
)");

render_area.add_render_task(
    shape,
    &shader
);

auto aspect_frame = AspectFrame(1);
aspect_frame.set_child(render_area);
window.set_child(aspect_frame);
```
\how_to_generate_this_image_end

We see that the first argument to `Shader::create_from_string` is the **shader type**, which is either `ShaderType::FRAGMENT` or `ShaderType::VERTEX`. If one or both of these is not specific when creating a render task, mousetrap will use the default shaders, which are printed here for reference

### Default Vertex Shader

This is the default vertex shader:

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

Note that all vertex shaders have to have this layout, meaning `_vertex_position_in`, `_vertex_color_in` and `_vertex_texture_coordinate_in` , which supply the shader with the corresponding data from the shape wer are rendering, **cannot be renamed**, all mousetrap vertex shaders have to use exactly these names at exactly the `location` state above. Similarly, the output `_vertex_color`, `_texture_coordinates` and `_vertex_position` have to have these names, and only these three variables can be outputed. 

This shaders uniform is `_transform`, which is a \a{GLTransform} bound to the `RenderTask`. If one is present, the vertex shader should respect this transform and apply it to the shapes vertices. If a `RenderTask` had no transform registered, the `_transform` uniform will be the identity transform.

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

We see that the vertex-shader output variables appear as inputs here. Again, these names are fixed, the shader should always use exactly these names. Similarly, we should use `_fragment_color` as the output, not `gl_FragCoord`. 

The uniform `_texture` is a pointer to the texture data registered via `Shape::set_texture`. If no texture is present, this data will be `0`. `_texture_set`, then, is a boolean flag telling us whether the shape does or does not have a texture. If a texture is present, `_texture_set` will be `1`, if not texture is present, `_texture_set` will have a value of `0`. 

### Binding Uniforms

Both the vertex and fragment shader make use of uniforms. These, unlike the `in` and `out` variables can be modified freely, and we can forward any object from CPU-side to the shaders.

To bind a uniform, we first need a CPU-side value. Let's say we want our fragment shader to color the shape a certain color, which is specified by a CPU-side program. The vertex shader would look like this:

```glsl
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
```

To set the value of `_color_rgba`, we should use `RenderTask`, which has an interface for registering values, called `RenderTask::set_uniform_*`, where `*` is the type of the uniform.

The following types can be registered:

| C++ Type      | `RenderTask` function   | GLSL Uniform Type |
|---------------|-------------------------|-------------------|
| `float`       | `set_uniform_float`     |`float`            |
| `int32_t`     | `set_uniform_int`       |`int`              |
 | `uint32_t`    | `set_uniform_uint`      |`uint`             |
 | `Vector2f`    | `set_uniform_vec2`      |`vec2`             |
| `Vector3f`    | `set_uniform_vec3`      |`vec3`             |
| `Vector4f`    | `set_uniform_vec4`      |`vec4`             |
| `GLTransform` | `set_uniform_transform` |`mat4x4`           |
| `RGBA`        | `set_uniform_rgba`      | `vec4`            |
| `HSVA`        | `set_uniform_hsva`      | `vec4`            |

With this, we can set our custom `_color_rgba` uniform like this:

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

In summary, while we cannot choose the `in` and `out` variables of either shader type, we have full control of the uniforms, giving us all the flexibility we need to accomplish complex shader tasks.

## Transforms

As mentioned before, \a{GLTransform} is the C++-side object that represents spatial transforms. It is called `GLTransform` because it **uses the GL coordinate system**. Applying a `GLTransform` to widget- or texture-space coordinates will not work.

Internally, a `GLTransform` is a 4x4 matrix. At any time, we can directly access this matrix by modifying the public member `GLTransform::transform`. 

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

We can combine two transforms using `GLTransform::combine` and if we wish to apply the transfrom CPU-side to a `Vector2f` or `Vector3f`, we can use `GLTransform::apply_to`.

While we could apply the transform to each vertex of a `Shape` CPU-side, it is much more performant to do this kind of math GPU-side. To do this, we register the transform with a `RenderTask`. Afterwards, the transform will be forwarded to the vertex shaders `_transform` uniform, which is then applied to the shapes vertices automatically:

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

## Blend Mode

As the last part of a render task, we have **blend mode**. Mousetrap offers the following blend modes, which are part of the enum \a{BlendMode}:

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

Which are familiar from graphics editors such as PhotoShop or GIMP.

If left unspecified, `RenderTask` will use `BlendMode::NORMAL`, which represents traditional alpha blending, in which the alpha value of both colors is treated as their emission.

## 
