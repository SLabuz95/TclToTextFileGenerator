//#include"actions.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include"Conditional/conditionals.hpp"
#include"TcFileModifier/Config/Actions/FCT_products.hpp"
#include"Tcl2CaplPanels/ConfigEditor/rulesphasepanel.hpp"

using namespace Panels::Configuration::View;
using ActionsModifier = ModifierActionsFactory::ListOfBases;
using ActionsModifierList = ModifierActionsList::List<ActionsModifier>;
using ListItem = ActionsModifierList::ListItem;
using ActionView = ListItem::View;
using List = ListItem::List;
using Panel = ListItem::Panel;
using RawRuleView = List::RawRuleView;
template<class Base>
using ContextMenuInterface = Utils::ContextMenuBuilder::Interface<Base>;


template<>
RawRuleView& ActionsModifierList::parentWidget()const{
    //qDebug() << Super::parentWidget();
    //qDebug() << Super::parentWidget()->parentWidget();
    //qDebug() << Super::parentWidget()->parentWidget()->parentWidget();

    return *static_cast<RawRuleView*>(Super::parentWidget()->parentWidget()->parentWidget()); //Panel -> Splitter -> RuleView
}

template<>
Panel& ActionsModifierList::parentWidget_panel()const{
    return *static_cast<Panel*>(Super::parentWidget()); //Panel
}

template<>
ActionView::ActionView(List& list, const QStringList& typeCBItems, ActionPtr action)
    : actionTypeComboBox(typeCBItems)
{
    setParent(list.viewport());
    mainLayout.setVerticalSpacing(0);
    mainLayout.setContentsMargins(0,0,0,0);
    mainLayout.addRow("Typ akcji:",&actionTypeComboBox);
    if(action){
        actionTypeComboBox.setCurrentIndex(
                    Action::toUnderlyng(
                        ModifierActions::TypeInfo::toTypeForConfigPanel( action->type())
                        )
                    );
    }
    actionTypeComboBox.installEventFilter(this);
    actionTypeComboBox.view()->installEventFilter(this);
    QWidget* widget = new QWidget();
    mainLayout.addRow(widget);
    setLayout(&mainLayout);
    setParent(list.viewport());

    dataView_ = ActionDataView::createView(widget, action);
    if(dataView_){
        dataView_->setSpacing(0);
        dataView_->setContentsMargins(0,0,0,0);
        widget->setLayout(dataView_);
    }else{
        delete widget;
    }


}

template<>
List& ActionView::parentWidget()const{
    return *static_cast<List*>(Super::parentWidget()->parentWidget()); // Viewport (1 parent) -> List (2 parent)
}
template<>
void ActionView::readAction(ActionPtr& action){
    action = ModifierActionsFactory::create( // Tranlate index to actionType
                parentWidget().parentWidget().isExecutablesList(&parentWidget().parentWidget_panel())?
                ModifierActions::TypeInfo::fromExecutableTypeForConfigPanel(
                                                     Action::fromUnderlying(actionTypeComboBox.currentIndex())) :
                Action::fromUnderlying(actionTypeComboBox.currentIndex()));
    if(dataView_){
        dataView_->readAction(*action);
    }
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
void ActionsModifierList::loadActions(ActionsRef actions)
{
    using Action = std::decay_t<ActionsRef>::Iterator;
    //setUpdatesEnabled(false);
    for(Action action = actions.begin(); action < actions.end(); action++)
        addNewItem(*action);
    //setUpdatesEnabled(true);
}

template<>
void ActionsModifierList::readActions(ActionsRef actions)
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
void ActionsModifierList::execRequest_ContextMenu<ActionsModifierList::Request_ContextMenu::Add>(ListItem*)
{
   addNewItem();
}

template<>
template<>
void ActionsModifierList::execRequest_ContextMenu<ActionsModifierList::Request_ContextMenu::Clone>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    //item->clone();
    new ListItem(*item);
}

template<>
template<>
void ActionsModifierList::execRequest_ContextMenu<ActionsModifierList::Request_ContextMenu::Remove>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    delete item;

}

template<>
template<>
void ActionsModifierList::execRequest_ContextMenu<ActionsModifierList::Request_ContextMenu::Clear>(ListItem*)
{
    QListWidget& listWidget = parentWidget().parentWidget();
    QListWidgetItem* parentListItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    RawRuleView::ListsView listsView = parentWidget().listsView();
    int listsHeightDiff = listsView.conditionalList.height() - listsView.executablesList.height();
    clear();
    if(&listsView.conditionalList == this){ // Conditionals List calculation
        if(listsHeightDiff >= 0){ // Conditional List is greater
            parentListItem->setSizeHint(parentWidget().sizeHint() -= QSize(0, listsHeightDiff));
            qDebug() << "ConditonalList: Conditional List is greater: Change : Item SH" << parentListItem->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
        }else{// Executables List is greater
            // No change
            qDebug() << "ConditonalList: Executables List is greater: No change :Item SH" << parentListItem->sizeHint() << " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "List Diff " << listsHeightDiff;

        }
    }else{
        if(listsHeightDiff >= 0){ // Conditional List is greater
            qDebug() << "ExecutablesList: Conditional List is greater: No change :Item SH" << parentListItem->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "List Diff " << listsHeightDiff;
            // No change
        }else{// Executables List is greater
            qDebug() << "ExecutablesList: Conditional List is greater: Change :Item SH" << parentListItem->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "+List Diff " << listsHeightDiff;
            parentListItem->setSizeHint(parentWidget().sizeHint() += QSize(0, listsHeightDiff));
        }
    }
    qApp->processEvents();
}

template<>
ContextMenuInterface<QListWidget>& ActionsModifierList::parentContextMenu()const
{
    return *static_cast<ParentContextMenu*>(&parentWidget().parentWidget()); //Splitter -> RuleView
}

template<>
void ActionsModifierList::extendContextMenu(ContextMenuConfig& config)const
{
    config.addMenu("Akcje",{
                                     new QAction("Dodaj"),
                                     new QAction("Klonuj"),
                                     new QAction("Usuń"),
                                     new QAction("Usuń wszystkie")
                                   });
    parentContextMenu().extendContextMenu(config);
}

template<>
void ActionsModifierList::interpretContextMenuResponse(ContextMenuConfig::ActionIndex index, QContextMenuEvent* cev){
    using ActionFuncs = ActionsModifierList::Request_ContextMenu_Func[];
    using Request = ActionsModifierList::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &ActionsModifierList::execRequest_ContextMenu<Request::Add>,
        &ActionsModifierList::execRequest_ContextMenu<Request::Clone>,
        &ActionsModifierList::execRequest_ContextMenu<Request::Remove>,
        &ActionsModifierList::execRequest_ContextMenu<Request::Clear>,
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
void ActionsModifierList::contextMenuEvent(QContextMenuEvent *cev){
    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking
    using ActionFuncs = ActionsModifierList::Request_ContextMenu_Func[];
    using Request = ActionsModifierList::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &ActionsModifierList::execRequest_ContextMenu<Request::Add>,
        &ActionsModifierList::execRequest_ContextMenu<Request::Clear>,
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
            if(parentWidget().parentWidget().isExecutablesList(&parentWidget().parentWidget_panel())){
                createActionDataView(ModifierActions::TypeInfo::fromExecutableTypeForConfigPanel(
                                         Action::fromUnderlying(actionTypeComboBox.currentIndex())));
            }else{
                createActionDataView(Action::fromUnderlying(actionTypeComboBox.currentIndex()));
            }
        }
    }
        break;
    default:
        break;
    }
    return Super::eventFilter(obj, ev);
}


template<>
ActionsModifierList
::List()
{
    // Initiailzie
    /*setStyleSheet("QListView::item{"
    "border: 2px solid #6a6ea9;"
    "border-radius: 6px;"
    "}");*/
    //setMovement(QListView::Snap);
    //setHeaderLabels({"Akcje warunkowe"});
    //setIndentation(0);
    //setMovement(Snap);
    setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setDragDropOverwriteMode(true);

    viewport()->installEventFilter(this);
}


template<>
ListItem::ListItem(ActionsModifierList& list, ActionPtr action)
    : QListWidgetItem(&list),
      view_(list,
     list.parentWidget().isConditionalsList(&list.parentWidget_panel())?
        ModifierActions::TypeInfo::conditionalTypeNamesForConfigPanel() :
        ModifierActions::TypeInfo::executableTypeNamesForConfigPanel(),
        action) // PASS VIEWPORT AS PARENT
{
    //setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
    qDebug() << "Before AddItem";
    list.addItem(this);
    qDebug() << "After AddItem";
    list.setItemWidget(this, &view_);
    qDebug() << "After SetItem";
    setSizeHint( view().sizeHint());
    qDebug() << "After SetSizeHint";
}




template<>
ActionsModifierList &ListItem::list() const
{ return *static_cast<ActionsModifierList*>(QListWidgetItem::listWidget()); }

template<>
void List::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles){
    if(roles.contains(Qt::SizeHintRole)){
        QListWidget& listWidget = parentWidget().parentWidget();
        QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
        RawRuleView::ListsView listsView = parentWidget().listsView();
        int heightDiff = sizeHint().height() - height();
        int listsHeightDiff = listsView.conditionalList.height() - listsView.executablesList.height();
        qDebug() << "Height" << height() << "ListDiff" << listsHeightDiff << "HeightDiff" << heightDiff << "SH" << sizeHint() ;
        if(&listsView.conditionalList == this){ // Conditionals List calculation
            if(heightDiff >= 0){ // Increase size
                if(listsHeightDiff >= 0){ // Conditional List is greater
                    // Increse
                    qDebug() << "ConditonalList: Conditional List is greater: Increase Change " << heightDiff << " : Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                    item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff));
                    qDebug() << "ConditonalList: Conditional List is greater: Increase Change " << heightDiff << " : Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                }else{// Executables List is greater
                    if(heightDiff > -listsHeightDiff){ // If change greater than other list height
                        qDebug() << "ConditonalList: Executables List is greater: Increase Change " << heightDiff << " : Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff + listsHeightDiff));
                        qDebug() << "ConditonalList: Executables List is greater: Increase Change " << heightDiff << " : Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                    }else{
                        qDebug() << "ConditonalList: Executables List is greater Decrease No Change " << heightDiff << " : Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                    }
                }
            }else{ // Decrease size
                if(listsHeightDiff >= 0){ // Conditional List is greater
                    // Decrease
                    if(-heightDiff > listsHeightDiff){
                        qDebug() << "ConditonalList: Conditional List is greater: Decrease Change " << heightDiff << " : Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                        item->setSizeHint(parentWidget().sizeHint() -= QSize(0, listsHeightDiff));
                        qDebug() << "ConditonalList: Conditional List is greater: Decrease Change " << heightDiff << " : Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                    }else{
                        qDebug() << "ConditonalList: Conditional List is greater: Decrease Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff));
                        qDebug() << "ConditonalList: Conditional List is greater: Decrease Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                    }
                }else{// Executables List is greater
                    // No change
                    qDebug() << "ConditonalList: Executable List is greater: Decrease No Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                }
            }
        }else{
            if(heightDiff >= 0){ // Increase size
                if(listsHeightDiff >= 0){ // Conditional List is greater
                    if(heightDiff > listsHeightDiff){ // If change greater than other list height
                        qDebug() << "ExecutablesList: Conditional List is greater: Increase Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff - listsHeightDiff));
                        qDebug() << "ExecutablesList: Conditional List is greater: Increase Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                    }
                }else{// Executables List is greater
                    // Increse
                    qDebug() << "ExecutablesList: Executables List is greater: Increase Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                    item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff));
                    qDebug() << "ExecutablesList: Executables List is greater: Increase Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                }
            }else{ // Decrease size
                if(listsHeightDiff >= 0){ // Conditional List is greater
                    qDebug() << "ExecutablesList: Conditional List is greater: Decrease No Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                    // No change
                }else{// Executables List is greater                    
                    if(-heightDiff > -listsHeightDiff){ // If change greater than other list height
                        qDebug() << "ExecutablesList: Executables List is greater: Decrease Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, listsHeightDiff));
                        qDebug() << "ExecutablesList: Executables List is greater: Decrease Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                    }else{
                        qDebug() << "ExecutablesList: Executables List is greater: Decrease Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
                        item->setSizeHint(parentWidget().sizeHint() += QSize(0, heightDiff));
                        qDebug() << "ExecutablesList: Executables List is greater: Decrease Change  " << heightDiff << ": Item SH" << item->sizeHint() <<  " Raw Rule View SizeHint"  << parentWidget().sizeHint() << "Height" << height() << "-List Diff " << listsHeightDiff;
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
    if(&listsView.conditionalList == this){ // Conditionals List calculation
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
