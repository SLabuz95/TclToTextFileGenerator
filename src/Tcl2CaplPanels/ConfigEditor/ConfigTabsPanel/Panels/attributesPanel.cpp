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
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setDragDropOverwriteMode(true);

    viewport()->installEventFilter(this);
    addTopLevelItem(new ListItem(hardcodedItemFlags, "PREFIX_TEXT_FOR_FILE"));
    addTopLevelItem(new ListItem(hardcodedItemFlags, "POSTFIX_TEXT_FOR_FILE"));
    addTopLevelItem(new ListItem(hardcodedItemFlags, "MODEL_PROCESS_STATUS"));
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
void AttributesPanel::execRequest_ContextMenu<AttributesPanel::Request_ContextMenu::EditAttribute>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "AttributesPanel::ContextMenu::editAttribute", "Item is null");
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
    Q_ASSERT_X(item != nullptr, "AttributesPanel::ContextMenu::editAttributeValue", "Item is null");
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
    Q_ASSERT_X(item != nullptr, "AttributesPanel::ContextMenu::RemoveAttribute", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    //tryToManageAttributesName(item->text(0), QString());
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
    QString PREFIX_TEXT_FOR_FILE;
    QString POSTFIX_TEXT_FOR_FILE;
    int count = 0;
    for(int i = 0; i < topLevelItemCount(); i++){
        if(topLevelItem(i)->text(0) == "PREFIX_TEXT_FOR_FILE"){
            PREFIX_TEXT_FOR_FILE = topLevelItem(i)->text(1);
            count++;
            if(count == numbOfHardcodedAttributes){
                break;
            }
        }
        if(topLevelItem(i)->text(0) == "POSTFIX_TEXT_FOR_FILE"){
            POSTFIX_TEXT_FOR_FILE = topLevelItem(i)->text(1);
            count++;
            if(count == numbOfHardcodedAttributes){
                break;
            }
        }
        if(topLevelItem(i)->text(0) == "MODEL_PROCESS_STATUS"){
            PREFIX_TEXT_FOR_FILE = topLevelItem(i)->text(1);
            count++;
            if(count == numbOfHardcodedAttributes){
                break;
            }
        }
    }
    clear();
    addTopLevelItem(new ListItem(hardcodedItemFlags, "PREFIX_TEXT_FOR_FILE", PREFIX_TEXT_FOR_FILE));
    addTopLevelItem(new ListItem(hardcodedItemFlags, "POSTFIX_TEXT_FOR_FILE", POSTFIX_TEXT_FOR_FILE));
    addTopLevelItem(new ListItem(hardcodedItemFlags, "MODEL_PROCESS_STATUS"));
}

bool AttributesPanel::tryToManageAttributesName(QString oldName, QString newName){
   /* Q_ASSERT_X(not (oldName.isEmpty() and newName.isEmpty()), "AttributesPanel::tryToManageAttributesName", "OldName and NewName cant be empty");
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
*/
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
            if(indexOfTopLevelItem(item) >= numbOfHardcodedAttributes){
                actions = {
                    new QAction("Dodaj atrybut"),
                    new QAction("Edytuj nazwę atrybutu"),
                    new QAction("Edytuj wartość atrybutu"),
                    new QAction("Usuń atrybut"),
                    new QAction("Usuń wszystkie atrybuty")
                };
                actionFuncs = {
                    &AttributesPanel::execRequest_ContextMenu<Request::AddAttribute>,
                    &AttributesPanel::execRequest_ContextMenu<Request::EditAttribute>,
                    &AttributesPanel::execRequest_ContextMenu<Request::EditAttributeValue>,
                    &AttributesPanel::execRequest_ContextMenu<Request::RemoveAttribute>,
                    &AttributesPanel::execRequest_ContextMenu<Request::ClearAttributes>,
                };
            }else{
                actions = {
                    new QAction("Dodaj atrybut"),
                    new QAction("Edytuj wartość atrybutu"),
                    new QAction("Usuń wszystkie atrybuty")
                };
                actionFuncs = {
                    &AttributesPanel::execRequest_ContextMenu<Request::AddAttribute>,
                    &AttributesPanel::execRequest_ContextMenu<Request::EditAttributeValue>,
                    &AttributesPanel::execRequest_ContextMenu<Request::ClearAttributes>,
                };
            }
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
            if(indexOfTopLevelItem(curEditItemInfo.item) >= numbOfHardcodedAttributes or currentColumn() == 1){
                curEditItemInfo.oldStr = curEditItemInfo.item->text(0);
                curEditItemInfo.column = currentColumn();
                editItem(curEditItemInfo.item, curEditItemInfo.column);
            }else{                
                curEditItemInfo.item = nullptr;
            }
        }else{
            using Request = AttributesPanel::Request_ContextMenu;
            execRequest_ContextMenu<Request::AddAttribute>(nullptr);
        }
    }

    if(ev->type() == QEvent::ChildRemoved and obj == viewport()){
        if(curEditItemInfo.item){
            if(curEditItemInfo.item->text(0).isEmpty()){ // Remove Item
                if(not curEditItemInfo.oldStr.isEmpty()) // old name isnt empty (removeProcedure)
                    //tryToManageAttributesName(curEditItemInfo.oldStr, QString());
                delete curEditItemInfo.item;
            }else{  // Not empty (New Procedure or Change Procedure)
                if(curEditItemInfo.item->text(0) != curEditItemInfo.oldStr){    // name changed
                    if(curEditItemInfo.oldStr.isEmpty()){  // New Procedure
                        int i = 0;
                        for( i = 0; i < topLevelItemCount(); i++){
                            if(topLevelItem(i) != curEditItemInfo.item and
                                    topLevelItem(i)->text(0) == curEditItemInfo.item->text(0)){
                                break;
                            }
                        }
                        if(i != topLevelItemCount()){
                            delete curEditItemInfo.item;
                        }
//                        if(not tryToManageAttributesName(QString(), curEditItemInfo.item->text(0))){ // Failed (Duplicated Name)
//                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text(0) + QStringLiteral("\" already exists."));
//                            delete curEditItemInfo.item;
//                        }
                    }else{  // Change Procedure Name
//                        if(not tryToManageAttributesName(curEditItemInfo.oldStr, curEditItemInfo.item->text(0))){ // Failed (Duplicated Name)
//                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Name"), QStringLiteral("Procedure \"") + curEditItemInfo.item->text(0) + QStringLiteral("\" already exists."));
//                            curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
//                        }
                        int i = 0;
                        for( i = 0; i < topLevelItemCount(); i++){
                            if(topLevelItem(i) != curEditItemInfo.item and
                                    topLevelItem(i)->text(0) == curEditItemInfo.item->text(0)){
                                break;
                            }
                        }
                        if(i != topLevelItemCount()){
                            delete curEditItemInfo.item;
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


void AttributesPanel::loadAttributes(Config::Attributes& attributes){
    clear();
     for(Config::Attributes::Iterator attribute = attributes.begin(); attribute != attributes.end(); attribute++){
         addTopLevelItem(new ListItem(attribute.key(), attribute.value().value));
     }
}

void AttributesPanel::readAttributes(Config::Attributes& attributes){
    attributes.clear();
    for(int i = 0 ; i < topLevelItemCount(); i++){
        attributes.insert(topLevelItem(i)->text(0), Config::Attribute{topLevelItem(i)->text(1)});
    }
}
