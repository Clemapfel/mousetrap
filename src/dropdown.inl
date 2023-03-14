//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/18/23
//

namespace mousetrap
{
    namespace detail
    {
        struct _DropDownItem;
        _DropDownItem* drop_down_item_new(Widget* in, Widget* label, DropDown* owner, size_t function_id);
    }

    template<typename Function_t, typename T>
    void DropDown::push_back(
            Widget* list_widget,
            Widget* when_selected_label_widget,
            Function_t on_select_f,
            T on_select_data)
    {
        auto function_id = _current_function_id++;
        _functions.insert({function_id, [data = on_select_data, f = on_select_f](){
            f(data);
        }});

        auto* item = detail::drop_down_item_new(
                list_widget,
                when_selected_label_widget,
                this,
                function_id
        );
        g_list_store_append(_model, item);
    }

    template<typename Function_t, typename T>
    void DropDown::push_back(
            Widget* list_widget,
            Widget* when_selected_label_widget,
            Function_t on_select_f)
    {
        auto function_id = _current_function_id++;
        _functions.insert({function_id, [f = on_select_f](){
            f();
        }});

        auto* item = detail::drop_down_item_new(
                list_widget,
                when_selected_label_widget,
                this,
                function_id
        );
        g_list_store_append(_model, item);
    }
}
