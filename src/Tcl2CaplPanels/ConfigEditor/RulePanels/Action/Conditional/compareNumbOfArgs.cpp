#include"compareNumbOfArgs.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using CompareNumbOfArgsActionView = ActionsList_NS::CompareNumbOfArgsActionView;
using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using ActionView = CompareNumbOfArgsActionView::ActionView;
using ParentContextMenu = CompareNumbOfArgsActionView::ParentContextMenu;

// CompareNumbOfArgs Action View Definitions -----------------------------------

CompareNumbOfArgsActionView::CompareNumbOfArgsActionView(QWidget* parent)
    : ActionDataView(parent)
{
    addWidget(&listOfIndexes);
}

CompareNumbOfArgsActionView::CompareNumbOfArgsActionView(QWidget* parent, ActionRef pAction)
    : CompareNumbOfArgsActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        listOfIndexes.loadIndexes(action.numbOfArgs());
    }
}

void CompareNumbOfArgsActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    listOfIndexes.readAll(action.numbOfArgs());
}

CompareNumbOfArgsActionView::DataView* CompareNumbOfArgsActionView::create(QWidget* parent, ActionRef action){
    return new CompareNumbOfArgsActionView(parent, action);
}


void ListOfIndexes::loadIndexes(const Action::NumbOfArgumentsList& list){
    using Index = Action::NumbOfArgumentsList::ConstIterator;
    setUpdatesEnabled(false);
    for(Index index = list.constBegin(); index < list.constEnd(); index++)
    {
        ListItem* item = new ListItem(*this, *index);
        addItem(item);
    }
    setUpdatesEnabled(true);
}

QWidget& ListOfIndexes::itemView()const{
    // Splitter -> Widget (Widget with Layout of DataView) -> ItemView (Any)
    return *Super::parentWidget()->parentWidget();
}

QListWidget& ListOfIndexes::itemListView()const{
    // itemView -> Viewport -> List
    return *static_cast<QListWidget*>(itemView().parentWidget()->parentWidget());
}

// List of Indexes Definitions ---------------------------------------------------

QWidget* ListOfIndexes::ItemDelegate::
createEditor(QWidget* parent,
             const QStyleOptionViewItem& ,
             const QModelIndex& index) const
{
    // For List Item Type == EmptyStringItem dont edit
    QLineEdit* editor = nullptr;
    editor = new QLineEdit(parent);
    editor->setValidator(new QRegularExpressionValidator(QRegularExpression(RegExpCore::regExpForUint)));
    return editor;
}


void ListOfIndexes::ItemDelegate::
setEditorData(QWidget* editor,
              const QModelIndex& index) const
{
   QItemDelegate::setEditorData(editor, index);
}

void ListOfIndexes::ItemDelegate::
setModelData(QWidget* editor,
             QAbstractItemModel* model,
             const QModelIndex& index) const
{
   QItemDelegate::setModelData(editor, model, index);
}

void ListOfIndexes::ItemDelegate::
updateEditorGeometry(QWidget* editor,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) const
{
   QItemDelegate::updateEditorGeometry(editor, option, index);
}

template<>
void ListOfIndexes::execRequest_ContextMenu<ListOfIndexes::Request_ContextMenu::Add>(ListItem* )
{

    if(curEditItemInfo.item){
        closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }

    curEditItemInfo = {new ListItem(*this), QString()};
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
    // Notify about Change
    delete item;
    qApp->processEvents();
    QListWidget& listWidget = itemListView();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());
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

    // Notify about Change::ClearedAll
    clear();
    qApp->processEvents();
    QListWidget& listWidget = itemListView();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());
}

ListOfIndexes::ChangeAction ListOfIndexes::tryToManageIndexes(QString oldIndex, QString newIndex){
    if(curEditItemInfo.item){
        // Confirm that index or argument isnt duplicated
        if(curEditItemInfo.item->text().isEmpty()){
            delete curEditItemInfo.item;
            qApp->processEvents();
            QListWidget& listWidget = itemListView();
            QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
            item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
        }else{
            // Confirm that index or argument isnt duplicated
            // Remove if its
            bool indexOrArgumentIsntDuplicated = true;
            QString textToCompare = curEditItemInfo.item->text();

            for(int itemIndex = 0 ; itemIndex < count(); itemIndex++){
                if(item(itemIndex) != curEditItemInfo.item and
                        item(itemIndex)->text() == textToCompare){
                    indexOrArgumentIsntDuplicated = false;
                    break;
                }
            }

            if(indexOrArgumentIsntDuplicated){

            }else{
                delete curEditItemInfo.item;
                qApp->processEvents();
                QListWidget& listWidget = itemListView();
                QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
                item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
            }

        }
        curEditItemInfo = {};
    }
}


bool ListOfIndexes::eventFilter(QObject* obj, QEvent* ev){    

    if(ev->type() == QEvent::ChildRemoved and obj == viewport()){
        tryToManageIndexes(curEditItemInfo.oldIndex, QString());

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


void ListOfIndexes::reloadGui(){

}

ParentContextMenu& ListOfIndexes::parentContextMenu()const
{
    return *static_cast<ParentContextMenu*>(&itemListView()); // ActionView -> ActionList
}

void ListOfIndexes::extendContextMenu(ContextMenuConfig&)const{}

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
        parentContextMenu().interpretContextMenuResponse(index - functionsSize, cev);
    }else{
        ListItem* item = itemAt(cev->pos());
        (this->*(actionFunc[index]))(item);
    }
}
