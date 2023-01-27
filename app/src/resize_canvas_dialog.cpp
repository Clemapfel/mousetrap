//
// Created by clem on 1/27/23.
//

#include <app/resize_canvas_dialog.hpp>

namespace mousetrap
{
    ResizeCanvasDialog::AlignmentSelector::ButtonAndArrow::ButtonAndArrow(Alignment alignment, const std::string& id)
        : alignment(alignment), arrow(get_resource_path() + "icons/alignment_arrow_center.png") //" + id)
    {
        button.set_child(&arrow);
    }

    ResizeCanvasDialog::AlignmentSelector::ButtonAndArrow::operator Widget*()
    {
        return &button;
    }

    ResizeCanvasDialog::AlignmentSelector::AlignmentSelector(ResizeCanvasDialog* owner)
        : _owner(owner)
    {
        _top_row.push_back(_arrows.at(TOP_LEFT));
        _top_row.push_back(_arrows.at(TOP));
        _top_row.push_back(_arrows.at(TOP_RIGHT));

        _center_row.push_back(_arrows.at(LEFT));
        _center_row.push_back(_arrows.at(CENTER));
        _center_row.push_back(_arrows.at(RIGHT));

        _bottom_row.push_back(_arrows.at(BOTTOM_LEFT));
        _bottom_row.push_back(_arrows.at(BOTTOM));
        _bottom_row.push_back(_arrows.at(BOTTOM_RIGHT));

        _main.push_back(&_top_row);
        _main.push_back(&_center_row);
        _main.push_back(&_bottom_row);
    }

    ResizeCanvasDialog::AlignmentSelector::operator Widget*()
    {
        return &_main;
    }

    ResizeCanvasDialog::ResizeCanvasDialog()
    {}

    ResizeCanvasDialog::operator Widget*()
    {
        return &_dialog;
    }

    void ResizeCanvasDialog::present()
    {
        _dialog.present();
    }
}
