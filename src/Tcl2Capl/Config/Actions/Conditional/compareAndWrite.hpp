#ifndef COMPAREANDWRITE_HPP
#define COMPAREANDWRITE_HPP


#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareAndWrite>::Properties
: public ConditionalsProductDefinition::Definition
{
    using FormattedString = ParametersFactory::Product<Parameters::FormattedString>;
    using ArgumentsMap = QMap<int, QStringList>;
protected:
    QString numbOfArguments_;
    ArgumentsMap argumentsMap_;
     FormattedString inputFormattedString_;
    //ListParam outputParamaters;

};

template<>
template<>
class ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareAndWrite>::Methods
: public ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareAndWrite>::Properties
{

};

template<>
template<>
class ConditionalsProducts::InterfaceData<ConditionalsTypes::CompareAndWrite>::Methods
: public ConditionalsProducts::Implementation<ConditionalsTypes::CompareAndWrite>
{
public:
    FormattedString& inputFormattedString(){return inputFormattedString_;}
    ArgumentsMap& argumentsMap(){return argumentsMap_;}
    const ArgumentsMap& argumentsMap()const{return argumentsMap_;}
    const QString& numbOfArguments()const{return numbOfArguments_;}
    QString& numbOfArguments(){return numbOfArguments_;}
    void toAction(UserProcedureRule::ConditionalActions::Type& conditional)override{
        using Action = UserProcedureRule::ConditionalActions::Type;
        using Parameters = Action::Parameters;
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
                                        QStringList{numbOfArguments()}
                                        << QString::number(expectedArguments.size())
                                        << expectedArguments
                                        << QStringList{QString::number(formattedStringParameters.size())}
                                        << formattedStringParameters
                                        );

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        using Action = UserProcedureRule::ConditionalActions::Type;
        using Parameters = Action::Parameters;
        xmlWriter.writeStartElement("executableAction");
        xmlWriter.writeAttribute("type", UserProcedure::Action::toStr_conditional(type()));
        // index_
        xmlWriter.writeEmptyElement("param");
        xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
        xmlWriter.writeAttribute("value", numbOfArguments());
        // argumentsMap_ - sequence of parameter pairs <int, list<string>>
        using ExpectedArgumentsByIndex = ArgumentsMap;
        using ArgumentsByIndexIter = ExpectedArgumentsByIndex::ConstIterator;
        for(ArgumentsByIndexIter argumentsByIndexIter = argumentsMap().constBegin();
            argumentsByIndexIter != argumentsMap().constEnd();
            argumentsByIndexIter++)
        {
            // Index
            xmlWriter.writeEmptyElement("param");
            xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
            xmlWriter.writeAttribute("value", QString::number(argumentsByIndexIter.key()));
            // stringsToCompare_
            xmlWriter.writeStartElement("param"); // List?
            xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
            for(QStringList::ConstIterator string = argumentsByIndexIter.value().constBegin(); string < argumentsByIndexIter.value().constEnd(); string++){
                xmlWriter.writeStartElement("param"); // String param?
                xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
                xmlWriter.writeCharacters(*string);
                xmlWriter.writeEndElement(); // List? End
            }
            xmlWriter.writeEndElement(); // List? End
        }
        // inputFormattedString_
        inputFormattedString_.toXmlContent(xmlWriter);
        xmlWriter.writeEndElement();
    }
};

#endif // COMPAREANDWRITE_HPP
