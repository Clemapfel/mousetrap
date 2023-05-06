# Chapter 7: Operating System Interface

In this chapter, we will learn:
+ How to properly do logging
+ How to copy/move/create/delete files 
+ How to access a files metadata
+ How to monitor a file changing
+ How to store arbitrary objects in a .ini file
+ How to use icons 
+ How to customize menu section layouts using icons

---

## Introduction: Logging

When shipping applications, stability is paramount. Nobody will use an app if it keeps crashing, especially if that crash may corrupt important files.

The best way to prevent crashing is to follow [proper testing procedures](https://www.globalapptesting.com/blog/software-testing). For a small team, it is inevitable that some things will eventually slip through the cracks. When an end-user comes to you with a problem or bug, they most likely will not be able to precisly describe the state of the application, and, depending on the user base, they may not be able to describe the problem at all.

This is where objective information about what exactly was happening is invaluable. **Logging** is the act of creating this information. Log info is usually saved to a file somewhere, this way when a crash or bug occurrs, we can simply ask the user to hand over the log file and diagnose it ourself.

When working through past chapters, you may have already encountered some logging information. For example, if we try to do the following:

```cpp
auto box = Box();
box.push_back(box);
```

We get the following message printed to our console:

```
(example_target:45245): mousetrap-CRITICAL **: 16:44:27.065: In Box::push_back: Attempting to insert widget into itself. This would cause an infinite loop
```

We cannot insert a widget into itself. Mousetrap prevented this action and printed a log message to inform us of this. 

### Log Message Properties

Let's go through each part of the message, one-by-one

#### Application ID

First we have `(example_target:45245)`, this is the identification of our application. During normal runtime, this information may not be very useful. Once the log is stored to a system-level log file, however, many applications may log at the same time. Knowing which log message came from which application is integral in that context.

#### Log Domain

Next we have `mousetrap-CRITICAL`. The word before the `-` is the **log domain**. This is a developer-defined identification that should show which part of the application or library caused the logging message. Pre-defined domains include `mousetrap` for mousetrap-specific warnings, `GTK` for GTK-based warning, `GLib`, `Gio`, `Gdk`, etc. As a user of mousetrap, you should choose a new log domain. For example, if we create a new application "Foo: Image Viewer", we should choose a descriptive log domain, such as `foo_image_viewer`, or `foo`.

#### Log Levels

`CRITICAL` is the **log level**. Log levels are one of the following:

+ `DEBUG` is for messages that should not appear when the end-user uses the application, they are **only meant for developers** themself
+ `INFO` is for **benign status updates**, for example `succesfully opened file at (...)`. These message will not be stored or printed to console, unless we specifically request the logging suite to do so
+ `WARNING` is for messages that should attempt to **prevent unstabled behavior** before it occurrs, for example when attempting to close a file while it is still being written to, a warning should be printed and the closing should be postponed until writing is done
+ `CRITICAL` are for errors. In many langauges, an error means the end of runtime, which is unacceptable for GUI applications. If the application throws an C++ exception, for example, that exception should be caught and printed as a `CRITICAL` log message. Software designers should take care that the application cannot crash under any circumstances
+ `FATAL` is the most severe log level and should only be used as an absolute last resort. Once a `FATAL` warning is printed, the application exits immediately

We see that our message from before was designated as `CRITICAL`. This is because usually adding a widget to itself would effectively deadlock the application, ending runtime, making it a severe issue that should be fixed and not be able to happen during runtime of a finished app.

#### Time Stamp

Next, we have `16:44:27.065`, this is the **time stamp** of the log message, with millisecond precision. When stored the log to a file, the current date and year is also appended to the time stamp.

#### Message

Lastly we have the **log message**. Log messages should contain the name of the function they are called from, for example, in the above message it says `In Box::push_back`, telling developers that the error happened in that function. This makes debugging easier.

Messages should not end with a `\n` (a newline), as one is automatically appended to the end of the message.

## Printing Messages

All interaction with the log is handled by \link mousetrap::log `mousetrap::log`\endlink, which is a singleton class. This means it cannot be instantiated, has no mutable members, and all functions of `log` are static.

We have one function for each log level, called `log::debug`, `log::info`, `log::warning`, `log::critical` and `log::fatal`. Each of these functions takes two arguments: the log message and the log domain, in that order. Application id, timestamp, etc. are automatically added. The message should be a plain string, it does not respect html or css properties and should not contain any control characters, such as `\t` or `\n`.

As mentioned before, messages of level `debug` and `info` are only printed if we speficially request so. We enabled these on a per-log-domain basis using `set_surpress_info` and `set_surpress_debug` respectively. For example, if our log domain is `foo`:

```cpp
// define custom domain
const LogDomain FOO_DOMAIN = "foo";

// print `INFO` level message but nothing will happen because it is surpressed by default
log::info("Surpressed message", FOO_DOMAIN);

// enable `INFO` level messages
log::set_surpress_info(FOO_DOMAIN, false);

// message will be printed
log::info("No longer surpressed message");
```

Shipped applications, that is applications intended for end-users that are no longer under development, should surpress all `DEBUG` and `INFO` messages. The should only be enabled during development.

### Logging to a File

If the operating system is Linux, many log message will be written to the default location, usually `/var/log`. On other operating systems, message may not be stored at all.

Regardless of OS, we can forward all logging, including that of mousetrap itself, to a file using `log::set_file`, which takes the file path as a string. If the file already exist, it will be appended to (as opposed to being overriden). If the file does not yet exist it will be created.

When stored to a file, logging message will have a different format that may or may not list additional information. The philosophy behind this is that it is better to log as much information as possible and use second party software to filter it, as opposed to missing crucial information for the sake of brevity.

```cpp
const LogDomain FOO_DOMAIN = "foo";
log::set_file("example_log.txt");
log::warning("In example.main: Example Message", FOO_DOMAIN);
```

Will add the following lines to a `example_log.txt`

```
[23-05-06 23:01:34,920]: In example.main: Example Message
	GLIB_DOMAIN foo
	MOUSETRAP_LEVEL WARNING
	PRIORITY 4
```
---

## File System

Most GUI applications are centralized around modifying files. Be it a text or image editor exporting files, or a video game creating a save file. Because of this, mousetrap offers a robust, operating-system-agnostic way of interacting with the disk.

First some vocabulary. There are two kinds of objects in a filesystems: **files** contain arbitrary data, while **directories** contain other files and/or other directories. We also call a directory a **folder**. 

Examples for "true" files include `.png`, `.txt`, `.cpp` text files, shared libraries, binaries, or executable files.

A **path** is a string where each folder name is followed by a `/`, (or `\` on windows, though this should be avoided). Examples include `/var/log`, `~/Desktop`, etc.

A **uri** (universal resource identifier) is another way to express the location of the file. It follows a [strict scheme](https://en.wikipedia.org/wiki/Uniform_Resource_Identifier), which is followed by most internet browsers for cross-system file sharing or connecting to the internet.  

### File Descriptor

When querying info about a file, we use \link mousetrap::FileDescriptor `mousetrap::FileDescriptor`\endlink. This object is non-mutating, meaning it is incapable of changing anything about the actual file on the disk. In other words, `FileDescriptor` is read-only. This is important to realize

We create a file descriptor from a path:

```cpp
auto readonly = FileDescriptor();
readonly.create_from_path("/usr/Desktop/example.txt");
```

Where the argument to `create_from_path` will be automatically detected as an absolute path (a path beginning at the root of a system, usually `/` on unix, `C:` on windows). If it is not an absolute path, it will be prefixed with the applications runtime directory. For example if we create a file from path `"resources/image.png"`, and our application is located in `/usr/bin`, then the path will be treated as `/usr/bin/resources/image.png`.

`FileDescriptor` does not make sure the file provided by the path or uri exists, or that it is a valid file. Creating a descriptor from an invalid path or a path that does not point to a file or folder works just fine, and we'll not get any warning. To check wether a file descriptor points to a valid file or folder by using `FileDescriptor::exists`. 

To check whether the descriptor paints to a file or folder, we use `FileDescriptor::is_file` and `FileDescriptor::is_folder` respectively. If the object pointed to by `FileDescriptor` does not exist, both of these functions will return false.

`FileDescriptor` allows us to query a variety of information about the file, including but not limited to:

+ `get_path` returns the location of the file as a path, eg. `~/Desktop/example.txt`
+ `get_uri` returns the location as a uri, eg. `file://~/Desktop/example.txt`
+ `get_file_extension` returns the extension, eg. `.txt`
+ `is_executable` checks whether the file is an executable, eg. `false`
+ `get_content_type` returns the [MIME type](https://en.wikipedia.org/wiki/Media_type), eg. `text/plain`

For additional more involved information, we can use `query_info`, which takes an **attribute identifier**. A list of identifiers can be found [here](https://docs.gtk.org/gio/index.html#constants), though not all of these may have a value, depending on the file type and system. If the value is unknown, an empty string will be returned.

If the file is a folder, we can use `get_children` to get all files and/or directories inside the folder. `get_children` takes a boolean as its only argument, which specifies whether it should list all children recursively or not.

---

## Manipulating the Disk

`FileDescriptor` is not mutating, it can only tells us information about a file, not modify them in any way. Instead, we use functions of `file_system`, which, like `log`, is a singleton. It provides all the standard file operations such as creating, deleting, copying or moving files. 

### Creating Files

`file_system::create_file_at` creates a file at a given location. It takes a file descriptor as its argument. If `should_replace`, its second argument, is set to false and the file already exists, no operation will be performed

```cpp
if (file_system::create_file_at(FileDescriptor("/absolute/path/to/file.txt", false))
    // open contents of file here
```

`file_system::create_directory_at` performs a similar action, except it creates a directory instead of a file.

### Deleting Files

To permanently delete a file, we use `file_system::delete_at`, which takes a file descriptor as its argument. This immediately deletes the file making it unable to be recovered. If we want to keep that option open, we instead use `file_system::move_to_trash`.

```cpp
auto to_delete = FileDescriptor("/path/to/delete/file.txt");
if (not file_system::move_to_trash(to_delete))
    log::warning("In example: Unable to delete file at `" + to_delete.get_path() + "`");
```

### Moving / Copying File

To move a file from one location to another, we use `file_system::move`. If we want to copy a file or directory instead of moving it, we use `file_system::copy`.

```cpp
auto from = FileDescriptor("/path/from/file.txt");
auto to = FileDescriptor("/different_path/to/file.txt");
if (not file_system::move(from, to))
    log::warning("In example: Unable to move file from `" + from.get_path() + "` to `" + to.get_path() + "`");
```

### Changing File Properties

\todo this feature is not yet implemented

### Accessing & Changing a files content

\todo this feature is not yet implemented. Use [`std::fstream`](https://en.cppreference.com/w/cpp/io/basic_fstream) instead.

---

### Monitoring File Changes

Often, when writing a GUI, we want the graphical interface to reflect the contents of a file on the disk. A good example would be a text editor. We can of couse modify the file from inside our own application. If the file is modified by something else, such as another application, our application should reflect this change. This is made possible by `FileMonitor`.

`FileMonitor` cannot be created directly, instead we need to first create a `FileDescriptor`, then cal `FileDescriptor::create_monitor`. 

`FileMonitor` works similar to a signal emitter, to register a callback to be called when the file changes, we use `on_file_changed`. This callback has the signature `(FileMonitorEvent event, const FileDescriptor& self, const FileDescriptor& other, (Data_t)) -> void`, where

+ `event` is a `FileMonitorEvent`, describing the type of action performed, see below
+ `self` is a descriptor pointing to the file that was modifier
+ `other` is a descriptor usually not pointing to any file, unless the event was `FileMonitorEvent::MOVED_IN` or `FileMonitorEvent::MOVED_OUT` , in which case it will point to whatever file was moved into or out of the directory which `self` points to
+ `Data_t` is any arbitrary data

The following monitor events are supported:

| `FileMonitorEvent` | Meaning | value of `self`          | value of `other`   |
|--------------------|---------|--------------------------|--------------------|
|`CHANGED` | File was modified in any way | modified file            | none               |
|`DELETED` | File was deleted | monitored file or folder | deleted file       |
| `CREATED` | File was created | monitored file or folder | newly created file |
| `ATTRIBUTE_CHANGED` | File metadata changed | changed file             | none |
| `RENAMED` | Files name changed | changed file             | none |
|`MOVED_IN` | File was moved into self | monitored folder         | moved file |
|`MOVED_OUT` | File was moved out of self | monitored folder         | moved file | 

For example, if we want to trigger an action anytime a file `/path/to/file.txt` is modified, we can do so like so:

```cpp
auto to_watch = FileDescriptor("/path/to/file.txt"); // equivalent to .create_from_path
auto monitor = to_watch.create_monitor();
monitor.on_file_changed([](FileMonitorEvent event, const FileDescriptor& self, const FileDescriptor&){
    if (event == FileMonitorEvent::CHANGED)
        std::cout << "file at `" << self.get_path() << "` changed" << std::endl;
});
```

---

## Glib Keyfiles

For data like images or sound, we have methods like `Image::save_to_file`, or `SoundBuffer::save_to_file` to store them on the disk. For custom objects, such as the state of our application, we have no such option. While it may sometimes be necessary, for most purposes we do not need to create a custom file type, instead we can use the [**GLib keyfiles**](https://docs.gtk.org/glib/struct.KeyFile.html), whose syntax is heavily inspired by Windows `.ini` settings files.

Keyfiles are human-readable and easy to edit by humans, which makes them better suited for certain purposes when compared to [json](https://docs.fileformat.com/web/json/) or [xml](https://docs.fileformat.com/web/xml/) files.

Thanks to `mousetrap::KeyFile`, loading, saving and modifying key files is made easy and convenient.

### Glib Keyfile Syntax

In a KeyFile, every line is on of four types:

+ **Empty**, it has no characters or only control characters and spaces
+ **Comment**, it begins with `#`
+ **Group**, has the form `[group_name]`, where group name is any name
+ **Key**, has the form `key=value`, where key is any name and value is of a format discussed below

For example, the following is a valid key file:

```txt
# keybindings 
[image_view.key_bindings]

# store current file
save_file=<Control>s

# miscellanous config
[image_view.window]

# default window size
width=400
height=300

# default background color
default_color_rgba=0.1;0.7;0.2;1
```

Let's go through each line one-by-one:

+ `# keybindings`, and other lines starting with `#`, are parsed as comments
+ `[image_view.key_bindings]` designates the first **group**. Similar to how actions are named, we should use `.` as scoping and pick descriptive names
+ `save_file=<Control>s` is the first key-value pair. It is in group `image_view.key_bindings`, it has the key `save_file` and the value `<Control>s`, which is a **string** that can be parsed as a shortcut
+ `[image_view.window]` is our second group, its name is `image_view.window`
+ `width=400` and `height=300` are two key-value pairs in group `[image_view.window]`. Their values are **numbers**, `400` and `300`, which are integers
+ `default_color_rgba=0.1;0.7;0.2;1` is the third key-vaue pair in group `image_view.window`. It has the value `0.1;0.7;0.2;1` which is a **list of numbers**. As suggested by this entries key, this list should be interpreted as `RGBA(0.1, 0.7, 0.2, 1)`

Key-value pairs belong to the group that was last opened. Groups cannot be nested, they always have a depth of 1 and every key-value-pair has to be inside a group.

### Accessing Values

If the above keyfile is stored at `resources/example_key_file.txt`, we can access the values of the above named keys like so:

```cpp
auto file = KeyFile();
file.load_from_file("resources/example_key_file.txt");

// keybindings in group `image_view.keybindings`
// retrieved as string
std::string save_file_keybinding = file.get_value("image_view.key_bindings", "save_file");

// size of window
int width = file.get_value_as<int>("image_view.window", "width");
int height = file.get_value_as<int>("image_view.window", "height");

// background color
RGBA default_color = file.get_value_as<RGBA>("image_view.window", "default_color_rgba");
```

We see that the general syntax to get a value that should be interpreted as C++-type `T`, from a `KeyFile` file, is `file.get_value_as<T>("group_name", "key_name")`. `KeyFile` automatically parses the string inside the kefile on the disk, and returns a C++ type. Only the following types are supported:

| Type                     | Example Value                                | Format                           |
|--------------------------|----------------------------------------------|----------------------------------|
 | bool                     | `true`                                       | `true`                           |
| std::vector<bool>        | `{true, false, true}`                        | `true;false;true`                |
| int32_t                  | `32`                                         | `32`                             |
| std::vector<int32_t>     | `{12, 34, 56}`                               | `12;34;56`                       |
| uint64_t                 | `984`                                        | `948`                            |
| std::vector<uint64_t>    | `{124, 123, 192}`                            | `124;123;192`                    |
| float                    | `3.14`                                       | `3.14`                           |
| std::vector<float>       | `{1.2, 3.4, 5.6}`                            | `1.2;3.4;5.6`                    |
| double                   | `3.14569`                                    | `3.14569`                        |
| std::vector<double>      | `{1.123, 0.151, 3.121}`                      | `1.123;0.151;3.121`              |
| std::string              | `"foo"`                                      | `foo`                            |
| std::vector<std::string> | `{"foo", "lib", "bar"}`                      | `foo;lib;bar`                    |
| RGBA                     | `RGBA(0.1, 0.9, 0, 1)`                       | `0.1;0.9;0.0;1.0`                |
 | Image | `RGBA(1, 0, 1, 1), RGBA(0.5, 0.5, 0.7, 0.0)` | `1.0;0.0;1.0;1.0;0.5;0.5;0.7;0.0` | 

We can also access comments. To access the comment above a group name `group_name`, we use `KeyFile::get_comment_above("group_name")`. If the comment is above a key-value-pair with key `key`, we would use `KeyFile::get_comment_above("group_name", "key")`.

### Storing Values

We use `KeyFile::set_value<T>` to modify the value of a keyfile entry. Like before, this functions takes a group- and key-identifier, along with the C++ type `T` of the value. `T` is again restricted to the types listed above.

For example, setting the value of `default_color_rgba` in group `image_view.window`:

```cpp
file.set_value<RGBA>("image_view.window", "default_color_rgba", RGBA(1, 0, 1, 1));
```

To modify comments, we use `KeyFile::add_comment_above`, which just like before takes only a group id to modify the comment above a group, or both a group id and key to modify the comment above a key-value-pair.

When writing to the keyfile, only the object in memory is modified, not the file on disk. Indeed, we can create an empty keyfile without specifying a path at all. To save our changes to the disk, we need to call `save_to_file`.

---

## Icons

We've seen before how to load and display an image using `ImageDisplay`. A common application for this is to choose the picture displayed as the child of a button. In common language, this picture would often be called an **icon**.

In applications, we will have dozens of these images that will be used as visual labels for a variety of widgets. While it is possible to simply store all these images as `.png`s and load them manually, this is hardly very scalable. Furthermore, we often want to be able to swap out all such icons, for example to allow for monochromatic or larger-sized images for the visually impaired. 

A better way to do this is to use `mousetrap::Icon`, instead of images.

An icon is similar to an image file, though it usually does not contain pixel data. Icons can be loaded from `.png` files, but also allow `.svg` (vector graphics), and `.ico` (web browser icons). Icons support the exact same file formats as `Image`, though it is recommended to load icons from vector-graphics files, as opposed to raster-base. `Icon` is similar to a file descriptor, in that it does not contain any data and it is immutable, we cannot change an icon using `Icon`.

### Creating and Viewing Icons

The simplest way to create an icon is to load it as if it was an `Image`. If we have a vector graphics file `resources/save_icon.svg`, we can load it like so:

```cpp
auto icon = Icon();
icon.create_from_file("resources/save_icon.svg", 48);
```

Where `48` means the icon will be initialized with a resolution of 48x48 pixels. `Icon`s can only be square.

If we want to display this icon using a widget, we can use `ImageDisplay::create_from_icon`. Ths is similar to images, however because `Icon` is not rasted-based, the `ImageDisplay` will scale much more smoothly to any size, as no interpolation has to take place.

Unlike `Image`, we have some `Icon`-only ways to display the picture, which makes `Icon` and `Image` suited for different purposes.

### Using Icons in Menus

One unique application for `Icon` is that it can be **used as a menu item**. We use `MenuModel::add_icon` to create a menu item of this type:

```cpp
// declare action
auto action = Action("noop.action");
action.set_function([](Action*){
  // do something
});

// create icon
auto icon = Icon();
icon.create_from_file(// ...

// create menu
auto model = MenuModel();
auto submenu = MenuModel();
auto section = MenuModel();

// add icon 
section.add_icon(icon, action);

// add section
submenu.add_section("Buttons!", section, MenuModel::CIRCULAR_BUTTONS);

// display as MenuBar
model.add_submenu("Menu", submenu);
auto menubar = MenuBar(model);
```

\image html menu_model_with_icon.png

Note that when calling `add_section`, we provided an optional 3rd argument. This  is the **section format**, of type ` MenuModel::SectionFormat`. 

There are \link mousetrap::MenuModel::SectionFormat multiple styles\endlink of format, which all look slightly different. We should use sections styles **only when all items in the section are icons**, that is, all menu items in the section were created using `add_icon`.

With icons and section formats, we can add some style to our menus. This is rarely necessary but it is a nice option to have. 

Note that an item cannot have both a text label and an icon at the same time. If we really need this behavior, we would need to create `Box` with both an `ImageDispay` and `Label`, then add a menu item using `MenuModel::add_widget` instead..

### IconTheme

\todo this section is not yet complete