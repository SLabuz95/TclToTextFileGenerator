#ifndef FILE_MODIFIER_ACTION_DEFINITION_HPP
#define FILE_MODIFIER_ACTION_DEFINITION_HPP

#include"TcFileModifier/tcfilemodifierconfigbase.hpp"
#include"External/Factory/productdef.hpp"
#include<QRegularExpression>


using ModifierAction = TcFileModifierConfigBase::ModifierRule::Action;
using RawModifierActionType = TcFileModifierConfigBase::ActionStat;
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
        inline static constexpr uint helperActionsSize(){return static_cast<std::underlying_type_t<Type>>(Type::Section_Helper_End)
                                                                - static_cast<std::underlying_type_t<Type>>(Type::Section_Helper_Begin);}
        inline static constexpr uint condtionalActionsSize(){return static_cast<std::underlying_type_t<Type>>(Type::Section_Conditional_End)
                                                                - static_cast<std::underlying_type_t<Type>>(Type::Section_Conditional_Begin);}
        inline static constexpr uint executableActionsSize(){return static_cast<std::underlying_type_t<Type>>(Type::Section_Executable_End)
                                                                - static_cast<std::underlying_type_t<Type>>(Type::Section_Executable_Begin);}

        inline static Type fromStr(QString& str){return static_cast<Type>(typeMap.indexOf(QRegularExpression(str, QRegularExpression::CaseInsensitiveOption)));}
        inline static QString toStr(Type type){return typeMap.at(std::underlying_type_t<Type>(type));}
        inline static const decltype(typeMap)& typeNames(){return typeMap;}
        inline static const decltype(typeMap) conditionalTypeNames(){
            return typeMap.mid(static_cast<std::underlying_type_t<Type>>(Type::Section_Conditional_Begin), condtionalActionsSize());
        }
        inline static const decltype(typeMap) executableTypeNames(){
            return typeMap.mid(static_cast<std::underlying_type_t<Type>>(Type::Section_Executable_Begin), executableActionsSize());
        }
        inline static const decltype(typeMap) helperTypeNames(){
            return typeMap.mid(static_cast<std::underlying_type_t<Type>>(Type::Section_Helper_Begin), helperActionsSize());
        }
        inline static const decltype(typeMap) conditionalTypeNamesForConfigPanel(){
            return helperTypeNames() + conditionalTypeNames();
        }
        inline static const decltype(typeMap) executableTypeNamesForConfigPanel(){
            return helperTypeNames() + executableTypeNames();
        }
        inline static Type toTypeForConfigPanel(Type type){
            if(isExecutableAction(type)){
                return static_cast<Type>(
                        static_cast<std::underlying_type_t<Type>>(type) - condtionalActionsSize()
                );
            }
            return type;
        }
        inline static Type fromExecutableTypeForConfigPanel(Type type){
            if(not isHelperAction(type)){
                return static_cast<Type>(
                        static_cast<std::underlying_type_t<Type>>(type) + condtionalActionsSize()
                );
            }
            return type;
        }
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
