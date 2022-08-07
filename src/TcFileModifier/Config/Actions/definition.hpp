#ifndef FILE_MODIFIER_ACTION_DEFINITION_HPP
#define FILE_MODIFIER_ACTION_DEFINITION_HPP

#include"TcFileModifier/tcfilemodifierconfigbase.hpp"
#include"External/Factory/productdef.hpp"
#include<QRegularExpression>


using ModifierAction = TcFileModifierConfigBase::ModifierRule::Action;
class QXmlStreamWriter;

namespace ModifierActions {
    enum class Type  : int{
        FCT_Begin,
        Section_Helper_Begin = FCT_Begin,

        Split = Section_Helper_Begin,
        Section_Helper_End,

        Section_Conditional_Begin = Section_Helper_End,
        StartsWith = Section_Conditional_Begin,
        Compare,
        Section_Conditional_End,

        Section_Executable_Begin = Section_Conditional_End,
        Write = Section_Executable_Begin,
        //CommentOut,
        ChangePhase,
        //Debug,
        Interpret,
        Section_Executable_End,

        FCT_End = Section_Executable_End,
        Size = FCT_End,
        None = -1

    };

    class TypeInfo{
        static const QList<QString> typeMap;
    public:
        inline static bool isHelperAction(Type type){return type >= Type::Section_Helper_Begin and  type < Type::Section_Helper_End;}
        inline static bool isConditionalAction(Type type){return type >= Type::Section_Conditional_Begin and  type < Type::Section_Conditional_End;}
        inline static bool isExecutableAction(Type type){return type >= Type::Section_Executable_Begin and  type < Type::Section_Executable_End;}
        inline static Type fromStr(QString& str){return static_cast<Type>(typeMap.indexOf(QRegularExpression(str, QRegularExpression::CaseInsensitiveOption)));}
        inline static QString toStr(Type type){return typeMap.at(std::underlying_type_t<Type>(type));}
        inline static const decltype(typeMap)& typeNames(){return typeMap;}
    };
}

using ModifierActionTypes = ModifierActions::Type;
using ModifierActionProductDefinition = ProductDefinition<ModifierActionTypes>;
//using ConditionalsFactory = Factory<ModifierActionTypes>;


template <>
struct ModifierActionProductDefinition::ImplementationData::Properties{

};

template <>
class ModifierActionProductDefinition::ImplementationData::Methods{

};

template <>
class ModifierActionProductDefinition::InterfaceData::Methods{
public:
    virtual void toXmlContent(QXmlStreamWriter& xmlWriter) = 0;
    virtual void toAction(ModifierAction&) = 0;
    //inline virtual RawFormatType rawFormatType() const = 0;
};

using ModifierActionProducts = ProductsConfiguration<ModifierActionTypes>;

#endif // FILE_MODIFIER_ACTION_DEFINITION_HPP
