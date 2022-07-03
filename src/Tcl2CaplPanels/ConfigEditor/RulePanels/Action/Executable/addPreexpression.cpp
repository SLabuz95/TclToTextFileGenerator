#include"addPreexpression.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using AddPreexpressionActionView = ActionsList_NS::AddPreexpressionActionView;
using ActionView = AddPreexpressionActionView::ActionView;

// AddPreexpressionActionView Action View Definitions -----------------------------------

AddPreexpressionActionView::AddPreexpressionActionView(ActionView& view)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

AddPreexpressionActionView::DataView* AddPreexpressionActionView::create(ActionView& view, ActionRef){
    return new AddPreexpressionActionView(view);
}


