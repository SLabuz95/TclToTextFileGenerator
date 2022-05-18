#include"configTabsPanel.hpp"
#include "Tcl2CaplPanels/ConfigEditor/configEditor.hpp"
#include "Panels/attributesPanel.hpp"
#include "Tcl2CaplPanels/ConfigEditor/writeOnlyProceduresList.hpp"

using namespace Panels::Configuration;
using ConfigPanel = List::ConfigPanel;

ConfigTabsPanel::ConfigTabsPanel(ConfigPanel& parent)
    : Super(&parent), configPanel(parent)
{
    panels[0] = new AttributesPanel(*this);
    panels[1] =   new WriteOnlyProceduresList(*this);
    panels[2] =    new QWidget();

    for(int panelNameIndex = 0; panelNameIndex < panelType2number(PanelType::Size) ; panelNameIndex++){
        addTab(panels[panelNameIndex], Navigation::List::navigationPanelNames[panelNameIndex]);
        panels[panelNameIndex]->show();
    }

}

bool ConfigTabsPanel::eventFilter(QObject* obj, QEvent* ev){

    return Super::eventFilter(obj, ev);
}

AttributesPanel& ConfigTabsPanel::attributesList(){
    return *static_cast<AttributesPanel*>(panels[panelType2number(PanelType::AttributesList)]);
}

WriteOnlyProceduresList& ConfigTabsPanel::writeOnlyProceduresList(){
    return *static_cast<WriteOnlyProceduresList*>(panels[panelType2number(PanelType::WriteOnlyProceduresList)]);
}