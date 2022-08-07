#include"proceduresList.hpp"
#include<QApplication>
#include<QMouseEvent>
#include<QMenu>
#include<QMessageBox>

/*
ProceduresList::ProceduresList()
{
    // ToolBar Setup
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
void ProceduresList::execRequest_ContextMenu<ProceduresList::Request_ContextMenu::AddProcedure>(ListItem*)
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
void ProceduresList::execRequest_ContextMenu<ProceduresList::Request_ContextMenu::EditProcedure>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ProceduresList::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        list.closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    curEditItemInfo = {item, item->text()};
    list.editItem(item);
}

template<>
void ProceduresList::execRequest_ContextMenu<ProceduresList::Request_ContextMenu::RemoveProcedure>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ProceduresList::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        list.closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    curEditItemInfo = {};
    if(tryToManageProceduresName(item->text(), QString()))
        delete item;
}


template<>
void ProceduresList::execRequest_ContextMenu<ProceduresList::Request_ContextMenu::ClearProcedures>(ListItem*)
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


bool ProceduresList::tryToManageProceduresName(QString oldName, QString newName){
    Q_ASSERT_X(not (oldName.isEmpty() and newName.isEmpty()), "ProceduresList::tryToProcessProcedureChanges", "OldName and NewName cant be empty");
    if(oldName.isEmpty()){  // NewName isnt empty and oldName is empty - NewProcedure
        if(removedProcedures.removeAll(newName) == 0){
            if(newProcedures.contains(newName) or
                    (savedProcedures->contains(newName)))
                return false;   // DUPLICATED
        }
        newProcedures.append(new Procedures::Type(newName));
    }else{  // Old Name isnt empty
        if(newName.isEmpty()){  // NewName is empty and oldName isnt empty - Remove Procedure
            removedProcedures.append(oldName);
        }else{  // NewName and oldName isnt empty - ChangedProcedure
            if(removedProcedures.removeAll(newName) == 0){
                if(newProcedures.contains(newName) or
                        savedProcedures->contains(newName))
                    return false;   // DUPLICATED
                // Find old procedure (first check newProcedures (higher priority), then saved)
                int oldProcedureFromNewIndex = -1;
                int oldProcedureFromSavedIndex = -1;
                if((oldProcedureFromNewIndex = newProcedures.indexOf(oldName)) == -1 and
                    (oldProcedureFromSavedIndex = savedProcedures->indexOf(oldName)) == -1)
                {   // Not Found at all (Internal Error)
                    QMessageBox::critical(nullptr, QStringLiteral("Internal Error"), QStringLiteral("Its highly recommended to reset application. Inform app's author about an error as well."));
                }else{  // procedure has been found
                    if(oldProcedureFromNewIndex != -1)
                        newProcedures.append(Procedures::Type(newProcedures.at(oldProcedureFromNewIndex), newName));
                    else
                        newProcedures.append(Procedures::Type(savedProcedures->at(oldProcedureFromSavedIndex), newName));
                }
            }else{
                // Find old and new procedure (first check newProcedures (higher priority), then saved)
                int oldProcedureFromNewIndex = -1;
                int oldProcedureFromSavedIndex = -1;
                Procedures::Iterator oldProcedureIter = nullptr;
                int newProcedureFromNewIndex = -1;
                int newProcedureFromSavedIndex = -1;
                Procedures::Iterator newProcedureIter = nullptr;
                if((oldProcedureFromNewIndex = newProcedures.indexOf(oldName)) == -1 and
                    (oldProcedureFromSavedIndex = savedProcedures->indexOf(oldName)) == -1)
                {   // Not Found at all (Internal Error)
                    QMessageBox::critical(nullptr, QStringLiteral("Internal Error"), QStringLiteral("Its highly recommended to reset application. Inform app's author about an error as well."));
                }else{  // procedure has been found
                    if(oldProcedureFromNewIndex != -1)
                        oldProcedureIter = newProcedures.begin() + oldProcedureFromNewIndex;
                    else
                        oldProcedureIter = newProcedures.begin() + oldProcedureFromSavedIndex;
                }
                if((newProcedureFromNewIndex = newProcedures.indexOf(newName)) == -1 and
                    (newProcedureFromSavedIndex = savedProcedures->indexOf(newName)) == -1)
                {   // Not Found at all (Internal Error)
                    QMessageBox::critical(nullptr, QStringLiteral("Internal Error"), QStringLiteral("Its highly recommended to reset application. Inform app's author about an error as well."));
                }else{  // procedure has been found
                    if(oldProcedureFromNewIndex != -1)
                        newProcedureIter = newProcedures.begin() + newProcedureFromNewIndex;
                    else
                        newProcedureIter = newProcedures.begin() + newProcedureFromSavedIndex;
                }
                (*newProcedureIter) = Procedures::Type((*oldProcedureIter), newName);
            }
            removedProcedures.append(oldName);
        }
    }

    return true;
}


bool ProceduresList::eventFilter(QObject* obj, QEvent* ev){   

    if(ev->type() ==  QEvent::ContextMenu)
        {
            QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);

            ListItem* item = itemAt(cev->pos());

            // Specify file and error checking
            using Actions = QList<QAction*>;
            using ActionFuncs = QList<ProceduresList::Request_ContextMenu_Func>;
            using Request = ProceduresList::Request_ContextMenu;
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
                    &ProceduresList::execRequest_ContextMenu<Request::AddProcedure>,
                    &ProceduresList::execRequest_ContextMenu<Request::EditProcedure>,
                    &ProceduresList::execRequest_ContextMenu<Request::RemoveProcedure>,
                    &ProceduresList::execRequest_ContextMenu<Request::ClearProcedures>,
                };
            }else{
                actions = {
                  new QAction("Dodaj procedure"),
                    new QAction("Usuń wszystkie procedury")
                };
                actionFuncs = {
                    &ProceduresList::execRequest_ContextMenu<Request::AddProcedure>,
                    &ProceduresList::execRequest_ContextMenu<Request::ClearProcedures>,
                };
            }


            // After configuration
            if(menu){
                menu->addActions(actions);
                int index = actions.indexOf( menu->exec(cev->globalPos()));
                if(index >= 0){
                    Q_ASSERT_X(index < actionFuncs.size(), "ProceduresList Menu", "Index error for action functions");
                    (this->*(actionFuncs.at(index)))(item);
                }
                delete menu, menu = nullptr;
            }
        }

    if(ev->type() == QEvent::MouseButtonDblClick and obj == list.viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        //curEditItemInfo.item = itemAt(mev->pos());
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

void ProceduresList::loadProcedures(SavedProceduresPtr procedures, LoadProceduresSettings settings){
    clearChanges();
    savedProcedures = procedures;
    if(settings == LoadProceduresSettings::LoadGui){
        // Load Gui
    }
}

void ProceduresList::reloadGui(){
    list.clear();
    list.addItems((*savedProcedures));
}
*/
