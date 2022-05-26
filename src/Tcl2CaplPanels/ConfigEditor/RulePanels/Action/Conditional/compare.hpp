#ifndef ACTION_PANEL_COMPARE_HPP
#define ACTION_PANEL_COMPARE_HPP

#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"../action.hpp"


namespace Panels::Configuration::View::Actions{


    class CompareActionView
            : public ActionViewBase<ConditionalsFactory::Product<ConditionalsTypes::Compare>>
    {
        using Conditionals = ConditionalsFactory::ListOfBases;
        using ConditionalsList = ActionsList::ActionsList<Conditionals,
                                Actions::ActionViewBase<ConditionalsFactory::ProductBase>>;
        using ListItem = ConditionalsList::ListItem;



    };

}

#endif // ACTION_PANEL_COMPARE_HPP
