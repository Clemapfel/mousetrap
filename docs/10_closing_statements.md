
<details><summary><b><tt>Example</tt> of what I want</b></summary>

Code snippets from this section can be run using the following `main.cpp`:

```cpp
#include <mousetrap.hpp>
using namespace mousetrap;

int main()
{
    auto app = Application("example.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(*app);
        
        // snippet here
      
        window.set_child(/* widget from snippet */);
        window.present();
    });
    return app.run();
}
```
</details>