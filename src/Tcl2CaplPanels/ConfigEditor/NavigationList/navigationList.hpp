#ifndef NAVIGATIONLIST_HPP
#define NAVIGATIONLIST_HPP

#include<QTreeWidget>

namespace  Panels::Configuration{
    class Panel;
}

namespace Panels::Configuration::Navigation{

    enum class PanelTypes{
        AttributesList,
        WriteOnlyProceduresList,
        Procedures,
        RulesManager,
        QuickRulesList,
        AdvancedRulesList,
        Size
    };

    constexpr static std::underlying_type_t<PanelTypes> panel2number(PanelTypes mode){
        return static_cast<std::underlying_type_t<PanelTypes>>(mode);
    }

    class NavigationElement : private QTreeWidgetItem{

    };

    class List : private QTreeWidget{

        using ConfigPanel = Configuration::Panel;

        static const QString navigationPanelNames[panel2number(PanelTypes::Size)];

        List(ConfigPanel& parent);

        ~List();

        ConfigPanel& configPanel;

    };

};


#endif // NAVIGATIONLIST_HPP
