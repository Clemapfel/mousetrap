# Chapter 1: Installation

In this chapter, we will learn
+ how to install mousetrap and it's dependencies
+ how to create our first GUI application
+ how to uninstall mousetrap if we decide to do so

If there is an issue with any step in this chapter, consider continuing onto the [troubleshooting section](#troubleshooting).

### Dependencies

To install mousetrap from source, the following dependencies are needed:

+ `CMake`: To build and install mousetrap
+ `GTK4`: Engine for all GUI elements
+ `OpenGL`: Hardware accelerated rendering
+ `GLM`: CPU-side OpenGL-related Math
+ `GLEW`: Loading of OpenGL extensions
+ `SFML`: Audio-playback and Joystick Interface

We'll also need a C++17-capable compiler, for example those provided by GCC or clang.

Installing these dependencies is OS-specific, the following section will provide instructions on how to do so:

#### Debian / Ubuntu

```shell
sudo apt-get install aptitude   
sudo aptitude install cmake            # install CMake
sudo aptitude install libgtk-4-dev     # install GTK4
sudo aptitude install libgl1-mesa-dev  # install OpenGL
sudo aptitude install libglm-dev       # install GLM
sudo aptitude install libglew-dev      # install GLEW
sudo aptitude install libsfml-dev      # install SFML
```

#### Fedora

```shell
sudo dnf install cmake          # install CMake
sudo dnf install gtk4-devel     # install GTK4
sudo dnf install glm-devel      # install GLM
sudo dnf install glew-devel     # install GLEW
sudo dnf install SFML-devel     # install SFML
```

Where `OpenGL` is usually installed automatically on Fedora.

#### macOS

\not_yet_complete

#### Windows

\not_yet_complete

### Compilation

With the dependencies settled, we can go on to building and installing mousetrap from source.

#### Linux / macOS

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

#### Windows

\not_yet_complete

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

set(TARGET_NAME example_target)
set(PROJECT_NAME example_project)

# declare project
project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)

# find mousetrap
find_package(mousetrap REQUIRED)

# declare target
add_executable(${TARGET_NAME} main.cpp)

# link mousetrap libraries, including mousetrap itself
target_link_libraries(${TARGET_NAME} PRIVATE
    ${MOUSETRAP_LIBRARIES}
)

# add mousetrap include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${MOUSETRAP_INCLUDE_DIRECTORIES}
)

# add std:: for C++17
target_compile_features(${TARGET_NAME} PUBLIC
    cxx_std_17
)
```

Where `TARGET_NAME` and `PROJECT_NAME` can be freely modified.

`find_package(mousetrap REQUIRED)` loads mousetrap and its associated libraries. This command sets two variables: `MOUSETRAP_LIBRARIES` and `MOUSETRAP_INLUDE_DIRECTORIES`. We need to link against all the libraries contained in the former, and we need to declare all directories contained in the latter as include directories for our custom target, an executable in this case.

Lastly, we set `target_compile_features` for our target to C++17, which automatically links against the C++ standard library.

### main.cpp

Now, in the same directory as `CMakeLists.txt` from the previous section, we add a file `main.cpp` with the following contents:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    // declare application
    auto app = Application("test.app");
    
    // initialization
    app.connect_signal_activate([](Application* app)
    {
        // create window
        auto window = Window(*app);

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

```shell
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

\not_yet_complete

## Uninstallation

To remove mousetrap from our machine, we only need to delete the following:

+ `libmousetrap.so`, usually installed to `/usr/local/lib` or `/user/local/lib64`
+ folder `mousetrap` and all its contents, usually installed to `/usr/local/include`
+ file `mousetrap.hpp`, also usually in `/usr/local/include`

If the above paths do not contain the mentioned files, we can manually search for them, for example by using the `locate` command on Linux / macOS. Mousetrap does not install any files other than the ones mentioned here.