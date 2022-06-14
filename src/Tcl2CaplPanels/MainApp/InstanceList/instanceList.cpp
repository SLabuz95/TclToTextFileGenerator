#include "instanceList.hpp"
#include"Tcl2CaplPanels/MainApp/mainApp.hpp"
#include<QResizeEvent>
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>
#include"Tcl2CaplPanels/tcl2caplfileparserinstance.hpp"
#include"Tcl2CaplPanels/tcl2caplinstructionpanel.hpp"

template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::AddFilesInterpreter>(InstanceListElement* )
{

    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {new InstanceListElement, QString(), new Tcl2CaplFileParserInstance};
    addTopLevelItem(curEditItemInfo.item);
    editItem(curEditItemInfo.item);
}


template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::AddTextInterpreter>(InstanceListElement* )
{

    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {new InstanceListElement, QString(), new Tcl2CaplInstructionPanel};
    addTopLevelItem(curEditItemInfo.item);
    editItem(curEditItemInfo.item);
}

template<>
void InstanceList::execRequest_ContextMenu<InstanceList::Request_ContextMenu::EditInterpreterName>(InstanceListElement* item)
{
    Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::editInterpreter", "Item is null");
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
    Q_ASSERT_X(item != nullptr, "InstanceList::ContextMenu::removeInstance", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    //tryToManageAttributesName(item->text(0), QString());
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
            const int indexOfInstanceElement = indexOf(item);
            if(indexOfInstanceElement == -1){ // Not found -> Item is child of main navigation element
                //const int& indexOfMainNavigationElementChild = indexOfMainNavigationElement;
                //DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(topLevelItem(indexOfMainNavigationElement));
            }else{
                if((curEditItemInfo.item = itemAt(mev->pos()))){
                    curEditItemInfo.oldStr = curEditItemInfo.item->text(0);
                    editItem(curEditItemInfo.item);
                }else{
                    //using Request = InstanceList::Request_ContextMenu;
                    //execRequest_ContextMenu<Request::AddInstance>(nullptr);
                }
            }
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
                    new QAction("Dodaj interpreter plikow"),
                    new QAction("Dodaj interpreter tekstu"),
                    new QAction("Edytuj nazwe interpretera"),
                    new QAction("Usuń interpreter"),
                  new QAction("Dodaj instancje"),
                  new QAction("Edytuj nazwę instancji"),
                  new QAction("Usuń instancje")
                };
                actionFuncs = {
                    &InstanceList::execRequest_ContextMenu<Request::AddFilesInterpreter>,
                    &InstanceList::execRequest_ContextMenu<Request::AddTextInterpreter>,
                    &InstanceList::execRequest_ContextMenu<Request::EditInterpreterName>,
                    &InstanceList::execRequest_ContextMenu<Request::RemoveInterpreter>,
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
                    Q_ASSERT_X(index < actionFuncs.size(), "InstanceList Menu", "Index error for action functions");
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
            if(curEditItemInfo.item){

                const int indexOfInstanceElement = indexOf(curEditItemInfo.item);
                if(indexOfInstanceElement == -1){ // Not found -> Item is child of main navigation element
                    const int& indexOfMainInstanceElementChild = indexOfInstanceElement;
                    if(curEditItemInfo.item->text(0).isEmpty()){ // Remove Item
                        if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeInstance and delete all releted configInfo + Input Panels)
                        {   // Remove SubWindow -> Calc SubWindow Index and remove from MainApp Window
                           delete curEditItemInfo.item;
                        }else{  // Remove (New Instance will not be created)
                            // New Instance - Widget is prepared in memory - Remove from curEditItem
                            delete curEditItemInfo.interpreterWindow;
                            curEditItemInfo.interpreterWindow = nullptr;
                            delete curEditItemInfo.item;
                        }
                    }else{
                        if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeInstance and delete all releted configInfo + Input Panels)
                        {   // Change instance name (Maybe unique name control) - update SubWindow Name
                            // Calc SubWindow Index and update SubWindow Name
                        }else{  // New instance (Maybe unique name control)
                            // Calc SubWindow Index and Add To View
                            mainApp.insertSubWindow(getSubWindowPositionInfo(curEditItemInfo.item), curEditItemInfo.interpreterWindow);
                        }
                    }

                }else{

                    //const int& indexOfMainNavigationElementChild = indexOfMainNavigationElement;
                    //DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(topLevelItem(indexOfMainNavigationElement));
                    if(curEditItemInfo.item->text(0).isEmpty()){ // Remove Item
                        if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeInstance and delete all releted configInfo + Input Panels)
                        {   // Ask if user wants to remove instance (maybe check if any InputPanels and Local Config Changes)
                           delete curEditItemInfo.item;
                        }else{  // Remove (New Instance will not be created)
                            delete curEditItemInfo.item;
                        }
                    }else{
                        if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeInstance and delete all releted configInfo + Input Panels)
                        {   // Change instance name (Maybe unique name control)- update SubWindows Name

                        }else{  // New instance (Maybe unique name control)
                            curEditItemInfo.item->setExpanded(true);
                        }
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
