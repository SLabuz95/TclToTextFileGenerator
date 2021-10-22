#ifndef QUICK_HPP
#define QUICK_HPP


#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Rules/definition.hpp"

template<>
template<>
struct RulesProducts::ImplementationData<RulesTypes::QuickRule>::Properties
: protected RulesProductDefinition::Definition
{
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

   // Product(NumbOfArguments numbOfArguments) : numbOfArguments(numbOfArguments){}
   // Product(NumbOfArguments numbOfArguments, ControlFlag controlFlag = ControlFlag::_Default)
    //    :   ProductBase(controlFlag), numbOfArguments(numbOfArguments){};

protected:
    NumbOfAcceptableConditions numbOfAcceptableConditions = 0;
    NumbOfArguments numbOfArguments = -1;
    ExpectedArgumentsByIndex expectedArgumentsByIndex;
    WriteActions writeActions_;
    ExpectedArgumentsWithoutIndex expectedArguments;

};

template<>
template<>
class RulesProducts::ImplementationData<RulesTypes::QuickRule>::Methods  : public RulesProducts::ImplementationData<RulesTypes::QuickRule>::Properties{

};

template<>
template<>
class RulesProducts::InterfaceData<RulesTypes::QuickRule>::Methods : public RulesProducts::Implementation<RulesTypes::QuickRule>{
public:
    inline NumbOfAcceptableConditions getNumbOfAcceptableConditions() const{return numbOfAcceptableConditions;}
    inline NumbOfArguments& getNumbOfArguments(){return numbOfArguments;}
    //void toRule(UserProcedureRule&) override;
    inline LastInsertedIndexIter addIndex(Index index){return expectedArgumentsByIndex.insert(index, {});}
    inline bool isIndexExist(Index index)const{return expectedArgumentsByIndex.keys().contains(index);}
    inline ExpectedArgumentsByIndex& getExpectedArgumentsByIndex(){return expectedArgumentsByIndex;}
    inline WriteActions& writeActions(){return writeActions_;}
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
};

#endif // QUICK_HPP
