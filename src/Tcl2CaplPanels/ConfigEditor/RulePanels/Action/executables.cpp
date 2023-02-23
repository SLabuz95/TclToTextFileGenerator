#include"actions.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"
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
ActionView::ActionView(List& list, ActionPtr action)
{
    setParent(list.viewport());
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
    action = ExecutablesFactory::create(Action::fromUnderlying(actionTypeComboBox.currentIndex()));

    if(dataView_){
        dataView_->readAction(*action);
    }
}

template<>
List& ActionView::parentWidget()const{
    return *static_cast<List*>(Super::parentWidget()->parentWidget()); // Viewport (1 parent) -> List (2 parent)
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
        item->setSizeHint(sizeHint());
        qApp->processEvents();
    }
    return true;
}

template<>
RawRuleView& ExecutablesList::parentWidget()const{
    return *static_cast<RawRuleView*>(Super::parentWidget()->parentWidget()->parentWidget()); //Panel -> Splitter -> RuleView
}

template<>
void ExecutablesList::loadActions(ActionsRef actions)
{
    using Action = std::decay_t<ActionsRef>::Iterator;
    setUpdatesEnabled(false);
    for(Action action = actions.begin(); action < actions.end(); action++)
        addNewItem(*action);
    setUpdatesEnabled(true);
}

template<>
void ExecutablesList::readActions(ActionsRef actions)
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
}

template<>
template<>
void ExecutablesList::execRequest_ContextMenu<ExecutablesList::Request_ContextMenu::Clear>(ListItem*)
{
    QListWidget& listWidget = parentWidget().parentWidget();
    QListWidgetItem* parentListItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    RawRuleView::ListsView listsView = parentWidget().listsView();
    int listsHeightDiff = listsView.conditionalList.height() - listsView.executablesList.height();
    clear();
    if(listsHeightDiff >= 0){ // Conditional List is greater
        // No change
    }else{// Executables List is greater
        parentListItem->setSizeHint(parentWidget().sizeHint() += QSize(0, listsHeightDiff));
    }
    qApp->processEvents();
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
    setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setDragDropOverwriteMode(true);

    viewport()->installEventFilter(this);

}


template<>
ListItem::ListItem(ExecutablesList& list, ActionPtr action)
    : QListWidgetItem(&list), view_(list, action)
{
    //setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
    list.addItem(this);
    list.setItemWidget(this, &view_);
    setSizeHint(QSize(0, view().sizeHint().height()));
}


template<>
ExecutablesList &ListItem::list() const
{
    return *static_cast<ExecutablesList*>(QListWidgetItem::listWidget());
}

template<>
void List::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles){
    if(roles.contains(Qt::SizeHintRole)){
        QListWidget& listWidget = parentWidget().parentWidget();
        QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
        RawRuleView::ListsView listsView = parentWidget().listsView();
        int heightDiff = sizeHint().height() - height();
        int listsHeightDiff = listsView.conditionalList.height() - listsView.executablesList.height();
        if(&listsView.conditionalList == static_cast<void*>(this)){ // Conditionals List calculation
            if(heightDiff >= 0){ // Increase size
                if(listsHeightDiff >= 0){ // Conditional List is greater
                    // Increse
                    item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff));
                }else{// Executables List is greater
                    if(heightDiff > -listsHeightDiff){ // If change greater than other list height
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff + listsHeightDiff));
                    }
                }
            }else{ // Decrease size
                if(listsHeightDiff >= 0){ // Conditional List is greater
                    // Decrease
                    if(-heightDiff > listsHeightDiff){
                        item->setSizeHint(parentWidget().sizeHint() -= QSize(0, listsHeightDiff));
                    }else{
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff));
                    }
                }else{// Executables List is greater
                    // No change
                }
            }
        }else{
            if(heightDiff >= 0){ // Increase size
                if(listsHeightDiff >= 0){ // Conditional List is greater
                    if(heightDiff > listsHeightDiff){ // If change greater than other list height
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff - listsHeightDiff));
                    }
                }else{// Executables List is greater
                    // Increse
                    item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff));
                }
            }else{ // Decrease size
                if(listsHeightDiff >= 0){ // Conditional List is greater
                    // No change
                }else{// Executables List is greater
                    if(-heightDiff > -listsHeightDiff){ // If change greater than other list height
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, listsHeightDiff));
                    }else{
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff));
                    }
                }
            }
        }
    }
    Super::dataChanged(topLeft, bottomRight, roles);
}

template<>
void List::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end){
    QListWidget& listWidget = parentWidget().parentWidget();
    QListWidgetItem* parentListItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    RawRuleView::ListsView listsView = parentWidget().listsView();
    int heightDiff = 0;
    int listsHeightDiff = listsView.conditionalList.height() - listsView.executablesList.height();
    for(int i = start; i <= end; i++)
        heightDiff += item(i)->sizeHint().height();
    if(&listsView.conditionalList == static_cast<void*>(this)){ // Conditionals List calculation
        if(listsHeightDiff >= 0){ // Conditional List is greater
            // Decrease
            if(heightDiff > listsHeightDiff){
                parentListItem->setSizeHint(parentWidget().sizeHint() -= QSize(0, listsHeightDiff));
            }else{
                parentListItem->setSizeHint(parentWidget().sizeHint() -= QSize(0, heightDiff));
            }
        }else{// Executables List is greater
            // No change
        }
    }else{
        if(listsHeightDiff >= 0){ // Conditional List is greater
            // No change
        }else{// Executables List is greater
            if(heightDiff > -listsHeightDiff){ // If change greater than other list height
                parentListItem->setSizeHint(parentWidget().sizeHint() += QSize(0, listsHeightDiff));
            }else{
                parentListItem->setSizeHint(parentWidget().sizeHint() -= QSize(0, heightDiff));
            }
        }
    }
    Super::rowsAboutToBeRemoved(parent, start, end);
}
