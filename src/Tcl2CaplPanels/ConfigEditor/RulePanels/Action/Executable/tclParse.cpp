#include"tclParse.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using TclParseActionView = ActionsList_NS::TclParseActionView;
using ActionView = TclParseActionView::ActionView;

// TclParseActionView Action View Definitions -----------------------------------

TclParseActionView::TclParseActionView(QWidget* parent)
    : ActionDataView(parent)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

TclParseActionView::TclParseActionView(QWidget* parent, ActionPtr pAction)
    : TclParseActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        formattedStringList.loadRules(action.inputFormattedString().parameters());
    }
}


void TclParseActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    formattedStringList.readRules(action.inputFormattedString().parameters());
}

TclParseActionView::DataView* TclParseActionView::create(QWidget* parent, ActionRef action){
    return new TclParseActionView(parent, action);
}


