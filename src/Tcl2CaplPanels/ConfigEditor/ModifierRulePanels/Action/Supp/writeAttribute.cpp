#include"writeAttribute.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ModifierActionsList;
using WriteAttributeActionView = ActionsList_NS::WriteAttributeActionView;
using ActionView = WriteAttributeActionView::ActionView;

// WriteAttributeActionView Action View Definitions -----------------------------------

WriteAttributeActionView::WriteAttributeActionView(QWidget* parent)
    : ActionDataView(parent)
{
    attributeNameEditor.setClearButtonEnabled(true);
    attributeNameEditor.setPlaceholderText("Nazwa");
    addWidget(&attributeNameEditor);
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

WriteAttributeActionView::WriteAttributeActionView(QWidget* parent, ActionPtr pAction)
    : WriteAttributeActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        attributeNameEditor.setText(action.name());
        formattedStringList.loadRules(action.inputFormattedString());
    }
}


void WriteAttributeActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    action.setName(attributeNameEditor.text());
    formattedStringList.readRules(action.inputFormattedString());
}

WriteAttributeActionView::DataView* WriteAttributeActionView::create(QWidget* parent, ActionRef action){
    return new WriteAttributeActionView(parent, action);
}


