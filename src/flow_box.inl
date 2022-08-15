// 
// Copyright 2022 Clemens Cords
// Created on 8/14/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    FlowBox::FlowBox()
    {
        _native = GTK_FLOW_BOX(gtk_flow_box_new());
        set_selection_mode(GTK_SELECTION_NONE);
    }

    GtkWidget* FlowBox::get_native()
    {
        return GTK_WIDGET(_native);
    }

    void FlowBox::set_activate_on_single_click(bool b)
    {
        gtk_flow_box_set_activate_on_single_click(_native, b);
    }

    void FlowBox::set_min_children_per_line(size_t n)
    {
        gtk_flow_box_set_min_children_per_line(_native, n);
    }

    void FlowBox::set_max_children_per_line(size_t n)
    {
        gtk_flow_box_set_max_children_per_line(_native, n);
    }

    void FlowBox::set_column_spacing(float v)
    {
        gtk_flow_box_set_column_spacing(_native, v);
    }

    void FlowBox::set_row_spacing(float v)
    {
        gtk_flow_box_set_row_spacing(_native, v);
    }

    void FlowBox::set_selection_mode(GtkSelectionMode mode)
    {
        gtk_flow_box_set_selection_mode(_native, mode);
    }

    GtkFlowBoxChild* FlowBox::get_child_at_index(size_t i)
    {
        return gtk_flow_box_get_child_at_index(_native, i);
    }

    GtkFlowBoxChild* FlowBox::get_child_at_pos(int x, int y)
    {
        return gtk_flow_box_get_child_at_pos(_native, x, y);
    }


}