#include"addPredefinition.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using AddPredefinitionActionView = ActionsList_NS::AddPredefinitionActionView;
using ActionView = AddPredefinitionActionView::ActionView;

// AddPredefinitionActionView Action View Definitions -----------------------------------

AddPredefinitionActionView::AddPredefinitionActionView(QWidget* parent)
    : ActionDataView(parent)
{
    mainLayout.setVerticalSpacing(0);
    mainLayout.setContentsMargins(0,0,0,0);
    mainLayout.addRow("Typ akcji:",&scopeComboBox);
    scopeComboBox.installEventFilter(this);
    scopeComboBox.view()->installEventFilter(this);
    splitter.addWidget(&formattedStringList);
    mainLayout.addRow(&splitter);
    addLayout(&mainLayout);
}

AddPredefinitionActionView::AddPredefinitionActionView(QWidget* parent, ActionPtr pAction)
 : AddPredefinitionActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        scopeComboBox.setCurrentIndex(PredefinitionsController::cast_scope(action.scope()));
        formattedStringList.loadRules(action.inputFormattedString().parameters());
    }
}

void AddPredefinitionActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    action.setScope(PredefinitionsController::castTo_scope(scopeComboBox.currentIndex()));
    formattedStringList.readRules(action.inputFormattedString().parameters());
}

AddPredefinitionActionView::DataView* AddPredefinitionActionView::create(QWidget* parent, ActionRef action){
    return new AddPredefinitionActionView(parent, action);
}


