1. Install [Homebrew](https://brew.sh/), unless already installed

2. Open terminal, then:
   ```shell
   sudo brew install meson
   sudo brew install ninja
   sudo brew install glew
   sudo brew install gtk4
   sudo brew install libadwaita
   sudo brew install glm
    ```

3. Clone mousetrap apple branch
   ```shell
   git clone https://github.com/clemapfel/mousetrap.git
   cd mousetrap
   git fetch
   git checkout mac_main_menu
    ```

4. Configure Project
   ```shell
   # in /mousetrap
   meson setup build
    ```

5. Edit `mousetrap/test/apple_test.cpp`
   See comments in source file

6. Compile Library and Executable
   ```shell
   # in /mousetrap
   cd build
   ninja
    ```

(a lot of warnings may appear, ignore these)

7. Run Executable
   ```shell
   # in /mousetrap/build
   ./apple_test
    ```

8. Remove everything once you're done
   ```shell
   # in /mousetrap
   cd ..
   sudo rm -r mousetrap
    ```
