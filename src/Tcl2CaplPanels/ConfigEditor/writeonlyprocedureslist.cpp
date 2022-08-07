#include"writeOnlyProceduresList.hpp"
#include"ConfigTabsPanel/configTabsPanel.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

using namespace Panels::Configuration;

WriteOnlyProceduresList::WriteOnlyProceduresList(ConfigTabsPanel& configTabs)
{
    layout.addWidget(&list);

    list.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    list.viewport()->installEventFilter(this);
    list.setSortingEnabled(true);
    list.setMovement(QListView::Snap);
    list.setDefaultDropAction(Qt::DropAction::MoveAction);
    list.setDragDropMode(QAbstractItemView::InternalMove);
    list.setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);


    setLayout(&layout);
    show();
}

template<>
void WriteOnlyProceduresList::execRequest_ContextMenu<WriteOnlyProceduresList::Request_ContextMenu::AddProcedure>(ListItem* )
{

    if(curEditItemInfo.item){
        list.closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {new ListItem, QString()};
    list.addItem(curEditItemInfo.item);
    list.editItem(curEditItemInfo.item);
}

template<>
void WriteOnlyProceduresList::execRequest_ContextMenu<WriteOnlyProceduresList::Request_ContextMenu::EditProcedure>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "WriteOnlyProceduresList::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        list.closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {item, item->text()};
    list.editItem(item);
}

template<>
void WriteOnlyProceduresList::execRequest_ContextMenu<WriteOnlyProceduresList::Request_ContextMenu::RemoveProcedure>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "WriteOnlyProceduresList::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        list.closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    ChangeAction changeAction = tryToManageProceduresName(item->text(), QString());
    // Notify about Change
    delete item;
}

template<>
void WriteOnlyProceduresList::execRequest_ContextMenu<WriteOnlyProceduresList::Request_ContextMenu::ClearProcedures>(ListItem*)
{
    if(curEditItemInfo.item)
    {
        list.closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    curEditItemInfo = {};
    // Maybe clear NewProcedures and replace Removed with savedProcedures?
    for(int i = 0; i < list.count(); i++){
        tryToManageProceduresName(list.item(i)->text(), QString());
    }
    // Notify about Change::ClearedAll
    list.clear();
}

WriteOnlyProceduresList::ChangeAction WriteOnlyProceduresList::tryToManageProceduresName(QString oldName, QString newName){
    Q_ASSERT_X(not (oldName.isEmpty() and newName.isEmpty()), "WriteOnlyProceduresList::tryToManageProceduresName", "Only OldName or NewName can be empty ");
    bool restoreRemovedFlag = false;
    if(oldName.isEmpty()){  // NewName isnt empty and oldName is empty - NewProcedure
        if(newProcedures.contains(newName) or
                ( savedProcedures->contains(newName) and (restoreRemovedFlag = removedProcedures.removeAll(newName) == 0))) // if removed 0 then not removed (duplicate in list), if not 0, removed -> restore action
                    return ChangeAction::DuplicatedError;   // DUPLICATED
        newProcedures.append(newName); // New Procedure
        return (restoreRemovedFlag)? ChangeAction::Restored : ChangeAction::New;
    }else{  // Old Name isnt empty - Change or remove
        if(newName.isEmpty()){  // NewName is empty and oldName isnt empty - Remove Procedure
            if((restoreRemovedFlag = newProcedures.removeAll(oldName) == 0)){ // Not new procedure - removed, otherwise removed from new procedures - removed too
                removedProcedures.append(oldName);
            }
            return ChangeAction::Removed;
        }else{  // NewName and oldName isnt empty - ChangedProcedure
            bool savedProceduresContainsNewName = false;
            if(newProcedures.contains(newName) or
                    ((savedProceduresContainsNewName = savedProcedures->contains(newName)) and (restoreRemovedFlag = removedProcedures.removeAll(newName) == 0)))
                        return ChangeAction::DuplicatedError;   // DUPLICATED
           if(newProcedures.removeAll(oldName) == 0)
           {    // Old name in savedProcedures
               removedProcedures.append(oldName);
               if(not savedProceduresContainsNewName){
                   newProcedures.append(newName);
                   return ChangeAction::NewAndRemoved;
               }else{
                   return ChangeAction::Removed;
               }
           }else{
               if(not savedProceduresContainsNewName){
                   newProcedures.append(newName);
                   return ChangeAction::New;
               }else{
                   return ChangeAction::NoAction;
               }
           }
        }
    }
}


bool WriteOnlyProceduresList::eventFilter(QObject* obj, QEvent* ev){
    if(ev->type() ==  QEvent::ContextMenu)
        {
            QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);

            ListItem* item = itemAt(cev->pos());

            // Specify file and error checking

            using Actions = QList<QAction*>;
            using ActionFuncs = QList<WriteOnlyProceduresList::Request_ContextMenu_Func>;
            using Request = WriteOnlyProceduresList::Request_ContextMenu;
            Actions actions;
            ActionFuncs actionFuncs;
            QMenu* menu = nullptr;

            menu = new QMenu;
            if(item){
                actions = {
                  new QAction("Dodaj procedure"),
                  new QAction("Edytuj procedure"),
                  new QAction("Usuń procedure"),
                    new QAction("Usuń wszystkie procedury")
                };
                actionFuncs = {
                    &WriteOnlyProceduresList::execRequest_ContextMenu<Request::AddProcedure>,
                    &WriteOnlyProceduresList::execRequest_ContextMenu<Request::EditProcedure>,
                    &WriteOnlyProceduresList::execRequest_ContextMenu<Request::RemoveProcedure>,
                    &WriteOnlyProceduresList::execRequest_ContextMenu<Request::ClearProcedures>,
                };
            }else{
                actions = {
                  new QAction("Dodaj procedure"),
                    new QAction("Usuń wszystkie procedury")
                };
                actionFuncs = {
                    &WriteOnlyProceduresList::execRequest_ContextMenu<Request::AddProcedure>,
                    &WriteOnlyProceduresList::execRequest_ContextMenu<Request::ClearProcedures>,
                };
            }

            // After configuration
            if(menu){
                menu->addActions(actions);
                int index = actions.indexOf( menu->exec(cev->globalPos()));
                if(index >= 0){
                    Q_ASSERT_X(index < actionFuncs.size(), "WriteOnlyProceduresList Menu", "Index error for action functions");
                    (this->*(actionFuncs.at(index)))(item);
                }
                delete menu, menu = nullptr;
            }
        }

    if(ev->type() == QEvent::MouseButtonDblClick and obj == list.viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        if((curEditItemInfo.item = itemAt(mev->pos()))){
            curEditItemInfo.oldName = curEditItemInfo.item->text();
            list.editItem(curEditItemInfo.item);
        }
    }

    if(ev->type() == QEvent::ChildRemoved and obj == list.viewport()){
        if(curEditItemInfo.item){
            if(curEditItemInfo.item->text().isEmpty()){ // Remove Item
                if(not curEditItemInfo.oldName.isEmpty()){ // old name isnt empty (removeProcedure)
                    ChangeAction changeAction = tryToManageProceduresName(curEditItemInfo.oldName, QString());
                    // Notify about Change
                }
                delete curEditItemInfo.item;
            }else{  // Not empty (New Procedure or Change Procedure)
                if(curEditItemInfo.item->text() != curEditItemInfo.oldName){    // name changed
                    if(curEditItemInfo.oldName.isEmpty()){  // New Procedure
                        ChangeAction changeAction = tryToManageProceduresName(QString(), curEditItemInfo.item->text());
                        if(changeAction == ChangeAction::DuplicatedError){ // Failed (Duplicated Name)
                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text() + QStringLiteral("\" already exists."));
                            delete curEditItemInfo.item;
                        }else{
                            // Notify about change

                        }
                    }else{  // Change Procedure Name
                        ChangeAction changeAction = tryToManageProceduresName(curEditItemInfo.oldName, curEditItemInfo.item->text());
                        if(changeAction == ChangeAction::DuplicatedError){ // Failed (Duplicated Name)
                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text() + QStringLiteral("\" already exists."));
                            curEditItemInfo.item->setText(curEditItemInfo.oldName);
                        }else{
                            // Notify about change
                        }
                    }
                }
            }
            curEditItemInfo = {};
        }
    }


    return Super::eventFilter(obj, ev);

}


void WriteOnlyProceduresList::loadProcedures(WriteOnlyProceduresPtr writeOnlyProcedures, LoadProceduresSettings settings){
    clearChanges();
    savedProcedures = writeOnlyProcedures;
    if(settings == LoadProceduresSettings::LoadGui){
        // Load Gui
    }
}

void WriteOnlyProceduresList::reloadGui(){
    list.clear();
    list.addItems((*savedProcedures));
}
