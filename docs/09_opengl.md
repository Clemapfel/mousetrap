# Chapter 9: Native Rendering

In this chapter we will learn:
+ How to use `RenderArea`
+ How to draw any shape
+ How to bind a shape, transform, and shader for rendering
+ How to render to a multi-sampled texture

Mousetrap has proven to have a lot of features, but it is missing a critical piece. So far, all widgets are either supplied by mousetrap directly, or are a compound widget made up of supplied widgets. This is fine for most applications, but some require absolute flexibility. In this chapter, we'll learn about \a{RenderArea}, which uses OpenGL to render any shape or image. This along with event controllers will allow use to build fully customized widget.

## RendeArea

This central widget of this chapter is `RenderArea`. At first, it may seem very simple:

```cpp
auto area = RenderArea();
```

Using OpenGL is hard, which is why mousetrap aims to abstract it fully in common applications, the first of which is rendering an arbitrary shape.

## Vertices

In OpenGL, any shape that can be drawn is made up of **vertices**. In mousetrap, a vertex is represented by the \a{Vertex} struct, which has three properties:

+ `position`: position in 3d space
+ `texture_coordinates`: texture coordainte in 2d texture space
+ `color`: color in RGBA

### Coordinate Spaces

\todo this section is not yet complete, see https://learnopengl.com/Getting-started/Coordinate-Systems

## Rendering Shapes

Before we learn about the different types of spaces, we need to learn how to render them. Consider the simplest shape, which is a point. A point is always exactly 1 pixel on screen.

We construct a point shape like so:

```cpp
auto point = Shape();
point.as_point({0, 0});
```

Where {0, 0} is the center of the viewport. To render this point, we first need our `RenderArea` instance. 


### Binding a Context

### Render Tasks

### Shape Types

### Vertex Properties

### Textures

### Shaders

### Transforms

### RenderTextures

### mousetrap::RenderTexture

### Multi Sample Anti Aliasing