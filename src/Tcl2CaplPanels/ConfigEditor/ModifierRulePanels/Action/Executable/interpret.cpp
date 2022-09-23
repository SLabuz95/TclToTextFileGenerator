#include"interpret.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ModifierActionsList;
using InterpretActionView = ActionsList_NS::InterpretActionView;
using ActionView = InterpretActionView::ActionView;

// InterpretActionView Action View Definitions -----------------------------------

InterpretActionView::InterpretActionView(QWidget* parent)
    : ActionDataView(parent)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

InterpretActionView::InterpretActionView(QWidget* parent, ActionPtr pAction)
    : InterpretActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        formattedStringList.loadRules(action.inputFormattedString());
    }
}


void InterpretActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    formattedStringList.readRules(action.inputFormattedString());
}

InterpretActionView::DataView* InterpretActionView::create(QWidget* parent, ActionRef action){
    return new InterpretActionView(parent, action);
}


