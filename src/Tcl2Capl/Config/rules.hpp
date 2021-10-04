#ifndef RULES_HPP
#define RULES_HPP

#include"tclToCAPL.hpp"
#include"External/Factory/factory.hpp"
#include"actions.hpp"


using TclProcedureInterpreter = TCLInterpreter::TCLProceduresInterpreter;
using Settings = TclProcedureInterpreter::ProdecuresSettings;
using UserProcedure = TclProcedureInterpreter::ProcedureDefinition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;

// Product Definition
namespace Rules {
    enum class Type : uint{
        FCT_Begin,

        QuickRule = FCT_Begin, // To generate raw rule
        RawRule,    // Like in Interpreter
        RulesForArgument, // Raw Rule but for specific argument index only

        FCT_End,

    };
}



using RulesType = Rules::Type;
using RulesFactory = Factory<RulesType>;

template <>
class FactoryProductDefinition<RulesType>::ProductInterface{
    ControlFlag controlFlag_;
public:
    ProductInterface(){}
    ProductInterface(ControlFlag flag) : controlFlag_(flag){}
    virtual ~ProductInterface(){}

    inline void changeControlFlag(ControlFlag cf){controlFlag_ = cf;}
    inline ControlFlag controlFlag()const{return controlFlag_;}
    virtual void toRule(UserProcedureRule&) = 0;
    virtual void toXmlContent(QXmlStreamWriter& xmlWriter);
};

// Product Types

/*template<RulesType ruleType>
using ProductBase = RulesFactory::ProductBase<ruleType>;
*/
template<>
template<>
class RulesFactory::Product<RulesType::RawRule> : public UserProcedureRule, public ProductBase<RulesType::RawRule>{
public:
    using ConditionalActions = ConditionalsFactory::ListOfBases;
    using ExecutableActions = ExecutablesFactory::ListOfBases;

    Product(){}
    Product(ControlFlag controlFlag) : ProductBase(controlFlag){}
    ~Product(){}

protected:
    ConditionalActions conditions_;
    ExecutableActions executables_;

public:
    inline ConditionalActions& conditions(){return conditions_;}
    inline ExecutableActions& executables(){return executables_;}
   void toRule(UserProcedureRule&) override;
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>
template <>
class RulesFactory::Product<RulesType::QuickRule> : public ProductBase<RulesType::QuickRule>{

public:
    using NumbOfArguments = UserProcedure::NumbOfArguments;
    using NumbOfAcceptableConditions = uint;
    using Index = UserProcedure::Index;
    using ExpectedArgument = QString;
    using ExpectedArguments = QList<ExpectedArgument>;
    using ExpectedArgumentsByIndex = QMap<Index, ExpectedArguments>;
    using ExpectedArgumentsWithoutIndex = ExpectedArguments;
    using LastInsertedIndexIter = ExpectedArgumentsByIndex::Iterator;
    using WriteActions = UserProcedureRule::ExecutableActions;

    Product(NumbOfArguments numbOfArguments) : numbOfArguments(numbOfArguments){}
    Product(NumbOfArguments numbOfArguments, ControlFlag controlFlag = ControlFlag::_Default)
        :   ProductBase(controlFlag), numbOfArguments(numbOfArguments){};
    ~Product(){}

protected:
    NumbOfAcceptableConditions numbOfAcceptableConditions = 0;
    NumbOfArguments numbOfArguments = -1;
    ExpectedArgumentsByIndex expectedArgumentsByIndex;
    WriteActions writeActions_;
    ExpectedArgumentsWithoutIndex expectedArguments;

public:
    inline NumbOfAcceptableConditions getNumbOfAcceptableConditions() const{return numbOfAcceptableConditions;}
    inline NumbOfArguments& getNumbOfArguments(){return numbOfArguments;}
    void toRule(UserProcedureRule&) override;
    inline LastInsertedIndexIter addIndex(Index index){return expectedArgumentsByIndex.insert(index, {});}
    inline bool isIndexExist(Index index)const{return expectedArgumentsByIndex.keys().contains(index);}
    inline ExpectedArgumentsByIndex& getExpectedArgumentsByIndex(){return expectedArgumentsByIndex;}
    inline WriteActions& writeActions(){return writeActions_;}
    void toXmlContent(QXmlStreamWriter& xmlWriter) override;
};

template<>
template <>
class RulesFactory::Product<RulesType::RulesForArgument> : public ProductBase<RulesType::RulesForArgument>{
public:
    using Index = int;
    using AdvancedRules = QList<RulesFactory::Product<RulesType::RawRule>>;
public:
    static constexpr Index INDEX_NO_VALUE = -1;
    Product(){}
    Product(Index index) : index_(index){}
    ~Product(){}


protected:
    Index index_ = INDEX_NO_VALUE;
    AdvancedRules dynamicRules_;
    AdvancedRules rulesOnMove_;

public:
    inline Index index()const {return index_;}
    inline AdvancedRules& dynamicRules(){return dynamicRules_;}
    inline AdvancedRules& rulesOnMove(){return rulesOnMove_;}
    void toRule(UserProcedureRule&) override;
    void toXmlContent(QXmlStreamWriter&) override;
};



#endif // RULES_HPP
