//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/10/23
//


namespace mousetrap
{
    template<typename... Ts>
    void ColumnView::push_back_row(Ts... widgets)
    {
        std::vector<std::reference_wrapper<Widget>> refs = {
            std::ref(widgets)...
        };

        if (refs.size() > get_n_columns())
            log::warning("In ColumnView::push_back_rows: Attempt to push " + std::to_string(refs.size()) + " widgets, but ColumnView only has " + std::to_string(get_n_columns()) + " columns");

        uint64_t insert_i = get_n_rows();
        for (uint64_t i = 0; i < get_n_columns(); ++i)
        {
            if (i >= refs.size())
                continue;

            auto column = get_column_at(i);
            set_widget_at(column, insert_i, refs.at(i));
        }
    }

    template<typename... Ts>
    void ColumnView::push_front_row(Ts... widgets)
    {
        std::vector<std::reference_wrapper<Widget>> refs = {
            std::ref(widgets)...
        };

        if (refs.size() > get_n_columns())
            log::warning("In ColumnView::push_front_row: Attempt to push " + std::to_string(refs.size()) + " widgets, but ColumnView only has " + std::to_string(get_n_columns()) + " columns");

        uint64_t insert_i = 0;
        for (uint64_t i = 0; i < get_n_columns(); ++i)
        {
            if (i >= refs.size())
                continue;

            auto column = get_column_at(i);
            set_widget_at(column, insert_i, refs.at(i));
        }
    }

    template<typename... Ts>
    void ColumnView::insert_row_at(uint64_t i, Ts... widgets)
    {
        std::vector<std::reference_wrapper<Widget>> refs = {
        std::ref(widgets)...
        };

        if (refs.size() > get_n_columns())
            log::warning("In ColumnView::insert_row_at: Attempt to push " + std::to_string(refs.size()) + " widgets, but ColumnView only has " + std::to_string(get_n_columns()) + " columns");

        uint64_t insert_i = i;
        for (uint64_t i = 0; i < get_n_columns(); ++i)
        {
            if (i >= refs.size())
                continue;

            auto column = get_column_at(i);
            set_widget_at(column, insert_i, refs.at(i));
        }
    }
}
