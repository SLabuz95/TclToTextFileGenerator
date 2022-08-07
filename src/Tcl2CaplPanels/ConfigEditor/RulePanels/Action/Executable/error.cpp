#include"error.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using ErrorActionView = ActionsList_NS::ErrorActionView;
using ActionView = ErrorActionView::ActionView;

// ErrorActionView Action View Definitions -----------------------------------

ErrorActionView::ErrorActionView(QWidget* parent)
    : ActionDataView(parent)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

ErrorActionView::ErrorActionView(QWidget* parent, ActionPtr pAction)
    : ErrorActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        formattedStringList.loadRules(action.inputFormattedString().parameters());
    }
}

void ErrorActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    formattedStringList.readRules(action.inputFormattedString().parameters());
}

ErrorActionView::DataView* ErrorActionView::create(QWidget* parent, ActionRef action){
    return new ErrorActionView(parent, action);
}


