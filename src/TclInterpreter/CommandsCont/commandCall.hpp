#ifndef COMMANDCALL_HPP
#define COMMANDCALL_HPP
#include"commandDef.hpp"
#include"TclInterpreter/SavedStatCont/savedStatCont.hpp"

namespace Tcl::Interpreter::Command{
    using namespace Tcl::Interpreter::Core;
    using namespace Tcl::Interpreter;
    class Call{
        public:
        // Concept -----------------------------------------------------
        using Name = Definition::ProcedureName;
        using PreExpression = OutputCommand;
        using PreExpressions = QVector<PreExpression>;
        using SquareBracketLevel = uint;
        class Parameter;
        using Parameters = QVector<Parameter>;
        using ProcedureDefinitionIterator = Types::CommandDefinitionIterator;

        // End of Concepts ||||||||||||||||||||||||||||||||||||||||||||
        // Concept Definition ------------------------------------------
        // ----
        class Parameter{

            SavedStat _savedStat;
            //UserInteraction _userInteraction = UserInteraction::NotRequired;
            Parameters rawParameterStats;
        public:

            Parameter(SavedStat savedStat) : _savedStat(savedStat){}
            Parameter(SavedStat savedStat, Call& procedureCall) : _savedStat(savedStat){
                rawParameterStats = {Parameter(Stat::Word, procedureCall.name())};
                rawParameterStats += procedureCall.rawParameters();
            }
            Parameter(const Stat stat, OutputCommand command) : _savedStat{stat, command}{}
            //Parameter(SavedStat savedStat) : _savedStat(savedStat){}

            inline Stat stat()const{return _savedStat.stat();}
            inline QString command()const{return _savedStat.command();}
            //inline void setUserIteractionRequired(){_userInteraction = UserInteraction::Required;}
            inline void setStat(Stat stat){_savedStat.setStat(stat);}
            inline void setCommand(OutputCommand command){_savedStat.setCommand(command);}
            //inline UserInteraction userIteraction(){return _userInteraction;}
            inline SavedStat& savedStat(){return _savedStat;}
            inline const Parameters& rawParameters()const{return rawParameterStats;}

            QString toString(Definition::Format::Target);
            inline void clearMemory(){
                using Parameter = Parameters::Iterator;
                _savedStat.clearMemory();
                for(Parameter param = rawParameterStats.begin() ; param < rawParameterStats.end(); param++)
                    param->clearMemory();
            }

        };
        // End of Concept Definition ||||||||||||||||||||||||||||||||||
        // Objects ----------------------------------------------------
        protected:
            Name _name;
            Parameters _parameters;
            Parameters _rawParameters;
            ProcedureDefinitionIterator _procedureDefinition;
            //UserInteraction _userInteraction;

        // End of Objects ||||||||||||||||||||||||||||||||||||||||||||
        // Functions --------------------------------------------------

        // End of Functions |||||||||||||||||||||||||||||||||||||||||||
        // Interface -------------------------------------------------
        public:
            Call(QString name)
                : _name(name), _procedureDefinition(&Definition::defaultUnknownProcedureDefinition)
            {
                if(name.isEmpty())
                    throw std::runtime_error("New Procedure Call without name and default procedure definition");
            }

            Call(ProcedureDefinitionIterator procedureDefinition)
                : _procedureDefinition(procedureDefinition)/*, _userInteraction(_procedureDefinition->userInteraction)*/
            {}

            //Call& operator=(Call&& call){_name = call._name; _arguments = call._arguments;}
            inline void nextArgument(Parameter& arg){_parameters.append(arg); _rawParameters.append(arg);}
            inline Parameters::size_type parametersLength()const{return _parameters.size();}
            inline Parameters::size_type lastArgumentIndex()const{return parametersLength();}
            inline QString name()const{return (_name.isEmpty())? _procedureDefinition->name : _name;}
            inline Definition::RulesForArguments::Iterator lastRulesForArgument_dynamicCheck()const{
                return (lastArgumentIndex() < _procedureDefinition->rulesForArguments.size())?
                            _procedureDefinition->rulesForArguments.begin() + lastArgumentIndex() :
                            _procedureDefinition->rulesForArguments.end();}
            inline Definition::RulesForArguments::Iterator lastRulesForArgument_onMoved()const{
                return (lastArgumentIndex() - 1 < _procedureDefinition->rulesForArguments.size())?
                            _procedureDefinition->rulesForArguments.begin() + lastArgumentIndex() - 1 :
                            _procedureDefinition->rulesForArguments.end();}
            inline Definition::RulesForArguments::Iterator rulesForUnspecifiedArgument()const{
                return &_procedureDefinition->rulesForUnspecifiedArgument;}
            inline Error isRulesInRange(Definition::RulesForArguments::Iterator rules)const{
                return (rules < _procedureDefinition->rulesForArguments.constEnd())?
                            Error::NoError :
                            Error::Error;}
            inline Definition::RulesOnEndOfCall& rulesOnEndOfCall()const{
                return _procedureDefinition->rulesOnEndOfProcedureCall;
            }

            inline bool isRulesForArgumentsEmpty()const{return _procedureDefinition->isRulesForArgumentsEmpty();}
            inline bool isRulesEmpty()const{return _procedureDefinition->isRulesEmpty();}
            inline Call::Parameters::reference lastParameter(){return _parameters.last();}
            inline Parameters& parameters(){return _parameters;}
            //inline QString generateFunctionDefinitionExample(){return QString("// ") + name() + "(" + generatFunctionDefinitionExampleParameters() + ")";}
            inline QString generateFunctionDefinitionExample(){return QString("") + name() + " " + generateFunctionDefinitionExampleParameters() + "";}
            inline QString generateFunctionDefinitionExampleParameters(){
                QString str;
                for(Parameters::Iterator parameter = parameters().begin(); parameter < _parameters.end(); parameter++)
                    str.append(parameter->toString(Definition::Format::Target::TclFormat) + " ");
                str.chop(QStringLiteral(" ").length());
                return str;
            }
            inline QList<Stat> parametersStats(){
                QList<Stat> stats(parameters().size());
                for(Parameters::Iterator parameter = parameters().begin(); parameter < _parameters.end(); parameter++)
                    stats[parameter - parameters().begin()] = parameter->stat();
                return stats;//QList<Stat>(stats.begin(), stats.end());
            }
            inline Parameters& rawParameters(){return _rawParameters;}
            inline bool isUserInteractionRequired()const{return /*_userInteraction == UserInteraction::Required;*/ _procedureDefinition->userInteraction == UserInteractionStatus::Required;}
            inline void clearMemory(){
                using Parameter = Parameters::Iterator;
                for(Parameter param = _rawParameters.begin(); param < _rawParameters.end(); param++)
                    param->clearMemory();
            }
            QString tryToAddEndOfExpressionSign()const{
                if(name() == "expr_parser")
                    return QString();
                if( not _parameters.isEmpty()){
                    switch(_parameters.last().stat()){
                    case Stat::Script:
                        return QString("\n");
                    }
                }
                return QString(";\n");
            }
            // End of Interface |||||||||||||||||||||||||||||||||||||||||||||
    };

    namespace Types {
        using Calls = QVector<Call>;
    };
};
#endif // COMMANDCALL_HPP
