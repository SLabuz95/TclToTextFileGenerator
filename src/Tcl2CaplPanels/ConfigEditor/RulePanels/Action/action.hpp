#ifndef ACTION_HPP
#define ACTION_HPP
#include<QString>
#include<QWidget>
#include<QVBoxLayout>
#include<QComboBox>
#include"Tcl2Capl/controllerconfig.hpp"

namespace Panels::Configuration::View::Actions{

    template<class Action>
    class ActionViewBase : public QWidget{
        using MainLayout = QVBoxLayout;

        class ActionTypeComboBox : public QComboBox{
        public:
            ActionTypeComboBox(){
                if constexpr (std::is_same_v<typename Action::ProductsList, Tcl::Command::Definition::Action::Conditional>)
                    addItems(Tcl::Command::Definition::Action::conditionalsNames());
                else
                    addItems(Tcl::Command::Definition::Action::executablesNames());
            }
        };

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
