#include"rulesprocedurepanel.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>

RulesProcedurePanel::RulesProcedurePanel(){
    layout.addWidget(&quickRulesList);
    setLayout(&layout);
}

void RulesProcedurePanel::loadProcedure(ProcedureRef procedureRef){
    quickRulesList.loadRules(procedureRef.rulesOnEndOfCall());
}


void RulesProcedurePanel::QuickRulesList::RulesList::loadRules(QuickRulesRef rules)
{
    using QuickRule = std::decay_t<QuickRulesRef>::Iterator;
    setUpdatesEnabled(false);
    for(QuickRule rule = rules.begin(); rule < rules.end(); rule++)
        addNewItem(*rule);
}


RulesProcedurePanel::QuickRulesList::QuickRulesList(){
    layout.addWidget(&list);

    setLayout(&layout);
    installEventFilter(this);
}

template<>
void RulesProcedurePanel::QuickRulesList::execRequest_ContextMenu<RulesProcedurePanel::QuickRulesList::Request_ContextMenu::AddRule>(ListItem*)
{
   list.addNewItem();
}

template<>
void RulesProcedurePanel::QuickRulesList::execRequest_ContextMenu<RulesProcedurePanel::QuickRulesList::Request_ContextMenu::CloneRule>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    //item->clone();
    new ListItem(*item);
}

template<>
void RulesProcedurePanel::QuickRulesList::execRequest_ContextMenu<RulesProcedurePanel::QuickRulesList::Request_ContextMenu::RemoveRule>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    delete item;
}

template<>
void RulesProcedurePanel::QuickRulesList::execRequest_ContextMenu<RulesProcedurePanel::QuickRulesList::Request_ContextMenu::ClearRules>(ListItem*)
{
    list.clear();
}

void RulesProcedurePanel::QuickRulesList::contextMenuEvent(QContextMenuEvent *ev){
    QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);

    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking

    using Actions = QList<QAction*>;
    using ActionFuncs = QList<QuickRulesList::Request_ContextMenu_Func>;
    using Request = QuickRulesList::Request_ContextMenu;
    Actions actions;
    ActionFuncs actionFuncs;
    QMenu* menu = nullptr;

    menu = new QMenu;
    if(item){
        actions = {
          new QAction("Dodaj regułe"),
          new QAction("Klonuj regułe"),
          new QAction("Usuń regułe"),
          new QAction("Usuń wszystkie reguły")
        };
        actionFuncs = {
            &QuickRulesList::execRequest_ContextMenu<Request::AddRule>,
            &QuickRulesList::execRequest_ContextMenu<Request::CloneRule>,
            &QuickRulesList::execRequest_ContextMenu<Request::RemoveRule>,
            &QuickRulesList::execRequest_ContextMenu<Request::ClearRules>,
        };
    }else{
        actions = {
            new QAction("Dodaj regułe"),
            new QAction("Usuń wszystkie reguły")
        };
        actionFuncs = {
            &QuickRulesList::execRequest_ContextMenu<Request::AddRule>,
            &QuickRulesList::execRequest_ContextMenu<Request::ClearRules>,
        };
    }

    // After configuration
    if(menu){
        menu->addActions(actions);
        int index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "QuickRulesList Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
    return;
}


RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ExpectedArgumentsList(){
    ItemDelegate* itemDelegateObj = new ItemDelegate(*this);
    setItemDelegate(itemDelegateObj);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    setSortingEnabled(true);
    viewport()->installEventFilter(this);
}


void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::addIndex()
{
    if(curEditItem){
        closePersistentEditor(curEditItem);
        qApp->processEvents();
    }

    curEditItem = new ListItem(ListItem::ItemType::IndexItem);
    addTopLevelItem(curEditItem);
    editItem(curEditItem);

    qDebug() << isPersistentEditorOpen(curEditItem);
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::loadExpectedArguments(QuickRule& rule)
{
    /*using ArgumentsByIndexIter = QuickRule::ExpectedArgumentsByIndex::Iterator;
    for(ArgumentsByIndexIter argumentsByIndexIter = rule.getExpectedArgumentsByIndex().begin();
        argumentsByIndexIter != rule.getExpectedArgumentsByIndex().end();
        argumentsByIndexIter++)
    {
        curEditItem = new ListItem(ListItem::ItemType::IndexItem, QString::number(argumentsByIndexIter.key()), argumentsByIndexIter.value());
        addTopLevelItem(curEditItem);
    }*/
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::clearEditItem()
{
    if(curEditItem){
        closePersistentEditor(curEditItem);
        qApp->processEvents();
    }
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::addEditItem(ListItem *item)
{
    Q_ASSERT_X(item , "ExpectedArgumentsList::addEditItem", "Item is null");
    clearEditItem();

    editItem(curEditItem = item);

}

QWidget* RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ItemDelegate::
createEditor(QWidget* parent,
             const QStyleOptionViewItem& option,
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

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ItemDelegate::
setEditorData(QWidget* editor,
              const QModelIndex& index) const
{
   QItemDelegate::setEditorData(editor, index);
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ItemDelegate::
setModelData(QWidget* editor,
             QAbstractItemModel* model,
             const QModelIndex& index) const
{
   QItemDelegate::setModelData(editor, model, index);
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ItemDelegate::
updateEditorGeometry(QWidget* editor,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) const
{
   QItemDelegate::updateEditorGeometry(editor, option, index);
}

QString RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ListItem::
toolTipText() const{
    switch(type()){
    case ItemType::IndexItem:       return QStringLiteral("Indeks: ") + text(0);
    case ItemType::ArgumentItem:    return QStringLiteral("Argument: \"") + text(0) + QStringLiteral("\"");
    default:
        break;
    }
    return QString();
}


void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ListItem::addArgument()
{
    Q_ASSERT_X(type() == ItemType::IndexItem , "ExpectedArgumentsList::ListItem::addArgument", "Item isnt IndexItem");

    ListItem* newItem = new ListItem(ListItem::ItemType::ArgumentItem);
    addChild(newItem);
    if(not isExpanded()) setExpanded(true);
    treeWidget()->addEditItem(newItem);

    qDebug() << treeWidget()->isPersistentEditorOpen(newItem);
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ListItem::addEmptyStringArgument()
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

        qDebug() << treeWidget()->isPersistentEditorOpen(newItem);
    }
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ListItem::loadArguments(QStringList& arguments){
    /*using ArgumentsByIndexIter = QuickRule::ExpectedArgumentsByIndex::Iterator;
    for(QStringList::Iterator argument = arguments.begin();
        argument != arguments.end();
        argument++)
    {
        loadArgument(*argument);
    }*/
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::ListItem::loadArgument(QString& argument)
{
    ListItem* newItem = new ListItem(ListItem::ItemType::ArgumentItem, argument);
    addChild(newItem);
}

template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::Request_ContextMenu::NewIndex>(ListItem* item){
    addIndex();
}

template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::Request_ContextMenu::NewArgument>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::NewArgument>", "Item is null");
    if(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem) {
        item = item->parent();
    }
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::NewArgument>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::IndexItem , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::NewArgument>", "Item isnt IndexItem");

    item->addArgument();
}

template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::Request_ContextMenu::AddEmptyStringArg>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::AddEmptyStringArg>", "Item is null");
    if(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem){
        item = item->parent();
    }
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::AddEmptyStringArg>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::IndexItem, "execRequest_ContextMenu<AddEmptyStringArg::Request_ContextMenu::AddEmptyStringArg>", "Item isnt IndexItem");

    item->addEmptyStringArgument();
}


template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::Request_ContextMenu::RemoveIndex>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveIndex>", "Item parent is null");
    if(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem) {
        item = item->parent();
    }
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveIndex>", "Item is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::IndexItem, "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveIndex>", "Item isnt IndexItem");

    if(curEditItem == item) clearEditItem();
    delete item;
}

template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::Request_ContextMenu::EditIndex>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditIndex>", "Item is null");
    if(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem){
        item = item->parent();
    }
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditIndex>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::IndexItem , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditIndex>", "Item isnt IndexItem");

    addEditItem(item);
}


template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::Request_ContextMenu::RemoveArgument>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveArgument>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem, "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::RemoveArgument>", "Item isnt ArgumentItem");

    if(curEditItem == item) clearEditItem();
    delete item;
}

template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::Request_ContextMenu::EditArgument>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditArgument>", "Item parent is null");
    Q_ASSERT_X(item->type() == ListItem::ItemType::ArgumentItem or item->type() == ListItem::ItemType::EmptyStringItem, "execRequest_ContextMenu<ExpectedArgumentsList::Request_ContextMenu::EditArgument>", "Item isnt ArgumentItem");

    if(not item->parent()->isExpanded()) item->parent()->setExpanded(true);
    addEditItem(item);
}

template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::Request_ContextMenu::RemoveAllIndexes>(ListItem* item){
    //clearEditItem();
    clear();
}

template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::Request_ContextMenu::RemoveAllArguments>(ListItem* item){
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
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::contextMenuEvent(QContextMenuEvent *mev){

        ListItem* item = itemAt(mev->pos());

        using Actions = QList<QAction*>;
        using ActionFuncs = QList<Request_ContextMenu_Func>;
        using Request = Request_ContextMenu;

        Actions actions;
        ActionFuncs actionFuncs;
        QMenu* menu = new QMenu;

        if(item){
            // Specify file and error checking

            switch(item->type()){
            case ListItem::ItemType::IndexItem:
            {
                actions = {
                    new QAction("Dodaj indeks"),
                    new QAction("Dodaj argument"),
                    new QAction("Dodaj pusty argument"),
                    new QAction("Edytuj indeks"),
                    new QAction("Usun indeks"),
                    new QAction("Usun wszystkie indeksy"),
                    new QAction("Usun wszystkie argumenty")
                };
                actionFuncs = {
                    &Self::execRequest_ContextMenu<Request::NewIndex>,
                    &Self::execRequest_ContextMenu<Request::NewArgument>,
                    &Self::execRequest_ContextMenu<Request::AddEmptyStringArg>,
                    &Self::execRequest_ContextMenu<Request::EditIndex>,
                    &Self::execRequest_ContextMenu<Request::RemoveIndex>,
                    &Self::execRequest_ContextMenu<Request::RemoveAllIndexes>,
                    &Self::execRequest_ContextMenu<Request::RemoveAllArguments>,
                };
            }
                break;
            case ListItem::ItemType::EmptyStringItem:
            case ListItem::ItemType::ArgumentItem:
            {
                actions = {
                    new QAction("Dodaj indeks"),
                    new QAction("Dodaj argument"),
                    new QAction("Dodaj pusty argument"),
                    new QAction("Edytuj indeks"),
                    new QAction("Edytuj argument"),
                    new QAction("Usun indeks"),
                    new QAction("Usun argument"),
                    new QAction("Usun wszystkie indeksy"),
                    new QAction("Usun wszystkie argumenty ")
                };
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
                };
            }
                break;

            default:
                delete menu;
                menu = nullptr;
            }
        }else{
            menu = new QMenu;
            actions = {
                new QAction("Dodaj indeks"),
                new QAction("Usun wszystkie indeksy")
            };
            actionFuncs = {
                &Self::execRequest_ContextMenu<Request::NewIndex>,
                &Self::execRequest_ContextMenu<Request::RemoveAllIndexes>
            };

        }
        // After configuration
        if(menu){
            menu->addActions(actions);
            int index = actions.indexOf( menu->exec(mev->globalPos()));
            if(index >= 0){
                Q_ASSERT_X(index < actionFuncs.size(), "ExpectedArgumentList Menu", "Index error for action functions");
                (this->*(actionFuncs.at(index)))(item);
            }
            delete menu;
            menu = nullptr;
        }
    return ;
}


bool RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ExpectedArgumentsList::eventFilter(QObject* obj, QEvent* ev){
    if(ev->type() == QEvent::MouseButtonDblClick and obj == viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        curEditItem = itemAt(mev->pos());
    }

    if(ev->type() == QEvent::LayoutRequest and obj == viewport()){  // Any change

    }

    if(ev->type() == QEvent::ChildRemoved and obj == viewport()){
        if(curEditItem){
            // Confirm that index or argument isnt duplicated
            if(curEditItem->text(0).isEmpty()){
                delete curEditItem;
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
                                 parentItem->child(itemIndex)->text(0) == textToCompare){
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
                }

                if(indexOrArgumentIsntDuplicated){
                    curEditItem->setToolTip(0, curEditItem->toolTipText());
                }else{
                    delete curEditItem;
                }

            }
            curEditItem = nullptr;
        }
    }
    return Super::eventFilter(obj, ev);
}

RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::OutputsList(){
    setMovement(QListView::Snap);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setDragDropMode(QAbstractItemView::InternalMove);
    viewport()->installEventFilter(this);
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::loadOutputs(QuickRule &rule){
   /* using WriteActionsIter = QuickRule::WriteActions::Iterator;
    for(WriteActionsIter writeAction = rule.writeActions().begin();
        writeAction != rule.writeActions().end();
        writeAction++)
    {

        //curEditItem = new ListItem(ListItem::ItemType::IndexItem, QString::number(argumentsByIndexIter.key()), argumentsByIndexIter.value());
        //addTopLevelItem(curEditItem);
    }*/
}

RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemContent::ItemContent(Parent* parent)
    : parent(parent)
{
    const QStringList outputTitleComboBoxOptions =
    {
        "Tekst",
        "Nazwa procedury",
        "Argument o indeksie",
        "Wszystkie argumenty od wybranego indeksu",
        //"Separator",
        "Format"
    };

    // titleConfig
    mainLayout.setSpacing(0);

    outputTitleRemoveButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    //outputTitleRemoveButton.setText("Test");
    outputTitleRemoveButton.installEventFilter(this);
    outputTitleComboBox.addItems(outputTitleComboBoxOptions);
    outputTitleComboBox.view()->installEventFilter(this);
    outputTitleLayout.setSpacing(0);
    outputTitleLayout.setContentsMargins(0,0,0,0);
    outputTitleLayout.addWidget(&outputTitleComboBox, Qt::AlignLeft);
    outputTitleRemoveButton.setSizePolicy(QSizePolicy::Policy::Fixed, outputTitleRemoveButton.sizePolicy().verticalPolicy());
    outputTitleLayout.addWidget(&outputTitleRemoveButton, Qt::AlignRight);

    mainLayout.setSpacing(0);
    mainLayout.setContentsMargins(0,0,0,0);
    mainLayout.addRow("Typ: ", &outputTitleLayout);

    createOutputContentLayout_priv(Parent::ItemType::PlainTextItem);

    setLayout(&mainLayout);
}

bool RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemContent::eventFilter(QObject* obj, QEvent* ev){
    switch(ev->type()){
    case QEvent::Leave:
    {
        if(obj == outputTitleComboBox.view()){
            createOutputContentLayout_priv(fromULying(outputTitleComboBox.currentIndex()));
        }
    }
        break;
    case QEvent::MouseButtonPress:
    {
        if(obj == &outputTitleRemoveButton){
            delete parent;
        }
    }
        break;
    default:
        break;
    }
    return Super::eventFilter(obj, ev);
}

template<>
class RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemContent::OutputContent
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemType::PlainTextItem> : public QFormLayout
{
public:
    OutputContent() : textLineEdit(new LineEdit){
        addRow("Tekst: ", textLineEdit);
        textLineEdit->setClearButtonEnabled(true);
    }
    ~OutputContent(){}

    class LineEdit : public QLineEdit{
    public:
        LineEdit(){
            setStyleSheet("border: 1px solid red;");
        }
        bool event(QEvent* ev)override{
            if(ev->type() == QEvent::InputMethodQuery){
                if(text().isEmpty()){
                    setStyleSheet("border: 1px solid red;");
                }else{
                    setStyleSheet("");
                }
            }
            return QLineEdit::event(ev);
        }
    };

protected:
    LineEdit* textLineEdit = nullptr;

};

template<>
class RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemContent::OutputContent
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemType::IndexItem> : public QFormLayout
{
public:
    OutputContent() : indexSpinBox(new QSpinBox){
        addRow("Indeks: ", indexSpinBox);
        indexSpinBox->setRange(-99, 99);
        indexSpinBox->setValue(0);
    }
    ~OutputContent(){}

protected:
    QSpinBox* indexSpinBox = nullptr;

};

template<>
class RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemContent::OutputContent
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemType::ArgumentsFromItem> : public QFormLayout
{
public:
    OutputContent() : indexSpinBox(new QSpinBox), separatorLineEdit(new QLineEdit){
        addRow("Indeks: ", indexSpinBox);
        indexSpinBox->setRange(-99, 99);
        indexSpinBox->setValue(0);
        addRow("Separator: ", separatorLineEdit);
        separatorLineEdit->setClearButtonEnabled(true);
        separatorLineEdit->setStyleSheet("border: 1px solid blue;");
    }
    ~OutputContent(){}


protected:
    QSpinBox* indexSpinBox = nullptr;
    QLineEdit* separatorLineEdit = nullptr;

};

template<>
class RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemContent::OutputContent
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemType::FormatItem> : public QFormLayout
{
public:
    OutputContent() : formatRule(new QComboBox){
        const QStringList formatRuleComboBoxOptions =
        {
            "Raw",    // Orginal Interpreter Read
            "TclFormat",
            "CaplFormat",
            "ProcedureParametersStat",    // Number
            "Command",
            "SnprintfFormat"
        };

        addRow("Format: ", formatRule);
        formatRule->addItems(formatRuleComboBoxOptions);
        formatRule->view()->installEventFilter(this);
    }
    ~OutputContent(){}

protected:
    QComboBox* formatRule = nullptr;

};

template<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemType _ItemType>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemContent::createOutputContentLayout
()
{
    using ItemType = Parent::ItemType;
    if constexpr(Self::isOutputContentLayoutCreatedForSpecifiedType<
            _ItemType,
            // List of all types which create layouts
            ItemType::PlainTextItem,
            ItemType::ArgumentsFromItem,
            ItemType::IndexItem,
           // ItemType::SeparatorItem,
            ItemType::FormatItem
            >())
    {        
        mainLayout.addRow(outputContentLayout = new OutputContent<_ItemType>());        
    }
}  // Dont create layout for not specialized item types


const RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemContent::OutputContent_CreateContent_Func
RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::ListItem::ItemContent::createOutputContentFunctions
[numbOfItemTypes()] =
{
    &Self::createOutputContentLayout<Parent::ItemType::PlainTextItem>,
    &Self::createOutputContentLayout<Parent::ItemType::NameItem>,
    &Self::createOutputContentLayout<Parent::ItemType::IndexItem>,
    &Self::createOutputContentLayout<Parent::ItemType::ArgumentsFromItem>,
    //&Self::createOutputContentLayout<Parent::ItemType::SeparatorItem>,
    &Self::createOutputContentLayout<Parent::ItemType::FormatItem>,
};




template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::Request_ContextMenu::New>(ListItem* item){
    addNewOutput();
}

template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::Request_ContextMenu::Remove>(ListItem* item){
    Q_ASSERT_X(item , "execRequest_ContextMenu<OutputsList::Request_ContextMenu::Remove>", "Item is null");

    delete item;
}

template<>
void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::execRequest_ContextMenu
<RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::Request_ContextMenu::RemoveAll>(ListItem* item){
    clear();
}

void RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::OutputsList::contextMenuEvent(QContextMenuEvent *ev){
        ListItem* item = itemAt(ev->pos());

        using Actions = QList<QAction*>;
        using ActionFuncs = QList<Request_ContextMenu_Func>;
        using Request = Request_ContextMenu;

        Actions actions;
        ActionFuncs actionFuncs;
        QMenu* menu = new QMenu;

        if(item){
            // Specify file and error checking
                actions = {
                    new QAction("Dodaj nowy"),
                    new QAction("Usun element"),
                    new QAction("Usun wszystkie")
                };
                actionFuncs = {
                    &Self::execRequest_ContextMenu<Request::New>,
                    &Self::execRequest_ContextMenu<Request::Remove>,
                    &Self::execRequest_ContextMenu<Request::RemoveAll>
                };

        }else{
            menu = new QMenu;
            actions = {
                new QAction("Dodaj nowy"),
                new QAction("Usun wszystkie")
            };
            actionFuncs = {
                &Self::execRequest_ContextMenu<Request::New>,
                &Self::execRequest_ContextMenu<Request::RemoveAll>
            };

        }
        // After configuration
        if(menu){
            menu->addActions(actions);
            int index = actions.indexOf( menu->exec(ev->globalPos()));
            if(index >= 0){
                Q_ASSERT_X(index < actionFuncs.size(), "OutputsList Menu", "Index error for action functions");
                (this->*(actionFuncs.at(index)))(item);
            }
            delete menu;
            menu = nullptr;
        }
    return;
}

RulesProcedurePanel::QuickRulesList::RulesList::RulesList(){
    // Initiailzie
    /*setStyleSheet("QListView::item{"
    "border: 2px solid #6a6ea9;"
    "border-radius: 6px;"
    "}");*/
    setMovement(QListView::Snap);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setDragDropMode(QAbstractItemView::InternalMove);

}

RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ItemContent(ListItem& item, QuickRuleRef rule)
: item_(item){    
    // Setup layout
    centralLayout.setSpacing(0);

    closeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    closeButton.installEventFilter(this);
    centralLayout.addWidget(&closeButton, 1, Qt::AlignRight);

    numbOfArgumentCondition.setValidator(new QRegularExpressionValidator(QRegularExpression("(\\>[\\d]+|[\\d+]+)?")));
    settingsLayout.addRow("Liczba argumentów: ", &numbOfArgumentCondition);
    ruleControlComboBox.setCurrentText(
                Tcl::Command::Definition::Rule::toStr(
                    Tcl::Command::Definition::Rule::Control::BreakRuleCheck));

    settingsLayout.addRow("Flaga kontrolna reguly: ", &ruleControlComboBox);

    centralLayout.addLayout(&settingsLayout, Qt::AlignTop);

    expectedArgumentsList.setHeaderLabels({"Indeksy", "Aktywne", "Spodziewane argumenty"});
    expectedArgumentsList.setSortingEnabled(true);
    quickRuleInput.addWidget(&expectedArgumentsList);
    //outputsList.setHeaderLabels({"Format", "Zawartość"});
    quickRuleInput.addWidget(&outputsList);

    centralLayout.addWidget(&quickRuleInput);

    setLayout(&centralLayout);

    // Rule available
    if(rule){   // Exists
        /*QuickRule& quickRule = *static_cast<QuickRule*>(rule);
        // NumbOfArguments
        if(quickRule.getNumbOfArguments() != -1){
            numbOfArgumentCondition.setText(QString::number(quickRule.getNumbOfArguments()));
        }
        // ControlFlag
        ruleControlComboBox.setCurrentIndex(static_cast<std::underlying_type_t<ControlFlag>>(quickRule.controlFlag()));
        // Expected Arguments
        expectedArgumentsList.loadExpectedArguments(quickRule);*/
    }
}

RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::ItemContent(ItemContent* itemF)
: ItemContent(itemF->item()){
    centralLayout.setSpacing(0);

    closeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    closeButton.installEventFilter(this);
    centralLayout.addWidget(&closeButton, 1, Qt::AlignRight);

    numbOfArgumentCondition.setValidator(new QRegularExpressionValidator(QRegularExpression("(\\>[\\d]+|[\\d+]+)?")));
    settingsLayout.addRow("Liczba argumentów: ", &numbOfArgumentCondition);
    ruleControlComboBox.setCurrentText(
                Tcl::Command::Definition::Rule::toStr(
                    Tcl::Command::Definition::Rule::Control::BreakRuleCheck));


    settingsLayout.addRow("Flaga kontrolna reguly: ", &ruleControlComboBox);

    centralLayout.addLayout(&settingsLayout, Qt::AlignTop);

    expectedArgumentsList.setHeaderLabels({"Indeksy", "Aktywne", "Spodziewane argumenty"});
    expectedArgumentsList.setSortingEnabled(true);
    quickRuleInput.addWidget(&expectedArgumentsList);
    //outputsList.setHeaderLabels({"Format", "Zawartość"});
    quickRuleInput.addWidget(&outputsList);

    centralLayout.addWidget(&quickRuleInput);

    setLayout(&centralLayout);
}

bool RulesProcedurePanel::QuickRulesList::RulesList::ListItem::ItemContent::eventFilter(QObject *obj, QEvent *ev){
    if(obj == &closeButton and ev->type() == QEvent::MouseButtonPress){
        delete &item();
        return true;
    }
    return QWidget::eventFilter(obj, ev);
}
