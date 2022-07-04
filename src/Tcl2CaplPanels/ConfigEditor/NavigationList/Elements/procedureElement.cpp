#include"procedureElement.hpp"
#include<QMenu>
#include<QContextMenuEvent>
#include<QApplication>
#include<QMessageBox>


using namespace Panels::Configuration::Navigation::Procedure;

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::AddIndex>(ListItem* )
{

    if(curEditItemInfo.item){
        static_cast<Navigation::List*>(treeWidget())->closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {new ListItem, QString()};
    addChild(curEditItemInfo.item);
    static_cast<Navigation::List*>(treeWidget())->editItem(curEditItemInfo.item);

}

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::EditIndex>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "DefaultProcedureElement::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        static_cast<Navigation::List*>(treeWidget())->closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {item, item->text(0)};
    static_cast<Navigation::List*>(treeWidget())->editItem(item, 0);
}

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::RemoveIndex>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "DefaultProcedureElement::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        static_cast<Navigation::List*>(treeWidget())->closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    tryToManageIndex(item->text(0), QString());
    delete item;
}

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::ClearIndexes>(ListItem*)
{
    if(curEditItemInfo.item)
    {
        static_cast<Navigation::List*>(treeWidget())->closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    curEditItemInfo = {};
    while(childCount() != 2){
        delete child(2); // Remove First index
    }
}
bool DefaultProcedureElement::tryToManageIndex(QString oldIndex, QString newIndex){

    Q_ASSERT_X(not (oldIndex.isEmpty() and newIndex.isEmpty()), "DefaultProcedureElement::tryToManageIndex", "OldIndex and NewIndex cant be empty");
    if(curEditItemInfo.item){
        // Confirm that index or argument isnt duplicated
        if(curEditItemInfo.item->text(0).isEmpty()){
            delete curEditItemInfo.item;
        }else{
            // Confirm that index or argument isnt duplicated
            // Remove if its
            bool indexOrArgumentIsntDuplicated = true;
            QString textToCompare = curEditItemInfo.item->text(0);

            for(int itemIndex = 0 ; itemIndex < childCount(); itemIndex++){
                if(child(itemIndex) != curEditItemInfo.item and
                        child(itemIndex)->text(0) == textToCompare){
                    indexOrArgumentIsntDuplicated = false;
                    break;
                }
            }

            if(indexOrArgumentIsntDuplicated){

            }else{
                delete curEditItemInfo.item;
            }

        }
        curEditItemInfo = {};
    }

    return true;
}

void DefaultProcedureElement::menuControl(QContextMenuEvent* cev, ListItem* item){
    // Specify file and error checking
    using Actions = QList<QAction*>;
    using ActionFuncs = QList<DefaultProcedureElement::Request_ContextMenu_Func>;
    using Request = DefaultProcedureElement::Request_ContextMenu;
    Actions actions;
    ActionFuncs actionFuncs;
    QMenu* menu = nullptr;

    menu = new QMenu;
    if(item){
        actions = {
          new QAction("Dodaj indeks"),
          new QAction("Edytuj indeks"),
          new QAction("Usuń indeks"),
          new QAction("Usuń indeksy")
        };
        actionFuncs = {
            &DefaultProcedureElement::execRequest_ContextMenu<Request::AddIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::EditIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::RemoveIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::ClearIndexes>,
        };
    }else{
        actions = {
            new QAction("Dodaj indeks"),
            new QAction("Usuń indeksy")
        };
        actionFuncs = {
            &DefaultProcedureElement::execRequest_ContextMenu<Request::AddIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::ClearIndexes>,
        };
    }

    // After configuration
    if(menu){
        menu->addActions(actions);
        int index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "DefaultProcedureElement Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
}


void DefaultProcedureElement::edittingFinished(){
    if(curEditItemInfo.item){
        if(curEditItemInfo.item->text(0).isEmpty()){ // Remove Item
            if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeProcedure)
                tryToManageIndex(curEditItemInfo.oldStr, QString());
            delete curEditItemInfo.item;
        }else{  // Not empty (New Procedure or Change Procedure)
            if(curEditItemInfo.item->text(0) != curEditItemInfo.oldStr){    // name changed
                if(curEditItemInfo.oldStr.isEmpty()){  // New Procedure
                    if(not tryToManageIndex(QString(), curEditItemInfo.item->text(0))){ // Failed (Duplicated Name)
                        QMessageBox::warning(nullptr, QStringLiteral("Duplicated Index"), QStringLiteral("Index \"") + curEditItemInfo.item->text(0) + QStringLiteral("\" already exists."));
                        delete curEditItemInfo.item;
                    }
                }else{  // Change Procedure Name
                    if(not tryToManageIndex(curEditItemInfo.oldStr, curEditItemInfo.item->text(0))){ // Failed (Duplicated Name)
                        QMessageBox::warning(nullptr, QStringLiteral("Duplicated Index"), QStringLiteral("Index \"") + curEditItemInfo.item->text(0) + QStringLiteral("\" already exists."));
                        curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                    }
                }
            }
        }
        curEditItemInfo = {};
    }
}



template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::AddProcedure>(ListItem* )
{

    if(curEditItemInfo.item){
        static_cast<Navigation::List*>(treeWidget())->closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {new ListItem, QString()};
    addChild(curEditItemInfo.item);
    static_cast<Navigation::List*>(treeWidget())->editItem(curEditItemInfo.item);

}

template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::EditProcedure>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ProceduresElement::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        static_cast<Navigation::List*>(treeWidget())->closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {item, item->text(0)};
    static_cast<Navigation::List*>(treeWidget())->editItem(item, 0);
}

template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::RemoveProcedure>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ProceduresElement::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        static_cast<Navigation::List*>(treeWidget())->closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    tryToManageIndex(item->text(0), QString());
    delete item;
}

template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::ClearProcedures>(ListItem*)
{
    if(curEditItemInfo.item)
    {
        static_cast<Navigation::List*>(treeWidget())->closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    curEditItemInfo = {};
    for(int i = 0; i < static_cast<Navigation::List*>(treeWidget())->topLevelItemCount(); i++){
        tryToManageIndex(static_cast<Navigation::List*>(treeWidget())->topLevelItem(i)->text(0), QString());
    }
    static_cast<Navigation::List*>(treeWidget())->clear();
}
bool ProceduresElement::tryToManageIndex(QString oldIndex, QString newIndex){

    Q_ASSERT_X(not (oldIndex.isEmpty() and newIndex.isEmpty()), "ProceduresElement::tryToManageIndex", "OldIndex and NewIndex cant be empty");
    if(oldIndex.isEmpty()){  // NewName isnt empty and oldIndex is empty - NewProcedure
        if(newIndexes.contains(newIndex)/* or
                ( savedAttributes->contains(newIndex) and removedAttributes.removeAll(newIndex) == 0)*/)
                    return false;   // DUPLICATED
        newIndexes.append({newIndex});
    }else{  // Old Name isnt empty
        if(newIndex.isEmpty()){  // NewName is empty and oldIndex isnt empty - Remove Procedure
            if(newIndexes.removeAll(oldIndex) == 0);
                //removedAttributes.append({oldIndex});
        }else{  // NewName and oldIndex isnt empty - ChangedProcedure
            //bool savedIndexesContainsNewName = false;
            if(newIndexes.contains(newIndex)/* or
                    ((savedAttributesContainsNewName = savedAttributes->contains(newIndex)) and removedAttributes.removeAll(newIndex) == 0)*/)
                        return false;   // DUPLICATED
           if(newIndexes.removeAll(oldIndex) == 0)
           {    // Old name in savedAttributes
               //removedAttributes.append({oldIndex});
           }
           //if(not savedIndexesContainsNewName)
               newIndexes.append({newIndex});
        }
    }

    return true;
}

void ProceduresElement::menuControl(QContextMenuEvent* cev, ListItem* item){
    // Specify file and error checking
    using Actions = QList<QAction*>;
    using ActionFuncs = QList<ProceduresElement::Request_ContextMenu_Func>;
    using Request = ProceduresElement::Request_ContextMenu;
    Actions actions;
    ActionFuncs actionFuncs;
    QMenu* menu = nullptr;

    menu = new QMenu;
    if(item){
        actions = {
          new QAction("Dodaj procedure"),
          new QAction("Edytuj procedure"),
          new QAction("Usuń procedure"),
          new QAction("Usuń procedury")
        };
        actionFuncs = {
            &ProceduresElement::execRequest_ContextMenu<Request::AddProcedure>,
            &ProceduresElement::execRequest_ContextMenu<Request::EditProcedure>,
            &ProceduresElement::execRequest_ContextMenu<Request::RemoveProcedure>,
            &ProceduresElement::execRequest_ContextMenu<Request::ClearProcedures>,
        };
    }else{
        actions = {
            new QAction("Dodaj procedure"),
            new QAction("Usuń procedury")
        };
        actionFuncs = {
            &ProceduresElement::execRequest_ContextMenu<Request::AddProcedure>,
            &ProceduresElement::execRequest_ContextMenu<Request::ClearProcedures>,
        };
    }

    // After configuration
    if(menu){
        menu->addActions(actions);
        int index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "ProceduresElement Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
}


void ProceduresElement::edittingFinished(){
    if(curEditItemInfo.item){
        if(curEditItemInfo.item->text(0).isEmpty()){ // Remove Item
            if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeProcedure)
                tryToManageIndex(curEditItemInfo.oldStr, QString());
            delete curEditItemInfo.item;
        }else{  // Not empty (New Procedure or Change Procedure)
            if(curEditItemInfo.item->text(0) != curEditItemInfo.oldStr){    // name changed
                if(curEditItemInfo.oldStr.isEmpty()){  // New Procedure
                    if(not tryToManageIndex(QString(), curEditItemInfo.item->text(0))){ // Failed (Duplicated Name)
                        QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text(0) + QStringLiteral("\" already exists."));
                        delete curEditItemInfo.item;
                    }
                }else{  // Change Procedure Name
                    if(not tryToManageIndex(curEditItemInfo.oldStr, curEditItemInfo.item->text(0))){ // Failed (Duplicated Name)
                        QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text(0) + QStringLiteral("\" already exists."));
                        curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                    }
                }
            }
        }
        curEditItemInfo = {};
    }
}







