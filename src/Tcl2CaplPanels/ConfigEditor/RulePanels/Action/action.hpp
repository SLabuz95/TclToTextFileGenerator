#ifndef ACTION_HPP
#define ACTION_HPP
#include<QString>
#include<QWidget>

namespace Panels::Configuration::View::Actions{

    template<class Action>
    class ActionViewBase : public QWidget{
    public:
        using Error = QString;
        using ActionRef = const Action* ;
        using ActionDef = Action;
        using ActionType = typename Action::ProductsList;

        using CreateFunction =  ActionViewBase* (*)(ActionRef);
        using CreateFunctionTable = CreateFunction[];
        /*
        static constexpr CreateFunction initCreateFunctionTable(){
            for(ActionType productType = ActionType::FCT_Begin; productType < ActionType::FCT_End; productType++){
                //static_assert (std::is_base_of_v<ProductBase<productType>, Product<productType>>, "Invalid product base");
                createFunctionTable[toUnderlyng(productType)] = &Factory::create<productType>;
            }

            return createFunctionTable[0];
        }

        constexpr static CreateFunctionTable createFunctionTable = {initCreateFunctionTable()};
*/
        static ActionViewBase* create(ActionRef);
        static CreateFunctionTable createFunctionTable;

        virtual Action toAction() = 0;
        virtual constexpr ActionType type()const = 0;

    };

}

#endif // ACTION_HPP
