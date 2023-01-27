//
// Created by clem on 1/27/23.
//

#include <app/resize_canvas_dialog.hpp>

namespace mousetrap
{

    ResizeCanvasDialog::AlignmentSelector::ButtonAndArrow::ButtonAndArrow(Alignment alignment, const std::string& id)
        : _alignment(alignment), _arrow(get_resource_path() + "icons/alignment_arrow_center.png") //" + id)
    {
        _button.set_child(&_arrow);
    }

    ResizeCanvasDialog::AlignmentSelector::ButtonAndArrow::operator Widget*()
    {
        return &_button;
    }

    ResizeCanvasDialog::AlignmentSelector::AlignmentSelector(ResizeCanvasDialog* owner)
        : _owner(owner)
    {
        _arrows = {
            {TOP_LEFT, new ButtonAndArrow(TOP_LEFT, "top_left")},
            {TOP, new ButtonAndArrow(TOP, "top")},
            {TOP_RIGHT, new ButtonAndArrow(TOP_RIGHT, "top_right")},
            {RIGHT, new ButtonAndArrow(RIGHT, "right")},
            {BOTTOM_RIGHT, new ButtonAndArrow(BOTTOM_RIGHT, "bottom_right")},
            {BOTTOM, new ButtonAndArrow(BOTTOM, "bottom")},
            {BOTTOM_LEFT, new ButtonAndArrow(BOTTOM_LEFT, "bottom_left")},
            {LEFT, new ButtonAndArrow(LEFT, "left")},
            {CENTER, new ButtonAndArrow(CENTER, "center")}
        };

        _top_row.push_back(*_arrows.at(TOP_LEFT));
        _top_row.push_back(*_arrows.at(TOP));
        _top_row.push_back(*_arrows.at(TOP_RIGHT));

        _center_row.push_back(*_arrows.at(LEFT));
        _center_row.push_back(*_arrows.at(CENTER));
        _center_row.push_back(*_arrows.at(RIGHT));

        _bottom_row.push_back(*_arrows.at(BOTTOM_LEFT));
        _bottom_row.push_back(*_arrows.at(BOTTOM));
        _bottom_row.push_back(*_arrows.at(BOTTOM_RIGHT));

        _main.push_back(&_top_row);
        _main.push_back(&_center_row);
        _main.push_back(&_bottom_row);
    }

    ResizeCanvasDialog::AlignmentSelector::operator Widget*()
    {
        return &_main;
    }

    ResizeCanvasDialog::ResizeCanvasDialog()
    {
        _window.set_child(_alignment_selector);
    }

    ResizeCanvasDialog::operator Widget*()
    {
        return &_window;
    }

    void ResizeCanvasDialog::present()
    {
        _window.present();
    }
}

/*
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
        _arrows =  {
            {TOP_LEFT, ButtonAndArrow(TOP_LEFT, "top_left")},
            {TOP, ButtonAndArrow(TOP, "top")},
            {TOP_RIGHT, ButtonAndArrow(TOP_RIGHT, "top_right")},
            {RIGHT, ButtonAndArrow(RIGHT, "right")},
            {BOTTOM_RIGHT, ButtonAndArrow(BOTTOM_RIGHT, "bottom_right")},
            {BOTTOM, ButtonAndArrow(BOTTOM, "bottom")},
            {BOTTOM_LEFT, ButtonAndArrow(BOTTOM_LEFT, "bottom_left")},
            {LEFT, ButtonAndArrow(LEFT, "left")},
            {CENTER, ButtonAndArrow(CENTER, "center")}
        };

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
*/