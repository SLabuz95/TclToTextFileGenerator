#include "navigationList.hpp"
#include "Tcl2CaplPanels/ConfigEditor/configEditor.hpp"

using namespace Panels::Configuration::Navigation;

const QString List::navigationPanelNames[panel2number(PanelTypes::Size)]{
            QString("Atrybuty"),
            QString("Procedury - tryb raportowy"),
            QString("Procedury"),
            QString("Priorytety"),
            QString("Reguły szybkie"),
            QString("Reguły zaawansowane"),
};

List::List(ConfigPanel& parent)
    : configPanel(parent)
{
    QList<QTreeWidgetItem*> items(sizeof(navigationPanelNames), nullptr);
    for(decltype(items)::Iterator item = items.begin(); item < items.end(); item++){
        *item = new QTreeWidgetItem(this, {navigationPanelNames[item - items.begin()]});
    }

    addTopLevelItems(items);
}
