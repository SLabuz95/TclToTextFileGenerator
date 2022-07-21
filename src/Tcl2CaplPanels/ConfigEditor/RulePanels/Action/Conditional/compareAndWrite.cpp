#include"compareAndWrite.hpp"
#include"Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include<QWidget>

using namespace Panels::Configuration::View::ActionsList;
using ParentContextMenu = CompareAndWriteActionView::ParentContextMenu;

CompareAndWriteActionView::ExpectedArgumentsList::ExpectedArgumentsList(){
    ItemDelegate* itemDelegateObj = new ItemDelegate(*this);
    setItemDelegate(itemDelegateObj);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setSortingEnabled(true);
    sortByColumn(0, Qt::SortOrder::AscendingOrder);
    viewport()->installEventFilter(this);
}


void CompareAndWriteActionView::ExpectedArgumentsList::addIndex()
{
    if(curEditItem){
        closePersistentEditor(curEditItem);
        qApp->processEvents();
    }

    curEditItem = new ListItem(ListItem::ItemType::IndexItem);
    addTopLevelItem(curEditItem);
    editItem(curEditItem);

}


QWidget& CompareAndWriteActionView::ExpectedArgumentsList::itemView()const{
    // Splitter -> Widget (Widget with Layout of DataView) -> ItemView (Any)
    return *Super::parentWidget()->parentWidget()->parentWidget();
}

QListWidget& CompareAndWriteActionView::ExpectedArgumentsList::itemListView()const{
    // itemView -> Viewport -> List
    return *static_cast<QListWidget*>(itemView().parentWidget()->parentWidget());
}

void CompareAndWriteActionView::ExpectedArgumentsList::loadExpectedArguments(Action::ArgumentsMap& argumentsMap)
{
    using ExpectedArgumentsByIndex = Action::ArgumentsMap;
    using ArgumentsByIndexIter = ExpectedArgumentsByIndex::Iterator;
    for(ArgumentsByIndexIter argumentsByIndexIter = argumentsMap.begin();
        argumentsByIndexIter != argumentsMap.end();
        argumentsByIndexIter++)
    {
        curEditItem = new ListItem(ListItem::ItemType::IndexItem, QString::number(argumentsByIndexIter.key()), argumentsByIndexIter.value());
        addTopLevelItem(curEditItem);
    }
}

void CompareAndWriteActionView::ExpectedArgumentsList::readExpectedArguments(Action::ArgumentsMap& argumentsMap)
{
    using ExpectedArgumentsByIndex = Action::ArgumentsMap;
    using ArgumentsByIndexIter = ExpectedArgumentsByIndex::Iterator;
    ArgumentsByIndexIter newIndexInMap;
    for(int i = 0; i < topLevelItemCount(); i++){
        newIndexInMap = argumentsMap.insert(topLevelItem(i)->text(0).toInt(), {});
        topLevelItem(i)->readArguments(newIndexInMap.value());
    }
}

void CompareAndWriteActionView::ExpectedArgumentsList::clearEditItem()
{
    if(curEditItem){
        closePersistentEditor(curEditItem);
        qApp->processEvents();
    }
}

void CompareAndWriteActionView::ExpectedArgumentsList::addEditItem(ListItem *item)
{
    Q_ASSERT_X(item , "ExpectedArgumentsList::addEditItem", "Item is null");
    clearEditItem();

    editItem(curEditItem = item);
}

QWidget* CompareAndWriteActionView::ExpectedArgumentsList::ItemDelegate::
createEditor(QWidget* parent,
             const QStyleOptionViewItem& ,
             const QModelIndex& index) const
{
    // For List Item Type == EmptyStringItem dont edit
    QLineEdit* editor = nullptr;
    if(not (list.itemFromIndex(index)->type() == ListItem::ItemType::EmptyStringItem) ){
        editor = new QLineEdit(parent);
        if(not index.parent().isValid())
            editor->setValidator(new QRegularExpressionValidator(QRegularExpression(RegExpCore::regExprForIntRange_MinusNinetyNine2NinetyNine)));
    }
   return editor;
}

void CompareAndWriteActionView::ExpectedArgumentsList::ItemDelegate::
setEditorData(QWidget* editor,
              const QModelIndex& index) const
{
   QItemDelegate::setEditorData(editor, index);
}

void CompareAndWriteActionView::ExpectedArgumentsList::ItemDelegate::
setModelData(QWidget* editor,
             QAbstractItemModel* model,
             const QModelIndex& index) const
{
   QItemDelegate::setModelData(editor, model, index);
}

void CompareAndWriteActionView::ExpectedArgumentsList::ItemDelegate::
updateEditorGeometry(QWidget* editor,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) const
{
   QItemDelegate::updateEditorGeometry(editor, option, index);
}

QString CompareAndWriteActionView::ExpectedArgumentsList::ListItem::
toolTipText() const{
    switch(type()){
    case ItemType::IndexItem:       return QStringLiteral("Indeks: ") + text(0);
    case ItemType::ArgumentItem:    return QStringLiteral("Argument: \"") + text(2) + QStringLiteral("\"");
    default:
        break;
    }
    return QString();
}


void CompareAndWriteActionView::ExpectedArgumentsList::ListItem::addArgument()
{
    Q_ASSERT_X(type() == ItemType::IndexItem , "ExpectedArgumentsList::ListItem::addArgument", "Item isnt IndexItem");

    ListItem* newItem = new ListItem(ListItem::ItemType::ArgumentItem);
    addChild(newItem);
    if(not isExpanded()) setExpanded(true);
    treeWidget()->addEditItem(newItem);

}

void CompareAndWriteActionView::ExpectedArgumentsList::ListItem::addEmptyStringArgument()
{
    Q_ASSERT_X(type() == ItemType::IndexItem , "ExpectedArgumentsList::ListItem::addEmptyStringArgument", "Item isnt IndexItem");

    bool emptyArgumentDoesntExist = true;
    // Confirm that EmptyArgumentItem doesnt exist
    for(int itemIndex = 0 ; itemIndex < childCount(); itemIndex++){
        if(child(itemIndex)->type() == ItemType::EmptyStringItem){
            emptyArgumentDoesntExist = false;
            break;
        }
    }

    if(emptyArgumentDoesntExist){
        ListItem* newItem = new ListItem(ListItem::ItemType::EmptyStringItem);
        addChild(newItem);
        if(not isExpanded()) setExpanded(true);
        treeWidget()->addEditItem(newItem);
    }
}

void CompareAndWriteActionView::ExpectedArgumentsList::ListItem::loadArguments(QStringList& arguments){
    for(QStringList::Iterator argument = arguments.begin();
        argument != arguments.end();
        argument++)
    {
        loadArgument(*argument);
    }
}



void CompareAndWriteActionView::ExpectedArgumentsList::ListItem::loadArgument(QString& argument)
{
    ListItem* newItem = new ListItem(ListItem::ItemType::ArgumentItem, argument);
    addChild(newItem);
}

void CompareAndWriteActionView::ExpectedArgumentsList::ListItem::readArguments(QStringList& arguments){
    arguments.resize(childCount());
    int i = 0;
    for(QStringList::Iterator argument = arguments.begin();
        argument != arguments.end();
        argument++, i++)
    {
        (*argument) = child(i)->readArgument();
    }
}

template<>
void CompareAndWriteActionView::ExpectedArgumentsList::execRequest_ContextMenu
<CompareAndWriteActionView::ExpectedArgumentsList::Request_ContextMenu::NewIndex>(ListItem* ){
    addIndex();
}

template<>
void CompareAndWriteActionView::ExpectedArgumentsList::execRequest_ContextMenu
<CompareAndWriteActionView::ExpectedArgumentsList::Request_ContextMenu::NewArgument>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::NewArgument>", "Item is null");
    if(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem) {
        item = item->parent();
    }
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::NewArgument>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::IndexItem , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::NewArgument>", "Item isnt IndexItem");

    item->addArgument();
}

template<>
void CompareAndWriteActionView::ExpectedArgumentsList::execRequest_ContextMenu
<CompareAndWriteActionView::ExpectedArgumentsList::Request_ContextMenu::AddEmptyStringArg>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::AddEmptyStringArg>", "Item is null");
    if(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem){
        item = item->parent();
    }
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::AddEmptyStringArg>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::IndexItem, "execRequest_ContextMenu<AddEmptyStringArg::Request_ContextMenu::AddEmptyStringArg>", "Item isnt IndexItem");

    item->addEmptyStringArgument();
}


template<>
void CompareAndWriteActionView::ExpectedArgumentsList::execRequest_ContextMenu
<CompareAndWriteActionView::ExpectedArgumentsList::Request_ContextMenu::RemoveIndex>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveIndex>", "Item parent is null");
    if(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem) {
        item = item->parent();
    }
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveIndex>", "Item is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::IndexItem, "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveIndex>", "Item isnt IndexItem");

    if(curEditItem == item) clearEditItem();
    delete item;
    qApp->processEvents();
    QListWidget& listWidget = itemListView();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());
}

template<>
void CompareAndWriteActionView::ExpectedArgumentsList::execRequest_ContextMenu
<CompareAndWriteActionView::ExpectedArgumentsList::Request_ContextMenu::EditIndex>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditIndex>", "Item is null");
    if(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem){
        item = item->parent();
    }
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditIndex>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::IndexItem , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditIndex>", "Item isnt IndexItem");

    addEditItem(item);
}


template<>
void CompareAndWriteActionView::ExpectedArgumentsList::execRequest_ContextMenu
<CompareAndWriteActionView::ExpectedArgumentsList::Request_ContextMenu::RemoveArgument>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveArgument>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem, "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveArgument>", "Item isnt ArgumentItem");

    if(curEditItem == item) clearEditItem();
    delete item;
    qApp->processEvents();
    QListWidget& listWidget = itemListView();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());
}

template<>
void CompareAndWriteActionView::ExpectedArgumentsList::execRequest_ContextMenu
<CompareAndWriteActionView::ExpectedArgumentsList::Request_ContextMenu::EditArgument>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditArgument>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem, "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditArgument>", "Item isnt ArgumentItem");

    if(not item->parent()->isExpanded()) item->parent()->setExpanded(true);
    addEditItem(item);
}

template<>
void CompareAndWriteActionView::ExpectedArgumentsList::execRequest_ContextMenu
<CompareAndWriteActionView::ExpectedArgumentsList::Request_ContextMenu::RemoveAllIndexes>(ListItem*){
    //clearEditItem();
    clear();
    qApp->processEvents();
    QListWidget& listWidget = itemListView();
    QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
}

template<>
void CompareAndWriteActionView::ExpectedArgumentsList::execRequest_ContextMenu
<CompareAndWriteActionView::ExpectedArgumentsList::Request_ContextMenu::RemoveAllArguments>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveAllArguments>", "Item is null");
    if(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem){
        item = item->parent();
    }
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveAllArguments>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::IndexItem , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveAllArguments>", "Item isnt IndexItem");

    auto children = item->takeChildren();

    for(decltype (children)::Iterator child = children.begin(); child != children.end(); child++)
    {
        if(curEditItem == (*child)) clearEditItem();
        delete (*child);
    }
    qApp->processEvents();
    QListWidget& listWidget = itemListView();
    QListWidgetItem* pItem = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
    pItem->setSizeHint(listWidget.itemWidget(pItem)->sizeHint());
}

ParentContextMenu& CompareAndWriteActionView::ExpectedArgumentsList::parentContextMenu()const
{
    return *static_cast<ParentContextMenu*>(&itemListView()); //ItemListView
}

void CompareAndWriteActionView::ExpectedArgumentsList::extendContextMenu(ContextMenuConfig&)const{
}

void CompareAndWriteActionView::ExpectedArgumentsList::interpretContextMenuResponse(ContextMenuConfig::ActionIndex index, QContextMenuEvent* cev){
    using ActionFuncs = Request_ContextMenu_Func[];
    using Request = Request_ContextMenu;
    constexpr ActionFuncs indexActionFunc = {
        &Self::execRequest_ContextMenu<Request::NewIndex>,
        &Self::execRequest_ContextMenu<Request::NewArgument>,
        &Self::execRequest_ContextMenu<Request::AddEmptyStringArg>,
        &Self::execRequest_ContextMenu<Request::EditIndex>,
        &Self::execRequest_ContextMenu<Request::RemoveIndex>,
        &Self::execRequest_ContextMenu<Request::RemoveAllIndexes>,
        &Self::execRequest_ContextMenu<Request::RemoveAllArguments>,
    };
    constexpr ActionFuncs argumentActionFunc = {
        &Self::execRequest_ContextMenu<Request::NewIndex>,
        &Self::execRequest_ContextMenu<Request::NewArgument>,
        &Self::execRequest_ContextMenu<Request::AddEmptyStringArg>,
        &Self::execRequest_ContextMenu<Request::EditIndex>,
        &Self::execRequest_ContextMenu<Request::EditArgument>,
        &Self::execRequest_ContextMenu<Request::RemoveIndex>,
        &Self::execRequest_ContextMenu<Request::RemoveArgument>,
        &Self::execRequest_ContextMenu<Request::RemoveAllIndexes>,
        &Self::execRequest_ContextMenu<Request::RemoveAllArguments>,
    };
    constexpr uint indexFunctionsSize = std::extent_v<decltype(indexActionFunc)>;
    constexpr uint argumentFunctionsSize = std::extent_v<decltype(argumentActionFunc)>;
    ListItem* item = itemAt(cev->pos());
    if(item->type() == ListItem::ItemType::IndexItem){
        if(index >= indexFunctionsSize){
            parentContextMenu().interpretContextMenuResponse(index - indexFunctionsSize, cev);
        }else{
            (this->*(indexActionFunc[index]))(item);
        }
    }else{
        if(index >= argumentFunctionsSize){
            parentContextMenu().interpretContextMenuResponse(index - argumentFunctionsSize, cev);
        }else{
            (this->*(argumentActionFunc[index]))(item);
        }
    }
}

void CompareAndWriteActionView::ExpectedArgumentsList::contextMenuEvent(QContextMenuEvent *mev){
        ListItem* item = itemAt(mev->pos());

        // Specify file and error checking
        using ActionFuncs = Request_ContextMenu_Func[];
        using Request = Request_ContextMenu;
        constexpr ActionFuncs actionFunc = {
            &Self::execRequest_ContextMenu<Request::NewIndex>,
            &Self::execRequest_ContextMenu<Request::RemoveAllIndexes>
        };
        constexpr uint functionsSize = std::extent_v<decltype(actionFunc)>;
        ContextMenuConfig contextMenuConfig;

        if(item){
            // Specify file and error checking
            switch(item->type()){
            case ListItem::ItemType::IndexItem:
            {
                contextMenuConfig.addActions(
                            {
                    new QAction("Dodaj indeks"),
                    new QAction("Dodaj argument"),
                    new QAction("Dodaj pusty argument"),
                    new QAction("Edytuj indeks"),
                    new QAction("Usun indeks"),
                    new QAction("Usun wszystkie indeksy"),
                    new QAction("Usun wszystkie argumenty")
                });/*
                actionFuncs = {
                    &Self::execRequest_ContextMenu<Request::NewIndex>,
                    &Self::execRequest_ContextMenu<Request::NewArgument>,
                    &Self::execRequest_ContextMenu<Request::AddEmptyStringArg>,
                    &Self::execRequest_ContextMenu<Request::EditIndex>,
                    &Self::execRequest_ContextMenu<Request::RemoveIndex>,
                    &Self::execRequest_ContextMenu<Request::RemoveAllIndexes>,
                    &Self::execRequest_ContextMenu<Request::RemoveAllArguments>,
                };*/
            }
                break;
            case ListItem::ItemType::EmptyStringItem:
            case ListItem::ItemType::ArgumentItem:
            {
                contextMenuConfig.addActions(
                            {
                   new QAction("Dodaj indeks"),
                    new QAction("Dodaj argument"),
                    new QAction("Dodaj pusty argument"),
                    new QAction("Edytuj indeks"),
                    new QAction("Edytuj argument"),
                    new QAction("Usun indeks"),
                    new QAction("Usun argument"),
                    new QAction("Usun wszystkie indeksy"),
                    new QAction("Usun wszystkie argumenty ")
                });
                /*
                actionFuncs = {
                    &Self::execRequest_ContextMenu<Request::NewIndex>,
                    &Self::execRequest_ContextMenu<Request::NewArgument>,
                    &Self::execRequest_ContextMenu<Request::AddEmptyStringArg>,
                    &Self::execRequest_ContextMenu<Request::EditIndex>,
                    &Self::execRequest_ContextMenu<Request::EditArgument>,
                    &Self::execRequest_ContextMenu<Request::RemoveIndex>,
                    &Self::execRequest_ContextMenu<Request::RemoveArgument>,
                    &Self::execRequest_ContextMenu<Request::RemoveAllIndexes>,
                    &Self::execRequest_ContextMenu<Request::RemoveAllArguments>,
                };*/
            }
            break;
            }
        }else{
            contextMenuConfig.addActions(
                        {
                new QAction("Dodaj indeks"),
                new QAction("Usun wszystkie indeksy")
            });
        }
        // After configuration
        parentContextMenu().extendContextMenu(contextMenuConfig);
        qsizetype&& index = contextMenuConfig.exec(mev);
        if(index >= 0){
            if(item){
                interpretContextMenuResponse(index, mev);
            }else{
                if(index >= functionsSize){
                    parentContextMenu().interpretContextMenuResponse(index - functionsSize, mev);
                }else{
                    (this->*(actionFunc[index]))(item);
                }
            }
        }
    return ;
}


bool CompareAndWriteActionView::ExpectedArgumentsList::eventFilter(QObject* obj, QEvent* ev){
    if(ev->type() == QEvent::MouseButtonDblClick and obj == viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        curEditItem = itemAt(mev->pos());
    }

    if(ev->type() == QEvent::LayoutRequest and obj == viewport()){  // Any change        
        QListWidget& listWidget = itemListView();
        QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
        QWidget* widget = listWidget.itemWidget(item);
        if(widget)
            item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
    }

    if(ev->type() == QEvent::ChildRemoved and obj == viewport()){
        if(curEditItem){
            // Confirm that index or argument isnt duplicated
            if(curEditItem->text((curEditItem->type() == ListItem::ItemType::IndexItem)? 0 : 2).isEmpty()){
                delete curEditItem;
                qApp->processEvents();
                QListWidget& listWidget = itemListView();
                QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
                item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
            }else{
                // Confirm that index or argument isnt duplicated
                // Remove if its
                bool indexOrArgumentIsntDuplicated = true;
                QString textToCompare = curEditItem->text(0);
                switch(curEditItem->type()){
                case ListItem::ItemType::ArgumentItem:
                {
                    ListItem* parentItem = curEditItem->parent();
                    for(int itemIndex = 0 ; itemIndex < parentItem->childCount(); itemIndex++){
                        if(parentItem->child(itemIndex) != curEditItem and
                                parentItem->child(itemIndex)->type() == ListItem::ItemType::ArgumentItem and
                                 parentItem->child(itemIndex)->text(2) == textToCompare){
                            indexOrArgumentIsntDuplicated = false;
                            break;
                        }
                    }
                }
                    break;
                case ListItem::ItemType::IndexItem:
                {
                    for(int itemIndex = 0 ; itemIndex < topLevelItemCount(); itemIndex++){
                        if(topLevelItem(itemIndex) != curEditItem and
                                topLevelItem(itemIndex)->text(0) == textToCompare){
                            indexOrArgumentIsntDuplicated = false;
                            break;
                        }
                    }
                }
                    break;
                default:
                    break;
                }

                if(indexOrArgumentIsntDuplicated){
                    curEditItem->setToolTip((curEditItem->type() == ListItem::ItemType::IndexItem)? 0 : 2, curEditItem->toolTipText());
                }else{
                    delete curEditItem;
                    qApp->processEvents();
                    QListWidget& listWidget = itemListView();
                    QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
                    item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
                }

            }
            curEditItem = nullptr;
        }
    }
    return Super::eventFilter(obj, ev);
}

CompareAndWriteActionView::DataView* CompareAndWriteActionView::create(QWidget* parent, ActionRef action){
    return new CompareAndWriteActionView(parent, action);
}

CompareAndWriteActionView::CompareAndWriteActionView(QWidget* parent)
    : ActionDataView(parent)
/*: item_(item)*/
{
    // Setup layout
    centralLayout.setSpacing(0);

    closeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    closeButton.installEventFilter(this);
    centralLayout.addWidget(&closeButton, 1, Qt::AlignRight);

    numbOfArgumentCondition.setValidator(new QRegularExpressionValidator(QRegularExpression("(\\>[\\d]+|[\\d+]+)?")));
    settingsLayout.addRow("Liczba argumentów: ", &numbOfArgumentCondition);
    /*ruleControlComboBox.setCurrentText(
                Tcl::Command::Definition::Rule::toStr(
                    Tcl::Command::Definition::Rule::Control::BreakRuleCheck));*/

    //settingsLayout.addRow("Flaga kontrolna reguly: ", &ruleControlComboBox);

    centralLayout.addLayout(&settingsLayout, Qt::AlignTop);

    expectedArgumentsList.setHeaderLabels({"Indeksy", "Aktywne", "Spodziewane argumenty"});
    expectedArgumentsList.setSortingEnabled(true);
    quickRuleInput.addWidget(&expectedArgumentsList);
    quickRuleInput.addWidget(&formattedStringList);
    //outputsList.setHeaderLabels({"Format", "Zawartość"});
    //quickRuleInput.addWidget(&outputsList);
    centralLayout.addWidget(&quickRuleInput);

    addLayout(&centralLayout);

}

CompareAndWriteActionView::CompareAndWriteActionView(QWidget* parent, ActionPtr pAction)
    : CompareAndWriteActionView(parent)
{
    if(pAction){
        Action& action = *static_cast<Action*>(pAction);
        numbOfArgumentCondition.setText(action.numbOfArguments());
        expectedArgumentsList.loadExpectedArguments(action.argumentsMap());
        formattedStringList.loadRules(action.inputFormattedString().parameters());
    }
}


void CompareAndWriteActionView::readAction(ActionBase& fAction){
    Action& action = *static_cast<Action*>(&fAction);
    action.numbOfArguments() = numbOfArgumentCondition.text();
    expectedArgumentsList.readExpectedArguments(action.argumentsMap());
    formattedStringList.readRules(action.inputFormattedString().parameters());
}
