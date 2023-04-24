# Chapter 7: Operating System Interface

In this chapter we will learn:
+ How to properly do logging
+ How to copy/move/create/delete files 
+ How to access a files content and metadata
+ How to monitor a file changing
+ How to store arbitrary objects in a .ini file
+ How to use and manage icons

## Introduction: Logging

When shipping applications, stability is paramount. Nobody will use an app if it keeps crashing, especially if that crash modifies files which may be corrupted.

The best way to prevent crashing is to follow [proper testing procedures](https://www.globalapptesting.com/blog/software-testing), but for a small team some things will eventually slip through the cracks. When a user comes to you with a problem, they most likely will not be able to precisly describe the state of the application, and, depending on the user base, they may not be able to describe the problem in detail at all.
This is where objective information about what exactly was happening is invaluable. **Logging** is the act of creating this information, which is usually saved to a file somewhere so that, when a crash or bug occurrs, we can simply ask the user to hand over the log file and go through it ourself.

When working through past chapters, you may have already encountered some logging information. For example, if we try to do the following:

```cpp
auto box = Box();
box.push_back(box);
```

We get the following message printed to our console:

```
(example_target:45245): mousetrap-CRITICAL **: 16:44:27.065: In Box::push_back: Attempting to insert widget into itself. This would cause an infinite loop
```

This is because we cannot insert a widget into itself, which is why mousetrap prevented the action and printed a log message instead. Let's go through each part of the message one-by-one

#### Application ID

First we have `(example_target:45245)`, this is the identification of our application. For runtime this may not be useful information, however when this log is stored to a system-level log file, many applications may log at the same time, making application identification an integral part of the log message.

#### Log Domain

Next we have `mousetrap-CRITICAL`. The word before the `-` is the **log domain**. This is a developer-defined identification that should show which part of the application or library caused the logging message. Domains include `mousetrap` for mousetrap-specific warnings, `GTK` for GTK-based warning, `GLib`, `Gio`, `Gdk`, etc.. As developers, we should choose our own log domain, so if we create a new application `FooImageViewer`, we should choose a descriptive log domain, such as `foo_image_viewer`, or `foo`.

#### Log Levels

`CRITICAL` is the **log level**. It can be one of the following:

+ `DEBUG` is for messages that should not appear when the end-user uses the application, they are only meant for developers themself
+ `INFO` is for benign status updates, for example `succesfully opened file x`. These message will not be stored unless we specifically request the logging suite to do so
+ `WARNING` is for messages that should attempt to prevent behavior before it occurrs, for example when attempting to close a file while it is still being written to
+ `CRITICAL` are for message that show an error. In many langauges, an error means the end of runtime, however this would be unacceptable for GUI applications. If the application throws an exception, for example, that exception should be caught and printed as a `CRITICAL` log message. Software designers should take that the application does not crash under normal circumstances.
+ `FATAL` is the most severe log level and should only be used as an absolute last resort. Once a `FATAL` warning is printed, the application exits.

We see that our message was designated as `CRITICAL`, meaning the application still kept running, even though an error occurred. mousetrap takes great care to prevent misuse of the library from ending runtime, the programming error of inserting a box into itself would cause usually cause the application to deadlock. Mousetrap prevented this and notified the user/developer with a `CRITICAL` warning

#### Time Stamp

Next, we have `16:44:27.065`, this is the 24h time the message was printed, with millisecond precision. When stored the log to a file, the current data and year is appended to the time stamp

#### Message

Lastly we have the **log message**. Log messages should contain the name of the function they are produced from, for example in the above message it says `In Box::push_back`, telling developers that the error happened in that function. This makes debugging easier. 

Messages should not end with a `\n` (a newline), as one is automatically appended to the end of the message.

## Printing Messages

All interaction with the log is handling through \link mousetrap::log `mousetrap::log` \endlink, which cannot be instantiated. All functions of `log` are static, so they are invoked with the namespace-like `::` notation.

We have one function for each log level, called `log::debug`, `log::info`, `log::warning`, `log::critical` and `log::fatal`. Each of these functions takes two arguments, the log message and the log domain, in that order. Application id, timestamp, etc. are automatically added. The message should be a plain string, it does not respect html or css properties and should not contain any control characters, such as `\t` or `\n`.

We mentioned before that messages of level `debug` and `info` are only printed if we speficially request so. This enabling is on per log domain via `set_surpress_info` and `set_surpress_debug` respectively. For example, if our log domain is `foo`:

```cpp
// define custom domain
const std::string FOO_DOMAIN = "foo";

// print `INFO` level message, nothing will happen because it is surpress by default
log::info("Surpressed message", FOO_DOMAIN);

// enable `INFO` level messages
log::set_surpress_info(FOO_DOMAIN, false);

// this message will be printed
log::info("No longer surpressed message");
```

Shipped applications, that is applications intended for end-users that are no longer under development, should surpress all `DEBUG` and `INFO` message, which is why they are disabled by default.

### Logging to a File

If the operating system is Linux, many log message will be written to the default location, usually `/var/log`. On other operating systems, message may not be stored at all.

We can forward all logging, including that of mousetrap itself, to a file by calling `log::set_file`, which takes the file path as a string. If the file already exist, it will be appended to (as opposed to being override), if the file does not yet exist it will be created.

When stored to a file, logging message will have a different format that lists additional information. The philosophy behind this is that it is better to log as much information as possible and use second party software to filter it, as opposed to missing crucial information for the sake of brevity.

---

## File System

Most GUI applications are centralized around modifying files. Be it a text or image editor exporting files, or a video game creating a save file. For this purpose, mousetrap offers a robust, operating-system-agnostic way of interacting with the disk.

First some vocabulary, there are two kinds of entries in file-systems, **files** contain arbitrary data, while **directories** contain other files and/or other directories. We sometimes call a directory a **folder**. 

Examples for true files include `.png`, `.txt`, `.cpp` text files, shared libraries and even binaries.

A **path** is a string where each folder name is followed by a `/`, (or `\` on windows, though this should be avoided). Examples include `/var/log`, `~/Desktop`, etc.

A **uri** (universal resource identifier) is another way to express the location of the file. It follows a [strict scheme](https://en.wikipedia.org/wiki/Uniform_Resource_Identifier), which is followed by most internet browsers for cross-system file sharing or connecting to the internet.  

### File Descriptor

When querying info about a file, we use \link mousetrap::FileDescriptor `mousetrap::FileDescriptor` \endlink. This object is non-mutating, meaning it is incapable of changing anything about the actual file on the disk. `FileDescriptor` is read-only.

We create a file descriptor from a path or uri like so:

```cpp
auto readonly = FileDescriptor();
readonly.create_from_path("/usr/Desktop/example.txt");
```

Where the argument to `create_from_path` will be automatically detected as an absolute path (a path beginning at the root of a system, usually `/` on unix, `C:` on windows). If it is not an absolute path, it will be prefixed with the applications runtime directory. For example if we create a file from path `"resources/image.png"`, and our application is located in `/usr/bin/`, then the path will be treated as `/usr/bin/resources/image.png`.

`FileDescriptor` does not make sure the file provided by the path or uri exists, or that it is a valid file. Creating a descriptor from an invalid path or a path that does not point to a file or folder works just fine, and we will get no warning. We can check whether it does actually point to a valid file or folder by using `FileDescriptor::exists()`. 

To check whether the pointed to file is a file or folder, we use `is_file` and `is_folder` respectively. If the object pointed to by `FileDescriptor` does not exist, both of these functions will return false.

`FileDescriptor` allows us to query a variety of information about the file, these include:

+ `get_path` returns the location of the file as a path, `~/Desktop/example.txt`
+ `get_uri` returns the location as a uri, `file://~/Desktop/example.txt`
+ `get_file_extension` returns the extension, `.txt`
+ `is_executable` checks whether the file is an executable, `false`
+ `get_content_type` returns the [MIME type](https://en.wikipedia.org/wiki/Media_type), `text/plain`

For additional, more involved information, we can use `query_info`, which takes an attribute identifier. A list of identifiers can be found [here](https://docs.gtk.org/gio/index.html#constants), though not all of these may have a value, depending on the file type. If the value is unknown, an empty string will be returned.

If the file is a folder, we can use `get_children` to get all files and/or directories inside the folder. `get_children` takes a boolean as its only argument, which specifies whether it should list all children and all children of children recursively.

### Manipulating the Disk

`FileDescriptor` is no mutating, it can only tells us information about a file, not modify them in any way. To do this, we instead use functions of `file_system`, which, like `log`, is a uninstantiatable class with only static functions. It provides all the standard file operations such as creating / deleting files, copying or moving them. 

#### Creating Files

\link mousetrap::file_system::create_file_at create_file_at \endlink creates a file at a given location. It takes a file descriptor as its argument. If `should_replace`, its second argument, is set to false and the file already exists, no operation will be performed

```cpp
if (file_system::create_file_at(FileDescriptor("/absolute/path/to/file.txt", false))
    // open contents of file
```

\link mousetrap::file_system_create_directory_at create_directory_at \endlink performs the same action, except it creates a directory.

#### Deleting Files

To permanently delete a file, we use \link mousetrap::file_system::delete_at delete_at\endlink. If we want to instead move the file safely to the system trash, we use \link mousetrap::file_system::move_to_trash move_to_trash\endlink, which is generally recommended.

```cpp
auto to_delete = FileDescriptor("/path/to/delete/file.txt");
if (not file_system::move_to_trash(to_delete))
    log::warning("In example: Unable to delete file at `" + to_delete.get_path() + "`");
```

#### Moving / Copying File

To move a file from one location to another, we use \link mousetrap::file_system::move move\endlink. If we want to copy the file or directory, we use \link mousetrap::file_system::copy copy\endlink.

```cpp
auto from = FileDescriptor("/path/from/file.txt");
auto to = FileDescriptor("/different_path/to/file.txt");
if (not file_system::move(from, to))
    log::warning("In example: Unable to move file from `" + from.get_path() + "` to `" + to.get_path() + "`");
```

#### Changing File Properties

\todo this feature is not yet implemented

---

### Monitoring File Changes

Often, when writing a GUI, we want the graphical interface to reflect the contents of a file on the disk, a good example would be a text editor. What if that file changes outside of our control, for example because the user modified the text with the system console? For this scenario, mousetrap provides a mechanism to watch over files, called `FileMonitor`.

`FileMonitor` cannot be created directly, we can only create it from a `FileDescriptor`, by calling `create_monitor`, which returns a `FileMonitor` instance. `FileMonitor` works similar to a signal emitter, we can register a callback to be called when the file changes using `on_file_changed`. This callback has the signature `(FileMonitorEvent event, const FileDescriptor& self, const FileDescriptor& other, (Data_t)) -> void`, where

+ `event` is a \link mousetrap::FileMonitorEvent FileMonitorEvent\endlink describing the type of action performed
+ `self` is a descriptor pointing to the file that was modifier
+ `other` is a descriptor usually not pointing to any file, unless the event was `FileMonitorEvent::MOVED_IN` or `FileMonitorEvent::MOVED_OUT` , in which case it will point to whatever file was moved into or out of the directory which `self` points to
+ `Data_t` is any arbitrary data

For example, if we want to trigger an action anytime a file `/path/to/file.txt` is modified, we can do so like so:

```cpp
auto to_watch = FileDescriptor("/path/to/file.txt"); // equivalent to .create_from_path
auto monitor = to_watch.create_monitor();
monitor.on_file_changed([](FileMonitorEvent event, const FileDescriptor& self, const FileDescriptor&){
    if (event == FileMonitorEvent::CHANGED)
        std::cout << "file at `" << self.get_path() << "` changed" << std::endl;
});
```

See the \link mousetrap::FileMonitorEvent documentation on `FileMonitorEvent`\endlink for information on different types of file modification events.

---

## Glib Keyfiles

For data like images or sound, we have methods like `Image::save_to_file`, or `SoundBuffer::save_to_file` to store them on the disk. For custom objects, such as the state of our application, we have no such option. While it may sometimes be necessary, for most purposes we do not need to create a custom file type, instead we can use the [GLib keyfile syntax](https://docs.gtk.org/glib/struct.KeyFile.html), which is heavily inspired by Windows' `.ini` settings files.

Keyfiles are human-readable and easy to edit by humans, which makes them better suited for certain purposes when compared to [json](https://docs.fileformat.com/web/json/) or [xml](https://docs.fileformat.com/web/xml/) files.

Note that we do not have to write keyfiles ourself, we have `mousetrap::KeyFile` to do this for use. It may still be useful to know how the syntax of them works, which is what the next section will be about.

### Glib Keyfile Syntax

In a KeyFile, every line is on of four types:

+ **Empty**, which means it has no characters or only control characters and spaces
+ **Comment**, which means it begins with `#`
+ **Group**, which has the form `[group_name]`, where group name is any name
+ **Key**, which has the form `key=value`, where key is any name and value has a format discussed below

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

+ `# keybindings`, and other lines starting with `#` are parsed as comments
+ `[image_view.key_bindings]` designates the first **group**. Similar to how actions are named, we should use `.` as scoping and pick descriptive names
+ `save_file=<Control>s` is the first key-value pair. It is in group `image_view.key_bindings`, it has the key `save_file` and the value `<Control>s`, which is a **string** that can be parsed as a shortcut
+ `[image_view.window]` is our second group, it's name is `image_view.window`
+ `width=400` and `height=300` are two key-value pairs in group `[image_view.window]`. Their values are **numbers**: `400` and `300`, which are integers
+ `default_color_rgba=0.1;0.7;0.2;1` is the third key-vaue pair in group `image_view.window`. It has the value `0.1;0.7;0.2;1` which is **list of numbers**. The suffix `rgba` suggests that these four numbers are the values of each respective color component, so this key should be treated as having the value `RGBA(0.1, 0.7, 0.2, 1)`

Key-value pairs belong to the group that was last opened, groups cannot be nested, the always have a depth of 1.

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

We see that the general syntax to get a value of type `T` from a `KeyFile` file is: `file.get_value_as<T>("group_name", "key_name")`. `KeyFile` automatically treats the string in the corresponding file as the format `T` is associated with. Only the following types are supported:

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

We can furthermore read comments, if the comment is above a group line, `[image_view.window]` in our example, we can retrieve the comment with `file.get_comment_above("image_view.window")`. If the comment is above a key, we use `file.get_comment_above("image_view.window", "width")`.

### Setting Values

Just like accessing values, we use `KeyFile::set_value<T>`, which also takes a group and key identifier, along with a value of type `T`. If `T` is not one of the above mentioned supported files, a `CRITICAL` error will be logged. 

We can even modify comments, using `file.add_comment_above`, which works just like `get_comment_above`, just with an added string argument containing the comment.

When writing to the keyfile, only the object in memory is modified, not the object on disk. To "save" our changes to the keyfile, we need to call `save_to_file`.

---

## Icons

We've seen in the section on widgets how to load an image, create an `ImageDisplay`, then use it to display the image. In applications, we will have dozens of these images that will be used as visual labels for a variety of widgets. While it is possible to simply store all these images as `.png`s and load them manually, this is hardly scalable. Furthermore, we often want to be able to swap out all visual labels, for example to allow for monochromatic or larger size images for the visually impaired. 

To accomplish this, we should use **icons** instead of images.

An icon is similar to an image file, though it usually does not contain pixel data. Icons can be loaded from `.png` files, but also accept `.svg` (vector graphics), and `.ico` (web browser icons), among many more platform-specific formats. Such an icon is managed by `mousetrap::Icon`, which is similar to a file descriptor, in that it does not contain any data, and it does not mutate any file on disk.

### Creating and Viewing Icons

The simplest way to load an icon is to load it as if it was an `Image`, let's say we havea 48x48 pixel image file at `resources/save_icon.svg`. We can load it like so:

```cpp
auto icon = Icon();
icon.create_from_file("resources/save_icon.svg", 48);
```

If we want to display it as a widget, we can choose to use `ImageDisplay::create_from_icon`. The only difference between this an simply using an `Image` is that icons have a two additional properties:

+ `name` of type `mousetrap::IconID`, unique identifies the icon
+ `scale`, which is an integer of value 1 or greater

We see that this makes them easier to keep track of, we can hash or index them using their name, and we can easily double their size if the user requests to do so. 

