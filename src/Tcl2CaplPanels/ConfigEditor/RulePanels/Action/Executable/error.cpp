#include"error.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using ErrorActionView = ActionsList_NS::ErrorActionView;
using ActionView = ErrorActionView::ActionView;

// ErrorActionView Action View Definitions -----------------------------------

ErrorActionView::ErrorActionView(ActionView& view)
{
    splitter.addWidget(&formattedStringList);
    addRow(&splitter);
}

ErrorActionView::DataView* ErrorActionView::create(ActionView& view, ActionRef){
    return new ErrorActionView(view);
}


