#ifndef CONFIGTABSPANEL_HPP
#define CONFIGTABSPANEL_HPP

#include<QTabWidget>
#include"Tcl2CaplPanels/ConfigEditor/NavigationList/navigationList.hpp"

namespace Panels::Configuration{

    class Panel;
    class AttributesPanel;
    class WriteOnlyProceduresList;

    class ConfigTabsPanel : public QTabWidget{

        using Super = QTabWidget;
    public:
        using ConfigPanel = Configuration::Panel;
        ConfigTabsPanel(ConfigPanel& parent);
        virtual ~ConfigTabsPanel() override{}

        ConfigPanel& configPanel;

        QWidget* panels[Navigation::panelType2number(Navigation::PanelType::Size)];

        AttributesPanel& attributesList();
        WriteOnlyProceduresList& writeOnlyProceduresList();

        WIDGET_ADD_EVENT_FILTER

    };

}
#endif // CONFIGTABSPANEL_HPP
