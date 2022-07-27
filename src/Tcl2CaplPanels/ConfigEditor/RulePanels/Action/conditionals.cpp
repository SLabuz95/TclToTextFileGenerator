//#include"actions.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include"Conditional/conditionals.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.hpp"


#include"action.hpp"

using namespace Panels::Configuration::View;
using Conditionals = ConditionalsFactory::ListOfBases;
using ConditionalsList = ActionsList::List<Conditionals>;
using ListItem = ConditionalsList::ListItem;
using ActionView = ListItem::View;
using List = ListItem::List;
using RawRuleView = List::RawRuleView;
template<class Base>
using ContextMenuInterface = Utils::ContextMenuBuilder::Interface<Base>;

template<>
ActionView::ActionView(List& list, ActionPtr action)
 : Super(list.viewport())
{    
    mainLayout.setVerticalSpacing(0);
    mainLayout.setContentsMargins(0,0,0,0);
    mainLayout.addRow("Typ akcji:",&actionTypeComboBox);
    if(action)
        actionTypeComboBox.setCurrentIndex( Action::toUnderlyng(action->type()) );
    actionTypeComboBox.installEventFilter(this);
    actionTypeComboBox.view()->installEventFilter(this);
    QWidget* widget = new QWidget();
    mainLayout.addRow(widget);
    setLayout(&mainLayout);

    dataView_ = ActionDataView::createView(widget, action);
    if(dataView_){
        dataView_->setSpacing(0);
        dataView_->setContentsMargins(0,0,0,0);
        widget->setLayout(dataView_);
    }

}

template<>
void ActionView::readAction(ActionPtr& action){
    action = ConditionalsFactory::create(Action::fromUnderlying(actionTypeComboBox.currentIndex()));
    if(dataView_){
        dataView_->readAction(*action);
    }
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
bool ActionView::createActionDataView(ActionType type){
    if(not dataView_ or dataView_->type() != type){
        QWidget* widget = new QWidget();
        if(dataView_){
            mainLayout.removeRow(mainLayout.rowCount() - 1);
        }
        mainLayout.addRow(widget);
        dataView_ = ActionDataView::createView(widget, type);
        if(dataView_){
            dataView_->setSpacing(0);
            dataView_->setContentsMargins(0,0,0,0);
        }else{
            delete widget;
        }
        qApp->processEvents();
        QListWidget& listWidget = parentWidget();
        QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
        QWidget* wwidget = listWidget.itemWidget(item);
        if(wwidget)
            item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
        qApp->processEvents();
    }
    return true;
}

template<>
void ConditionalsList::loadActions(ActionsRef actions)
{
    using Action = std::decay_t<ActionsRef>::Iterator;
    //setUpdatesEnabled(false);
    for(Action action = actions.begin(); action < actions.end(); action++)
        addNewItem(*action);
    //setUpdatesEnabled(true);
}

template<>
void ConditionalsList::readActions(ActionsRef actions)
{
    actions.reserve(count());
    actions.resize(count());
    using Action = std::decay_t<ActionsRef>::Iterator;
    Action action = actions.begin();
    for(int i = 0; i < actions.size(); i++, action++)
        item(i)->readAction(*action);
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
    qApp->processEvents();
    QListWidget& listWidget = parentWidget().parentWidget();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());

}

template<>
template<>
void ConditionalsList::execRequest_ContextMenu<ConditionalsList::Request_ContextMenu::Clear>(ListItem*)
{
    clear();    
    qApp->processEvents();
    QListWidget& listWidget = parentWidget().parentWidget();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());
}

template<>
ContextMenuInterface<QListWidget>& ConditionalsList::parentContextMenu()const
{
    return *static_cast<ParentContextMenu*>(&parentWidget().parentWidget()); //Splitter -> RuleView
}

template<>
void ConditionalsList::extendContextMenu(ContextMenuConfig& config)const
{
    config.addMenu("Akcje warunkowe",{
                                     new QAction("Dodaj"),
                                     new QAction("Klonuj"),
                                     new QAction("Usuń"),
                                     new QAction("Usuń wszystkie")
                                   });
    parentContextMenu().extendContextMenu(config);
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
        parentContextMenu().interpretContextMenuResponse(index - functionsSize, cev);
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

template<>
bool ActionView::eventFilter(QObject* obj, QEvent* ev){
    switch(ev->type()){    
    case QEvent::ContextMenu:
    {
        if(obj == &actionTypeComboBox){
            parentWidget().contextMenuEvent(static_cast<QContextMenuEvent*>(ev));
        }
    }
        break;
    case QEvent::Leave:
    {
        if(obj == actionTypeComboBox.view()){
            createActionDataView(Action::fromUnderlying(actionTypeComboBox.currentIndex()));
        }
    }
        break;        
    default:
        break;
    }
    return Super::eventFilter(obj, ev);
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
    setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setDragDropOverwriteMode(true);

    viewport()->installEventFilter(this);

}


template<>
ListItem::ListItem(ConditionalsList& list, ActionPtr action)
    : QListWidgetItem(&list), view_(list, action) // PASS VIEWPORT AS PARENT
{
    //setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
    list.addItem(this);
    list.setItemWidget(this, &view_);
    qApp->processEvents();
}




template<>
ConditionalsList &ListItem::list() const
{ return *static_cast<ConditionalsList*>(QListWidgetItem::listWidget()); }


