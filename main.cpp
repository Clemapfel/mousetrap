//
// Created by clem on 7/31/22.
//

#include <include/gl_common.hpp>
#include <include/gtk_common.hpp>
#include <include/gl_area.hpp>
#include <include/widgets/color_picker.hpp>
#include <include/widgets/hsv_triangle_select.hpp>
#include <include/window.hpp>
#include <include/widgets/brush_selection.hpp>
#include <include/widgets/brush_designer.hpp>
#include <include/widgets/canvas.hpp>
#include <include/widgets/palette_view.hpp>
#include <include/image.hpp>
#include <include/adjustment.hpp>
#include <include/stack.hpp>

#include <include/window.hpp>

using namespace mousetrap;


static void test()
{
    std::cout << "test" << std::endl;
}

int main()
{
    gtk_init(nullptr, nullptr);

    auto main_window = Window(GTK_WINDOW_TOPLEVEL);
    main_window.connect_signal("destroy", gtk_main_quit, nullptr);

    main_window.add_events(GDK_KEY_PRESS_MASK);
    main_window.add_events(GDK_KEY_RELEASE_MASK);

    // init opengl
    gtk_widget_realize(main_window.get_native());
    gtk_initialize_opengl(GTK_WINDOW(main_window.get_native()));

    auto* color_picker = new ColorPicker(150);
    auto* color_picker_instance = (ColorPicker*) color_picker;
    color_picker_instance->set_size_request({200, 300});
    //gtk_container_add(GTK_CONTAINER(main_window), color_picker_instance->get_native());

    auto* hsv_triangle_select = new HsvTriangleSelect();
    auto* hsv_triangle_select_instance = (HsvTriangleSelect*) hsv_triangle_select;
    hsv_triangle_select_instance->set_size_request({200, 200});
    //gtk_container_add(GTK_CONTAINER(main_window), hsv_triangle_select_instance->get_native());

    auto brush_selection = BrushSelection();
    brush_selection.set_size_request({200, 200});
    //gtk_container_add(GTK_CONTAINER(main_window), brush_selection.get_native());

    auto* brush_designer = new BrushDesigner(300, 16, 16);
    //gtk_container_add(GTK_CONTAINER(main_window), brush_designer->get_native());

    auto* canvas = new Canvas(64, 64);
    //main_window.add(canvas);

    auto* palette_view = new PaletteView(GTK_ORIENTATION_HORIZONTAL);
    main_window.add(palette_view);

    // TODO
    std::vector<WidgetWrapper<GtkImage>> images;
    auto image = Image();
    image.create(128, 128);

    for (size_t i = 0; i < 16; ++i)
    {
       for (size_t x = 0; x < image.get_size().x; ++x)
           for (size_t y = 0; y < image.get_size().y; ++y)
               image.set_pixel(x, y, HSVA(rand() / float(RAND_MAX), 1, 1, 1));

       images.push_back((GtkImage*) gtk_image_new_from_pixbuf(image.to_pixbuf()));
    }

    auto stack = Stack();

    size_t i = 0;
    for (auto img : images)
        stack.add_child(&img, std::to_string(i++));

    auto stack_switcher = StackSwitcher(&stack);
    auto stack_sidebar = StackSidebar(&stack);

    auto box = VBox();

    box.add(stack);
    box.add(stack_switcher);

    //main_window.add(box);

    // TODO

    // render loop
    gtk_widget_show_all(main_window.get_native());
    main_window.present();
    gtk_main();

    return 0;
}