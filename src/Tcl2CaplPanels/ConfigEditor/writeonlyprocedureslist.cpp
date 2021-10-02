#include"writeOnlyProceduresList.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

WriteOnlyProceduresList::WriteOnlyProceduresList()
{
    layout.addWidget(&list);

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
    tryToManageProceduresName(item->text(), QString());
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
    for(int i = 0; i < list.count(); i++){
        tryToManageProceduresName(list.item(i)->text(), QString());
    }
    list.clear();
}

bool WriteOnlyProceduresList::tryToManageProceduresName(QString oldName, QString newName){
    Q_ASSERT_X(not (oldName.isEmpty() and newName.isEmpty()), "WriteOnlyProceduresList::tryToManageProceduresName", "OldName and NewName cant be empty");
    if(oldName.isEmpty()){  // NewName isnt empty and oldName is empty - NewProcedure
        if(newProcedures.contains(newName) or
                ( savedProcedures->contains(newName) and removedProcedures.removeAll(newName) == 0))
                    return false;   // DUPLICATED
        newProcedures.append(newName);
    }else{  // Old Name isnt empty
        if(newName.isEmpty()){  // NewName is empty and oldName isnt empty - Remove Procedure
            if(newProcedures.removeAll(oldName) == 0)
                removedProcedures.append(oldName);
        }else{  // NewName and oldName isnt empty - ChangedProcedure
            bool savedProceduresContainsNewName = false;
            if(newProcedures.contains(newName) or
                    ((savedProceduresContainsNewName = savedProcedures->contains(newName)) and removedProcedures.removeAll(newName) == 0))
                        return false;   // DUPLICATED
           if(newProcedures.removeAll(oldName) == 0)
           {    // Old name in savedProcedures
               removedProcedures.append(oldName);
           }
           if(not savedProceduresContainsNewName)
               newProcedures.append(newName);
        }
    }

    return true;
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
                if(not curEditItemInfo.oldName.isEmpty()) // old name isnt empty (removeProcedure)
                    tryToManageProceduresName(curEditItemInfo.oldName, QString());
                delete curEditItemInfo.item;
            }else{  // Not empty (New Procedure or Change Procedure)
                if(curEditItemInfo.item->text() != curEditItemInfo.oldName){    // name changed
                    if(curEditItemInfo.oldName.isEmpty()){  // New Procedure
                        if(not tryToManageProceduresName(QString(), curEditItemInfo.item->text())){ // Failed (Duplicated Name)
                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text() + QStringLiteral("\" already exists."));
                            delete curEditItemInfo.item;
                        }
                    }else{  // Change Procedure Name
                        if(not tryToManageProceduresName(curEditItemInfo.oldName, curEditItemInfo.item->text())){ // Failed (Duplicated Name)
                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text() + QStringLiteral("\" already exists."));
                            curEditItemInfo.item->setText(curEditItemInfo.oldName);
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
