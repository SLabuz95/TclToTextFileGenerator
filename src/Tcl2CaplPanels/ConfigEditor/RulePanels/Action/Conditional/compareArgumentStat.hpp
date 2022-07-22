#ifndef ACTION_PANEL_COMPAREARGUMENTSTAT_HPP
#define ACTION_PANEL_COMPAREARGUMENTSTAT_HPP

#include"Tcl2CaplPanels/ConfigEditor/RulePanels/FormattedString/formattedString.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ActionsList::CompareArgumentStatActionView{

    class CompareArgumentStatActionView
            : public ActionDataView<ConditionalsFactory::ListOfBases>
    {
        static constexpr ActionType actionType = ConditionalsTypes::CompareArgumentStat;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using Action = ConditionalsFactory::Product<actionType>;

        CompareArgumentStatActionView(QWidget*);
        CompareArgumentStatActionView(QWidget*, ActionPtr);
    public:
        static ActionDataView* create(QWidget*, ActionRef);
        using ActionView = ActionView;
    protected:
        using Conditionals = ConditionalsFactory::ListOfBases;
        using ConditionalsList = List<Conditionals>;
        using ListItem = ConditionalsList::ListItem;

        // GUI Elements
        // List of Indexes definition
    public:

    protected:
        // GUI Layout

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return actionType;}
        void readAction(ActionBase&) override;

    };

}


#endif // COMPAREARGUMENTSTAT_HPP
