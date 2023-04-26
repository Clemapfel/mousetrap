# Chapter 6: Image & Sound

In this chapter we will learn:
+ How to represent colors in mousetrap
+ How to load, store, modify and display 2d images
+ How to load, store, and play audio, both from memory and from the disk

## Introduction

Mousetrap was originally part of a never-released app for frame-by-frame animation. With the seizing of development for 
that app, I decided to plug out its GUI engine, polish it and its documentation, and make it 
available publicly as mousetrap.

I note this back-story here because a lot of the mousetrap library was specifically designed for this purpose, mousetrap has robust image-processing features . Images can effortly be exchanged with both GTK4 and OpenGL component, making editing and displaying
images easy and - thanks to hardware accelleration - fast. 

Mousetraps origin makes it suited for apps about illustration, animation, and scientific image processing.

## Image

Images in RAM are managed by the `mousetrap::Image` class. This class is tied to the widget architecture and can only be used in the context of an application.

### Creating an Image

#### Loading an Image from Disk

Most commonly, we will want load an image into RAM to modify or display it. To do this we first instantiate image using its default constructor. This image will be of size 0x0, to fill it with data from the disk we call `Image::create_from_file`, which takes the path to an image as a string:

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
| Windows Metafile        | `.wmf` `.apm`  |
| Windows animated cursor | `.ani`  |
| BMP                     | `.bmp`  |
| GIF                     | `.gif`  |
| MacOS X icon            | `.icns`  |
| Windows icon            | `.ico` `.cur`  |
| JPEG XL image           | `.jxl`  |
| PNM/PBM/PGM/PPM         | `.pnm` `.pbm` `.pgm` `.ppm`  |
| QuickTime               | `.qtif` `.qif`  |
| Scalable Vector Graphics | `.svg` `.svgz` `.svg.gz`  |
| Targa                   | `.tga` `.targa`  |
| TIFF                    | `.tiff` `.tif`  |
| WebP                    | `.webp`  |
| XBM                     | `.xbm`  |
| XPM                     | `.xpm`  |

#### Creating an Image from Scratch

Sometimes we want to fill an image with our own custom image data. For this, using `Image::create`, we can allocate an image of a given size, filling each pixel with the supplied color. For example, the following allocates and image of size 400x300 and fills every color with red (`RGBA(1, 0, 0, 1)`):

```cpp
auto image = Image();
image.create(400, 300, RGBA(1, 0, 0, 1));
```

### Modifying an Image

Having created an all-red image, we probably want to modify it by setting individual pixels to a certain color. We can use `Image::get_pixel` to access a specific pixel and `Image::set_pixel` to replace that pixels color with the given color.

For an image of size `n*m`, that is an image with a width of `n` pixels and a height of `m` pixels, the top-left-most pixel has index `(0, 0)`, the top-right-most pixel `(0, n)`, the bottom-right-most pixel has index `(n, m)` and the the bottom-left-most pixel `(n, 0)`. In this way, an image is a just a matrix, where each element is a color, as opposed to a single number.

### Whole-Image Transforms

If we want to change in images size, we have two options: *scaling*  the image, and *cropping*. These operations work identical to those in common image-manipulation programs such as Gimp or Photoshop.

##### Scaling

To scale an image, we call `Image::as_scaled`. This functions returns a new image, it does not modify the original image. For example, scaling our 400x300 image to 800x600:

```cpp
auto image = // ... 400x300 image
auto scaled = image.as_scaled(800, 600);
```

When scaling, we have a choice of the scaling algorithm, or *interpolation*. Mousetrap offers 4 choices, represented by the values of enum `InterpolationType`:

+ `InterpolationType::NEAREST`: nearest neighbor, no interpolation
+ `InterpolationType::BILINEAR`: linear scaling
+ `InterpolationType::TILES`: nearest neighbor for enlargment, linear for shrinking
+ `InterpolationType::HYPERBOLIC`: hyperbolic interpolation

\todo example images

By default, the scaling mode is `TILES`.

##### Cropping

We crop an image using `Image::as_cropped`. Similar to `as_scaled`, this functions returns a newly allocated image, it does not modify the original image.

`as_cropped` takes 4 arguments, the new width and height of the cropped image, and the x- and y-**offset**. The offset sets which pixel is used as the new top-left coordinate of the cropped image. This offset can be negative, any space that does not have pixeldata will be filled with `RGBA(0, 0, 0, 0)`

\todo example image

##### Flipping

Lastly we have `Image::as_flipped` which flips the image along the x- and/or y-axis. Just like before, `as_flipped` returns a newly allocated image and does not modify the original. It takes two arguments, a boolean indicating along which axis the image should be flipped.

### Image Data

While the two color presentations present, `RGBA` and `HSVA` have each components as a 32-bit float, for image this format would take up an unreasonable amount of memory. Because of this, internally, image data is stored as row-major, 8-bit RGBA, meaning a color is represents as four, 8-bit component, with the red, green, blue and alpha component in that order, an pixels are stored row-first. For example, consider the following 2x2 image:

```
RGBA(1.0, 1.0, 1.0, 1.0), RGBA(0.75, 0.75, 0.75, 1.0)
RGBA(0.5, 0.5, 0.5, 1.0), RGBA(0.0, 0.0, 0.0, 1.0)
```

This image is stored in memory as follows:

```
255 255 255 255 191 191 191 255 128 128 128 255 0 0 0 255
```

We rarely have to interact with the raw memory, as `get_pixel` and `set_pixel` automatically do the conversion and position of pixels for us. If we do have to handle the raw memory, we can access it using `Image::data`, which returns a `void*`. For an image of size `n*m`, `data` be an array of `n*m*4` 8-bit unsigned integers (`unsigned char`). We can also get the number of elements in `data` by calling `Image::get_data_size`.

### Displaying Images

Now that we know how to load and manipulate images in memory, we most likely will want a way to display them to the user. We've already seen a widget capable of this `ImageDisplay`. So far we have been loading the image straight from the disk, however `ImageDisplay` also takes an image in memory as the argument to `create_from_image`.

By default, `ImageDisplay` will expand according `Widget::set_expand`. Expansion like this will scale the image automatically. If this is not desired, we tell `ImageDisplay` to display the image 1:1 (that is, 1 pixel on screen is equivalent to 1 pixel of the image data) by setting its size request to the images size, and disabling expansion in both dimensions:

```cpp
auto image = Image();
image.create_from_file( //... load image of size 400x300

auto image_display = ImageDisplay();
image_display.create_from_image(image);
image_display.set_expand(false); // prevent expansion
image_display.set_size_request(image_display.get_size())); // size request to scale 1:1
```

\todo example image

Note that once an `ImageDisplay` is created, changing the image does not change the image display. `ImageDisplay` automatically copies the image data, usually to the graphics card, and it cannot be modified afterwards. If we want to change the image `ImageDisplay` displays, we need to call `create_from_image` again.

`ImageDisplay` is enough for most purposes, however we will learn about a more low-level and flexible way of displaying images using OpenGL textures in the next chapter.

## Sound

Mousetrap supports audio playback. This can happen from two locations, streamed from memory (RAM), or streamed from the disk

### SoundBuffer

A sound in memory is managed by `mousetrap::SoundBuffer`, which is very similar to `Image` in its interface. We can load a sound from a file using `create_from_file`,
which similarly takes the path to the file as a string. Just like with `Image`, we can save the sound to disk using `save_to_file`.

#### Support Sound Formats

The following formats are support:

| Format Name                 | File Extensions |
|-----------------------------|-----------------|
| Waveform                    | `.wav`          |
| OGG/Vorbis                  | `.ogg` |
 | FLAC                        | `.flac` |

`.mp3` is not supported, due to copyright issues.

#### Manipulating Sound

\note As of v0.1.0, sound buffer data cannot be manipulated, this feature is planned for a future update

#### Sound Playback

The analog of `ImageDisplay`, which displays image data, is `Sound`, which plays sound data for the user. By default, sound will start at the beginning of the sound buffer, play the entire sound, then stop indefinitely. To trigger this behavior, we call `Sound::play`.

Similarly, `Sound::pause` pause the playback, after which it can be resume by calling `play` again. If `Sound::stop` is called instead of pause, when the sound resumes playing it will instead start from the beginning of the file.

\cpp_code_begin
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
\cpp_code_end


`Sound` offse

### Sound Playback from Disk

For long tracks such a entire pieces of music, loading the entire file into RAM is impractical. For this purpose, mousetrap offers `Music`. This class takes a filename as a string, just like `SoundBuffer`, unlike it, however, it streams sound directly from the disk, making it suitable for large audio files.

In addition to the volume/pitch/position controls that `Sound` offser, `Music` allows a user to select a "loop region". This is a stretch of time through which the music will loop, when `set_should_loop` is enabled. We set this region by calling `set_loop_region`, which takes two time stamps, the start and end of the region in seconds.





