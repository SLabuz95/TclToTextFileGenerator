#include"exceptionsPanel.hpp"
#include<QApplication>
#include<QMenu>
#include<QAction>
#include<QAbstractItemView>
#include<QSpinBox>
#include"analysisPanel.hpp"

ExceptionsPanel::ExceptionsPanel(AnalysisPanel& panel)
    : QWidget(&panel),
      exceptionsList(*this)
{


    layout.addWidget(&exceptionsList);
    setLayout(&layout);
}


ExceptionsPanel::ExceptionsList::ExceptionsList(ExceptionsPanel& panel)
    : QTreeWidget(&panel)
{
    //layout.addWidget(&list);

    //setLayout(&layout);
    //installEventFilter(this);
}

void ExceptionsPanel::ExceptionsList::load(FunctionDefinitions::Definitions::Iterator defs)
{
    clear();
    using Definitions = FunctionDefinitions::DefinitionInfo;
    for(Definitions::Iterator definition = defs.value().begin();
        definition != defs.value().end();
        definition++)
    {
        QTreeWidgetItem* definitionItem = new QTreeWidgetItem(this);
        definitionItem->setText(0, "Liczba parametrów: " + QString::number(definition.key()));
        using ParametersNumb = Definitions::mapped_type;
        for(ParametersNumb::Iterator parametersNumb = definition.value().begin();
            parametersNumb != definition.value().end();
            parametersNumb++)
        {
            QTreeWidgetItem* parametersItem = new QTreeWidgetItem();
            parametersItem->setText(0, "Parametry: " + toString(parametersNumb.key()));
            definitionItem->addChild(parametersItem);
            using Example = ParametersNumb::mapped_type::Iterator;
            for(Example example = parametersNumb.value().begin();
                example != parametersNumb.value().end();
                example++)
            {
                parametersItem->addChild(new QTreeWidgetItem({*example}));
            }
        }
    }
}

void ExceptionsPanel::ExceptionsList::load(FunctionDefinitions::DefinitionInfo::Iterator defInfo)
{
    clear();
    using Definitions = FunctionDefinitions::DefinitionInfo;
    using ParametersNumb = Definitions::mapped_type;
    for(ParametersNumb::Iterator parametersNumb = defInfo.value().begin();
        parametersNumb != defInfo.value().end();
        parametersNumb++)
    {
        QTreeWidgetItem* parametersItem = new QTreeWidgetItem(this);
        parametersItem->setText(0, "Parametry: " + toString(parametersNumb.key()));
        using Example = ParametersNumb::mapped_type::Iterator;
        for(Example example = parametersNumb.value().begin();
            example != parametersNumb.value().end();
            example++)
        {
            parametersItem->addChild(new QTreeWidgetItem({*example}));
        }
    }
}

QString ExceptionsPanel::ExceptionsList::toString(FunctionDefinitions::Parameters params){
    using Param = decltype(params)::Iterator;
    using Stat = decltype(params)::Type;
    QString retString = "(";

    for(Param param = params.begin(); param != params.end(); param++){
        switch(*param){
        case Stat::Script:
        case Stat::EndOfCodeBlock:
            retString += "CodeBlock";
            break;
        case Stat::Const:
            retString += "Const";
            break;
        case Stat::EndOfExpression:
            retString += "Expression";
            break;
        case Stat::BracesEnd:
            retString += "List";
            break;
        case Stat::String:
        case Stat::StringInQuotes:
            retString += "String";
            break;
        case Stat::CommandSubbing:
            retString += "FunctionCall";
            break;
        case Stat::Word:
            retString += "Operator";
            break;
        case Stat::PendingSnprintf:
            retString += "PendingComplexString";
            break;
        case Stat::PendingString:
            retString += "PendingString";
            break;
        case Stat::Snprintf:
            retString += "ComplexString";
            break;
        case Stat::Variable:
            retString += "Variable";
            break;
        default:
            break;
        }
        retString += ", ";
    }

    if(retString.endsWith(", "))
        retString.chop(QString(", ").length());


    return retString + ")";
}
