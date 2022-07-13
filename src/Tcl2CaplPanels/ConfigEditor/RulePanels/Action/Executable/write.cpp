#include"write.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using WriteActionView = ActionsList_NS::WriteActionView;
using ActionView = WriteActionView::ActionView;

// WriteActionView Action View Definitions -----------------------------------

WriteActionView::WriteActionView(QWidget* parent)
    : ActionDataView(parent)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

WriteActionView::WriteActionView(QWidget* parent, ActionPtr pAction)
    : WriteActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        formattedStringList.loadRules(action.inputFormattedString().parameters());
    }
}


void WriteActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    formattedStringList.readRules(action.inputFormattedString().parameters());
}

WriteActionView::DataView* WriteActionView::create(QWidget* parent, ActionRef action){
    return new WriteActionView(parent, action);
}


