#include "navigationList.hpp"
#include "Tcl2CaplPanels/ConfigEditor/configEditor.hpp"
#include<QMouseEvent>
#include<QMessageBox>
#include<QResizeEvent>

using namespace Panels::Configuration::Navigation;
using ConfigPanel = List::ConfigPanel;

const QString List::navigationPanelNames[panelType2number(PanelType::Size)]{
            QString("Atrybuty"),
            QString("Procedury - tryb raportowy"),
            QString("Procedury"),
            //QString("Priorytety"),
            //QString("Reguły szybkie"),
            //QString("Reguły zaawansowane"),
};

List::List(ConfigPanel& parent)
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
            int indexOfMainNavigationElement = indexOf(item);
            if(indexOfMainNavigationElement == -1){ // Not found -> Item is child of main navigation element
                int& indexOfMainNavigationElementChild = indexOfMainNavigationElement;

            }else{
                callNavigationElementFct(number2panelType(indexOfMainNavigationElement));
            }
        }
    }
    //  Hide / unhide
    switch(ev->type()){
    case QEvent::Resize:
    {
        if(obj == this){
            QResizeEvent& resizeEvent = *static_cast<QResizeEvent*>(ev);

            if(resizeEvent.size().width() == 0)
                configPanel.navigationListDisappeared();
            else
                configPanel.navigationListAppeared();
        }
    }
        break;
    default:
        break;
    }

    return Super::eventFilter(obj, ev);
}

template<>
void List::navigationElementFct<PanelType::AttributesList>(){

}

template<>
void List::navigationElementFct<PanelType::WriteOnlyProceduresList>(){

}

template<>
void List::navigationElementFct<PanelType::Procedures>(){

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

List::NavigationElementFctPtr List::navigationElementFcts[panelType2number(PanelType::Size)]
{
    &List::navigationElementFct<PanelType::AttributesList>,
    &List::navigationElementFct<PanelType::WriteOnlyProceduresList>,
    &List::navigationElementFct<PanelType::Procedures>,
};

List::NavigationElementChildFctPtr List::navigationElementChildFcts[panelType2number(PanelType::Size)]
{
    &List::navigationElementChildFct<PanelType::AttributesList>,
    &List::navigationElementChildFct<PanelType::WriteOnlyProceduresList>,
    &List::navigationElementChildFct<PanelType::Procedures>,
};
