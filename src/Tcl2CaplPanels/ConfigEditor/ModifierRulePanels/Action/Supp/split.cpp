#include"split.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ModifierActionsList;
using SplitActionView = ActionsList_NS::SplitActionView;
using ActionView = SplitActionView::ActionView;

// SplitActionView Action View Definitions -----------------------------------

SplitActionView::SplitActionView(QWidget* parent)
    : ActionDataView(parent)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

SplitActionView::SplitActionView(QWidget* parent, ActionPtr pAction)
    : SplitActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        formattedStringList.loadRules(action.inputFormattedString());
    }
}


void SplitActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    formattedStringList.readRules(action.inputFormattedString());
}

SplitActionView::DataView* SplitActionView::create(QWidget* parent, ActionRef action){
    return new SplitActionView(parent, action);
}


