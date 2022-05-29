//#include"actions.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"Conditional/conditionals.hpp"
#include"Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.hpp"


#include"action.hpp"

using namespace Panels::Configuration::View;
using Conditionals = ConditionalsFactory::ListOfBases;
using ConditionalsList = ActionsList::List<Conditionals>;
using ListItem = ConditionalsList::ListItem;
using ActionView = ListItem::View;
using List = ListItem::List;
using RawRuleView = List::RawRuleView;

template<>
ActionView::ActionView(ListItem& item)
    : dataView_(ActionDataView::create(*this))
{
    mainLayout.setSpacing(0);
    mainLayout.setContentsMargins(0,0,0,0);
    mainLayout.addRow(&actionTypeComboBox);
    mainLayout.addRow(dataView_);
    setLayout(&mainLayout);
}

template<>
List& ActionView::parentWidget()const{
    return *static_cast<List*>(Super::parentWidget()->parentWidget()); // Viewport (1 parent) -> List (2 parent)
}

template<>
RawRuleView& ConditionalsList::parentWidget()const{
    return *static_cast<RawRuleView*>(Super::parentWidget()->parentWidget()); //Splitter -> RuleView
}

template<>
void ConditionalsList::loadActions(ActionsRef actions)
{
    using Action = std::decay_t<ActionsRef>::Iterator;
    setUpdatesEnabled(false);
    for(Action action = actions.begin(); action < actions.end(); action++)
        addNewItem(*action);
}


template<>
template<>
void ConditionalsList::execRequest_ContextMenu<ConditionalsList::Request_ContextMenu::Add>(ListItem*)
{
   addNewItem();
}

template<>
template<>
void ConditionalsList::execRequest_ContextMenu<ConditionalsList::Request_ContextMenu::Clone>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    //item->clone();
    new ListItem(*item);
}

template<>
template<>
void ConditionalsList::execRequest_ContextMenu<ConditionalsList::Request_ContextMenu::Remove>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    delete item;
}

template<>
template<>
void ConditionalsList::execRequest_ContextMenu<ConditionalsList::Request_ContextMenu::Clear>(ListItem*)
{
    clear();
}


template<>
void ConditionalsList::extendContextMenu(ContextMenuConfig& config){
    config.addMenu("Akcje warunkowe",{
                                     new QAction("Dodaj"),
                                     new QAction("Klonuj"),
                                     new QAction("Usuń"),
                                     new QAction("Usuń wszystkie")
                                   });
    parentWidget().parentWidget().extendContextMenu(config);
}

template<>
void ConditionalsList::interpretContextMenuResponse(ContextMenuConfig::ActionIndex index, QContextMenuEvent* cev){
    using ActionFuncs = ConditionalsList::Request_ContextMenu_Func[];
    using Request = ConditionalsList::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &ConditionalsList::execRequest_ContextMenu<Request::Add>,
        &ConditionalsList::execRequest_ContextMenu<Request::Clone>,
        &ConditionalsList::execRequest_ContextMenu<Request::Remove>,
        &ConditionalsList::execRequest_ContextMenu<Request::Clear>,
    };
    constexpr uint functionsSize = std::extent_v<decltype(actionFunc)>;
    if(index >= functionsSize){
        parentWidget().parentWidget().interpretContextMenuResponse(index - functionsSize, cev);
    }else{
        ListItem* item = itemAt(cev->pos());
        (this->*(actionFunc[index]))(item);
    }
}

template<>
void ConditionalsList::contextMenuEvent(QContextMenuEvent *cev){
    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking
    using ActionFuncs = ConditionalsList::Request_ContextMenu_Func[];
    using Request = ConditionalsList::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &ConditionalsList::execRequest_ContextMenu<Request::Add>,
        &ConditionalsList::execRequest_ContextMenu<Request::Clear>,
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

template<>
ConditionalsList
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


template<>
ListItem::ListItem(ConditionalsList& list)
    : view_(*this)
{
    //setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
    list.addItem(this);
    list.setItemWidget(this, &view_);
    qApp->processEvents();
    setSizeHint(view().minimumSizeHint());
}


template<>
ListItem::ListItem(ConditionalsList& list, ActionPtr action)
    : view_(*this)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
    //list.addItem(this);
    //list.setItemWidget(this, view_);
    //setSizeHint(view()->sizeHint());
};


template<>
ConditionalsList &ListItem::list() const
{ return *static_cast<ConditionalsList*>(QListWidgetItem::listWidget()); }

