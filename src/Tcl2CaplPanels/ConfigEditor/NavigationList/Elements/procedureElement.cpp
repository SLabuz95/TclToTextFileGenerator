#include"procedureElement.hpp"
#include<QMenu>
#include<QContextMenuEvent>
#include<QApplication>



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
    /*Q_ASSERT_X(item != nullptr, "AttributesPanel::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {item, item->text(0)};
    editItem(item, 0);*/
}

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::RemoveIndex>(ListItem* item)
{
    /*Q_ASSERT_X(item != nullptr, "AttributesPanel::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    tryToManageAttributesName(item->text(0));
    delete item;*/
}

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::ClearIndexes>(ListItem*)
{
    /*if(curEditItemInfo.item)
    {
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    curEditItemInfo = {};
    for(int i = 0; i < topLevelItemCount(); i++){
        tryToManageAttributesName(topLevelItem(i)->text(0));
    }
    clear();*/
}
bool DefaultProcedureElement::tryToManageIndex(QString oldIndex, QString newIndex){

    Q_ASSERT_X(not (oldIndex.isEmpty() and newIndex.isEmpty()), "DefaultProcedureElement::tryToManageIndex", "OldIndex and NewIndex cant be empty");
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
            bool savedAttributesContainsNewName = false;
            if(newIndexes.contains(newIndex)/* or
                    ((savedAttributesContainsNewName = savedAttributes->contains(newIndex)) and removedAttributes.removeAll(newIndex) == 0)*/)
                        return false;   // DUPLICATED
           if(newIndexes.removeAll(oldIndex) == 0)
           {    // Old name in savedAttributes
               //removedAttributes.append({oldIndex});
           }
           if(not savedAttributesContainsNewName)
               newIndexes.append({newIndex});
        }
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
          new QAction("Dodaj atrybut"),
          new QAction("Edytuj nazwę atrybutu"),
          new QAction("Edytuj wartość atrybutu"),
          new QAction("Usuń atrybut"),
          new QAction("Usuń wszystkie atrybuty")
        };
        actionFuncs = {
            &DefaultProcedureElement::execRequest_ContextMenu<Request::AddIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::EditIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::RemoveIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::ClearIndexes>,
        };
    }else{
        actions = {
            new QAction("Dodaj atrybut"),
            new QAction("Usuń wszystkie atrybuty")
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
            Q_ASSERT_X(index < actionFuncs.size(), "AttributesPanel Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
}











