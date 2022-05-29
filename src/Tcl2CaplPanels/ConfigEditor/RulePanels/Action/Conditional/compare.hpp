#ifndef ACTION_PANEL_COMPARE_HPP
#define ACTION_PANEL_COMPARE_HPP

#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"


namespace Panels::Configuration::View::ActionsList{


    class CompareActionView
            : public ActionDataView<ConditionalsFactory::Product<ConditionalsTypes::Compare>>
    {
        using Conditionals = ConditionalsFactory::ListOfBases;
        using ConditionalsList = List<Conditionals>;
        using ListItem = ConditionalsList::ListItem;



    };

}

#endif // ACTION_PANEL_COMPARE_HPP
