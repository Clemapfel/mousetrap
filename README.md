# Notes

---

# Canvas

## Canvas Settings
+ grid color
+ guide color
+ pixel cursor color

## Canvas Widgets
+ x-mirror, y-mirror line
+ frame scroll
+ layer scroll

---

# Layers
```cpp
struct Layer {
    PixelBuffer _pixel_buffer;
    std::string _name;
    BlendMode _blend_mode;
    float _alpha;
    bool _hidden;
    bool _locked;
}
```

## Layer View

+ new layer
+ delete layer
+ merge down
+ move layer up / down
+ flatten selected
+ flatten all
+ lock layer

---

# Palette

## Palette Settings
+ tile size
+ selection frame color
+ keybind index for colors 1 - 9 (0 is erase)

## Palette View
+ load default
+ save as default
+ load
+ save

+ lock palette
+ sort by hue

---

# Toolbox

## Toolbox Settings
+ keybinds for each tool
