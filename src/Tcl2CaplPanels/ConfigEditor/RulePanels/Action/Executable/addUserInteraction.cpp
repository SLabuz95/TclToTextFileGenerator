#include"addUserInteraction.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using AddUserInteractionActionView = ActionsList_NS::AddUserInteractionActionView;
using ActionView = AddUserInteractionActionView::ActionView;

// AddUserInteractionActionView Action View Definitions -----------------------------------

AddUserInteractionActionView::AddUserInteractionActionView(QWidget* parent)
    : ActionDataView(parent)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

AddUserInteractionActionView::AddUserInteractionActionView(QWidget* parent, ActionPtr pAction)
    : AddUserInteractionActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        formattedStringList.loadRules(action.inputFormattedString().parameters());
    }
}

void AddUserInteractionActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    formattedStringList.readRules(action.inputFormattedString().parameters());
}
AddUserInteractionActionView::DataView* AddUserInteractionActionView::create(QWidget* parent, ActionRef action){
    return new AddUserInteractionActionView(parent, action);
}


