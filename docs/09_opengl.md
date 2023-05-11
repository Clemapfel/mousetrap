# Chapter 9: Native Rendering

In this chapter we will learn:
+ How to use `RenderArea`
+ How to draw any shape
+ How to bind a shape, transform, and shader for rendering
+ How to render to a multi-sampled texture

Mousetrap has proven to have a lot of features, but it is missing a critical piece. So far, all widgets are either supplied by mousetrap directly, or a compound widget. This is fine for most applications, but some require absolute flexibility. In this chapter, we will learn to use `RenderArea`, which lets user **render arbitrary shapes**, allowing us to build an interface from scratch.

## RendeArea

This central widget of this chapter is `RenderArea`. At first, it may seem very simple:

```cpp
auto area = RenderArea();
```

But this call abstracts away an entire graphics pipeline. Using OpenGL is hard, mousetrap allows us to not touch opengl at all directly. First lets check what render area actually looks like:

\todo figure

It's.. invisible. This is intended, without interaction `RenderArea` is fully transparent. This is because we have to add **shapes** and **bind them for render**.

### Shapes

In opengl, anything that can appear on screen is made up of **vertices**. The way these vertices are connected and their properties determine what shape will be rendered. 

All shapes in mousetrap are `Shape`. There is no "rectangle shape", any shape can be a rectangle:

```cpp
auto shape = Shape();
shape.as_rectangle({

### Signal Activate

## Shapes

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