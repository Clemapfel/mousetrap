# Chapter 6: Image & Sound

In this chapter we will learn:
+ How to use colors in mousetrap
+ How to load, store, modify, and display 2d images
+ How to load, store, and play audio

## Introduction

 mousetrap features a robust and fully featured image-processing interface, which features many of the relevant image-modifying functions a graphic editor, such as an image manipulation app,  Before we can talk about images, we need to talk about individual colors.

## Colors

\todo

## Images

In general, images are a 2-dimensional matrix of colors. Each element in the matrix is called a **pixel**. An image of size 400x300 will have a 400 * 300 = 120000 pixes. Each pixel is a color in RGBA format, meaning it has 4 components, as state above.

Images are represented by the  `mousetrap::Image` class. This class is not a widget or signal emitter, it is simply a way to manage memory in the form of a pixel matrix in RAM. We need the help of other classes if we want to show the image on screen.

### Creating an Image

#### Loading an Image from Disk

Most commonly, we will want load an image into RAM from an already existing file. This can be achieved with `Image::create_from_file`, which takes the path to an image file as a string:

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

#### Creating an Image from Scratch

Sometimes we want to fill an image with our own custom image data. For this, we use `Image::create`, which allocates an image of a given size, and fills each pixel with the a color supplied as an optional argument. For example, the following allocates and image of size 400x300 and fills every color with red (`RGBA(1, 0, 0, 1)`):

```cpp
auto image = Image();
image.create(400, 300, RGBA(1, 0, 0, 1));
```

If unspecified, the image will be filled with `RGBA(0, 0, 0, 0)`.

### Saving an Image to Disk

We can call `Image::save_to_file`, which automatically stores the image as a file to the given path. The file format will be automatically determined by the extension of the path, so a path `/resources/save_image.png` will store the image as a PNG image. Depending on the file format, this is a very costly operation.

### Modifying an Image

An all-red image will usually not be very useful, we most likely will want to fill the image with custom data, possibly algorithmically. 

To modify a pixel, we use `Image::set_pixel`, which takes the pixels coordinate. For image of size `nxm`, that is a width of `n` and height of `m`, the x-coordinate of a pixel is in `[0, n)`, meaning (`0`, `1`, ..., `n-2`, `n-1`)`, while the y-coordinate is in `[0, m)`, meaning (`0`, `1`, ..., `m-2`, `m-1`).

Similarly, we can access any pixel using `Image::get_pixel`. If the coordinates are out of range, a soft warning will be printed and `RGBA(0, 0, 0, 0)` will be returned.

### Whole-Image Transforms

To change an images size, we have two options: **scaling**  the image and **cropping**. These operations work identical to those in common image-manipulation programs such as Gimp or Photoshop.

##### Scaling

To scale an image, we call `Image::as_scaled`. This functions returns a new image, it does not modify the original image. For example, scaling our 400x300 image to 800x600:

```cpp
auto image = // ... 400x300 image
auto scaled = image.as_scaled(800, 600);
```

Only `scaled` will be of size `800x600`, `image` has not changed.

When scaling, we have a choice of scaling algorithm, which chooses what information to fill pixels with through **interpolation**. Mousetrap offers 4 interpolation types, as the enum `InterpolationType`:

+ `InterpolationType::NEAREST`: nearest neighbor, no interpolation
+ `InterpolationType::BILINEAR`: linear scaling
+ `InterpolationType::TILES`: nearest neighbor for enlargment, linear for shrinking
+ `InterpolationType::HYPERBOLIC`: hyperbolic interpolation

\todo example images

By default, the scaling mode is `TILES`.

##### Cropping

We crop an image using `Image::as_cropped`. Similar to `as_scaled`, this functions returns a newly allocated image, it does not modify the original image.

`as_cropped` takes 4 arguments, the new width and height, and the x- and y-**offset**. The offset specifies which pixel is used as the new top-left coordinate of the cropped image. This offset can be negative, and we can specify a sizer greater as that of the current image. This is similar to the "resize canvas" feature of many image editing applications. Any newly allocated space that is not part of the original image will be filled with `RGBA(0, 0, 0, 0)`.

\todo example image

##### Flipping

Lastly we have `Image::as_flipped` which flips the image along the x- and/or y-axis. Just like before, `as_flipped` returns a newly allocated image and does not modify the original. It takes two arguments, booleans indicating along which axis the image should be flipped. 

```cpp
auto image = Image(//...
auto horizontally_flipped = image.as_flipped(true, false);
auto vertically_flipped = image.as_flipped(false, true);
```

### Image Data

While `mousetrap::RGBA`, the class used to express color has four 32-bit components, for an entire image this would take too much spacy in memory. Instead, mousetrap uses the `RGBA8888` format, which allocates 8 bits per color component. The components are stored in-line, in row-major order. Consider the following 2x2 image as an example

```
RGBA(1.0, 1.0, 1.0, 1.0), RGBA(0.75, 0.75, 0.75, 1.0)
RGBA(0.5, 0.5, 0.5, 1.0), RGBA(0.0, 0.0, 0.0, 1.0)
```

This image is stored in memory as follows:

```
255 255 255 255 191 191 191 255 128 128 128 255 0 0 0 255
```

We get these by multiplying each `RGBA` component with 255, rounding, then storing each pixel as 4 sequential 8-bit integers. 

We rarely have to interact with the raw memory, as `get_pixel` and `set_pixel` automatically do the conversion and position of pixels for us. If we do have to handle the raw memory, we can access it using `Image::data`, which returns a `void*`. For an image of size `n*m`, `data` will point to a C array with `n*m*4` 8-bit unsigned integers (`unsigned char`), where `n` is the image width, `m` is the image height.
We can also get the number of elements in `data` using `Image::get_data_size`.

### Displaying Images

Now that we know how to load and manipulate images in memory, we most likely will want a way to display them to the user in some way. We've already seen a widget capable of this: `ImageDisplay`. So far we have been loading the image straight from the disk, however `ImageDisplay` also takes an image in memory as the argument to `ImageDisplay::create_from_image`.

`ImageDisplay` copies the contents of the image to the graphics card and is read-only. This means that if we change our `Image`, `ImageDisplay` will not change. To update the `ImageDisplay` with the newest version of our image, we need to call `create_from_image` again. The advantage of this is that if our original `Image` goes out of scope and is freed, `ImageDisplay` will be unaffected.

By default, `ImageDisplay` will expand according `Widget::set_expand`. Since this expansion uses the widget display engine, it does not follow the scaling mode we set when calling `Image::scale`. To make sure `ImageDisplay` is always at the correct resolution and displays an image 1-to-1 (that is 1 pixel of the image is exactly 1 pixel on the screen), we can use the following trick:

```cpp
auto image = Image();
image.create_from_file( //... load image of size 400x300

auto image_display = ImageDisplay();
image_display.create_from_image(image);
image_display.set_expand(false); // prevent expansion
image_display.set_size_request(image_display.get_size())); // size request to scale 1:1
```

With no expansion, the widget will always follow its size-request, which is the conceptual minimum size the widget can allocated on screen. We can get the actual resolution of the underelying image (as opposed to the size of the widget) using `ImageDisplay::get_size`.

\todo example image

### OpenGL Textures

`ImageDisplay::create_from_image` is an extremely costly operation and would be insufficient to, for example, fluently display a 60 fps animation. Ẁe would have to call `ImageDisplay::create_from_image` every frame, but the loading and updating of the widget would most likely take more than 1/60th of a section on most machines.

For purposes like this, we should use a custom render widget and display an OpenGL `Texture`. We will learn how to do this in the [chapter on native rendering](09_opengl.md).

---

## Sound

Mousetrap supports audio playback. Sound can be streamed from two location, **from memory** and **from disk**. For from-memory playback we use `SoundBuffer` for the data and `Sound` for playback, for from-disk playback we use `Music`, which has no data component as it directly streams from the hard drive.

### SoundBuffer

A sound in memory is managed by `mousetrap::SoundBuffer`, which is conceptually similar to `Image`, in that it only manages the raw data in memory.

It provides many of the same functions, for example we can load a sound from a file using `SoundBuffer::create_from_file`, which supports the following file formats:

#### Support Sound Formats

The following formats are support:

| Format Name                 | File Extensions |
|-----------------------------|-----------------|
| Waveform                    | `.wav`          |
| OGG/Vorbis                  | `.ogg` |
 | FLAC                        | `.flac` |

`.mp3` is not supported, due to copyright issues.

### Saving an Image to Disk

Just like with `Image`, `SoundBuffer::save_to_file` will store our audio data on the disk, automatically choosing the correct file format based ont the extension supplied in the path that is the functions argument.

#### Manipulating Sound

\todo Manipulating sound is not yet implemented

#### Sound Playback

The analog of `ImageDisplay`, which displays image data, is `Sound`, which "displays" audio data by playing it over the end-users speakers. 

By default, playback will start at the beginning of the sound buffer, play the entire sound, then stop. We can start this sequence using `Sound::play`.

`Sound::pause` pauses the playback, after which it can be resumed by calling `play` again. If `Sound::stop` is called instead of `pause`, a subsequent use of `play` will make it so playback starts at the beginning, as opposed to where we left of.

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
#### Playback Offset and Loops

We can manually change at what part of the buffer the sound starts playing by called `set_playback_position`, which takes a float in `[0, 1]`, where `0` is the first sample of the audio data, `1` the very last. 
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

Lastly we can choose to loop the `Sound` with `Sound::set_should_loop`. If set to `true`, it will make it so that when playback reaches the end of the buffer, it jumps back to the very first sample and continues playing indefinitely, or until `pause`, `stop` or `Sound::set_should_loop(false)` is called.

### Sound Playback from Disk

For long tracks such a entire pieces of music, loading the entire file into RAM is impractical. For this purpose, mousetrap offers `Music`. This class takes a filename as a string, just like `SoundBuffer`, unlike it, however, it streams sound directly from the disk, making it suitable for large audio files.

In addition to the volume/pitch/position controls that `Sound` offser, `Music` allows a user to select a "loop region". This is a stretch of time through which the music will loop, when `set_should_loop` is enabled. We set this region by calling `set_loop_region`, which takes two time stamps, the start and end of the region in seconds.





