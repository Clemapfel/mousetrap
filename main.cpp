//
// Created by clem on 7/31/22.
//

#include <gtk/gtk.h>

#include <include/window.hpp>
#include <include/box.hpp>
#include <include/entry.hpp>
#include <include/application.hpp>
#include <include/gl_area.hpp>
#include <include/image_display.hpp>
#include <include/event_controller.hpp>
#include <include/menu_model.hpp>
#include <include/menu_button.hpp>
#include <include/column_view.hpp>
#include <include/image_display.hpp>
#include <include/list_view.hpp>
#include <include/detachable_box.hpp>
#include <include/paned.hpp>
#include <include/dropdown.hpp>
#include <include/button.hpp>
#include <include/check_button.hpp>

#include <app/global_state.hpp>
#include <app/primary_secondary_color_swapper.hpp>
#include <app/toolbox.hpp>

/*
#include <app/color_picker.hpp>
#include <app/verbose_color_picker.hpp>
#include <app/palette_view.hpp>
#include <app/brush_options.hpp>
#include <app/widget_container.hpp>
#include <app/canvas.hpp>
#include <app/layer_view.hpp>
 */

using namespace mousetrap;

void static_test(CheckButton* instance, std::string* data)
{
    std::cout << instance->get_native() << std::endl;
    std::cout << *((std::string*) data) << std::endl;
}

/*
 * template<typename T>
            using on_clicked_function_t = void(Button*, T);

            static void on_clicked_wrapper(GtkButton*, Button* instance)
            {
                if (instance->_on_clicked_f == nullptr)
                    return;

                (instance->_on_clicked_f)(instance, instance->_on_clicked_data);
            }

            template<typename Function_t, typename T>
            void connect_signal_clicked(Function_t function, T data)
            {
                auto temp =  std::function<on_clicked_function_t<T>>(function);
                _on_clicked_f = std::function<on_clicked_function_t<void*>>(*((std::function<on_clicked_function_t<void*>>*) &temp));
                _on_clicked_data = data;

                connect_signal("clicked", on_clicked_wrapper, this);
            }

            std::function<on_clicked_function_t<void*>> _on_clicked_f;
            void* _on_clicked_data;
 */

static void activate(GtkApplication* app, void*)
{
    state::shortcut_map = new ShortcutMap();
    state::shortcut_map->load_from_file("/home/clem/Workspace/mousetrap/app/keybindings.conf");

    state::main_window = new Window(GTK_WINDOW(gtk_application_window_new(app)));
    gtk_initialize_opengl(GTK_WINDOW(state::main_window->operator GtkWidget*()));

    //state::primary_secondary_color_swapper = new PrimarySecondaryColorSwapper();
    //state::toolbox = new Toolbox(GTK_ORIENTATION_HORIZONTAL);

    auto box = Box(GTK_ORIENTATION_HORIZONTAL);

    static auto lambda_test = [](Button* instance, std::string* data) -> void {
        std::cout << instance->get_native() << std::endl;
        std::cout << *data << std::endl;
    };

    static auto button = Button();
    button.connect_signal_clicked(lambda_test, new std::string("test"));
    box.push_back(&button);

    /*
    void(*t1)(Button*, void*) = (void(*)(Button*, void*)) static_test;
    t1(&button, new std::string("test"));

    void(*t2)(Button*, void*) = (void(*)(Button*, void*)) &lambda_test;
    //t2(&button, new std::string("test"));

    std::function<void(Button*, void*)> t3 = (void(*)(Button*, void*)) static_test;
    t3(&button, new std::string("test"));

    std::function<void(Button*, std::string*)> temp = lambda_test;

    std::function<void(Button*, void*)>* t4 = (std::function<void(Button*, void*)>*) &temp;
    (*t4)(&button, new std::string("test"));
     */

    // box.push_back(state::primary_secondary_color_swapper->operator Widget*());
    // box.push_back(state::toolbox->operator Widget*());

    state::main_window->set_child(&box);
    state::main_window->show();
    state::main_window->present();
    state::main_window->set_focusable(true);
    state::main_window->grab_focus();
}

int main()
{
    state::app = new Application(activate);
    return state::app->run();
}

/*
 *  if (false) {
 *    // TODO

    auto* tree_store = gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);

    GtkTreeIter toplevel_it, child_it;

    gtk_tree_store_append(tree_store, &toplevel_it, nullptr);
    gtk_tree_store_set(tree_store, &toplevel_it,
                       0, "test_top_01", -1);

    gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
    gtk_tree_store_set(tree_store, &child_it,
                       0, "test_child_01_01", -1);

    gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
    gtk_tree_store_set(tree_store, &child_it,
                       0, "test_child_01_02", -1);

    gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
    gtk_tree_store_set(tree_store, &child_it,
                       0, "test_child_01_03", -1);

    auto model = gtk_multi_selection_new(G_LIST_MODEL(tree_store));
    auto column_view = WidgetWrapper<GtkColumnView>(GTK_COLUMN_VIEW(gtk_column_view_new(GTK_SELECTION_MODEL(model))));

    std::vector<GtkColumnViewColumn*> columns;
    auto factory = WidgetWrapper<GtkSignalListItemFactory>(GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new()));
    factory.connect_signal("setup", on_setup);

    for (size_t i = 0; i < 10; ++i)
        columns.push_back(gtk_column_view_column_new(std::to_string(i).c_str(), GTK_LIST_ITEM_FACTORY(factory._native)));

    for (auto* col : columns)
        gtk_column_view_append_column(column_view._native, col);

    auto store = gtk_list_store_new(10);

    // TODO
 *
        auto* tree_store = gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);

        GtkTreeIter toplevel_it, child_it;

        gtk_tree_store_append(tree_store, &toplevel_it, nullptr);
        gtk_tree_store_set(tree_store, &toplevel_it,
                           0, "test_top_01", -1);

        gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
        gtk_tree_store_set(tree_store, &child_it,
                           0, "test_child_01_01", -1);

        gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
        gtk_tree_store_set(tree_store, &child_it,
                           0, "test_child_01_02", -1);

        gtk_tree_store_append(tree_store, &child_it, &toplevel_it);
        gtk_tree_store_set(tree_store, &child_it,
                           0, "test_child_01_03", -1);

        GtkTreeIter pixbuf_col_it;

        auto image = Image();
        image.create(100, 100, RGBA(1, 0, 1, 0.1));
        auto box = Box(GTK_ORIENTATION_HORIZONTAL);
        auto entry = Entry();
        box.add(&entry);

        gtk_tree_store_append(tree_store, &toplevel_it, nullptr);
        gtk_tree_store_set(tree_store, &child_it,
                           1, image.to_pixbuf(), -1);

        auto* tree_view = gtk_tree_view_new();
        gtk_tree_view_set_reorderable(GTK_TREE_VIEW(tree_view), true);

        auto* text_column = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(text_column, "text");
        auto* text_renderer = gtk_cell_renderer_accel_new();
        gtk_tree_view_column_pack_start(text_column, text_renderer, true);
        gtk_tree_view_column_add_attribute(text_column, text_renderer, "text", 0);

        auto* pixbuf_column = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(pixbuf_column, "pixbuf");
        auto* pixbuf_renderer = gtk_cell_renderer_pixbuf_new();
        gtk_tree_view_column_pack_start(pixbuf_column, pixbuf_renderer, true);
        gtk_tree_view_column_set_attributes(pixbuf_column, pixbuf_renderer, "pixbuf", 1, nullptr);

        gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(tree_store));
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), text_column);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), pixbuf_column);

        main_window.add(tree_view);
    }
 */