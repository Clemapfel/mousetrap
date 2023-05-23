# Chapter 1: Installation

In this chapter, we will learn
+ How to install mousetrap and its dependencies
+ How to create our first GUI application
+ How to uninstall mousetrap, if we decide to do so

---

## Dependencies

To install mousetrap from source, the following dependencies are needed:

+ `CMake`: To build and install mousetrap
+ `GTK4`: Engine for all GUI elements and File I/O
+ `OpenGL`: Hardware accelerated rendering
+ `GLM`: CPU-side OpenGL-related Math
+ `GLEW`: Loading of OpenGL extensions
+ `SFML`: Audio-playback, joystick interface

We'll also need a C++17-capable compiler, for example those provided by GCC or clang.

Installing these dependencies is OS-specific, the following section will provide instructions on how to do so:

### Debian / Ubuntu

```shell
sudo apt-get install aptitude   
sudo aptitude install cmake \  # install CMake
 libgtk-4-dev \      # install GTK4
 libgl1-mesa-dev  \  # install OpenGL
 libglm-dev  \       # install GLM
 libglew-dev \       # install GLEW
 libsfml-dev         # install SFML
```

### Fedora

```shell
sudo dnf install cmake \  # install CMake
  gtk4-devel \            # install GTK4
  glm-devel  \            # install GLM
  glew-devel \            # install GLEW
  SFML-devel              # install SFML
```

Where `OpenGL` is usually installed automatically on Fedora.

### MacOS

\todo This section is not yet complete.

Consider using [Homebrew](https://brew.sh/) to install the listed dependencies

### Windows

\todo This section is not yet complete. 

Consider following guides linked here to to install each dependency

+ [SFML](https://www.sfml-dev.org/tutorials/2.5/start-vc.php)
+ [GTK4](https://www.gtk.org/docs/installations/windows)
+ [OpenGL](https://medium.com/swlh/setting-opengl-for-windows-d0b45062caf)
+ [GLEW](https://glew.sourceforge.net/install.html)

Or use [chocolatey](https://chocolatey.org/) or [winget](https://learn.microsoft.com/en-us/windows/package-manager/winget/) to install dependencies available at these systems.

---

## Compilation

With the dependencies settled, we can go on to building and installing mousetrap from source.

### Linux / MacOS

On Unix, run the following commands:

```bash
# in any public directory
git clone https://github.com/Clemapfel/mousetrap.git
cd mousetrap
mkdir build
cd build
cmake ..
sudo make install -j 4
cd ../..
rm -r mousetrap
```

Where `sudo` is necessary to install to the default library and include directory. If we want to install mousetrap to a different location, we can add `-DCMAKE_INSTALL_PREFIX=/path/to/custom/location` at the end of `cmake ..`. This is usually not recommended.

### Windows

\todo This section is not yet complete. 

Consider using the [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/) to open mousetrap as a project, then select install from your build menu.

---

## Hello World

Now that everything we need is installed, we can continue onto creating our first project:

### CMakeLists.txt

After creating a folder `example` in any public directory:

```shell
mkdir example
cd example
```

We create a file `CMakeLists.txt` with the following contents:

```cmake
cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME example_project)   # name of project
set(TARGET_NAME example_target)     # name of executable

# declare project
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)

# find mousetrap
find_package(mousetrap REQUIRED)

# declare target
add_executable(${TARGET_NAME} main.cpp)

# link mousetrap libraries, including mousetrap itself
target_link_libraries(${TARGET_NAME} PRIVATE
    "${MOUSETRAP_LIBRARIES}"
)

# add mousetrap include directories
target_include_directories(${TARGET_NAME} PRIVATE
    "${MOUSETRAP_INCLUDE_DIRECTORIES}"
)

# link C++17 standard library
target_compile_features(${TARGET_NAME} PUBLIC
    cxx_std_17
)
```

Where `TARGET_NAME` and `PROJECT_NAME` can be freely modified.

`find_package(mousetrap REQUIRED)` loads mousetrap and its associated libraries. This command sets two variables: `MOUSETRAP_LIBRARIES` and `MOUSETRAP_INLUDE_DIRECTORIES`.

We need to link against all the libraries contained in the former, and we need to declare all directories contained in the latter as include directories for our custom target. This is done with  `target_link_libraries` and `target_include_directories` respectively.

Lastly, we set `target_compile_features` for our target to C++17, which automatically links against the C++ standard library.

### main.cpp

Now, in the same directory as `CMakeLists.txt`, we add a file `main.cpp` with the following contents:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    // declare application
    auto app = Application("test.app");
    
    // initialization
    app.connect_signal_activate([](Application& app)
    {
        // create window
        auto window = Window(app);

        // create label
        auto label = Label("Hello World");
        label.set_margin(75);

        // add label to window
        window.set_child(label);
        
        // show window
        window.present();
    });

    // start main loop
    return app.run();
}
```

We'll learn in the coming chapters what each of these lines do.

## Compilation

After saving `main.cpp`, we can compile our project like so:

```bash
# in the same directory as CMakeLists.txt
mkdir build
cd build
cmake ..
make
```

This will deposit an executable with the name of the CMake variable `TARGET_NAME`. Assuming that name is still `example_target`, we can now run that executable:

```shell
./example_target
```

\image html example_test_hello_world.png

If everything seems to be working: great! We can continue onto learning how to create our own applications.

## Troubleshooting

\todo This section is not yet complete. Considering [opening an issue on GitHub](https://github.com/Clemapfel/mousetrap/issues/new) if you are having trouble.

## Uninstallation

If we decide to remove mousetrap from our machine, we only need to delete the following:

+ `libmousetrap.so`, usually installed to `/usr/local/lib` or `/user/local/lib64`
+ folder `mousetrap` and all its contents, usually installed to `/usr/local/include`
+ file `mousetrap.hpp`, also usually in `/usr/local/include`

If the above paths do not contain the mentioned files, we can manually search for them, for example by using the `locate` command on Linux / macOS. Mousetrap does not install any files other than the ones mentioned here.