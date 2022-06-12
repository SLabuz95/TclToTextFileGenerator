#include "navigationList.hpp"
#include "Tcl2CaplPanels/ConfigEditor/ConfigViewPanel/configViewPanel.hpp"
#include "Tcl2CaplPanels/ConfigEditor/ConfigTabsPanel/configTabsPanel.hpp"
#include "Elements/procedureElement.hpp"
#include<QMouseEvent>
#include<QMessageBox>
#include<QResizeEvent>

using namespace Panels::Configuration::Navigation;
using ConfigViewPanel = Panels::Configuration::View::ConfigViewPanel;
using ProcedureElement = Procedure::ProcedureElement;
using DefaultProcedureElement = Procedure::DefaultProcedureElement;

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
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    //setIndentation(0);

    QList<QTreeWidgetItem*> items(panelType2number(PanelType::Size), nullptr);
    for(decltype(items)::Iterator item = items.begin(); item < items.end(); item++){
        if(number2panelType(item - items.begin()) == PanelType::DefaultProcedure)
            *item = new DefaultProcedureElement(this, {navigationPanelNames[item - items.begin()]});
        else{
            *item = new NavigationElement(this, {navigationPanelNames[item - items.begin()]});
        }
        (*item)->setFirstColumnSpanned(true);

    }

    // Add procedures children

    // A

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
                DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(topLevelItem(indexOfMainNavigationElement));

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
    case QEvent::ContextMenu:
    {
        if(obj == viewport()){
            QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);
            QTreeWidgetItem* item = itemAt(cev->pos());
            if(item){
                NavigationElement* nitem = static_cast<NavigationElement*>(item);
                const int indexOfMainNavigationElement = indexOf(nitem);
                if(indexOfMainNavigationElement == -1){ // Not found -> Item is child of main navigation element
                    const int& indexOfMainNavigationElementChild = indexOfMainNavigationElement;

                }else{
                    DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(topLevelItem(indexOfMainNavigationElement));
                    dPE.menuControl(cev, static_cast<DefaultProcedureElement::ListItem*>(item));
                    //navigateMainPanel(number2panelType(indexOfMainNavigationElement));
                }
            }
        }

    }
        break;
    case QEvent::ChildRemoved:
    {

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
