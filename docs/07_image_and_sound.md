# Chapter 7: Image & Sound

In this chapter we will learn:
+ How to use colors in mousetrap
+ How to load, store, modify, and display 2d images
+ How to load, store, and play audio

---

<details><summary><b><tt>main.cpp</tt> for this chapter</b></summary>

Here is a `main.cpp` that allows us to run any code snippet mentioned in this chapter:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("example.app");
    app.connect_signal_activate([](Application* app)
    {
        auto window = Window(*app);
        auto button = Button();
        button.set_child("<b>&#9205;</b>");
        button.connect_signal_clicked([](Button*){
           // snippet goes here, press button to trigger it 
        });
        
        window.set_child(button);
        window.present();
    });
    return app.run();
}
```
</details>

---

## Introduction

Mousetrap was originally written as the GUI engine for an unreleased frame-by-frame animation app. This history is why it contains a fully-featured, image processing suite, making it well-suited out of the box for image-editing applications.

## Colors

Mousetrap offers two color representations, `RGBA` and `HSVA`, which have the following components:

| representation | component   | meaning         |
|----------------|-------------|-----------------|
| `RGBA`         | `r`         | red             |
| `RGBA`         | `g`         | green           |
| `RGBA`         | `b`         | blue            |
| `RGBA`         | `a`         | opacity (alpha) |
| ----------     | ----------- | --------        |
| `HSVA`         | `h`         | hue             |
| `HSVA`         | `s`         | saturation      |
| `HSVA`         | `v`         | value (chroma)  |
| `HSVA`         | `a`         | opacity (alpha) |

For more information on these color systems, see [here](https://en.wikipedia.org/wiki/RGBA_color_model) and [here](https://en.wikipedia.org/wiki/HSL_and_HSV).

For both representations, all components are 32-bit floats in `[0, 1]`. The **alpha** component is also called **opacity**, which the inverse of *transparency*. An alpha value of 1 means the color is fully opaque, a value of 0 means it is fully transparent, making it invisible.

### Converting Colors

We can freely convert between `RGBA` and `HSVA`. To do this, we use two purpose-built functions `rgba_to_hsva` and `hsva_to_rgba`:

```cpp
auto rgba = RGBA(// ...
auto as_hsva = rgba_to_hsva(rgba);
auto as_rgba = hsva_to_rgba(hsva);
assert(rgba == as_rgba) // true
```

### Color to Hexadecimal

Lastly, mousetrap offers a function to convert `RGBA` to its html-color-code. This code is a a string of the form `#RRGGBB`, where `RR` is the red, `GG` the green, and `BB` the blue component in hexadecimal. For example, the color `RGBA(1, 0, 1, 1)` would have the html-code `#FF00FF`, where the alpha component was ommitted. Using `html_code_to_rgba` and `rgba_to_html_code`, we freely convert between color and html-code.

For example, if we wanted to create an `Entry` that allows the user to past a color code in order to enter a color, we could do the following:

```cpp
auto html_code_entry = Entry();
html_code_entry.connect_signal_activate([](Entry* entry){
    auto rgba_maybe = RGBA(0, 0, 0, 0);
    if (is_valid_html_code(entry->get_tet(), rgba_maybe))
        std::cout << "HTML Color: " << rgba_maybe.r << " " << rgba_maybe.g << " " << rgba_maybe.g << std::endl;
})
```

If parsing was succesfull, `is_valid_html_code` will assign its second argument, `rgba_maybe` above, with the parsed color. This is safer than calling `html_code_to_rgba` directly, as it allows use to filer potentially malformatted strings caused by user error.

---

## Images

Now that we know how to handle colors, we continue onto images. In general, **images are a 2-dimensional matrix of colors**. Each element in the matrix is called a **pixel**. An image of size 400x300 will have a 400 * 300 = 120000 pixes. Each pixel is a color in `RGBA` format.

Images are represented by the \link mousetrap::Image `Image` class\endlink. This class is not a widget or signal emitter, it is simply a way to manage memory (in the form of a pixel matrix) in RAM. If we want to show an image on screen, we need the help of other widgets.

### Creating an Image

#### Loading an Image from Disk

Most commonly, we will want load an image from an already existing file. This can be achieved with `Image::create_from_file`, which takes the path to an image file as a string:

```cpp
auto image = Image();
image.create_from_file("/path/to/image.png");
```

#### Supported Image Formats

The following image formats are supported:

| Format Name             | File Extensions |
|-------------------------|-----------------|
| PNG                     | `.png`  |
| JPEG                    | `.jpeg` `.jpe` `.jpg`  |
| JPEG XL image           | `.jxl`  |
| Windows Metafile        | `.wmf` `.apm`  |
| Windows animated cursor | `.ani`  |
| BMP                     | `.bmp`  |
| GIF                     | `.gif`  |
| MacOS X icon            | `.icns`  |
| Windows icon            | `.ico` `.cur`  |
| PNM/PBM/PGM/PPM         | `.pnm` `.pbm` `.pgm` `.ppm`  |
| QuickTime               | `.qtif` `.qif`  |
| Scalable Vector Graphics | `.svg` `.svgz` `.svg.gz`  |
| Targa                   | `.tga` `.targa`  |
| TIFF                    | `.tiff` `.tif`  |
| WebP                    | `.webp`  |
| XBM                     | `.xbm`  |
| XPM                     | `.xpm`  |

It is recommended to use `Image` only for raster-based file types. For vector-graphics based types like `.ico` and `.svg`, mousetrap offers another more specialized class, called `mousetrap::Icon`. We will learn how to use `Icon` [in the next chapter](08_os_interface.md). 

#### Creating an Image from Scratch

Sometimes we want to fill an image with our own custom image data. For this, we use `Image::create`, which allocates an image of a given size and fills each pixel with the a color supplied as an optional argument. For example, the following allocates and image of size 400x300, then sets every pixel to red (`RGBA(1, 0, 0, 1)`):

```cpp
auto image = Image();
image.create(400, 300, RGBA(1, 0, 0, 1));
```

If unspecified, the image will be filled with `RGBA(0, 0, 0, 0)`, making it appear fully transparent.

### Saving an Image to Disk

We use `Image::save_to_file` to store an image, it will save the image data to the given path, automatically determining the correct file type based on the extension given. For example, using the path `/resources/save_image.png` will store the image as a PNG image.

### Modifying an Image

An all-red image will usually not be very useful, we will want to fill the image with custom data, possibly algorithmically. 

To overwrite a single pixel, we use `Image::set_pixel`, which takes as its first argument the pixels coordinate, then a color.

For image of size `n x m`, that is a width of `n` and height of `m`, the x-coordinate of a pixel has to be inside  `[0, n)`, meaning (`0`, `1`, ..., `n-2`, `n-1`), while the y-coordinate is in `[0, m)`, meaning (`0`, `1`, ..., `m-2`, `m-1`).

Similarly, we can access any pixel using `Image::get_pixel`, which only takes the pixels coordinates. If the coordinates are out of range, a soft warning will be printed and `RGBA(0, 0, 0, 0)` will be returned.

### Scaling

To change an images size, we have two options: **scaling**  the image and **cropping** it. These operations work identical to those in common image-manipulation programs such as Gimp or Photoshop.

To scale an image, we call `Image::as_scaled`. This functions returns a new image, it does not modify the original image. For example, scaling our 400x300 image to 800x600:

```cpp
auto image = // ... 400x300 image
auto scaled = image.as_scaled(800, 600);
```

Only `scaled` will be of size `800x600`, `image` has not changed.

#### Interpolation 

When scaling, we have a choice of scaling algorithm, which fills in newly generated pixels through **interpolation**. Mousetrap offers four interpolation types, supplied by the \link mousetrap::Image::InterpolationType enum `InterpolationType`\endlink. Below, we see how each type affects the final image, where the image labeled with `1x` is the original image with a resolution of 10x10 pixels.

\image html interpolation_nearest.png "InterpolationType::NEAREST"
<br>
\image html interpolation_bilinear.png "InterpolationType::BILINEAR"
<br>
\image html interpolation_hyperbolic.png "InterpolationType::HYPERBOLIC"
<br>
\image html interpolation_tiles.png "InterpolationType::TILES"
<br>

The main difference between `BILINEAR` and `HYPERBOLIC` is that of performance, `HYPERBOLIC` offers superior smoothing, but does so at about 1.5 times the speed when compared to `BILINEAR` interpolation, meaning `HYPERBOLIC` is about 50% slower. If no interpolation type is specified when calling `as_scaled`, `TILES` will be chosen by default.

### Cropping

We crop an image using `Image::as_cropped`. Similar to `as_scaled`, this functions returns a newly allocated image, it does not modify the original image.

`as_cropped` takes 4 arguments, the new width and height, and the x- and y-**offset**. The offset specifies which pixel is used as the new top-left coordinate of the cropped image. This offset can be negative, we can also specify a new resolution greater as that of the current image.  Any newly allocated space that is not part of the original image will be filled with `RGBA(0, 0, 0, 0)`. Cropping like this is often called "Resize Canvas" in common image manipulation applications.

### Flipping

Lastly we have `Image::as_flipped` which flips the image along the x- and/or y-axis. Just like before, `as_flipped` returns a newly allocated image and does not modify the original. It takes two booleans, indicating along which axis the image should be flipped:

```cpp
auto image = Image(//...
auto horizontally_flipped = image.as_flipped(true, false);
auto vertically_flipped = image.as_flipped(false, true);
```

### Image Data

While `mousetrap::RGBA` uses 32-bit float for each component, for an entire image this would take too much spacy in memory. Instead, mousetrap uses the `RGBA8888` format, which allocates 8 bits per color component. The components are stored in-line, in row-major order. Consider the following 2x2 image as an example:

```
RGBA(1.0, 1.0, 1.0, 1.0), RGBA(0.75, 0.75, 0.75, 1.0)
RGBA(0.5, 0.5, 0.5, 1.0), RGBA(0.0, 0.0, 0.0, 1.0)
```

This image is stored in memory as follows:

```
255 255 255 255 191 191 191 255 128 128 128 255 0 0 0 255
```

We acquire these values by multiplying each `RGBA` component with 255, rounding to the nearest integer, then storing each pixel as 4 sequential 8-bit unsigned integers. 

We rarely have to interact with the raw memory, as `get_pixel` and `set_pixel` automatically do the conversion and pixel position for us. If we do have to handle the raw memory, we can access it using `Image::data`. For an image of size `n*m`, `data` will point to a C array with `n*m*4` 8-bit unsigned integers (`unsigned char`), where `n` is the image width, `m` is the image height.

---

## Displaying Images

Now that we know how to load and manipulate images in memory, we most likely will want a way to display them. We've already seen a widget capable of this: `ImageDisplay`. So far we have been loading the image straight from the disk, however `ImageDisplay` is also able to be created from a `Image` directly. To do this, we supply our image as the only argument to `ImageDisplay::create_from_image`.

`ImageDisplay` copies the contents of the image to the graphics card, which is read-only. This means that if we change the original `Image`, `ImageDisplay` **will not change**. To update the `ImageDisplay` and thus the image on screen, we need to call `create_from_image` again. 

By default, `ImageDisplay` will expand according `Widget::set_expand`. All widgets are expanded using linear interpolation, which may blur images in an undesired way. To make sure `ImageDisplay` is always at the correct resolution and displays an image 1-to-1 (that is 1 pixel of the image is exactly 1 pixel on the screen), we can use the following trick:

```cpp
auto image = Image();
image.create_from_file( //... load image of size 400x300

auto image_display = ImageDisplay();
image_display.create_from_image(image);
image_display.set_expand(false); // prevent expansion
image_display.set_size_request(image_display.get_size())); // size request to scale 400x300 exactly
```

where `ImageDisplay::get_size` returns the resolution of the image the `ImageDisplay` was created from.

This way `image_display` will always follow its size-request, forcing it to always be exactly 400x300 pixels on screen. 

`ImageDisplay::create_from_image` is an extremely costly operation and would be insufficient to, for example, fluently display a 60 fps animation. Ẁe would have to call `ImageDisplay::create_from_image` every frame, which is not feasible given how performant this process tends to be.

In situations like this, we should instead use a custom render widget to display the image as an OpenGL **texture**. We will learn how to do this in the [chapter on native rendering](09_opengl.md).

---

## Sound

Mousetrap supports audio playback. Sound can be streamed from two location, **from memory** and **from disk**. We use `Sound` to stream sound from memory, and `Music` to stream from the disk directly.

### SoundBuffer

Audio data in memory is managed by `mousetrap::SoundBuffer`, which is conceptually similar to `Image` in that it only manages the raw data in memory, it can not "display" (play) the data.

`SoundBuffer` provides many of the same functions as `Image`, for example, we can load a sound from a file using `SoundBuffer::create_from_file`, which supports the following file formats:

#### Support Sound Formats

| Format Name                 | File Extensions |
|-----------------------------|-----------------|
| Waveform                    | `.wav`          |
| OGG/Vorbis                  | `.ogg` |
 | FLAC                        | `.flac` |

Due to licensing issues, `.mp3` is not supported.

#### Saving an Image to Disk

Just like with `Image`, `SoundBuffer::save_to_file` will store our audio data on the disk, automatically choosing the correct file format just like with `Image::save_to_file`.

#### Manipulating Sound

\todo Manipulating sound is not yet implemented

### Sound Playback from Memory

The analog of `ImageDisplay`, which displays image data, is `Sound`, which "displays" audio data by playing it over the end-users speakers. 

By default, playback will start at the beginning of the sound buffer, play the entire sound, then stop. We trigger this using `Sound::play`.

`Sound::pause` pauses the playback, after which it can be resumed by calling `play` again. If `Sound::stop` is called instead of `pause`, a subsequent use of `play` will make it so playback starts at the beginning, as opposed to where we last left off.

```cpp
// load buffer
auto buffer = SoundBuffer();
buffer.create_from_file(// ...

// create playback device
auto sound = Sound();
sound.create_from_buffer(buffer);

// play sound once
sound.play();
```

### Playback Offset and Loops

We can manually change at what part of the buffer the sound starts playing by called `Sound::set_playback_position`, which takes a float in `[0, 1]` as a fraction of the sounds length.

For example, if we have a sound that is 30s long, and we want to start the sound at 13s in, we can do the following:

```cpp
auto buffer = SoundBuffer();
buffer.create_from_file(// ...

auto sound = Sound();
sound.create_from_buffer(buffer);

sound.set_playback_position(seconds(13) / sound.get_duration()));
sound.play();
```

Where we used `mousetrap::Time` to calculate the fraction we need for `set_playback_position`.

Lastly, we can choose to loop the `Sound` with `Sound::set_should_loop`. If set to `true`, it will make it such that when playback reaches the end of the buffer, it jumps back to the very first sample and continues playing this way until `stop` is called.

### Sound Playback from Disk

For long tracks, such as multi-minute long pieces of music, `SoundBuffer` would take a large amount of space in RAM. To avoid this, mousetrap offers a the option to play music directly from the disk, minimizing RAM usage. This is possible through `mousetrap::Music`.

To create a `Music`, we use `Music::create_from_file`. The number of supported file formats is identical to those listed above.

Once we created the `Music` instance, we can start, pause and stop just like with `Sound`. 

`Musc` offers an additional feature: **loop region**. A loop region is a range of time that designate part of a music track, such that, if `set_should_loop` is set to `true` and the current playback position reaches the end of the loop region, it jumps to the start of the loop region. By default, the loop region is the entire duration of the file.

We can set the loop region like so:

```cpp
auto music = Musci(// load music file with duration of 4mins
music.set_should_loop(true);

// designate 1:00 to 2:30 as loop region
music.set_loop_region(seconds(60), second(120 + 30));
music.play();
```

This will start playback at 1:00 and continuously loop only this region.

### Volume, Pitch Control, Panning

For both `Music` and `Sound`, we can control the volume of playback with `set_volume`, which takes a floating point factor. A factor of 1 means no change in volume as compared to that of the file, `2` means twice as loud, `0.5` means half as loud.

Similarly, we can control the pitch of playback using `set_pitch`, which, just like `set_volume`, takes a factor.

Lastly, again for both `Sound` and `Music`, mousetrap offers the option of changing the **spacial position** of playback, often also called **panning**. This will simulate moving the sound in 3d space: left-to-right, top-to-bottom, near-to-far for the x, y and z axis respectively. 

For example, if we wanted to move the sound such that it is left of the listener, we would do:

```cpp
music.set_space_position(
 -1,  // x: left-right
 +0,  // y: down-up
 +0   // z: near-far
);
```

With these and loop regions, we have some control and can customize music tracks without having to change the file itself.
