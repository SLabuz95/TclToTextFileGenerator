#include "instanceList.hpp"
#include"Tcl2CaplPanels/MainApp/mainApp.hpp"
#include<QResizeEvent>
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::AddInstance>(InstanceListElement* )
{

    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {new InstanceListElement, QString()};
    addTopLevelItem(curEditItemInfo.item);
    editItem(curEditItemInfo.item);
}

template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::EditInstance>(InstanceListElement* item)
{
    Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::editInstance", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {item, item->text(0)};
    editItem(item, 0);
}

template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::RemoveInstance>(InstanceListElement* item)
{
    Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::removeInstance", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    //tryToManageAttributesName(item->text(0), QString());
    delete item;
}

InstanceList::InstanceList(MainWindow& parent)
    : QTreeWidget(&parent), mainApp(parent)
{
    setHeaderHidden(true);
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    //setIndentation(0);

    viewport()->installEventFilter(this);
    installEventFilter(this);
}

bool InstanceList::eventFilter(QObject* obj, QEvent* ev){
    if(ev->type() == QEvent::MouseButtonDblClick and obj == viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        InstanceListElement* item = itemAt(mev->pos());
        if(item){
            /*const int indexOfMainNavigationElement = indexOf(item);
            if(indexOfMainNavigationElement == -1){ // Not found -> Item is child of main navigation element
                const int& indexOfMainNavigationElementChild = indexOfMainNavigationElement;
                DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(topLevelItem(indexOfMainNavigationElement));

            }else{
                navigateMainPanel(number2panelType(indexOfMainNavigationElement));
            }*/
        }
    }
    //  Hide / unhide
    switch(ev->type()){
    case QEvent::Resize:
    {
        /*if(obj == this){
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
        }*/
    }
        break;
    case QEvent::ContextMenu:
    {
        if(obj == viewport()){
            QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);
            InstanceListElement* item = itemAt(cev->pos());
            /*if(item){
                NavigationElement* nitem = static_cast<NavigationElement*>(item);
                const int indexOfMainNavigationElement = indexOf(nitem);
                if(indexOfMainNavigationElement == -1){ // Not found -> Item is child of main navigation element
                    const int& indexOfMainNavigationElementChild = indexOfMainNavigationElement;

                }else{
                    DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(topLevelItem(indexOfMainNavigationElement));
                    dPE.menuControl(cev, static_cast<DefaultProcedureElement::ListItem*>(item));
                    //navigateMainPanel(number2panelType(indexOfMainNavigationElement));
                }
            }*/

            using Actions = QList<QAction*>;
            using ActionFuncs = QList<InstanceList::Request_ContextMenu_Func>;
            using Request = InstanceList::Request_ContextMenu;
            Actions actions;
            ActionFuncs actionFuncs;
            QMenu* menu = nullptr;

            menu = new QMenu;
            if(item){

                actions = {
                  new QAction("Dodaj instancje"),
                  new QAction("Edytuj nazwę instancji"),
                  new QAction("Usuń instancje")
                };
                actionFuncs = {
                    &InstanceList::execRequest_ContextMenu<Request::AddInstance>,
                    &InstanceList::execRequest_ContextMenu<Request::EditInstance>,
                    &InstanceList::execRequest_ContextMenu<Request::RemoveInstance>,
                };
            }else{
                actions = {
                    new QAction("Dodaj instancje")
                };
                actionFuncs = {
                    &InstanceList::execRequest_ContextMenu<Request::AddInstance>,
                };
            }

            // After configuration
            if(menu){
                menu->addActions(actions);
                int index = actions.indexOf( menu->exec(cev->globalPos()));
                if(index >= 0){
                    Q_ASSERT_X(index < actionFuncs.size(), "AttributesPanel Menu", "Index error for action functions");
                    (this->*(actionFuncs.at(index)))(item);
                }
                delete menu, menu = nullptr;
            }
        }

    }
        break;
    case QEvent::ChildRemoved:
    {
        if(obj == viewport()){
            QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);
            QTreeWidgetItem* item = itemAt(cev->pos());
            if(item){
               /* InstanceListElement* nitem = static_cast<InstanceListElement*>(item);
                const int indexOfMainNavigationElement = indexOf(nitem);
                if(indexOfMainNavigationElement == -1){ // Not found -> Item is child of main navigation element
                    const int& indexOfMainNavigationElementChild = indexOfMainNavigationElement;

                }else{
                    DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(topLevelItem(indexOfMainNavigationElement));
                    dPE.menuControl(cev, static_cast<DefaultProcedureElement::ListItem*>(item));
                    //navigateMainPanel(number2panelType(indexOfMainNavigationElement));
                }*/
            }
        }
    }
    break;
    default:
        break;
    }
    return Super::eventFilter(obj, ev);
}
