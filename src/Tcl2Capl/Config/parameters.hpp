#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include"tclToCAPL.hpp"
#include"External/Factory/factory.hpp"


using TclProcedureInterpreter = TCLInterpreter::TCLProceduresInterpreter;
using Settings = TclProcedureInterpreter::ProdecuresSettings;
using UserProcedure = TclProcedureInterpreter::ProcedureDefinition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;

// Product Definition
namespace Parameters {
    enum Type : uint{
        FCT_Begin,

        String = FCT_Begin,
        FormattedString,
        Integer,
        List,   // List of Parameters

        FCT_End,

    };

}

namespace FormatParameters {
    enum Type : uint{
        FCT_Begin,

        INDEX_OR_FULL_LINE = FCT_Begin,
        ARGS_AFTER_INDEX,
        SEPARATOR,
        TARGET,

        FCT_End,

    };

}

using ParametersType = Parameters::Type;
using FormatParametersType = FormatParameters::Type;
using ParametersFactory = Factory<ParametersType>;
using FormatParametersFactory = Factory<FormatParametersType>;

template <>
class FactoryProductDefinition<ParametersType>::ProductInterface{
public:
    ProductInterface(){}
    virtual ~ProductInterface(){}

    virtual void toXmlContent(QXmlStreamWriter& xmlWriter){};
};

template <>
class FactoryProductDefinition<FormatParametersType>::ProductInterface{
public:
    using RawFormatType = UserProcedure::Format::Rule;
    ProductInterface(){}
    virtual ~ProductInterface(){}

    inline virtual RawFormatType rawFormatType() const = 0;
    virtual void toXmlContent(QXmlStreamWriter& xmlWriter){};
};

// Product Types


template<>
template<>
class ParametersFactory::Product<ParametersType::String> : public ProductBase<ParametersType::FormattedString>{
public:
    Product(){}
    ~Product(){}

protected:
    QString content;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>
template<>
class ParametersFactory::Product<ParametersType::FormattedString> : public ProductBase<ParametersType::FormattedString>{
public:
    Product(){}
    ~Product(){}

protected:
    FormatParametersFactory::ProductInterface* data = nullptr;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>
template<>
class ParametersFactory::Product<ParametersType::Integer> : public ProductBase<ParametersType::Integer>{
public:
    Product(){}
    ~Product(){}

protected:
    qint32 value = NO_VALUE;

public:
   inline static constexpr int NO_VALUE = INT32_MIN;
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>
template<>
class ParametersFactory::Product<ParametersType::List> : public ProductBase<ParametersType::List>{
public:
    Product(){}
    ~Product(){}

protected:
    ListOfBases params;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

template<>
template<>
class FormatParametersFactory::Product<FormatParametersType::INDEX_OR_FULL_LINE> : public ProductBase<FormatParametersType::INDEX_OR_FULL_LINE>{
public:
    Product(){}
    ~Product(){}

protected:
    FormatParametersFactory::ProductInterface* data = nullptr;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   inline RawFormatType rawFormatType()const override final{return RawFormatType::INDEX_OR_FULL_LINE;}
};


template<>
template<>
class FormatParametersFactory::Product<FormatParametersType::ARGS_AFTER_INDEX> : public ProductBase<FormatParametersType::ARGS_AFTER_INDEX>{
public:
    Product(){}
    ~Product(){}

protected:
    FormatParametersFactory::ProductInterface* data = nullptr;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   inline RawFormatType rawFormatType()const override final{return RawFormatType::ARGS_AFTER_INDEX;}

};

template<>
template<>
class FormatParametersFactory::Product<FormatParametersType::SEPARATOR> : public ProductBase<FormatParametersType::SEPARATOR>{
public:
    Product(){}
    ~Product(){}

protected:
    FormatParametersFactory::ProductInterface* data = nullptr;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   inline RawFormatType rawFormatType()const override final{return RawFormatType::SEPARATOR;}

};

template<>
template<>
class FormatParametersFactory::Product<FormatParametersType::TARGET> : public ProductBase<FormatParametersType::TARGET>{
public:
    Product(){}
    ~Product(){}

protected:
    FormatParametersFactory::ProductInterface* data = nullptr;

public:
   void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   inline RawFormatType rawFormatType()const override final{return RawFormatType::TARGET;}

};

using FormatParam = ParametersFactory::Product<ParametersType::FormattedString>;
using IntegerParam = ParametersFactory::Product<ParametersType::Integer>;
using ListParam = ParametersFactory::Product<ParametersType::List>;


#endif // PARAMETERS_HPP
