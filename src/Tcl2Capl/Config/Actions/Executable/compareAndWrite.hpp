#ifndef COMPAREANDWRITE_HPP
#define COMPAREANDWRITE_HPP


#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::CompareAndWrite>::Properties
: public ExecutablesProductDefinition::Definition
{
    using FormattedString = ParametersFactory::Product<Parameters::FormattedString>;
    using ArgumentsMap = QMap<int, QStringList>;
protected:
    ArgumentsMap argumentsMap_;
     FormattedString inputFormattedString_;
    //ListParam outputParamaters;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::CompareAndWrite>::Methods
: public ExecutablesProducts::ImplementationData<ExecutablesTypes::CompareAndWrite>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::CompareAndWrite>::Methods
: public ExecutablesProducts::Implementation<ExecutablesTypes::CompareAndWrite>
{
public:
    FormattedString& inputFormattedString(){return inputFormattedString_;}
    ArgumentsMap& argumentsMap(){return argumentsMap_;}
    const ArgumentsMap& argumentsMap()const{return argumentsMap_;}
    void toAction(UserProcedureRule::ExecutableActions::Type& conditional)override{
        using Action = UserProcedureRule::ExecutableActions::Type;
        using Parameters = UserProcedureRule::ExecutableActions::Type::Parameters;
        Parameters formattedStringParameters;
        inputFormattedString().toActionParameters(formattedStringParameters);

        using ExpectedArgumentsByIndex = ArgumentsMap;
        using ArgumentsByIndexIter = ExpectedArgumentsByIndex::ConstIterator;
        Parameters expectedArguments;
        for(ArgumentsByIndexIter argumentsByIndexIter = argumentsMap().constBegin();
            argumentsByIndexIter != argumentsMap().constEnd();
            argumentsByIndexIter++)
        {
            expectedArguments
                    << QString::number(argumentsByIndexIter.key()) // Index Value
                    << QString::number(argumentsByIndexIter.value().size()) // Numb of Expected Arguments
                    << argumentsByIndexIter.value(); // Arguments list
        }

        conditional = Action(type(),
                                        QStringList{QString::number(expectedArguments.size())}
                                        << expectedArguments
                                        << QStringList{QString::number(formattedStringParameters.size())}
                                        << formattedStringParameters
                                        );

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};

#endif // COMPAREANDWRITE_HPP
