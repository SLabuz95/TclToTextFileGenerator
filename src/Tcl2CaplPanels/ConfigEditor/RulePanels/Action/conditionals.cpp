#include"actions.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"action.hpp"

using namespace Panels::Configuration::View;
using Conditionals = ConditionalsFactory::ListOfBases;
using ConditionalsList = ActionsList::ActionsList<Conditionals,
                        Actions::ActionViewBase<ConditionalsFactory::ProductBase>>;
using ListItem = ConditionalsList::ListItem;

template<>
void ConditionalsList::loadActions(ActionsRef actions)
{
    using Action = std::decay_t<ActionsRef>::Iterator;
    setUpdatesEnabled(false);
    for(Action action = actions.begin(); action < actions.end(); action++)
        addNewItem(*action);
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
}

template<>
template<>
void ConditionalsList::execRequest_ContextMenu<ConditionalsList::Request_ContextMenu::Clear>(ListItem*)
{
    clear();
}

template<>
void ConditionalsList::contextMenuEvent(QContextMenuEvent *ev){
    QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);
    ListItem* item = itemAt(cev->pos());

    // Specify file and error checking
    using Actions = QList<QAction*>;
    using ActionFuncs = QList<ConditionalsList::Request_ContextMenu_Func>;
    using Request = ConditionalsList::Request_ContextMenu;
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
            &ConditionalsList::execRequest_ContextMenu<Request::Add>,
            &ConditionalsList::execRequest_ContextMenu<Request::Clone>,
            &ConditionalsList::execRequest_ContextMenu<Request::Remove>,
            &ConditionalsList::execRequest_ContextMenu<Request::Clear>,
        };
    }else{
        actions = {
            new QAction("Dodaj regułe"),
            new QAction("Usuń wszystkie reguły")
        };
        actionFuncs = {
            &ConditionalsList::execRequest_ContextMenu<Request::Add>,
            &ConditionalsList::execRequest_ContextMenu<Request::Clear>,
        };
    }

    // After configuration
    if(menu){
        menu->addActions(actions);
        qsizetype&& index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "ConditionalsList Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        static_cast<void>(delete menu), menu = nullptr;
    }
    return;
}


template<>
ConditionalsList
::ActionsList(){
    // Initiailzie
    /*setStyleSheet("QListView::item{"
    "border: 2px solid #6a6ea9;"
    "border-radius: 6px;"
    "}");*/
    setMovement(QListView::Snap);
    setDefaultDropAction(Qt::DropAction::MoveAction);
    setDragDropMode(QAbstractItemView::InternalMove);

}


template<>
ListItem::ListItem(ConditionalsList& list)
    : view_(View::create(nullptr))
{
    list.addItem(this);
    list.setItemWidget(this, view_);
    setSizeHint(view()->sizeHint());
}


template<>
ListItem::ListItem(ConditionalsList& list, Action action)
    : view_(View::create(action))
{
    list.addItem(this);
    list.setItemWidget(this, view_);
    setSizeHint(view()->sizeHint());
};


template<>
ConditionalsList &ListItem::actionsList() const
{ return *static_cast<ConditionalsList*>(QListWidgetItem::listWidget()); }
