#include"compare.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>
#include<QInputDialog>
#include"Tcl2CaplPanels/ConfigEditor/ModifierRulePanels/modifierRawRule.hpp"
#include"Tcl2CaplPanels/ConfigEditor/rulesphasepanel.hpp"

namespace ActionsList_NS = Panels::Configuration::View::ModifierActionsList;
using CompareActionView = ActionsList_NS::CompareActionView::CompareActionView;
using ListOfIndexes = CompareActionView::ListOfIndexes;
using ActionView = CompareActionView::ActionView;
using ParentContextMenu = CompareActionView::ParentContextMenu;

// CompareNumbOfArgs Action View Definitions -----------------------------------

CompareActionView::CompareActionView(QWidget* parent)
 : ActionDataView(parent)
{
    splitter.addWidget(&listOfIndexes);
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

CompareActionView::CompareActionView(QWidget* parent, ActionPtr pAction)
    : CompareActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        listOfIndexes.loadIndexes(action.stringsToCompare());
        formattedStringList.loadRules(action.inputFormattedString());
    }
}

void CompareActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    listOfIndexes.readAll(action.stringsToCompare());
    formattedStringList.readRules(action.inputFormattedString());
}

CompareActionView::DataView* CompareActionView::create(QWidget* parent, ActionRef action){
    return new CompareActionView(parent, action);
}



// List of Indexes Definitions ---------------------------------------------------


template<>
void ListOfIndexes::execRequest_ContextMenu<ListOfIndexes::Request_ContextMenu::Add>(ListItem* )
{
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    ListItem* item = new ListItem(*this);
    addItem(item);
    scrollToItem(item);
    editItem(item);
}

template<>
void ListOfIndexes::execRequest_ContextMenu<ListOfIndexes::Request_ContextMenu::Edit>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ListOfIndexes::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    scrollToItem(item);
    editItem(item);
}

template<>
void ListOfIndexes::execRequest_ContextMenu<ListOfIndexes::Request_ContextMenu::MultiLineEdit>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ListOfIndexes::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    scrollToItem(item);
    item->multiLineItem = true;
    editItem(item);
}

template<>
void ListOfIndexes::execRequest_ContextMenu<ListOfIndexes::Request_ContextMenu::Remove>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ListOfIndexes::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    // Notify about Change
    delete item;
}

template<>
void ListOfIndexes::execRequest_ContextMenu<ListOfIndexes::Request_ContextMenu::Clear>(ListItem*)
{
    if(curEditItemInfo.item)
    {
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
    // Maybe clear NewProcedures and replace Removed with savedProcedures?
    for(int i = 0; i < count(); i++){
        tryToManageIndexes(item(i)->text(), QString());
    }
    // Notify about Change::ClearedAll
    clear();
}

ListOfIndexes::ChangeAction ListOfIndexes::tryToManageIndexes(QString oldIndex, QString newIndex){
    Q_ASSERT_X(not (oldIndex.isEmpty() and newIndex.isEmpty()), "ListOfIndexes::tryToManageIndexes", "Only OldIndex or NewIndex can be empty ");

    return ListOfIndexes::ChangeAction::NoAction;
}


bool ListOfIndexes::eventFilter(QObject* obj, QEvent* ev){

    return Super::eventFilter(obj, ev);

}

void ListOfIndexes::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end){
    ListItem *pItem = item(start);
    if(not pItem->text().isEmpty())
        tryToManageIndexes(pItem->text(), QString());
    Super::rowsAboutToBeRemoved(parent, start, end);
    using ActionsList = decltype(parentWidget().parentWidget());
    ActionsList& actionsList = parentWidget().parentWidget();
    QListWidgetItem* item = actionsList.itemAt(actionsList.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    item->setSizeHint(actionsList.itemWidget(item)->sizeHint());




}

void ListOfIndexes::rowsInserted(const QModelIndex &parent, int start, int end){



    //parentWidget().parentWidget().updateGeometry();
    //parentWidget().parentWidget().parentWidget().parentWidget().updateGeometry();
    using ActionsList = decltype(parentWidget().parentWidget());
    ActionsList& actionsList = parentWidget().parentWidget();
    QListWidgetItem* item = actionsList.itemAt(actionsList.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    QWidget* widget = actionsList.itemWidget(item);
    if(widget)
        item->setSizeHint(actionsList.itemWidget(item)->sizeHint());
    Super::rowsInserted(parent, start, end);

}


bool ListOfIndexes::edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event){
    using Trigger = QAbstractItemView::EditTrigger;
    //CurEditItemInfo tempEditItem = curEditItemInfo;
    switch(trigger){
    case Trigger::AllEditTriggers:
    case Trigger::DoubleClicked:
        if(index.isValid()){
            curEditItemInfo.item = itemFromIndex(index);
            curEditItemInfo.oldIndex = curEditItemInfo.item->text();
            if(curEditItemInfo.item->multiLineItem){
                MultiLineEditor editor(curEditItemInfo.oldIndex);
                switch(editor.exec()){
                case QDialog::Accepted:
                    curEditItemInfo.item->setText(editor.text());
                    processEditData(curEditItemInfo);
                    break;
                case QDialog::Rejected:
                    curEditItemInfo = {};
                    break;
                }
                return false;
            }
        }
    break;
    default:
        break;
    }

    return Super::edit(index, trigger, event);
}

void ListOfIndexes::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint){
    using Hint = QAbstractItemDelegate::EndEditHint;
    CurEditItemInfo tempInfo = curEditItemInfo;
    Super::closeEditor(editor, hint);
    switch(hint){
    case Hint::EditNextItem:
    case Hint::EditPreviousItem:
    {
        if(curEditItemInfo.item != tempInfo.item){
            processEditData(tempInfo);
        }
    }
        break;
    default:
        processEditData(curEditItemInfo);

    }
}

void ListOfIndexes::processEditData(CurEditItemInfo& curEditItemInfo)
{
    if(curEditItemInfo.item){
        if(curEditItemInfo.item->text().isEmpty()){ // Remove Item
            if(not curEditItemInfo.oldIndex.isEmpty()){ // Removed Existing item
                /*ChangeAction changeAction =*/ tryToManageIndexes(curEditItemInfo.oldIndex, QString());
                // Notify about Change
            }
            delete curEditItemInfo.item;
        }else{ // Not empty (New Index or Change Index)
            if(curEditItemInfo.item->text() != curEditItemInfo.oldIndex){    // index changed
                if(curEditItemInfo.oldIndex.isEmpty()){  // New index
                    ChangeAction changeAction = tryToManageIndexes(QString(), curEditItemInfo.item->text());
                    if(changeAction == ChangeAction::DuplicatedError){ // Failed (Duplicated Index)
                        QMessageBox::warning(nullptr, QStringLiteral("Duplicated Index"), QStringLiteral("Index \"") + curEditItemInfo.item->text() + QStringLiteral("\" already exists."));
                        delete curEditItemInfo.item;
                    }else{
                        // Notify about change
                        curEditItemInfo.item->multiLineItem = curEditItemInfo.item->text().contains("\n");

                        //curEditItemInfo.item->setToolTip(curEditItemInfo.item->text());
                    }
                }else{  // Change Index
                    ChangeAction changeAction = tryToManageIndexes(curEditItemInfo.oldIndex, curEditItemInfo.item->text());
                    if(changeAction == ChangeAction::DuplicatedError){ // Failed (Duplicated Index)
                        QMessageBox::warning(nullptr, QStringLiteral("Duplicated Index"), QStringLiteral("Index \"") + curEditItemInfo.item->text() + QStringLiteral("\" already exists."));
                        curEditItemInfo.item->setText(curEditItemInfo.oldIndex);
                    }else{
                        // Notify about change
                        curEditItemInfo.item->multiLineItem = curEditItemInfo.item->text().contains("\n");
                        //curEditItemInfo.item->setToolTip(curEditItemInfo.item->text());
                    }
                }
            }
        }
    }
    curEditItemInfo = {};
}

void ListOfIndexes::contextMenuEvent(QContextMenuEvent *cev){

    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking

    using ActionFuncs = ListOfIndexes::Request_ContextMenu_Func[];
    using Request = ListOfIndexes::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &ListOfIndexes::execRequest_ContextMenu<Request::Add>,
        &ListOfIndexes::execRequest_ContextMenu<Request::Clear>,
    };
    constexpr uint functionsSize = std::extent_v<decltype(actionFunc)>;
    ContextMenuConfig contextMenuConfig;
    if(item){
        contextMenuConfig.addActions(
                    {
                          new QAction("Dodaj napis"),
                          new QAction("Edytuj napis"),
                          new QAction("Edytuj napis (edytor)"),
                          new QAction("Usuń napis"),
                          new QAction("Usuń wszystkie napisy")
                    });
    }else{
        contextMenuConfig.addActions(
                    {
                        new QAction("Dodaj napis"),
                        new QAction("Usuń wszystkie napisy")
                    });
    }
    parentContextMenu().extendContextMenu(contextMenuConfig);
    qsizetype&& index = contextMenuConfig.exec(cev);
    if(index >= 0){
        if(item){
            interpretContextMenuResponse(index, cev);
        }else{
            if(index >= functionsSize){
                parentContextMenu().interpretContextMenuResponse(index - functionsSize, cev);
            }else{
                (this->*(actionFunc[index]))(item);
            }
        }
    }

}

ActionView& ListOfIndexes::parentWidget()const{
    return *static_cast<ActionView*>(Super::parentWidget()->parentWidget()->parentWidget()); // Splitter -> Widget of ActionDataView layout -> ActionView
}



void ListOfIndexes::loadIndexes(QStringList listOfStrings){
    using String = QStringList::Iterator;
    //setUpdatesEnabled(false);
    for(String string = listOfStrings.begin(); string < listOfStrings.end(); string++)
    {
        ListItem* item = new ListItem(*this, *string);
        addItem(item);
    }
    //setUpdatesEnabled(true);
}

void ListOfIndexes::reloadGui(){

}

ParentContextMenu& ListOfIndexes::parentContextMenu()const
{
    return *static_cast<ParentContextMenu*>(&parentWidget().parentWidget()); // ActionView -> ActionList
}

void ListOfIndexes::extendContextMenu(ContextMenuConfig&)const{}


void ListOfIndexes::interpretContextMenuResponse(ContextMenuConfig::ActionIndex index, QContextMenuEvent* cev){
    using ActionFuncs = ListOfIndexes::Request_ContextMenu_Func[];
    using Request = ListOfIndexes::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &ListOfIndexes::execRequest_ContextMenu<Request::Add>,
        &ListOfIndexes::execRequest_ContextMenu<Request::Edit>,
        &ListOfIndexes::execRequest_ContextMenu<Request::MultiLineEdit>,
        &ListOfIndexes::execRequest_ContextMenu<Request::Remove>,
        &ListOfIndexes::execRequest_ContextMenu<Request::Clear>,
    };
    constexpr uint functionsSize = std::extent_v<decltype(actionFunc)>;
    if(index >= functionsSize){
        parentContextMenu().interpretContextMenuResponse(index - functionsSize, cev);
    }else{
        ListItem* item = itemAt(cev->pos());
        (this->*(actionFunc[index]))(item);
    }
}
