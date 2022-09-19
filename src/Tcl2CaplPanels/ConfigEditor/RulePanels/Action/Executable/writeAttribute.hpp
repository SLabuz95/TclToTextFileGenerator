#ifndef ACTION_PANEL_WRITEATTRIBUTE_HPP
#define ACTION_PANEL_WRITEATTRIBUTE_HPP

#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include"Tcl2CaplPanels/ConfigEditor/RulePanels/FormattedString/formattedString.hpp"

namespace Panels::Configuration::View::ActionsList{

    class WriteAttributeActionView
            : public ActionDataView<ExecutablesFactory::ListOfBases>
    {
        static constexpr ActionType actionType = ExecutablesTypes::WriteAttribute;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using Action = ExecutablesFactory::Product<actionType>;

        WriteAttributeActionView(QWidget*);
        WriteAttributeActionView(QWidget*, ActionPtr);
        ~WriteAttributeActionView()override{
        }
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
#endif // ACTION_PANEL_WRITEATTRIBUTE_HPP
