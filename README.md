# Mousetrap: GTK4-based GUI Engine

> **If you are looking to run mousetrap using Julia, you are in the wrong place. Please visit [mousetrap.jl](https://github.com/Clemapfel/mousetrap.jl) instead.**

Standalone C++ component of the mousetrap GUI engine. 

While it can be used on its own, this library should be considered the back-end for the Julia component. The C++ -> Julia interface is available [here](https://github.com/Clemapfel/mousetrap_julia_binding/).

Note that documentation is lacking, a [full manual and tutorial](https://clemens-cords.com/mousetrap) is available for the Julia component only, which was the focus of development and polish. Autogenerated documentation as a well as a few examples can be found [here](https://clemens-cords.com/mousetrap_cpp).

---

## Installation

### Dependencies

##### Linux: Debian / Ubuntu

Execute, in any public directory:

```shell
sudo apt-get install aptitude   
sudo aptitude install 
 cmake \             # install CMake
 libgtk-4-dev \      # install GTK4
 libadwaita-1-dev \  # install Adwaita
 libgl1-mesa-dev  \  # install OpenGL
 libglm-dev  \       # install GLM
 libglew-dev         # install GLEW
```

##### Fedora

```shell
sudo dnf install 
  cmake \             # install CMake
  gtk4-devel \        # install GTK4
  libadwaita-devel \  # install Adwaita
  glm-devel  \        # install GLM
  glew-devel \       # install GLEW
```

Where `OpenGL` is usually installed automatically on Fedora.

##### MacOS

Install [homebrew](https://brew.sh/), then, in any public directory:

```shell
sudo brew install 
  cmake \  # install CMake
  gtk4 \   # install GTK4
  glm  \   # install GLM
  glew     # install GLEW
```

##### Windows

Consider following guides linked here, in order to install each of the following dependencies:

+ [GTK4](https://www.gtk.org/docs/installations/windows)
+ [OpenGL](https://medium.com/swlh/setting-opengl-for-windows-d0b45062caf)
+ [GLEW](https://glew.sourceforge.net/install.html)

To acquire adwaiata, download the already compiled shared library [here](https://github.com/JuliaBinaryWrappers/libadwaita_jll.jl/releases/).

GLM (OpenGL Mathematics) is header-only, download the source code [here](https://github.com/g-truc/glm), then install it such that it can be found by cmake.

`OpenGL` will usually be available through the wrapper shared library `OPENGL32.dll`, which should come with any Window 10 or newer system.

### Compiling

Enter a bash console, then, in any public directory:

```shell
git clone https://github.com/Clemapfel/mousetrap.git
cd mousetrap
mkdir build
cd build
cmake .. -DMOUSETRAP_ENABLE_OPENGL_COMPONENT=ON
sudo make install -j 8
```

If you would like to use meson instead of CMake, do:

```shell
git clone https://github.com/Clemapfel/mousetrap.git
cd mousetrap
meson setup build
cd build
sudo meson install
```

After which you can include mousetrap in your own `CMakeLists.txt` files likes so:

### Linking 

First, create a file `main.cpp`:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        auto label = Label("Hello World!");
        
        window.set_child(label);
        window.present();
    });

    return app.run();
}
```

Then, `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.18)

set(TARGET_NAME example_target)
set(PROJECT_NAME example_project)

project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)

find_package(mousetrap REQUIRED)

add_executable(${TARGET_NAME} main.cpp)

target_link_libraries(${TARGET_NAME} PRIVATE
    ${MOUSETRAP_LIBRARIES}
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${MOUSETRAP_INCLUDE_DIRECTORIES}
)

target_compile_features(${TARGET_NAME} PUBLIC
    cxx_std_17
)
```

Where `find_package(mousetrap)` sets two variables, `MOUSETRAP_LIBRARIES` and `MOUSETRAP_INCLUDE_DIRECTORIES`, which we made available to the new executable using `target_link_libraries` and `target_include_directories`.

Afterwards, you should be able to build your app using:

```shell
# in the same directory as our own `CMakeLists.txt`
mkdir build
cd build
cmake ..
make
# will deposit executable example_project_test
```

---

## Documentation

Auto-generated documentation is available [here](https://clemens-cords.com/mousetrap_cpp).

---

## License

Mousetrap is licensed under [GNU Lesser General Public License 3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html#license-text).

---

## Credits

This library was created by [C.Cords](https://clemens-cords.com). 
