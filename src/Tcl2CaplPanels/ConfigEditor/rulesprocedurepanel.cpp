#include"rulesprocedurepanel.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>

RulesProcedurePanel::RulesProcedurePanel(){
    layout.addWidget(&quickRulesList);
    layout.setContentsMargins(0,0,0,0);
    layout.setSpacing(0);
    setLayout(&layout);
}

void RulesProcedurePanel::loadProcedure(ProcedureRef procedureRef){
    quickRulesList.loadRules(procedureRef.rulesOnEndOfCall());
}


void RulesProcedurePanel::RawRulesList::RulesList::loadRules(QuickRulesRef rules)
{
    using QuickRule = std::decay_t<QuickRulesRef>::Iterator;
    setUpdatesEnabled(false);
    for(QuickRule rule = rules.begin(); rule < rules.end(); rule++)
        addNewItem(*rule);
}


RulesProcedurePanel::RawRulesList::RawRulesList(){
    layout.addWidget(&list);
    layout.setSpacing(0);
    layout.setContentsMargins(0,0,0,0);

    setLayout(&layout);

    installEventFilter(this);
}

template<>
void RulesProcedurePanel::RawRulesList::execRequest_ContextMenu<RulesProcedurePanel::RawRulesList::Request_ContextMenu::AddRule>(ListItem*)
{
   list.addNewItem();
}

template<>
void RulesProcedurePanel::RawRulesList::execRequest_ContextMenu<RulesProcedurePanel::RawRulesList::Request_ContextMenu::CloneRule>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    //item->clone();
    new ListItem(*item);
}

template<>
void RulesProcedurePanel::RawRulesList::execRequest_ContextMenu<RulesProcedurePanel::RawRulesList::Request_ContextMenu::RemoveRule>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    delete item;
}

template<>
void RulesProcedurePanel::RawRulesList::execRequest_ContextMenu<RulesProcedurePanel::RawRulesList::Request_ContextMenu::ClearRules>(ListItem*)
{
    list.clear();
}

void RulesProcedurePanel::RawRulesList::contextMenuEvent(QContextMenuEvent *ev){
    QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);

    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking

    using Actions = QList<QAction*>;
    using ActionFuncs = QList<RawRulesList::Request_ContextMenu_Func>;
    using Request = RawRulesList::Request_ContextMenu;
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
            &RawRulesList::execRequest_ContextMenu<Request::AddRule>,
            &RawRulesList::execRequest_ContextMenu<Request::CloneRule>,
            &RawRulesList::execRequest_ContextMenu<Request::RemoveRule>,
            &RawRulesList::execRequest_ContextMenu<Request::ClearRules>,
        };
    }else{
        actions = {
            new QAction("Dodaj regułe"),
            new QAction("Usuń wszystkie reguły")
        };
        actionFuncs = {
            &RawRulesList::execRequest_ContextMenu<Request::AddRule>,
            &RawRulesList::execRequest_ContextMenu<Request::ClearRules>,
        };
    }

    // After configuration
    if(menu){
        menu->addActions(actions);
        int index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "RawRulesList Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
    return;
}


RulesProcedurePanel::RawRulesList::RulesList::RulesList(){
    // Initiailzie
    /*setStyleSheet("QListView::item{"
    "border: 2px solid #6a6ea9;"
    "border-radius: 6px;"
    "}");*/
    setMovement(QListView::Snap);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setDragDropMode(QAbstractItemView::InternalMove);

}
