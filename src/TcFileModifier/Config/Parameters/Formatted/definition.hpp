#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_DEFINITION_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_DEFINITION_HPP

#include"TcFileModifier/tcfilemodifierconfigbase.hpp"
#include"External/Factory/factory.hpp"
#include<QRegularExpression>


using ModifierFormatParameter = TcFileModifierConfigBase::Format;
class QXmlStreamWriter;
namespace ModifierFormatParameters {
    enum class Type : int{
        FCT_Begin,

        TextItem = FCT_Begin,
        FullLineItem,
        IndexItem,
        ArgumentsFromItem,
        TargetItem,

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
    using Format = TcFileModifierConfigBase::Format;
}

using ModifierFormatParametersType = ModifierFormatParameters::Type;
using ModifierFormatParametersProductDefinition = ProductDefinition<ModifierFormatParametersType>;

template <>
struct ModifierFormatParametersProductDefinition::ImplementationData::Properties{
};

template <>
class ModifierFormatParametersProductDefinition::ImplementationData::Methods{

};

template <>
class ModifierFormatParametersProductDefinition::InterfaceData::Methods{
public:
    virtual void toXmlContent(QXmlStreamWriter& xmlWriter) = 0;
    virtual void toActionParameters(QStringList& parameters) = 0;
    //inline virtual RawFormatType rawFormatType() const = 0;
};

using ModifierFormatParametersProducts = ProductsConfiguration<ModifierFormatParametersType>;


#endif // FILE_MODIFIER_FORMATTED_PARAMETER_DEFINITION_HPP
