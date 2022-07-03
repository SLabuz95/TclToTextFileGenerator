//#include"actions.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include"formattedString.hpp"

using namespace Panels::Configuration::View::FormattedString;

ItemView::ItemView(ListItem& item)
    : formatRule(FormatParametersFactory::create()),
      dataView_(ItemDataView::createView(*this, formatRule))
{
    using OutputOption2FormatRuleMap = const QStringList;
    OutputOption2FormatRuleMap outputOption2FormatRuleMap =
    {
        "Tekst",
        "Nazwa procedury",
        "Argument o indeksie",
        "Wszystkie argumenty od wybranego indeksu",
        //"Separator",
        "Format"
    };
    QWidget* widget = new QWidget();
    mainLayout.setSpacing(0);
    mainLayout.setContentsMargins(0,0,0,0);
    mainLayout.addRow("Typ akcji:", &titleComboBox);
    titleComboBox.addItems(outputOption2FormatRuleMap);
    dataView_->setSpacing(0);
    dataView_->setContentsMargins(0,0,0,0);
    titleComboBox.installEventFilter(this);
    titleComboBox.view()->installEventFilter(this);
    widget->setLayout(dataView_);
    mainLayout.addRow(widget);
    setLayout(&mainLayout);
    qApp->processEvents();
}


List& ItemView::parentWidget()const{
    return *static_cast<List*>(Super::parentWidget()->parentWidget()); // Viewport (1 parent) -> List (2 parent)
}


bool ItemView::createFormatRuleDataView(FormatRuleType type){
    if(not dataView_ or dataView_->type() != type){
        delete formatRule;
        if(dataView_){
            mainLayout.removeRow(mainLayout.rowCount() - 1);
        }
        formatRule = FormatParametersFactory::create(type);
        dataView_ = ItemDataView::createView(*this, formatRule);
        if(dataView_){
            QWidget* widget = new QWidget();
            dataView_->setSpacing(0);
            dataView_->setContentsMargins(0,0,0,0);
            widget->setLayout(dataView_);
            mainLayout.addRow(widget);
        }else{
            delete formatRule;
            formatRule = nullptr;
        }
        qApp->processEvents();
        QListWidget& listWidget = parentWidget();
        QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
        item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
        qApp->processEvents();
        qDebug() << "Output Rule" << sizeHint();
    }
    return true;
}

/*

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
   //pItem->setData(Qt::SizeHintRole ,QVariant(listWidget.itemWidget(pItem)->sizeHint()));

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
    qApp->processEvents();
    QListWidget& listWidget = actionListView();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());
}

template<>
void List::execRequest_ContextMenu<List::Request_ContextMenu::Clear>(ListItem*)
{
    clear();
    qApp->processEvents();
    QListWidget& listWidget = actionListView();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());
}

QWidget& List::actionView()const{
    // Splitter -> Widget (Widget with Layout of ActionDataView) -> ActionView (Any - Conditional or Executable)
    return *Super::parentWidget()->parentWidget()->parentWidget();
}

QListWidget& List::actionListView()const{
     // itemView -> Viewport -> List
    return *static_cast<QListWidget*>(actionView().parentWidget()->parentWidget());
}

ParentContextMenu& List::parentContextMenu()const
{
    return *static_cast<ParentContextMenu*>(&actionListView()); //ActionsList
}

void List::extendContextMenu(ContextMenuConfig&)const{
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
        parentContextMenu().interpretContextMenuResponse(index - functionsSize, cev);
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


bool ItemView::eventFilter(QObject* obj, QEvent* ev){
    switch(ev->type()){
    case QEvent::ContextMenu:
    {
        if(obj == &titleComboBox){
            parentWidget().contextMenuEvent(static_cast<QContextMenuEvent*>(ev));
        }
    }
        break;
    case QEvent::Leave:
    {
        if(obj == titleComboBox.view()){
            createFormatRuleDataView(FormatRule::fromUnderlying(titleComboBox.currentIndex()));
        }
    }
        break;
    default:
        break;
    }
    return Super::eventFilter(obj, ev);
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
    setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
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
    setSizeHint(view().sizeHint());
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

