#include"phaseElement.hpp"
#include<QMenu>
#include<QContextMenuEvent>
#include<QApplication>
#include<QMessageBox>
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"
#include"Tcl2CaplPanels/ConfigEditor/NavigationList/navigationList.hpp"

using namespace Panels::Configuration::Navigation::Phase;

void PhaseElement::menuControl(QContextMenuEvent* cev, ListItem* item){
    // Specify file and error checking
    using Actions = QList<QAction*>;
    using ActionFuncs = QList<PhaseElement::Request_ContextMenu_Func>;
    using Request = PhaseElement::Request_ContextMenu;
    Actions actions;
    ActionFuncs actionFuncs;
    QMenu* menu = nullptr;

    menu = new QMenu;


    // After configuration
    if(menu){
        menu->addActions(actions);
        int index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "PhaseElement Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
}


template<>
void PhasesElement::execRequest_ContextMenu<PhasesElement::Request_ContextMenu::AddPhase>(ListItem* )
{

    treeWidget()->closePersistentEditor();

    ListItem* item = new ListItem();
    addChild(item);
    item->parent()->setExpanded(true);
    treeWidget()->scrollToItem(item);
    treeWidget()->editItem(item);

}


template<>
void PhasesElement::execRequest_ContextMenu<PhasesElement::Request_ContextMenu::EditPhase>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "PhasesElement::ContextMenu::editProcedure", "Item is null");
    treeWidget()->closePersistentEditor();

    item->parent()->setExpanded(true);
    treeWidget()->scrollToItem(item);

    treeWidget()->editItem(item);
}



template<>
void PhasesElement::execRequest_ContextMenu<PhasesElement::Request_ContextMenu::RemovePhase>(ListItem* item)
{
    Q_ASSERT_X(item != nullptr, "PhasesElement::ContextMenu::editProcedure", "Item is null");
    if(item->text(0) != "Default")
    {
        treeWidget()->closePersistentEditor();

        treeWidget()->configEditor().removePhase(item->text(0));
        for(int i = 0 ; i < item->childCount(); i++){
            if(treeWidget()->isNavigationElementActive(item->child(i)))
            {
                treeWidget()->deactivatePhase();
                break;
            }
        }
        delete item;
    }
}

template<>
void PhasesElement::execRequest_ContextMenu<PhasesElement::Request_ContextMenu::ClearPhases>(ListItem*)
{
    treeWidget()->closePersistentEditor();

    treeWidget()->configEditor().clearPhases();
    treeWidget()->deactivatePhase();
    while(childCount() != 1){
        delete child(1); // Remove First index
    }
}


void PhasesElement::menuControl(QContextMenuEvent* cev, ListItem* item){
    // Specify file and error checking
    using Actions = QList<QAction*>;
    using ActionFuncs = QList<PhasesElement::Request_ContextMenu_Func>;
    using Request = PhasesElement::Request_ContextMenu;
    Actions actions;
    ActionFuncs actionFuncs;
    QMenu* menu = nullptr;

    menu = new QMenu;
    if(item){
        actions = {
          new QAction("Dodaj fazę"),
          new QAction("Edytuj fazę"),
          new QAction("Usuń fazę"),
          new QAction("Usuń fazy")
        };
        actionFuncs = {
            &PhasesElement::execRequest_ContextMenu<Request::AddPhase>,
            &PhasesElement::execRequest_ContextMenu<Request::EditPhase>,
            &PhasesElement::execRequest_ContextMenu<Request::RemovePhase>,
            &PhasesElement::execRequest_ContextMenu<Request::ClearPhases>,
        };
    }else{
        actions = {
            new QAction("Dodaj fazę"),
            new QAction("Usuń fazy")
        };
        actionFuncs = {
            &PhasesElement::execRequest_ContextMenu<Request::AddPhase>,
            &PhasesElement::execRequest_ContextMenu<Request::ClearPhases>,
        };
    }

    // After configuration
    if(menu){
        menu->addActions(actions);
        int index = actions.indexOf( menu->exec(cev->globalPos()));
        if(index >= 0){
            Q_ASSERT_X(index < actionFuncs.size(), "PhasesElement Menu", "Index error for action functions");
            (this->*(actionFuncs.at(index)))(item);
        }
        delete menu, menu = nullptr;
    }
}






