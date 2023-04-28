//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/18/23
//

#include <mousetrap/log.hpp>

namespace mousetrap
{
    namespace detail
    {
        struct _DropDownItem;
        _DropDownItem* drop_down_item_new(size_t id, const Widget* in, const Widget* label, DropDown* owner, std::function<void(DropDown*)> f);
    }

    template<typename Function_t, typename Data_t>
    DropDown::ItemID DropDown::push_back(const Widget& list_widget, const Widget& label_widget, Function_t f_in, Data_t data_in)
    {
        assert_label_is_not_self("push_back", list_widget, label_widget);

        auto id = _current_id++;
        auto* item = detail::drop_down_item_new(
        id,
        &list_widget,
        &label_widget,
        this,
        [f = f_in, data = data_in](DropDown* instance){
            f(instance, data);
        });

        g_list_store_append(_model, item);
        return ItemID{id};
    }

    template<typename Function_t>
    DropDown::ItemID DropDown::push_back(const Widget& list_widget, const Widget& label_widget, Function_t f_in)
    {
        assert_label_is_not_self("push_back", list_widget, label_widget);

        auto id = _current_id++;
        auto* item = detail::drop_down_item_new(
        id,
        &list_widget,
        &label_widget,
        this,
        [f = f_in](DropDown* instance){
            f(instance);
        });

        g_list_store_append(_model, item);
        return ItemID{id};
    }

    template<typename Function_t, typename Data_t>
    DropDown::ItemID DropDown::push_front(const Widget& list_widget, const Widget& label_widget, Function_t f_in, Data_t data_in)
    {
        assert_label_is_not_self("push_front", list_widget, label_widget);

        auto id = _current_id++;
        auto* item = detail::drop_down_item_new(
        id,
        &list_widget,
        &label_widget,
        this,
        [f = f_in, data = data_in](DropDown* instance){
            f(instance, data);
        });

        g_list_store_prepend(_model, item);
        return ItemID{id};
    }

    template<typename Function_t>
    DropDown::ItemID DropDown::push_front(const Widget& list_widget, const Widget& label_widget, Function_t f_in)
    {
        assert_label_is_not_self("push_front", list_widget, label_widget);

        auto id = _current_id++;
        auto* item = detail::drop_down_item_new(
        id,
        &list_widget,
        &label_widget,
        this,
        [f = f_in](DropDown* instance){
            f(instance);
        });

        g_list_store_prepend(_model, item);
        return ItemID{id};
    }

    template<typename Function_t, typename Data_t>
    DropDown::ItemID DropDown::insert(size_t i, const Widget& list_widget, const Widget& label_widget, Function_t f_in, Data_t data_in)
    {
        assert_label_is_not_self("insert", list_widget, label_widget);

        if (i >= g_list_model_get_n_items(G_LIST_MODEL(_model)))
            return push_back(list_widget, label_widget, f_in, data_in);

        auto id = _current_id++;
        auto* item = detail::drop_down_item_new(
        id,
        &list_widget,
        &label_widget,
        this,
        [f = f_in, data = data_in](DropDown* instance){
            f(instance, data);
        });

        g_list_store_insert(_model, i, item);
        return ItemID{id};
    }

    template<typename Function_t>
    DropDown::ItemID DropDown::insert(size_t i, const Widget& list_widget, const Widget& label_widget, Function_t f_in)
    {
        assert_label_is_not_self("insert", list_widget, label_widget);

        if (i >= g_list_model_get_n_items(G_LIST_MODEL(_model)))
            return push_back(list_widget, label_widget, f_in);

        auto id = _current_id++;
        auto* item = detail::drop_down_item_new(
        id,
        &list_widget,
        &label_widget,
        this,
        [f = f_in](DropDown* instance){
            f(instance);
        });

        g_list_store_insert(_model, i, item);
        return ItemID{id};
    }
}
