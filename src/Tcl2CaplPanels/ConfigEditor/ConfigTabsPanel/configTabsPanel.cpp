#include"configTabsPanel.hpp"
#include"Tcl2CaplPanels/ConfigEditor/ConfigViewPanel/configViewPanel.hpp"
#include "Panels/attributesPanel.hpp"
#include "Tcl2CaplPanels/ConfigEditor/writeOnlyProceduresList.hpp"
#include "Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.hpp"

using namespace Panels::Configuration;
using RulesProcedurePanel = View::Rules::RulesProcedurePanel::Panel;
using ConfigViewPanel = Panels::Configuration::View::ConfigViewPanel;


ConfigTabsPanel::ConfigTabsPanel(ConfigViewPanel& parent)
    : Super(&parent), configPanel(parent)
{
    panels[0] = new AttributesPanel(*this);
    panels[1] = new WriteOnlyProceduresList(*this);
    panels[2] = new RulesProcedurePanel();
    panels[3] = new RulesProcedurePanel();

    for(int panelNameIndex = 0; panelNameIndex < panelType2number(PanelType::Size) ; panelNameIndex++){
        addTab(panels[panelNameIndex], Navigation::List::navigationPanelNames[panelNameIndex]);
        //panels[panelNameIndex]->show();
    }

}

bool ConfigTabsPanel::eventFilter(QObject* obj, QEvent* ev){

    return Super::eventFilter(obj, ev);
}


AttributesPanel& ConfigTabsPanel::attributesList(){
    return *static_cast<AttributesPanel*>(panels[0]);
}
WriteOnlyProceduresList& ConfigTabsPanel::writeOnlyProceduresList(){
    return *static_cast<WriteOnlyProceduresList*>(panels[1]);
}
RulesProcedurePanel& ConfigTabsPanel::rulesProcedureList(){
    return *static_cast<RulesProcedurePanel*>(panels[2]);
}
RulesProcedurePanel& ConfigTabsPanel::rulesDefaultProcedureList(){
    return *static_cast<RulesProcedurePanel*>(panels[3]);
}
