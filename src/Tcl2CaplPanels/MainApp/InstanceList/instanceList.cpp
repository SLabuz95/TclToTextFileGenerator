#include "instanceList.hpp"
#include"Tcl2CaplPanels/MainApp/mainApp.hpp"
#include<QResizeEvent>
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>
#include"Tcl2CaplPanels/MainApp/Interpreters/tcl2caplfileparserinstance.hpp"
#include"Tcl2CaplPanels/MainApp/Interpreters/tcl2caplinstructionpanel.hpp"
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"

template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::AddFilesInterpreter>(InstanceListElement* callerItem)
{
    if(callerItem->parent() != nullptr){ // Child of Instance Item (Config Item, Files/Text Interpreter Item)Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::editInstance", "Item is null");
       // Convert to Child of main list (Instance Item)
        callerItem = static_cast<InstanceListElement*>(callerItem->parent());
    }
    // OR Child of main list (Instance Item)
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    InstanceListElementInterpreter* child = new InstanceListElementInterpreter("Files Interpreter");
    callerItem->addChild(child);
    mainApp.insertSubWindow(getSubWindowPositionInfo(child), new Tcl2CaplFileParserInstance);
}


template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::AddTextInterpreter>(InstanceListElement* callerItem)
{
    if(callerItem->parent() != nullptr){ // Child of Instance Item (Config Item, Files/Text Interpreter Item)Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::editInstance", "Item is null");
       // Convert to Child of main list (Instance Item)
        callerItem = static_cast<InstanceListElement*>(callerItem->parent());
    }
    // OR Child of main list (Instance Item)
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    InstanceListElementInterpreter* child = new InstanceListElementInterpreter("Text Interpreter");
    callerItem->addChild(child);
    mainApp.insertSubWindow(getSubWindowPositionInfo(child), new Tcl2CaplInstructionPanel);
}

template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::EditInterpreterName>(InstanceListElement* item)
{
    Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::EditInterpreterName", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    curEditItemInfo = {item, item->text(0)};
    editItem(item, 0);
}

template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::RemoveInterpreter>(InstanceListElement* item)
{
    Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::RemoveInterpreter", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    mainApp.removeSubWindow(getSubWindowPositionInfo(item));
    delete item;
}

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
    if(item->parent() != nullptr){ // Child of Instance Item (Config Item, Files/Text Interpreter Item)Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::editInstance", "Item is null");
       // Convert to Child of main list (Instance Item)
        item = static_cast<InstanceListElement*>(item->parent());
    }
    // OR Child of main list (Instance Item)
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
    if(item->parent() != nullptr){ // Child of Instance Item (Config Item, Files/Text Interpreter Item)Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::editInstance", "Item is null");
       // Convert to Child of main list (Instance Item)
        item = static_cast<InstanceListElement*>(item->parent());
    }
    // OR Child of main list (Instance Item)
    Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::removeInstance", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    mainApp.removeInstance(item);
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
void InstanceList::contextMenuEvent(QContextMenuEvent *cev){
    enum class MenuType{
        ViewPort,
        InstanceItem,
        InstanceChildItem
    } menuType;
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
    Actions instanceActions;
    ActionFuncs actionFuncs;
    QMenu* menu = nullptr;
    QMenu* instanceMenu = nullptr;

    menu = new QMenu;
    if(item){
        // Menu Type
        const int indexOfInstanceElement = indexOf(item);
        if(indexOfInstanceElement == -1){ // Not found -> Item is child of instance list item
            if(item->parent()->indexOfChild(item) == 0){ // Is Config Item
                instanceActions = {
                    new QAction("Dodaj instancje"),
                    new QAction("Edytuj nazwę instancji"),
                    new QAction("Usuń instancje")
                };
                instanceMenu = new QMenu("Instancje");
                instanceMenu->addActions(instanceActions);
                actions = {
                    new QAction("Dodaj interpreter plikow"),
                    new QAction("Dodaj interpreter tekstu"),
                };
                menu->addActions(actions);
                actions.append(instanceActions);
                actionFuncs = {
                    &InstanceList::execRequest_ContextMenu<Request::AddFilesInterpreter>,
                    &InstanceList::execRequest_ContextMenu<Request::AddTextInterpreter>,
                    &InstanceList::execRequest_ContextMenu<Request::AddInstance>,
                    &InstanceList::execRequest_ContextMenu<Request::EditInstance>,
                    &InstanceList::execRequest_ContextMenu<Request::RemoveInstance>,
                };
            }else{ // Otherwise
                instanceActions = {
                    new QAction("Dodaj instancje"),
                    new QAction("Edytuj nazwę instancji"),
                    new QAction("Usuń instancje")
                };
                instanceMenu = new QMenu("Instancje");
                instanceMenu->addActions(instanceActions);
                actions = {
                    new QAction("Dodaj interpreter plikow"),
                    new QAction("Dodaj interpreter tekstu"),
                    new QAction("Edytuj nazwe interpretera"),
                    new QAction("Usuń interpreter"),
                };
                menu->addActions(actions);
                actions.append(instanceActions);
                actionFuncs = {
                    &InstanceList::execRequest_ContextMenu<Request::AddFilesInterpreter>,
                    &InstanceList::execRequest_ContextMenu<Request::AddTextInterpreter>,
                    &InstanceList::execRequest_ContextMenu<Request::EditInterpreterName>,
                    &InstanceList::execRequest_ContextMenu<Request::RemoveInterpreter>,
                    &InstanceList::execRequest_ContextMenu<Request::AddInstance>,
                    &InstanceList::execRequest_ContextMenu<Request::EditInstance>,
                    &InstanceList::execRequest_ContextMenu<Request::RemoveInstance>,
                };
            }
        }else{
            instanceActions = {
                new QAction("Dodaj instancje"),
                new QAction("Edytuj nazwę instancji"),
                new QAction("Usuń instancje")
            };
            instanceMenu = new QMenu("Instancje");
            instanceMenu->addActions(instanceActions);
            actions = {
                new QAction("Dodaj interpreter plikow"),
                new QAction("Dodaj interpreter tekstu"),
            };
            menu->addActions(actions);
            actions.append(instanceActions);
            actionFuncs = {
                &InstanceList::execRequest_ContextMenu<Request::AddFilesInterpreter>,
                &InstanceList::execRequest_ContextMenu<Request::AddTextInterpreter>,
                &InstanceList::execRequest_ContextMenu<Request::AddInstance>,
                &InstanceList::execRequest_ContextMenu<Request::EditInstance>,
                &InstanceList::execRequest_ContextMenu<Request::RemoveInstance>,
            };
        }
        // Create menu

    }else{
        actions = {
            new QAction("Dodaj instancje")
        };
        menu->addActions(actions);
        actionFuncs = {
            &InstanceList::execRequest_ContextMenu<Request::AddInstance>,
        };
    }

    // After configuration
    if(menu){
        if(instanceMenu)
            menu->addMenu(instanceMenu);
        int index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "InstanceList Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
}

bool InstanceList::eventFilter(QObject* obj, QEvent* ev){   
    switch(ev->type()){
    case QEvent::MouseButtonDblClick:
    {
        if(obj == viewport()){
            QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
            InstanceListElement* item = itemAt(mev->pos());
            if(item){
                const int indexOfInstanceElement = indexOf(item);
                if(indexOfInstanceElement == -1){ // Not found -> Item is child of instance list item
                    mainApp.activateInterpreter(getSubWindowPositionInfo(item));
                }
            }
        }
    }
        break;
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

    case QEvent::ChildRemoved:
    {
        if(obj == viewport()){
            if(curEditItemInfo.item){
                const int indexOfInstanceElement = indexOf(curEditItemInfo.item);
                if(indexOfInstanceElement == -1){ // Not found -> Item is child of Instance Item
                    if(curEditItemInfo.item->text(0).isEmpty()){ // Remove Item
                        if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeInstance and delete all releted configInfo + Input Panels)
                        {
                            curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                        }
                    }else{
                        mainApp.interpreterNameChanged(getSubWindowPositionInfo(curEditItemInfo.item));
                    }

                }else{

                    //const int& indexOfMainNavigationElementChild = indexOfMainNavigationElement;
                    //DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(topLevelItem(indexOfMainNavigationElement));
                    if(curEditItemInfo.item->text(0).isEmpty()){ // Remove Item
                        if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeInstance and delete all releted configInfo + Input Panels)
                        {
                            curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                        }else{  // Remove (New Instance will not be created)
                            delete curEditItemInfo.item;
                        }
                    }else{
                        if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeInstance and delete all releted configInfo + Input Panels)
                        {   // Change instance name (Maybe unique name control)- update SubWindows Name

                        }else{  // New instance (Maybe unique name control)
                            using ConfigPanel = Panels::Configuration::Panel;
                            InstanceListElementConfig* child = new InstanceListElementConfig("Konfiguracja");
                            curEditItemInfo.item->addChild(child);
                            mainApp.insertSubWindow(getSubWindowPositionInfo(child), new ConfigPanel(mainApp.app()));
                            curEditItemInfo.item->setExpanded(true);
                        }
                        mainApp.instanceNameChanged(curEditItemInfo.item);
                    }
                }

                curEditItemInfo = {};
            }
        }
    }
    break;
    default:
        break;
    }
    return Super::eventFilter(obj, ev);
}
