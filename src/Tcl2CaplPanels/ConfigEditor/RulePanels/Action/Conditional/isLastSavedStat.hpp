#ifndef ACTION_PANEL_ISLASTSAVEDSTAT_HPP
#define ACTION_PANEL_ISLASTSAVEDSTAT_HPP

#include"Tcl2CaplPanels/ConfigEditor/RulePanels/FormattedString/formattedString.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ActionsList::IsLastSavedStatActionView{

    class IsLastSavedStatActionView
            : public ActionDataView<ConditionalsFactory::ListOfBases>
    {
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;

        IsLastSavedStatActionView(ActionView&);
    public:
        static ActionDataView* create(ActionView&, ActionRef);
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
        constexpr ActionType type()const override{return ConditionalsTypes::IsLastSavedStat;}

    };

}


#endif // ISLASTSAVEDSTAT_HPP
