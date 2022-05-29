#include"compareNumbOfArgs.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using CompareNumbOfArgsActionView = ActionsList_NS::CompareNumbOfArgsActionView;
using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using ActionView = CompareNumbOfArgsActionView::ActionView;

// CompareNumbOfArgs Action View Definitions -----------------------------------

CompareNumbOfArgsActionView::CompareNumbOfArgsActionView(ActionView& view)
{
    addRow(&listOfIndexes);
}

CompareNumbOfArgsActionView::DataView* CompareNumbOfArgsActionView::create(ActionView& view, ActionRef){
    return new CompareNumbOfArgsActionView(view);
}



// List of Indexes Definitions ---------------------------------------------------


template<>
void ListOfIndexes::execRequest_ContextMenu<ListOfIndexes::Request_ContextMenu::Add>(ListItem* )
{

    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {new ListItem, QString()};
    addItem(curEditItemInfo.item);
    editItem(curEditItemInfo.item);
}

template<>
void ListOfIndexes::execRequest_ContextMenu<ListOfIndexes::Request_ContextMenu::Edit>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ListOfIndexes::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {item, item->text()};
    editItem(item);
}

template<>
void ListOfIndexes::execRequest_ContextMenu<ListOfIndexes::Request_ContextMenu::Remove>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ListOfIndexes::ContextMenu::editProcedure", "Item is null");
    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        curEditItemInfo = {};
        qApp->processEvents();
    }
    ChangeAction changeAction = tryToManageIndexes(item->text(), QString());
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
    curEditItemInfo = {};
    // Maybe clear NewProcedures and replace Removed with savedProcedures?
    for(int i = 0; i < count(); i++){
        tryToManageIndexes(item(i)->text(), QString());
    }
    // Notify about Change::ClearedAll
    clear();
}

ListOfIndexes::ChangeAction ListOfIndexes::tryToManageIndexes(QString oldIndex, QString newIndex){
    Q_ASSERT_X(not (oldIndex.isEmpty() and newIndex.isEmpty()), "ListOfIndexes::tryToManageIndexes", "Only OldIndex or NewIndex can be empty ");

}


bool ListOfIndexes::eventFilter(QObject* obj, QEvent* ev){    

    if(ev->type() == QEvent::MouseButtonDblClick and obj == viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        if((curEditItemInfo.item = itemAt(mev->pos()))){
            curEditItemInfo.oldIndex = curEditItemInfo.item->text();
            editItem(curEditItemInfo.item);
        }
    }

    if(ev->type() == QEvent::ChildRemoved and obj == viewport()){
        if(curEditItemInfo.item){
            if(curEditItemInfo.item->text().isEmpty()){ // Remove Item
                if(not curEditItemInfo.oldIndex.isEmpty()){ // old index isnt empty (removeIndex)
                    ChangeAction changeAction = tryToManageIndexes(curEditItemInfo.oldIndex, QString());
                    // Notify about Change
                }
                delete curEditItemInfo.item;
            }else{  // Not empty (New Index or Change Index)
                if(curEditItemInfo.item->text() != curEditItemInfo.oldIndex){    // index changed
                    if(curEditItemInfo.oldIndex.isEmpty()){  // New index
                        ChangeAction changeAction = tryToManageIndexes(QString(), curEditItemInfo.item->text());
                        if(changeAction == ChangeAction::DuplicatedError){ // Failed (Duplicated Index)
                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Index"), QStringLiteral("Index \"") + curEditItemInfo.item->text() + QStringLiteral("\" already exists."));
                            delete curEditItemInfo.item;
                        }else{
                            // Notify about change

                        }
                    }else{  // Change Index
                        ChangeAction changeAction = tryToManageIndexes(curEditItemInfo.oldIndex, curEditItemInfo.item->text());
                        if(changeAction == ChangeAction::DuplicatedError){ // Failed (Duplicated Index)
                            QMessageBox::warning(nullptr, QStringLiteral("Duplicated Index"), QStringLiteral("Index \"") + curEditItemInfo.item->text() + QStringLiteral("\" already exists."));
                            curEditItemInfo.item->setText(curEditItemInfo.oldIndex);
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
                          new QAction("Dodaj indeks"),
                          new QAction("Edytuj indeks"),
                          new QAction("Usuń indeks"),
                          new QAction("Usuń wszystkie indeksy")
                    });
    }else{
        contextMenuConfig.addActions(
                    {
                        new QAction("Dodaj indeks"),
                        new QAction("Usuń wszystkie indeksy")
                    });
    }
    parentWidget().parentWidget().extendContextMenu(contextMenuConfig);
    qsizetype&& index = contextMenuConfig.exec(cev);
    if(index >= 0){
        if(item){
            interpretContextMenuResponse(index, cev);
        }else{
            if(index >= functionsSize){
                parentWidget().parentWidget().interpretContextMenuResponse(index - functionsSize, cev);
            }else{
                (this->*(actionFunc[index]))(item);
            }
        }
    }

}

ActionView& ListOfIndexes::parentWidget()const{
    return *static_cast<ActionView*>(Super::parentWidget());
}

void ListOfIndexes::loadIndexes(){
    clearChanges();

}

void ListOfIndexes::reloadGui(){

}

//void ListOfIndexes::extendContextMenu(ContextMenuConfig& config);


void ListOfIndexes::interpretContextMenuResponse(ContextMenuConfig::ActionIndex index, QContextMenuEvent* cev){
    using ActionFuncs = ListOfIndexes::Request_ContextMenu_Func[];
    using Request = ListOfIndexes::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &ListOfIndexes::execRequest_ContextMenu<Request::Add>,
        &ListOfIndexes::execRequest_ContextMenu<Request::Edit>,
        &ListOfIndexes::execRequest_ContextMenu<Request::Remove>,
        &ListOfIndexes::execRequest_ContextMenu<Request::Clear>,
    };
    constexpr uint functionsSize = std::extent_v<decltype(actionFunc)>;
    if(index >= functionsSize){
        parentWidget().parentWidget().interpretContextMenuResponse(index - functionsSize, cev);
    }else{
        ListItem* item = itemAt(cev->pos());
        (this->*(actionFunc[index]))(item);
    }
}
