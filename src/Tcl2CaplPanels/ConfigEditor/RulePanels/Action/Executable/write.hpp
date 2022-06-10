#ifndef ACTION_PANEL_WRITE_HPP
#define ACTION_PANEL_WRITE_HPP

#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include"Tcl2CaplPanels/ConfigEditor/RulePanels/FormattedString/formattedString.hpp"

namespace Panels::Configuration::View::ActionsList{

    class WriteActionView
            : public ActionDataView<ExecutablesFactory::ListOfBases>
    {
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;

        WriteActionView(ActionView&);
    public:
        static ActionDataView* create(ActionView&, ActionRef);
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
        FormattedString::List formattedStringList;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return ExecutablesTypes::Write;}

    };

}
#endif // ACTION_PANEL_WRITE_HPP
