#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include"tclToCAPL.hpp"
#include"External/Factory/factory.hpp"
#include"parameters.hpp"

using TclProcedureInterpreter = TCLInterpreter::TCLProceduresInterpreter;
using Settings = TclProcedureInterpreter::ProdecuresSettings;
using UserProcedure = TclProcedureInterpreter::ProcedureDefinition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;

// Product Definition
namespace ConditionalActions {
    using Type = UserProcedure::Action::Conditional;
}

namespace ExecutableActions {
    using Type = UserProcedure::Action::Executable;
}

using ConditionalsTypes = ConditionalActions::Type;
using ExecutablesTypes = ExecutableActions::Type;
using ConditionalsFactory = Factory<ConditionalsTypes>;
using ExecutablesFactory = Factory<ExecutablesTypes>;

template <>
class FactoryProductDefinition<ConditionalsTypes>::ProductInterface{
public:
    ProductInterface(){}
    virtual ~ProductInterface(){}

    virtual void toXmlContent(QXmlStreamWriter& xmlWriter);
};

template <>
class FactoryProductDefinition<ExecutablesTypes>::ProductInterface{
public:
    ProductInterface(){}
    virtual ~ProductInterface(){}

    virtual void toXmlContent(QXmlStreamWriter& xmlWriter);
};

// Product Types

template<>template<>
class ConditionalsFactory::Product<ConditionalsTypes::CompareNumbOfArguments> : private QString, public ProductBase<ConditionalsTypes::CompareNumbOfArguments>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:
    IntegerParam numbOfArgs;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>template<>
class ConditionalsFactory::Product<ConditionalsTypes::IsLastSavedStat> : private QString, public ProductBase<ConditionalsTypes::IsLastSavedStat>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:
    IntegerParam lastSavedStat;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>template<>
class ConditionalsFactory::Product<ConditionalsTypes::Compare> : private QString, public ProductBase<ConditionalsTypes::Compare>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:
    ListParam stringsToCompare;
    ListParam inputFormattedString;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};


template<>template<>
class ExecutablesFactory::Product<ExecutablesTypes::Write> : private QString, public ProductBase<ExecutablesTypes::Write>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:
    ListParam outputParamaters;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>template<>
class ExecutablesFactory::Product<ExecutablesTypes::TclParse> : private QString, public ProductBase<ExecutablesTypes::TclParse>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:
    ListParam formattedStringToParse;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>template<>
class ExecutablesFactory::Product<ExecutablesTypes::Error> : private QString, public ProductBase<ExecutablesTypes::Error>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:
    ListParam outputParams;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>template<>
class ExecutablesFactory::Product<ExecutablesTypes::ChangeLastSavedStat> : private QString, public ProductBase<ExecutablesTypes::ChangeLastSavedStat>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:
    IntegerParam newStat;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>template<>
class ExecutablesFactory::Product<ExecutablesTypes::AddFunctionDefinition> : private QString, public ProductBase<ExecutablesTypes::AddFunctionDefinition>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>template<>
class ExecutablesFactory::Product<ExecutablesTypes::AddPreExpression> : private QString, public ProductBase<ExecutablesTypes::AddPreExpression>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:
    ListParam outputParams;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>template<>
class ExecutablesFactory::Product<ExecutablesTypes::AddUserInteraction> : private QString, public ProductBase<ExecutablesTypes::AddUserInteraction>{
public:
    Product(QString param) : QString(param){}
    ~Product(){}

protected:
    ListParam outputParams;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};


#endif // ACTIONS_HPP
