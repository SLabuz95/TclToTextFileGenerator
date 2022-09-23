#include"addPreexpression.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using AddPreexpressionActionView = ActionsList_NS::AddPreexpressionActionView;
using ActionView = AddPreexpressionActionView::ActionView;

// AddPreexpressionActionView Action View Definitions -----------------------------------

AddPreexpressionActionView::AddPreexpressionActionView(QWidget* parent)
    : ActionDataView(parent)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

AddPreexpressionActionView::AddPreexpressionActionView(QWidget* parent, ActionPtr pAction)
 : AddPreexpressionActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        formattedStringList.loadRules(action.inputFormattedString().parameters());
    }
}

void AddPreexpressionActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    formattedStringList.readRules(action.inputFormattedString().parameters());
}

AddPreexpressionActionView::DataView* AddPreexpressionActionView::create(QWidget* parent, ActionRef action){
    return new AddPreexpressionActionView(parent, action);
}


