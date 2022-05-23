#include "navigationList.hpp"
#include "Tcl2CaplPanels/ConfigEditor/ConfigViewPanel/configViewPanel.hpp"
#include "Tcl2CaplPanels/ConfigEditor/ConfigTabsPanel/configTabsPanel.hpp"
#include<QMouseEvent>
#include<QMessageBox>
#include<QResizeEvent>

using namespace Panels::Configuration::Navigation;
using ConfigViewPanel = Panels::Configuration::View::ConfigViewPanel;

const QString List::navigationPanelNames[panelType2number(PanelType::Size)]{
            QString("Atrybuty"),
            QString("Procedury - tryb raportowy"),
            QString("Procedury"),
            QString("Procedura domyślna")
            //QString("Priorytety"),
            //QString("Reguły szybkie"),
            //QString("Reguły zaawansowane"),
};

List::List(View::ConfigViewPanel& parent)
    : QTreeWidget(&parent), configPanel(parent)
{
    setHeaderHidden(true);
    setIndentation(0);

    QList<QTreeWidgetItem*> items(panelType2number(PanelType::Size), nullptr);
    for(decltype(items)::Iterator item = items.begin(); item < items.end(); item++){
        *item = new NavigationElement(this, {navigationPanelNames[item - items.begin()]});
        (*item)->setFirstColumnSpanned(true);
    }


    addTopLevelItems(items);
    viewport()->installEventFilter(this);
    installEventFilter(this);
}

bool List::eventFilter(QObject* obj, QEvent* ev){
    if(ev->type() == QEvent::MouseButtonDblClick and obj == viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        NavigationElement* item = itemAt(mev->pos());
        if(item){
            const int indexOfMainNavigationElement = indexOf(item);
            if(indexOfMainNavigationElement == -1){ // Not found -> Item is child of main navigation element
                const int& indexOfMainNavigationElementChild = indexOfMainNavigationElement;

            }else{
                navigateMainPanel(number2panelType(indexOfMainNavigationElement));
            }
        }
    }
    //  Hide / unhide
    switch(ev->type()){
    case QEvent::Resize:
    {
        if(obj == this){
            QResizeEvent& resizeEvent = *static_cast<QResizeEvent*>(ev);

            if(resizeEvent.oldSize().width() == 0){
                if( resizeEvent.size().width() != 0){   // Appear Condition
                    configPanel.navigationListAppeared();
                }
            }else{
                if(resizeEvent.size().width() == 0){ // Disappear Condition
                    configPanel.navigationListDisappeared();
                }
            }
        }
    }
        break;
    default:
        break;
    }

    return Super::eventFilter(obj, ev);
}

void List::navigateMainPanel(const PanelType tabPanel){
    View::ConfigTabs& configTabsPanel = configPanel.Panels::Super::ViewPanel::Super::get();
    configTabsPanel.setCurrentIndex(panelType2number(tabPanel));
}

template<>
void List::navigationElementChildFct<PanelType::AttributesList>(NavigationElement* const element){

}

template<>
void List::navigationElementChildFct<PanelType::WriteOnlyProceduresList>(NavigationElement* const element){

}

template<>
void List::navigationElementChildFct<PanelType::Procedures>(NavigationElement* const element){

}


List::NavigationElementChildFctPtr List::navigationElementChildFcts[panelType2number(PanelType::Size)]
{
    &List::navigationElementChildFct<PanelType::AttributesList>,
    &List::navigationElementChildFct<PanelType::WriteOnlyProceduresList>,
    &List::navigationElementChildFct<PanelType::Procedures>,
};
