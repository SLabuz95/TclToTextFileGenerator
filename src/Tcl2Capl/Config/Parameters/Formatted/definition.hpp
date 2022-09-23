#ifndef DEFINITION_PARAMETERS_FORMATTED_HPP
#define DEFINITION_PARAMETERS_FORMATTED_HPP

#include"TclInterpreter/tclToCAPL.hpp"
#include"External/Factory/factory.hpp"

using TclProcedureInterpreter = Tcl::Interpreter::Command::Controller;
using Settings = Tcl::Interpreter::Command::Settings;
using UserProcedure = Tcl::Interpreter::Command::Definition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;


namespace FormatParameters {
    enum class Type : int{
        FCT_Begin,

        TextItem = FCT_Begin,
        NameItem,
        IndexItem,
        ArgumentsFromItem,
        FormatItem,
        AttributeItem,

        FCT_End,
        Size = FCT_End,
        None = -1

    };

    class TypeInfo{
        static const QList<QString> typeMap;
    public:
        inline static Type fromStr(QString& str){return static_cast<Type>(typeMap.indexOf(QRegularExpression(str, QRegularExpression::CaseInsensitiveOption)));}
        inline static QString toStr(Type type){return typeMap.at(std::underlying_type_t<Type>(type));}
        inline static const decltype(typeMap)& typeNames(){return typeMap;}
    };
}

using FormatParametersType = FormatParameters::Type;
using FormatParametersProductDefinition = ProductDefinition<FormatParametersType>;

template <>
struct FormatParametersProductDefinition::ImplementationData::Properties{
};

template <>
class FormatParametersProductDefinition::ImplementationData::Methods{

};

template <>
class FormatParametersProductDefinition::InterfaceData::Methods{
public:
    virtual void toXmlContent(QXmlStreamWriter& xmlWriter) = 0;
    virtual void toActionParameters(QStringList& parameters) = 0;
    //inline virtual RawFormatType rawFormatType() const = 0;
};

using FormatParametersProducts = ProductsConfiguration<FormatParametersType>;


#endif // DEFINITION_PARAMETERS_FORMATTED_HPP
