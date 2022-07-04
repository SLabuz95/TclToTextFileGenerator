#include"actions.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"
#include"action.hpp"
#include"Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.hpp"

using namespace Panels::Configuration::View;
using Executables = ExecutablesFactory::ListOfBases;
using ExecutablesList = ActionsList::List<Executables>;
using ListItem = ExecutablesList::ListItem;
using ActionView = ListItem::View;
using List = ListItem::List;
using RawRuleView = List::RawRuleView;
template<class Base>
using ContextMenuInterface = Utils::ContextMenuBuilder::Interface<Base>;

template<>
ActionView::ActionView(ListItem& item)
   :   dataView_(ActionDataView::createView(*this, nullptr))
{
    mainLayout.setVerticalSpacing(0);
    mainLayout.setContentsMargins(0,0,0,0);
    mainLayout.addRow("Typ akcji:",&actionTypeComboBox);
    actionTypeComboBox.installEventFilter(this);
    actionTypeComboBox.view()->installEventFilter(this);
    if(dataView_){
        QWidget* widget = new QWidget();
        dataView_->setSpacing(0);
        dataView_->setContentsMargins(0,0,0,0);
        widget->setLayout(dataView_);
        mainLayout.addRow(widget);
    }
    setLayout(&mainLayout);
}

template<>
List& ActionView::parentWidget()const{
    return *static_cast<List*>(Super::parentWidget()->parentWidget()); // Viewport (1 parent) -> List (2 parent)
}

template<>
bool ActionView::createActionDataView(ActionType type){
    if(not dataView_ or dataView_->type() != type){
        if(dataView_){
            mainLayout.removeRow(mainLayout.rowCount() - 1);
        }
        dataView_ = ActionDataView::createView(*this, type);
        if(dataView_){
            QWidget* widget = new QWidget();
            dataView_->setSpacing(0);
            dataView_->setContentsMargins(0,0,0,0);
            widget->setLayout(dataView_);
            mainLayout.addRow(widget);
        }
        qApp->processEvents();
        QListWidget& listWidget = parentWidget();
        QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
        item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
        qApp->processEvents();
    }
    return true;
}

template<>
RawRuleView& ExecutablesList::parentWidget()const{
    return *static_cast<RawRuleView*>(Super::parentWidget()->parentWidget()); //Splitter -> RuleView
}

template<>
void ExecutablesList::loadActions(ActionsRef actions)
{
    using Action = std::decay_t<ActionsRef>::Iterator;
    setUpdatesEnabled(false);
    for(Action action = actions.begin(); action < actions.end(); action++)
        addNewItem(*action);
}


template<>
template<>
void ExecutablesList::execRequest_ContextMenu<ExecutablesList::Request_ContextMenu::Add>(ListItem*)
{
   addNewItem();
}

template<>
template<>
void ExecutablesList::execRequest_ContextMenu<ExecutablesList::Request_ContextMenu::Clone>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    //item->clone();
    new ListItem(*item);
}

template<>
template<>
void ExecutablesList::execRequest_ContextMenu<ExecutablesList::Request_ContextMenu::Remove>(ListItem* item)
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
void ExecutablesList::execRequest_ContextMenu<ExecutablesList::Request_ContextMenu::Clear>(ListItem*)
{
    clear();

    qApp->processEvents();
    QListWidget& listWidget = parentWidget().parentWidget();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());
}

template<>
ContextMenuInterface<QListWidget>& ExecutablesList::parentContextMenu()const
{
    return *static_cast<ParentContextMenu*>(&parentWidget().parentWidget()); //Splitter -> RuleView
}

template<>
void ExecutablesList::extendContextMenu(ContextMenuConfig& config)const{
    config.addMenu("Akcje",{
                                     new QAction("Dodaj"),
                                     new QAction("Klonuj"),
                                     new QAction("Usuń"),
                                     new QAction("Usuń wszystkie")
                                   });
    parentContextMenu().extendContextMenu(config);
}

template<>
void ExecutablesList::interpretContextMenuResponse(ContextMenuConfig::ActionIndex index, QContextMenuEvent* cev){
    using ActionFuncs = ExecutablesList::Request_ContextMenu_Func[];
    using Request = ExecutablesList::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &ExecutablesList::execRequest_ContextMenu<Request::Add>,
        &ExecutablesList::execRequest_ContextMenu<Request::Clone>,
        &ExecutablesList::execRequest_ContextMenu<Request::Remove>,
        &ExecutablesList::execRequest_ContextMenu<Request::Clear>,
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
void ExecutablesList::contextMenuEvent(QContextMenuEvent *cev){
    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking
    using ActionFuncs = ExecutablesList::Request_ContextMenu_Func[];
    using Request = ExecutablesList::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &ExecutablesList::execRequest_ContextMenu<Request::Add>,
        &ExecutablesList::execRequest_ContextMenu<Request::Clear>,
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
ExecutablesList
::List(){
    // Initiailzie
    /*setStyleSheet("QListView::item{"
    "border: 2px solid #6a6ea9;"
    "border-radius: 6px;"
    "}");*/
    //setMovement(QListView::Snap);
    //setHeaderLabels({"Akcje wykonywalne"});
    //setIndentation(0);
    //setMovement(Snap);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setDragDropMode(QAbstractItemView::InternalMove);
    setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setDragDropOverwriteMode(true);

    viewport()->installEventFilter(this);

}


template<>
ListItem::ListItem(ExecutablesList& list)
    : view_(*this)
{
    //setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
    list.addItem(this);
    list.setItemWidget(this, &view_);
    qApp->processEvents();
    setSizeHint(view().minimumSizeHint());
}


template<>
ListItem::ListItem(ExecutablesList& list, ActionPtr action)
    : view_(*this)
{
    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
    //list.addItem(this);
    //list.setItemWidget(this, view());
    //setSizeHint(view()->sizeHint());
};


template<>
ExecutablesList &ListItem::list() const
{
    return *static_cast<ExecutablesList*>(QListWidgetItem::listWidget());
}
