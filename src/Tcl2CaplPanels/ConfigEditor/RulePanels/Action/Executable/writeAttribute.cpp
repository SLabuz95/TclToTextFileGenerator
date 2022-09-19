#include"writeAttribute.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using WriteAttributeActionView = ActionsList_NS::WriteAttributeActionView;
using ActionView = WriteAttributeActionView::ActionView;

// WriteAttributeActionView Action View Definitions -----------------------------------

WriteAttributeActionView::WriteAttributeActionView(QWidget* parent)
    : ActionDataView(parent)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

WriteAttributeActionView::WriteAttributeActionView(QWidget* parent, ActionPtr pAction)
    : WriteAttributeActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        formattedStringList.loadRules(action.inputFormattedString().parameters());
    }
}


void WriteAttributeActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    formattedStringList.readRules(action.inputFormattedString().parameters());
}

WriteAttributeActionView::DataView* WriteAttributeActionView::create(QWidget* parent, ActionRef action){
    return new WriteAttributeActionView(parent, action);
}


