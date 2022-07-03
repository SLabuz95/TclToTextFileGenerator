#include"write.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using WriteActionView = ActionsList_NS::WriteActionView;
using ActionView = WriteActionView::ActionView;

// WriteActionView Action View Definitions -----------------------------------

WriteActionView::WriteActionView(ActionView& view)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

WriteActionView::DataView* WriteActionView::create(ActionView& view, ActionRef){
    return new WriteActionView(view);
}


