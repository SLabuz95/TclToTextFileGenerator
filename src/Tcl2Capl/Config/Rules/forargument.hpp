#ifndef FORARGUMENT_HPP
#define FORARGUMENT_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Rules/definition.hpp"

template<>
template<>
struct RulesFactory::ImplementationData<RulesTypes::RulesForArgument>::Properties{
public:
    using Index = int;
   // using AdvancedRules = QList<RulesFactory::Product<RulesType::RawRule>>;
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
class RulesFactory::ImplementationData<RulesTypes::RulesForArgument>::Methods : protected RulesFactory::ImplementationData<RulesTypes::RulesForArgument>::Properties{

};

template<>
template<>
class RulesFactory::InterfaceData<RulesTypes::RulesForArgument>::Methods : public RulesFactory::Implementation<RulesTypes::RulesForArgument>{
public:
    /*inline Index index()const {return index_;}
    inline AdvancedRules& dynamicRules(){return dynamicRules_;}
    inline AdvancedRules& rulesOnMove(){return rulesOnMove_;}
    void toRule(UserProcedureRule&) override;
    void toXmlContent(QXmlStreamWriter&) override;*/
};

#endif // FORARGUMENT_HPP
