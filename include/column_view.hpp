//
// Copyright 2022 Clemens Cords
// Created on 9/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/menu_model.hpp>

#include <vector>

namespace mousetrap
{
    namespace detail { struct _TreeColumnViewItem; }

    class TreeColumnView : public WidgetImplementation<GtkColumnView>
    {
        public:
            using Iterator = detail::_TreeColumnViewItem*;

            TreeColumnView(std::vector<std::string> titles, GtkSelectionMode = GtkSelectionMode::GTK_SELECTION_MULTIPLE);

            void set_show_column_separators(bool);
            void set_show_row_separators(bool);
            void set_enable_rubberband_selection(bool);

            void append_column(const std::string& title, std::vector<Widget*>);
            void prepend_column(const std::string& title, std::vector<Widget*>);
            void insert_column(size_t i, const std::string& title, std::vector<Widget*>);
            void move_column_to(size_t old_i, size_t new_i);
            void remove_column(size_t i);

            Iterator append_row(std::vector<Widget*>, Iterator = nullptr);
            Iterator prepend_row(std::vector<Widget*>, Iterator = nullptr);
            Iterator insert_row(size_t, std::vector<Widget*>, Iterator = nullptr);

            Iterator move_row_to(size_t old_position, size_t new_position, Iterator old_it, Iterator new_it);
            void remove_row(size_t, Iterator = nullptr);

            Widget* get_widget_at(size_t row_i, size_t col_i, Iterator = nullptr);
            void set_widget_at(size_t row_i, size_t col_i, Widget*, Iterator = nullptr);

            void set_widgets_in_row(size_t row_i, std::vector<Widget*>, Iterator = nullptr);
            std::vector<Widget*> get_widgets_in_row(size_t, Iterator = nullptr);

            void set_widgets_in_column(size_t col_i, std::vector<Widget*>);
            std::vector<Widget*> get_widgets_in_column(size_t col_i);

            void set_column_expand(size_t i, bool);
            void set_column_fixed_width(size_t i, int);
            void set_column_title(size_t i, const std::string&);
            void set_column_header_menu(size_t i, MenuModel*);
            void set_column_visible(size_t i, bool b);
            void set_column_resizable(size_t i, bool);

            size_t get_n_columns();
            size_t get_n_rows(Iterator);
            size_t get_n_rows_total(); // rows in root and recursively

        private:
            static void on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, void*);
            static void on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, void*);
            static void on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, void*);
            static void on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, void*);

            static GListModel* on_tree_list_model_create(void* item, void* user_data);
            static void on_tree_list_model_destroy(void* item);

            std::vector<GtkSignalListItemFactory*> _factories;

            GtkColumnView* _column_view;

            GListStore* _root;
            GtkTreeListModel* _tree_list_model;

            GtkSelectionModel* _selection_model;
            GtkSelectionMode _selection_mode;

            GtkColumnViewColumn* get_column(size_t i);
    };

    using ColumnView = TreeColumnView;
}