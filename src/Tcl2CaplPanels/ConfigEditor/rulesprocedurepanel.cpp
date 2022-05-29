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
void RulesList::execRequest_ContextMenu<RulesList::Request_ContextMenu::Add>(ListItem*)
{
   addNewItem();
}

template<>
void RulesList::execRequest_ContextMenu<RulesList::Request_ContextMenu::Clone>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    //item->clone();
    new ListItem(*item);
}

template<>
void RulesList::execRequest_ContextMenu<RulesList::Request_ContextMenu::Remove>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, __PRETTY_FUNCTION__, "No item");
    delete item;
}

template<>
void RulesList::execRequest_ContextMenu<RulesList::Request_ContextMenu::Clear>(ListItem*)
{
    clear();
}

void RulesList::contextMenuEvent(QContextMenuEvent *cev){
    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking

    using ActionFuncs = RulesList::Request_ContextMenu_Func[];
    using Request = RulesList::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &RulesList::execRequest_ContextMenu<Request::Add>,
        &RulesList::execRequest_ContextMenu<Request::Clear>,
    };
    constexpr uint functionsSize = std::extent_v<decltype(actionFunc)>;
    ContextMenuConfig contextMenuConfig;
    if(item){
        contextMenuConfig.addActions(
                    {
                          new QAction("Dodaj regułe"),
                          new QAction("Klonuj regułe"),
                          new QAction("Usuń regułe"),
                          new QAction("Usuń wszystkie reguły")
                    });
    }else{
        contextMenuConfig.addActions(
                    {
                        new QAction("Dodaj regułe"),
                        new QAction("Usuń wszystkie reguły")
                    });
    }
    qsizetype&& index = contextMenuConfig.exec(cev);
    if(index >= 0){
        if(item){
            interpretContextMenuResponse(index, cev);
        }else{
            if(index < functionsSize)
                (this->*(actionFunc[index]))(item);
        }
    }
}

void RulesList::interpretContextMenuResponse(ContextMenuConfig::ActionIndex index, QContextMenuEvent* cev){
    using ActionFuncs = RulesList::Request_ContextMenu_Func[];
    using Request = RulesList::Request_ContextMenu;
    constexpr ActionFuncs actionFunc = {
        &RulesList::execRequest_ContextMenu<Request::Add>,
        &RulesList::execRequest_ContextMenu<Request::Clone>,
        &RulesList::execRequest_ContextMenu<Request::Remove>,
        &RulesList::execRequest_ContextMenu<Request::Clear>,
    };
    constexpr uint functionsSize = std::extent_v<decltype(actionFunc)>;
    if(index < functionsSize){
        ListItem* item = itemAt(cev->pos());
        (this->*(actionFunc[index]))(item);
    }
}

RulesList::RulesList(){
    // Initiailzie
    /*setStyleSheet("QListView::item{"
    "border: 2px solid #6a6ea9;"
    "border-radius: 6px;"
    "}");*/
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setMovement(QListView::Snap);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setDragDropMode(QAbstractItemView::InternalMove);

}


void RulesList::extendContextMenu(ContextMenuConfig& config){
    config.addMenu("Reguły",{
                       new QAction("Dodaj"),
                       new QAction("Klonuj"),
                       new QAction("Usuń"),
                       new QAction("Usuń wszystkie")
                     });
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
