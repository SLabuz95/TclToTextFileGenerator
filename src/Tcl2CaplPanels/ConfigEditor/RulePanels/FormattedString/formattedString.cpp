//#include"actions.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include"formattedString.hpp"

using namespace Panels::Configuration::View::FormattedString;

ItemView::ItemView(ListItem& item)
    : dataView_(ItemDataView::createView(*this))
{
    mainLayout.setSpacing(0);
    mainLayout.setContentsMargins(0,0,0,0);
    mainLayout.addRow(&titleComboBox);
    mainLayout.addRow(dataView_);
    setLayout(&mainLayout);
}

ItemView& ItemDataView::parentWidget()const
{
    return *static_cast<ItemView*>(QFormLayout::parentWidget());
}

List& ItemView::parentWidget()const{
    return *static_cast<List*>(Super::parentWidget()->parentWidget()); // Viewport (1 parent) -> List (2 parent)
}
/*
RawRuleView& ConditionalsList::parentWidget()const{
    return *static_cast<RawRuleView*>(Super::parentWidget()->parentWidget()); //Splitter -> RuleView
}

void ConditionalsList::loadActions(ActionsRef actions)
{
    using Action = std::decay_t<ActionsRef>::Iterator;
    setUpdatesEnabled(false);
    for(Action action = actions.begin(); action < actions.end(); action++)
        addNewItem(*action);
}
*/

template<>
void List::execRequest_ContextMenu<List::Request_ContextMenu::Add>(ListItem*)
{
   addNewItem();
}

template<>
void List::execRequest_ContextMenu<List::Request_ContextMenu::Clone>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    //item->clone();
    new ListItem(*item);
}

template<>
void List::execRequest_ContextMenu<List::Request_ContextMenu::Remove>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    delete item;
}

template<>
void List::execRequest_ContextMenu<List::Request_ContextMenu::Clear>(ListItem*)
{
    clear();
}


void List::extendContextMenu(ContextMenuConfig& config){
    config.addMenu("Akcje warunkowe",{
                                     new QAction("Dodaj"),
                                     new QAction("Klonuj"),
                                     new QAction("Usuń"),
                                     new QAction("Usuń wszystkie")
                                   });
    //parentWidget().parentWidget().extendContextMenu(config);
}

void List::interpretContextMenuResponse(ContextMenuConfig::ActionIndex index, QContextMenuEvent* cev){
    using ActionFuncs = List::Request_ContextMenu_Func[];
    using Request = List::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &List::execRequest_ContextMenu<Request::Add>,
        &List::execRequest_ContextMenu<Request::Clone>,
        &List::execRequest_ContextMenu<Request::Remove>,
        &List::execRequest_ContextMenu<Request::Clear>,
    };
    constexpr uint functionsSize = std::extent_v<decltype(actionFunc)>;
    if(index >= functionsSize){
        //parentWidget().parentWidget().interpretContextMenuResponse(index - functionsSize, cev);
    }else{
        ListItem* item = itemAt(cev->pos());
        (this->*(actionFunc[index]))(item);
    }
}

void List::contextMenuEvent(QContextMenuEvent *cev){
    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking
    using ActionFuncs = List::Request_ContextMenu_Func[];
    using Request = List::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &List::execRequest_ContextMenu<Request::Add>,
        &List::execRequest_ContextMenu<Request::Clear>,
    };
    constexpr uint functionsSize = std::extent_v<decltype(actionFunc)>;
    ContextMenuConfig contextMenuConfig;
    if(item){
        contextMenuConfig.addActions(
                    {
                          new QAction("Dodaj akcje"),
                          new QAction("Klonuj akcje"),
                          new QAction("Usuń akcje"),
                          new QAction("Usuń wszystkie akcje")
                    });
    }else{
        contextMenuConfig.addActions(
                    {
                        new QAction("Dodaj akcje"),
                        new QAction("Usuń wszystkie akcje")
                    });
    }
    //parentWidget().parentWidget().extendContextMenu(contextMenuConfig);
    qsizetype&& index = contextMenuConfig.exec(cev);
    if(index >= 0){
        if(item){
            interpretContextMenuResponse(index, cev);
        }else{
            if(index >= functionsSize){
                //parentWidget().parentWidget().interpretContextMenuResponse(index - functionsSize, cev);
            }else{
                (this->*(actionFunc[index]))(item);
            }
        }
    }
}

List
::List(){
    // Initiailzie
    /*setStyleSheet("QListView::item{"
    "border: 2px solid #6a6ea9;"
    "border-radius: 6px;"
    "}");*/
    //setMovement(QListView::Snap);
    //setHeaderLabels({"Akcje warunkowe"});
    //setIndentation(0);
    //setMovement(Snap);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setDragDropOverwriteMode(true);

    viewport()->installEventFilter(this);

}


ListItem::ListItem(List& list)
    : view_(*this)
{
    //setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
    list.addItem(this);
    list.setItemWidget(this, &view_);
    qApp->processEvents();
    setSizeHint(view().minimumSizeHint());
}

/*
ListItem::ListItem(List& list, ActionPtr action)
    : view_(*this)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
    //list.addItem(this);
    //list.setItemWidget(this, view_);
    //setSizeHint(view()->sizeHint());
};*/


List &ListItem::list() const
{ return *static_cast<List*>(QListWidgetItem::listWidget()); }

