#ifndef ACTION_PANEL_ADDDEFINITION_HPP
#define ACTION_PANEL_ADDDEFINITION_HPP

#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include"Tcl2CaplPanels/ConfigEditor/RulePanels/FormattedString/formattedString.hpp"

namespace Panels::Configuration::View::ActionsList{

    class AddPredefinitionActionView
            : public ActionDataView<ExecutablesFactory::ListOfBases>
    {
        using PredefinitionsController = Tcl::Interpreter::PredefinitionsController;
        class ScopeComboBox : public QComboBox{
        public:
            ScopeComboBox() : QComboBox(){
                addItems(PredefinitionsController::scopeNames());
            }
        };

        static constexpr ActionType actionType = ExecutablesTypes::AddPredefinition;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using Action = ExecutablesFactory::Product<actionType>;

        AddPredefinitionActionView(QWidget*);
        AddPredefinitionActionView(QWidget*, ActionPtr);
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
        QFormLayout mainLayout;
        ScopeComboBox scopeComboBox;
        QSplitter splitter;
        FormattedString::Panel formattedStringList;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return actionType;}
        void readAction(ActionBase&) override;

    };

}

#endif // ACTION_PANEL_ADDPREDEFINITION_HPP
