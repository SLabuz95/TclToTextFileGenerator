#include"changePhase.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>
#include<QInputDialog>
#include"Tcl2CaplPanels/ConfigEditor/ModifierRulePanels/modifierRawRule.hpp"
#include"Tcl2CaplPanels/ConfigEditor/rulesphasepanel.hpp"

namespace ActionsList_NS = Panels::Configuration::View::ModifierActionsList;
using ChangePhaseActionView = ActionsList_NS::ChangePhaseActionView;
using ActionView = ChangePhaseActionView::ActionView;

// CompareNumbOfArgs Action View Definitions -----------------------------------

ChangePhaseActionView::ChangePhaseActionView(QWidget* parent)
 : ActionDataView(parent)
{
    phaseNameEditor.setClearButtonEnabled(true);
    phaseNameEditor.setPlaceholderText("Nazwa");
    addWidget(&phaseNameEditor);
    checkRulesFlag.setText("Weryfikuj reguły");
    addWidget(&checkRulesFlag);
}

ChangePhaseActionView::ChangePhaseActionView(QWidget* parent, ActionPtr pAction)
    : ChangePhaseActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        phaseNameEditor.setText(action.phaseName());
        checkRulesFlag.setChecked(action.checkRule());
    }
}

void ChangePhaseActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    action.setPhaseName(phaseNameEditor.text());
    action.setCheckRule(checkRulesFlag.isChecked());
}

ChangePhaseActionView::DataView* ChangePhaseActionView::create(QWidget* parent, ActionRef action){
    return new ChangePhaseActionView(parent, action);
}

