#include"attributesPanel.hpp"
#include"Tcl2CaplPanels/ConfigEditor/ConfigTabsPanel/configTabsPanel.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

using namespace Panels::Configuration;

AttributesPanel::AttributesPanel(ConfigTabsPanel& tabsPanel)
    : QTreeWidget(&tabsPanel), tabsPanel(tabsPanel)
{
    setHeaderLabels({"Nazwa", "Wartość"});
    setIndentation(0);
    //setMovement(Snap);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setDragDropMode(QAbstractItemView::InternalMove);
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);

    viewport()->installEventFilter(this);
}


template<>
void AttributesPanel::execRequest_ContextMenu<AttributesPanel::Request_ContextMenu::AddAttribute>(ListItem* )
{

    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {new ListItem, QString(), 0};
    addTopLevelItem(curEditItemInfo.item);
    editItem(curEditItemInfo.item);
}

template<>
void AttributesPanel::execRequest_ContextMenu<AttributesPanel::Request_ContextMenu::EditAttributeName>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "AttributesPanel::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {item, item->text(0), 0};
    editItem(item, 0);
}

template<>
void AttributesPanel::execRequest_ContextMenu<AttributesPanel::Request_ContextMenu::EditAttributeValue>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "AttributesPanel::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {item, item->text(1), 1};
    editItem(item, 1);
}

template<>
void AttributesPanel::execRequest_ContextMenu<AttributesPanel::Request_ContextMenu::RemoveAttribute>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "AttributesPanel::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    tryToManageAttributesName(item->text(0), QString());
    delete item;
}

template<>
void AttributesPanel::execRequest_ContextMenu<AttributesPanel::Request_ContextMenu::ClearAttributes>(ListItem*)
{
    if(curEditItemInfo.item)
    {
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    curEditItemInfo = {};
    for(int i = 0; i < topLevelItemCount(); i++){
        tryToManageAttributesName(topLevelItem(i)->text(0), QString());
    }
    clear();
}

bool AttributesPanel::tryToManageAttributesName(QString oldName, QString newName){
    Q_ASSERT_X(not (oldName.isEmpty() and newName.isEmpty()), "AttributesPanel::tryToManageAttributesName", "OldName and NewName cant be empty");
    if(oldName.isEmpty()){  // NewName isnt empty and oldName is empty - NewProcedure
        if(newAttributes.contains(newName) or
                ( savedAttributes->contains(newName) and removedAttributes.removeAll(newName) == 0))
                    return false;   // DUPLICATED
        newAttributes.append({newName});
    }else{  // Old Name isnt empty
        if(newName.isEmpty()){  // NewName is empty and oldName isnt empty - Remove Procedure
            if(newAttributes.removeAll(oldName) == 0)
                removedAttributes.append({oldName});
        }else{  // NewName and oldName isnt empty - ChangedProcedure
            bool savedAttributesContainsNewName = false;
            if(newAttributes.contains(newName) or
                    ((savedAttributesContainsNewName = savedAttributes->contains(newName)) and removedAttributes.removeAll(newName) == 0))
                        return false;   // DUPLICATED
           if(newAttributes.removeAll(oldName) == 0)
           {    // Old name in savedAttributes
               removedAttributes.append({oldName});
           }
           if(not savedAttributesContainsNewName)
               newAttributes.append({newName});
        }
    }

    return true;
}

bool AttributesPanel::tryToManageAttributesValue(QString name, QString oldValue, QString newValue){
    /*Q_ASSERT_X(not (oldName.isEmpty() and newName.isEmpty()), "AttributesPanel::tryToManageAttributesName", "OldName and NewName cant be empty");
    if(oldName.isEmpty()){  // NewName isnt empty and oldName is empty - NewProcedure
        if(newAttributes.contains(newName) or
                ( savedAttributes->contains(newName) and removedAttributes.removeAll(newName) == 0))
                    return false;   // DUPLICATED
        newAttributes.append({newName});
    }else{  // Old Name isnt empty
        if(newName.isEmpty()){  // NewName is empty and oldName isnt empty - Remove Procedure
            if(newAttributes.removeAll(oldName) == 0)
                removedAttributes.append({oldName});
        }else{  // NewName and oldName isnt empty - ChangedProcedure
            bool savedAttributesContainsNewName = false;
            if(newAttributes.contains(newName) or
                    ((savedAttributesContainsNewName = savedAttributes->contains(newName)) and removedAttributes.removeAll(newName) == 0))
                        return false;   // DUPLICATED
           if(newAttributes.removeAll(oldName) == 0)
           {    // Old name in savedAttributes
               removedAttributes.append(oldName);
           }
           if(not savedAttributesContainsNewName)
               newAttributes.append(newName);
        }
    }
*/
    return true;
}


bool AttributesPanel::eventFilter(QObject* obj, QEvent* ev){
    if(ev->type() ==  QEvent::ContextMenu)
        {
            QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);

            ListItem* item = itemAt(cev->pos());

            // Specify file and error checking

            using Actions = QList<QAction*>;
            using ActionFuncs = QList<AttributesPanel::Request_ContextMenu_Func>;
            using Request = AttributesPanel::Request_ContextMenu;
            Actions actions;
            ActionFuncs actionFuncs;
            QMenu* menu = nullptr;

            menu = new QMenu;
            if(item){
                actions = {
                  new QAction("Dodaj atrybut"),
                  new QAction("Edytuj nazwę atrybutu"),
                  new QAction("Edytuj wartość atrybutu"),
                  new QAction("Usuń atrybut"),
                    new QAction("Usuń wszystkie atrybuty")
                };
                actionFuncs = {
                    &AttributesPanel::execRequest_ContextMenu<Request::AddAttribute>,
                    &AttributesPanel::execRequest_ContextMenu<Request::EditAttributeName>,
                    &AttributesPanel::execRequest_ContextMenu<Request::EditAttributeValue>,
                    &AttributesPanel::execRequest_ContextMenu<Request::RemoveAttribute>,
                    &AttributesPanel::execRequest_ContextMenu<Request::ClearAttributes>,
                };
            }else{
                actions = {
                    new QAction("Dodaj atrybut"),
                    new QAction("Usuń wszystkie atrybuty")
                };
                actionFuncs = {
                    &AttributesPanel::execRequest_ContextMenu<Request::AddAttribute>,
                    &AttributesPanel::execRequest_ContextMenu<Request::ClearAttributes>,
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

    if(ev->type() == QEvent::MouseButtonDblClick and obj == viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        if((curEditItemInfo.item = itemAt(mev->pos()))){
            curEditItemInfo.oldStr = curEditItemInfo.item->text(0);
            curEditItemInfo.column = currentColumn();
            editItem(curEditItemInfo.item, curEditItemInfo.column);
        }
    }

    if(ev->type() == QEvent::ChildRemoved and obj == viewport()){
        if(curEditItemInfo.item){
            if(curEditItemInfo.item->text(0).isEmpty()){ // Remove Item
                if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeProcedure)
                    tryToManageAttributesName(curEditItemInfo.oldStr, QString());
                delete curEditItemInfo.item;
            }else{  // Not empty (New Procedure or Change Procedure)
                if(curEditItemInfo.item->text(0) != curEditItemInfo.oldStr){    // name changed
                    if(curEditItemInfo.oldStr.isEmpty()){  // New Procedure
                        if(not tryToManageAttributesName(QString(), curEditItemInfo.item->text(0))){ // Failed (Duplicated Name)
                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text(0) + QStringLiteral("\" already exists."));
                            delete curEditItemInfo.item;
                        }
                    }else{  // Change Procedure Name
                        if(not tryToManageAttributesName(curEditItemInfo.oldStr, curEditItemInfo.item->text(0))){ // Failed (Duplicated Name)
                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text(0) + QStringLiteral("\" already exists."));
                            curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                        }
                    }
                }
            }
            curEditItemInfo = {};
        }
    }


    return Super::eventFilter(obj, ev);

}


void AttributesPanel::loadAttributes(AttributesPtr atrributes, LoadProceduresSettings settings){
    clearChanges();
    savedAttributes = atrributes;
    if(settings == LoadProceduresSettings::LoadGui){
        // Load Gui
    }
}

void AttributesPanel::reloadGui(){
    clear();
    //addTopLevelItems((*savedAttributes));
}

