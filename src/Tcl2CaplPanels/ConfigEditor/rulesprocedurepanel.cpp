#include"rulesprocedurepanel.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>

using namespace Panels::Configuration::View::Rules::RulesProcedurePanel;

void RulesList::loadRules(RulesRef rules)
{
    using RawRule = std::decay_t<RulesRef>::Iterator;
    setUpdatesEnabled(false);
    for(RawRule rule = rules.begin(); rule < rules.end(); rule++)
        addNewItem(*rule);
}


template<>
void RulesList::execRequest_ContextMenu<RulesList::Request_ContextMenu::AddRule>(ListItem*)
{
   addNewItem();
}

template<>
void RulesList::execRequest_ContextMenu<RulesList::Request_ContextMenu::CloneRule>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    //item->clone();
    new ListItem(*item);
}

template<>
void RulesList::execRequest_ContextMenu<RulesList::Request_ContextMenu::RemoveRule>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    delete item;
}

template<>
void RulesList::execRequest_ContextMenu<RulesList::Request_ContextMenu::ClearRules>(ListItem*)
{
    clear();
}

void RulesList::contextMenuEvent(QContextMenuEvent *ev){
    QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);
    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking
    using Actions = QList<QAction*>;
    using ActionFuncs = QList<RulesList::Request_ContextMenu_Func>;
    using Request = RulesList::Request_ContextMenu;
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
            &RulesList::execRequest_ContextMenu<Request::AddRule>,
            &RulesList::execRequest_ContextMenu<Request::CloneRule>,
            &RulesList::execRequest_ContextMenu<Request::RemoveRule>,
            &RulesList::execRequest_ContextMenu<Request::ClearRules>,
        };
    }else{
        actions = {
            new QAction("Dodaj regułe"),
            new QAction("Usuń wszystkie reguły")
        };
        actionFuncs = {
            &RulesList::execRequest_ContextMenu<Request::AddRule>,
            &RulesList::execRequest_ContextMenu<Request::ClearRules>,
        };
    }

    // After configuration
    if(menu){
        menu->addActions(actions);
        qsizetype&& index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "RulesList Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        static_cast<void>(delete menu), menu = nullptr;
    }
    return;
}


RulesList::RulesList(){
    // Initiailzie
    /*setStyleSheet("QListView::item{"
    "border: 2px solid #6a6ea9;"
    "border-radius: 6px;"
    "}");*/
    setMovement(QListView::Snap);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setDragDropMode(QAbstractItemView::InternalMove);

}

ListItem::ListItem(RulesList& list)
    : rawRuleView_(*this)
{
    list.addItem(this);
    list.setItemWidget(this, &rawRuleView());
    setSizeHint(rawRuleView().sizeHint());
}

ListItem::ListItem(RulesList& list, RuleRef rule)
    : rawRuleView_(*this, rule)
{
    list.addItem(this);
    list.setItemWidget(this, &rawRuleView());
    setSizeHint(rawRuleView().sizeHint());
};

RulesList &ListItem::rulesList() const
{ return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }
