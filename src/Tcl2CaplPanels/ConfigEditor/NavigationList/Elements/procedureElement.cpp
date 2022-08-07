#include"procedureElement.hpp"
#include<QMenu>
#include<QContextMenuEvent>
#include<QApplication>
#include<QMessageBox>
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"
#include"Tcl2CaplPanels/ConfigEditor/NavigationList/navigationList.hpp"

using namespace Panels::Configuration::Navigation::Procedure;

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::AddIndex>(ListItem* )
{
    treeWidget()->closePersistentEditor();

    ListItem* item = new ListItem();
    addChild(item);
    item->parent()->setExpanded(true);
    treeWidget()->scrollToItem(item);

    treeWidget()->editItem(item);

}

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::EditIndex>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "DefaultProcedureElement::ContextMenu::editProcedure", "Item is null");
    treeWidget()->closePersistentEditor();

    item->parent()->setExpanded(true);
    treeWidget()->scrollToItem(item);
    treeWidget()->editItem(item);
}

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::RemoveIndex>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "DefaultProcedureElement::ContextMenu::removeIndex", "Item is null");
    treeWidget()->closePersistentEditor();

    if(treeWidget()->isDefaultProcedurePanel(this)){
        treeWidget()->configEditor().removeIndex(item->text(0).toULongLong());
        if(treeWidget()->isNavigationElementActive(item))
            treeWidget()->deactivateDefaultProcedureCategory();
    }else{
        treeWidget()->configEditor().removeIndex(item->parent()->text(0), item->text(0).toULongLong());
        if(treeWidget()->isNavigationElementActive(item))
            treeWidget()->deactivateProcedureCategory();
    }
    delete item;
}

template<>
void DefaultProcedureElement::execRequest_ContextMenu<DefaultProcedureElement::Request_ContextMenu::ClearIndexes>(ListItem*)
{
    treeWidget()->closePersistentEditor();
    bool defaultProcedure = treeWidget()->isDefaultProcedurePanel(this);
    if(defaultProcedure){
        treeWidget()->configEditor().clearIndexes();
        treeWidget()->deactivateDefaultProcedureCategory();
    }else{
        treeWidget()->configEditor().clearIndexes(text(0));
        treeWidget()->deactivateProcedureCategory();
    }
    while(childCount() != 2){
        if(treeWidget()->isNavigationElementActive(child(2)))
        {
            if(defaultProcedure){
                treeWidget()->deactivateDefaultProcedureCategory();
            }else{
                treeWidget()->deactivateProcedureCategory();
            }
        }
        delete child(2); // Remove First index
    }
}

void DefaultProcedureElement::menuControl(QContextMenuEvent* cev, ListItem* item){
    // Specify file and error checking
    using Actions = QList<QAction*>;
    using ActionFuncs = QList<DefaultProcedureElement::Request_ContextMenu_Func>;
    using Request = DefaultProcedureElement::Request_ContextMenu;
    Actions actions;
    ActionFuncs actionFuncs;
    QMenu* menu = nullptr;

    menu = new QMenu;
    if(item){
        actions = {
          new QAction("Dodaj indeks"),
          new QAction("Edytuj indeks"),
          new QAction("Usuń indeks"),
          new QAction("Usuń indeksy")
        };
        actionFuncs = {
            &DefaultProcedureElement::execRequest_ContextMenu<Request::AddIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::EditIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::RemoveIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::ClearIndexes>,
        };
    }else{
        actions = {
            new QAction("Dodaj indeks"),
            new QAction("Usuń indeksy")
        };
        actionFuncs = {
            &DefaultProcedureElement::execRequest_ContextMenu<Request::AddIndex>,
            &DefaultProcedureElement::execRequest_ContextMenu<Request::ClearIndexes>,
        };
    }

    // After configuration
    if(menu){
        menu->addActions(actions);
        int index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "DefaultProcedureElement Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
}


template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::AddProcedure>(ListItem* )
{

    treeWidget()->closePersistentEditor();

    ListItem* item = new ListItem();
    addChild(item);
    item->parent()->setExpanded(true);
    treeWidget()->scrollToItem(item);
    treeWidget()->editItem(item);

}

template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::AddIndex>(ListItem* item)
{
    if(item){
        item->execRequest_ContextMenu<ListItem::Request_ContextMenu::AddIndex>(nullptr);
    }
}

template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::EditProcedure>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ProceduresElement::ContextMenu::editProcedure", "Item is null");
    treeWidget()->closePersistentEditor();

    item->parent()->setExpanded(true);
    treeWidget()->scrollToItem(item);

    treeWidget()->editItem(item);
}


template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::EditProcedureMultiLine>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ProceduresElement::ContextMenu::editProcedure", "Item is null");
    treeWidget()->closePersistentEditor();
    View::NavigationList& list = *static_cast<View::NavigationList*>(treeWidget());
    list.requestMultiLineAccess();
    item->parent()->setExpanded(true);
    treeWidget()->scrollToItem(item);
    treeWidget()->editItem(item);
}

template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::RemoveProcedure>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "ProceduresElement::ContextMenu::editProcedure", "Item is null");
    treeWidget()->closePersistentEditor();

    treeWidget()->configEditor().removeProcedure(item->text(0));
    for(int i = 0 ; i < item->childCount(); i++){
        if(treeWidget()->isNavigationElementActive(item->child(i)))
        {
            treeWidget()->deactivateProcedureCategory();
            break;
        }
    }
    delete item;
}

template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::ClearIndexes>(ListItem* item)
{
    if(item){
        item->execRequest_ContextMenu<ListItem::Request_ContextMenu::ClearIndexes>(nullptr);
    }
}

template<>
void ProceduresElement::execRequest_ContextMenu<ProceduresElement::Request_ContextMenu::ClearProcedures>(ListItem*)
{
    treeWidget()->closePersistentEditor();

    treeWidget()->configEditor().clearProcedures();
    treeWidget()->deactivateProcedureCategory();
    auto children = takeChildren();
    for(auto child = children.begin(); child < children.end(); child++)
        delete *child;
}


void ProceduresElement::menuControl(QContextMenuEvent* cev, ListItem* item){
    // Specify file and error checking
    using Actions = QList<QAction*>;
    using ActionFuncs = QList<ProceduresElement::Request_ContextMenu_Func>;
    using Request = ProceduresElement::Request_ContextMenu;
    Actions actions;
    ActionFuncs actionFuncs;
    QMenu* menu = nullptr;

    menu = new QMenu;
    if(item){
        actions = {
          new QAction("Dodaj procedure"),
          new QAction("Dodaj indeks"),
          new QAction("Edytuj procedure"),
          new QAction("Edytuj procedure (edytor)"),
          new QAction("Usuń procedure"),
          new QAction("Usuń indeksy"),
          new QAction("Usuń procedury")
        };
        actionFuncs = {
            &ProceduresElement::execRequest_ContextMenu<Request::AddProcedure>,
            &ProceduresElement::execRequest_ContextMenu<Request::AddIndex>,
            &ProceduresElement::execRequest_ContextMenu<Request::EditProcedure>,
            &ProceduresElement::execRequest_ContextMenu<Request::EditProcedureMultiLine>,
            &ProceduresElement::execRequest_ContextMenu<Request::RemoveProcedure>,
            &ProceduresElement::execRequest_ContextMenu<Request::ClearIndexes>,
            &ProceduresElement::execRequest_ContextMenu<Request::ClearProcedures>,
        };
    }else{
        actions = {
            new QAction("Dodaj procedure"),
            new QAction("Usuń procedury")
        };
        actionFuncs = {
            &ProceduresElement::execRequest_ContextMenu<Request::AddProcedure>,
            &ProceduresElement::execRequest_ContextMenu<Request::ClearProcedures>,
        };
    }

    // After configuration
    if(menu){
        menu->addActions(actions);
        int index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "ProceduresElement Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
}






