# mousetrap: Multi-Media GUI Library

3. [Installation](#installation)<br>
3.1 [Dependencies](#dependencies)<br>
3.1.1 [Debian / Ubuntu](#debian--ubuntu)<br>
3.1.2 [Fedora](#fedora)<br>
3.1.3 [~~MacOS~~]()<br>
3.1.4 [~~Windows~~]()<br>
3.2 [Compiling from Source](#Compilation)<br>
3.2.1 [Linux / MacOS](#linux--macos)<br>
3.2.2 [~~Windows~~]()<br>
4. [Documentation](#documentation)<br>

## Documentation

Documentation is available [here](https://clemens-cords.com/mousetrap).

A minimal hello world project can be found in the `[example](./example)` folder here on GitHub.

## Installation

### Dependencies

To install mousetrap from source, the following dependencies are needed:

+ `CMake`: To build and install mousetrap
+ `GTK4`: Engine for all GUI elements 
+ `OpenGL`: Hardware accelerated rendering
+ `GLM`: CPU-side OpenGL-related Math
+ `GLEW`: Loading of OpenGL extensions
+ `SFML`: Audio-playback and Joystick Interface

Installing these dependencies is OS-specific, the following section will provide instructions on how to do so:

#### Debian / Ubuntu

```shell
sudo apt-get install aptitude   
sudo aptitude install cmake            # install cmake
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

#### MacOS

> \todo This section is not yet complete

#### Windows

> \todo This section is not yet complete

### Compilation

With the dependencies settled, we can go on to building and installing mousetrap from source.

#### Linux / MacOS

On Unix, run the following commands:

```shell
# in any public directory
git clone https://github.com/Clemapfel/mousetrap.git
cd mousetrap
mkdir build
cd build
cmake ..
sudo make install -j 4
cd ../..
```

Where `sudo` is necessary to install to the default library and include directory. If you would like to install mousetrap to a different location, add `-DCMAKE_INSTALL_PREFIX=/path/to/custom/location` at the end of `cmake ..`.

#### Windows

> \todo