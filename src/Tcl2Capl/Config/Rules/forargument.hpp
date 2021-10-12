#ifndef FORARGUMENT_HPP
#define FORARGUMENT_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Rules/definition.hpp"

template<>
template<>
struct RulesProducts::ImplementationData<RulesTypes::RulesForArgument>::Properties
: protected RulesProductDefinition::Definition
{
public:
    using Index = int;
   // using AdvancedRules = QList<RulesProducts::Product<RulesType::RawRule>>;
    static constexpr Index INDEX_NO_VALUE = -1;
   // Product(){}
   // Product(Index index) : index_(index){}

protected:
    Index index_ = INDEX_NO_VALUE;
   // AdvancedRules dynamicRules_;
  //  AdvancedRules rulesOnMove_;

};

template<>
template<>
class RulesProducts::ImplementationData<RulesTypes::RulesForArgument>::Methods
        : protected RulesProducts::ImplementationData<RulesTypes::RulesForArgument>::Properties
{

};

template<>
template<>
class RulesProducts::InterfaceData<RulesTypes::RulesForArgument>::Methods
        : public RulesProducts::Implementation<RulesTypes::RulesForArgument>{
public:
    /*inline Index index()const {return index_;}
    inline AdvancedRules& dynamicRules(){return dynamicRules_;}
    inline AdvancedRules& rulesOnMove(){return rulesOnMove_;}
    void toRule(UserProcedureRule&) override;
    void toXmlContent(QXmlStreamWriter&) override;*/
};

#endif // FORARGUMENT_HPP
