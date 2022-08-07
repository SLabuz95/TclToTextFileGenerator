#ifndef ACTION_PANEL_ADDUSERINTERACTION_HPP
#define ACTION_PANEL_ADDUSERINTERACTION_HPP

#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include"Tcl2CaplPanels/ConfigEditor/RulePanels/FormattedString/formattedString.hpp"

namespace Panels::Configuration::View::ActionsList{

    class AddUserInteractionActionView
            : public ActionDataView<ExecutablesFactory::ListOfBases>
    {
        static constexpr ActionType actionType = ExecutablesTypes::AddUserInteraction;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using Action = ExecutablesFactory::Product<actionType>;

        AddUserInteractionActionView(QWidget*);
        AddUserInteractionActionView(QWidget*, ActionPtr);
    public:
        static ActionDataView* create(QWidget*, ActionRef);
        using ActionView = ActionView;
    protected:
        using Executables = ExecutablesFactory::ListOfBases;
        using ExecutablesList = List<Executables>;
        using ListItem = ExecutablesList::ListItem;

        // GUI Elements
        // List of Indexes definition
    public:
    protected:
        // GUI Layout
        QSplitter splitter;
        FormattedString::Panel formattedStringList;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return actionType;}
        void readAction(ActionBase&) override;

    };

}
#endif // ACTION_PANEL_ADDUSERINTERACTION_HPP
