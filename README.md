# mousetrap


---

## Installation: C++ Library

### Dependencies

To install mousetrap, you will need the following dependencies:

+ `GTK4`, Version 4.8 or higher recommended but not necessary
+ `CMake`, 3.12 or newer
+ `GLM`, no version requirement
+ `GLEW`, no version requirement
+ `SFML`, no version requirement,
+ `OpenGL`, with a driver supporting OpenGL 3.2 Core or newer
+ `C++17`-capabled C++ compiler, such as clang, gcc, MSVC, etc.

`mousetrap`s dependenices where specifically picked, such that it can run on any PC that has a graphics card manufactured in the last 15 years, most version requirements should not be an issue.

Installing these dependencies is OS-specific, follow the following sections depending on what machine you are using:

#### Debian / Ubuntu

```shell
sudo apt-get install aptitude   
sudo aptitude install cmake            # install cmake
sudo aptitude install libgtk-4-dev     # install GTK4
sudo aptitude install libglm-dev       # install GLM
sudo aptitude install libglew-dev      # install GLEW
sudo aptitude install libsfml-dev      # install SFML
sudo aptitude install libgl1-mesa-dev  # install OpenGL
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

#### Mac OS

> \todo

#### Windows

> \todo

### Installation: C++ Library

With the dependencies settled, we can go on to building and installing mousetrap from source.

#### Linux / MacOS

On unix, run the following commands:

```shell
# in any public directory
git clone https://github.com/Clemapfel/mousetrap
cd mousetrap
mkdir build
cd build
cmake .. -DMOUSETRAP_BUILD_TESTS=ON
sudo make install -j 4
cd ../..
```

Where `sudo` is necessary to install to the default include directory.

#### Windows

> \todo

---

### Installation: Julia Binding

Instead of the previous section, after installing all the dependencies, execute, in any public directory:

```shell
# in any public directory
git clone https://github.com/JuliaInterop/libcxxwrap-julia
cd libcxxwrap-julia
mkdir build
cd build
cmake ..
sudo make install -j 4
cd ../..
```

Then

```shell
# in any public directory
git clone https://github.com/Clemapfel/mousetrap
cd mousetrap
mkdir build
cd build
cmake .. -DMOUSETRAP_BUILD_TESTS=ON -DMOUSETRAP_BUILD_JULIA_BINDING=ON
sudo make install -j 4
cd ../..
```