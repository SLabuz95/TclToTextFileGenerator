#ifndef CONFIGTABSPANEL_HPP
#define CONFIGTABSPANEL_HPP

#include<QTabWidget>
#include"Tcl2CaplPanels/ConfigEditor/NavigationList/navigationList.hpp"


namespace Panels::Configuration{
namespace View{
    namespace Rules{
        namespace RulesProcedurePanel{
        class RulesPanel;
        }
    }
    class ConfigViewPanel;
}

    class AttributesPanel;
    class WriteOnlyProceduresList;

    class ConfigTabsPanel : public QTabWidget{

        using Super = QTabWidget;
        using RulesProcedurePanel = View::Rules::RulesProcedurePanel::RulesPanel;
    public:
        ConfigTabsPanel(View::ConfigViewPanel& parent);
        virtual ~ConfigTabsPanel() override{}

        View::ConfigViewPanel& configPanel;

        QWidget* panels[Navigation::panelType2number(Navigation::PanelType::Size)];

        AttributesPanel& attributesList();
        WriteOnlyProceduresList& writeOnlyProceduresList();
        RulesProcedurePanel& rulesProcedureList();
        RulesProcedurePanel& rulesDefaultProcedureList();

        WIDGET_ADD_EVENT_FILTER

    };

}
#endif // CONFIGTABSPANEL_HPP
