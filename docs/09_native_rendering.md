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

In the [chapter on widgets](04_widgets.md), we noted that, so far, we were only create new widgets by combining already pre-defined widgets. We could create a widget that has a `Scale`, but we can't render our own scale with, for example, a square knob. In this chapter, we will change this. By using OpenGL to render any arbitrary shape, we can construct our own widgets pixel-by-pixel, line-by-line. Using this and the event system, we can craft our own `Scale` from scratch.

## RendeArea

This central widget of this chapter is `RenderArea`. At first, it may seem very simple:

```cpp
auto area = RenderArea();
```

Using OpenGL is hard, which is why mousetrap aims abstract as much as possible, meaning mousetrap users will have to work very little to accomplish tasks that would be complex in pure OpenGL. That said, mousetrap also provides an entry point for experienced users to fully take over rendering, allowing use of pure OpenGL and GLSL, if they choose to do so.

## Vertices

Any shape that can be drawn is made up of **vertices**. In mousetrap, a vertex is represented by the \a{Vertex} struct, which has three properties:

+ `position`: vertex position in 3d space
+ `texture_coordinates`: texture coordinates in 2d texture space
+ `color`: color in RGBA

### Coordinate Spaces

In this section, we will be using three different coordinate systems. **3D space**, **texture space**.

#### OpenGL Space

A vertices `position` is a point in 3D space, which is the left-handed coordinate system familiar from traditional geometry:

\image html https://learnopengl.com/img/getting-started/coordinate_systems_right_handed.png

<center>(source: [learnopengl.com](https://learnopengl.com/Getting-started/Coordinate-Systems))</center>

 Sticking to 2D for now, we assume the the z-coordainte is '0' from now on. This reduces the 3d space to a 2D plane, which is sometimes called **gl coordinates**. 
 
We see that the y-coordinate is positive when move "up", negative when moving "down". The x coordinate is negative when going "left", positive when going "right". 

The coordinates are **relative**, meaning they are normalized to `[-1, 1]`. This means that a shapes coordinates are **independent of the size of the viewport**. No matter if our `RenderArea` is of size 500x200 or 200x500, a shapes vertex coordinates do not change. The shape is dynamically resized depending on the viewport size.

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

A vertices `texture_coordinate` in texture space uses a different coordinate system. This is anchored at the top-left of the texture, with the y-coordinate increasing in value when going down and decreasing in value when going up. The x-coordinate increase in value when going right, and decreases when going left. Coordinates are normalized to `[0, 1]`.

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

This coordinate system is identical to relative widget space.

#### Converting between coordinates

`RenderArea` offers two static functions that allow us to convert between absolute widget space and 3d space. Recall that for a widget of size 400x300, the top left corner will be (0, 0), and the bottom right corner will be (400, 300). For example, `MotionEventController`s `motion` signal provides use with coordinates in widget space. 

To convert from widget to gl coordinates, we use `RenderArea::to_gl_coordinates`. For the inverse, we use `RenderArea::from_gl_coordinates`.

For example, to translate the position returned by `MotionEventController` to gl coordinates used by `Shape`, we can do the following:

```cpp
auto motion_controller = MotionEventController();
motion_controller.connect_signal_motion([](MotionEventController*, double widget_x, double widget_y){
    auto widget_coordinates = Vector2f(widget_x, widget_y);
    auto gl_coordinates = RenderArea::to_gl_coordinates(widget_coordinates);
    std::cout << "Current Positio in GL Space: " << gl_coordinates.x << " " < gl_coordinates.y << std::endl;
})
```

Note that we do not need an instance of `RenderArea`, as `to_gl_coordinates` is a static function.

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

Then add our shape as a **render task**. A render task binds all objects needed for rendering, then registeres this task with the render area. From that point onwards, whenever the render area emits its `render` signal (unless we have manually connected our own signal handler to it), that shape will be draw.

Putting it all together:

```cpp
auto point = Shape::Point({0, 0});
auto render_area = RenderArea();
render_area.add_render_task(point);
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

## Shaders

Mousetrap offers two entry points for shaders, **fragment** and **vertex** shaders. These shaders are written in GLSL. While users can write any behavior they want inside the body of these shaders, the in- and out- variables, as well as certain uniforms are set.

### Compiling Shaders

To create a shader, we first instantiate \a{Shader}, then compile code like so:

```cpp
auto shader = Shader();
shader.



