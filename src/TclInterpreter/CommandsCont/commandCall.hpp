#ifndef COMMANDCALL_HPP
#define COMMANDCALL_HPP
#include"commandDef.hpp"
#include"TclInterpreter/SavedStatCont/savedStatCont.hpp"

namespace Tcl::Interpreter::Command{
    using namespace Tcl::Interpreter::Core;
    using namespace Tcl::Interpreter;

    class Controller;
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
        using ParameterIndex = int;
        // End of Concepts ||||||||||||||||||||||||||||||||||||||||||||
        // Concept Definition ------------------------------------------
        // ----

        class Parameter{
            /* COMPLEX PARAMETERS IMPLEMENTATION - DONT REMOVE
            Stat stat_;
            QString outputCommand;

            using Params = QList<CompleteParameter>;
            union Data{
                Data() : complexParameters(){}
                Data(QString str) : rawParameter(str) {}
                ~Data(){ }
                QString rawParameter;
                Params complexParameters;
            } data;
            Parameter() : stat_(Stat::Word), data(QString()){}
            Parameter(const Parameter& parameter)
            : stat_(parameter.stat()),
              outputCommand(parameter.outputCommand)
            {
                if(parameter.isSimpleParameter()){
                    data.complexParameters.~Params();
                    new (&data.rawParameter) QString(parameter.data.rawParameter);
                }else{
                    data.complexParameters = parameter.data.complexParameters;
                }
            }
            Parameter(const Stat stat, OutputCommand command) : stat_{stat}{}
            ~Parameter(){
                if(stat() == Stat::Word or stat() == Stat::BackslashSubbing)
                    data.rawParameter.~QString();
                else
                    data.complexParameters.~Params();
            }
            inline bool isSimpleParameter()const{
                switch(stat_){
                case Stat::Word:
                case Stat::BackslashSubbing:
                case Stat::VariableSubbing:
                   return true;
                default:
                    break;
                }
                return false;
            }
            inline void appendRawCommand(OutputCommand command){ if(isSimpleParameter()) {data.rawParameter.append(command); outputCommand.append(command);};}
            Parameter& operator=(const Parameter& other)
            {
                if (this == &other)
                    return *this;

                // Clean up data if nessesary + copy data
                if(isSimpleParameter() != other.isSimpleParameter()){ // if not the same data type
                    if(isSimpleParameter()){
                        data.rawParameter.~QString();
                        new (&data.complexParameters) Params(other.data.complexParameters);
                    }else{
                        data.complexParameters.~Params();
                        new (&data.rawParameter) QString(other.data.rawParameter);
                    }
                }else{
                    if(isSimpleParameter()){
                        data.rawParameter = other.data.rawParameter;
                    }else{
                        data.complexParameters = other.data.complexParameters;
                    }
                }
                stat_ = other.stat();
                outputCommand = other.outputCommand;

            }
*/

            // Final Implementation? You dont require all raw parameters cause you cant change them
            // If you will allow access to parameters of complex parameters ( other calls), then maybe change to above Implementation
            QString outputCommand_;
            QString rawCommand_;
            Stat stat_;


        public:
            Parameter() : stat_(Stat::Word){}
            /*Parameter(const Parameter& parameter)
            : stat_(parameter.stat()),
              outputCommand(parameter.outputCommand)
            {
                if(parameter.isSimpleParameter()){
                    data.complexParameters.~Params();
                    new (&data.rawParameter) QString(parameter.data.rawParameter);
                }else{
                    data.complexParameters = parameter.data.complexParameters;
                }
            }*/
            Parameter(const Stat stat, OutputCommand command, OutputCommand rawCommand = QString()) : stat_{stat}
            {
                appendCommand(command, rawCommand);
            }
            inline Stat stat()const{return stat_;}
            inline void setStat(Stat stat){stat_ = stat;}
            inline OutputCommand& outputCommand(){return outputCommand_;}
            inline const OutputCommand& outputCommand()const{return outputCommand_;}
            inline OutputCommand& rawCommand(){return rawCommand_;}
            inline const OutputCommand& rawCommand()const{return rawCommand_;}
            inline void setOutputCommand(OutputCommand outputCommand){outputCommand_ = outputCommand;}
            inline void setRawCommand(OutputCommand rawCommmand){rawCommand_ = rawCommmand;}

            //inline QString command()const{return _savedStat.command();}
            //inline const QString& commandRef()const{return _savedStat.command();}
            //inline void setUserIteractionRequired(){_userInteraction = UserInteraction::Required;}
            //inline void setStat(Stat stat){_savedStat.setStat(stat);}
            //inline void setCommand(OutputCommand command){_savedStat.setCommand(command);}
            inline void appendCommand(OutputCommand command){ rawCommand_.append(command); outputCommand_.append(command);}
            inline void appendCommand(OutputCommand command, OutputCommand rawCommand){ rawCommand_.append(rawCommand); outputCommand_.append(command);}
            //inline UserInteraction userIteraction(){return _userInteraction;}
            //inline SavedStat& savedStat(){return _savedStat;}
            //inline const Parameters& rawParameters()const{return rawParameterStats;}

            QString toString(Definition::Format::Target);
            inline void clearMemory(){
                using Parameter = Parameters::Iterator;
//                _savedStat.clearMemory();
//                for(Parameter param = rawParameterStats.begin() ; param < rawParameterStats.end(); param++)
//                    param->clearMemory();
            }

            bool isEmpty()const
            {
                return stat() == Stat::Word and rawCommand_.isEmpty();
            }


        };
        // End of Concept Definition ||||||||||||||||||||||||||||||||||
        // Objects ----------------------------------------------------
        protected:
            Parameters _parameters;
            ProcedureDefinitionIterator _procedureDefinition;
            QString outputCommand_;
            QString rawCommand_;
            Stat stat_;
            ParameterIndex currentParameterIndex = -1;
            //Name _name;
            //Parameters _rawParameters;
            //UserInteraction _userInteraction;

        // End of Objects ||||||||||||||||||||||||||||||||||||||||||||
        // Functions --------------------------------------------------

        // End of Functions |||||||||||||||||||||||||||||||||||||||||||
        // Interface -------------------------------------------------
        private:

        public:

            Call(Stat);
            Call(Stat, Parameter&, ProcedureDefinitionIterator = nullptr, ParameterIndex = -1);
            //Call& operator=(Call&& call){_name = call._name; _arguments = call._arguments;}

            inline Stat stat()const{return stat_;}
            inline void changeStat(const Stat stat){stat_ = stat;}
            inline OutputCommand& outputCommand(){return outputCommand_;}
            inline void setOutputCommand(OutputCommand outputCommand){outputCommand_ = outputCommand;}
            inline void setRawCommand(OutputCommand rawCommmand){rawCommand_ = rawCommmand;}
            inline OutputCommand& rawCommand(){return rawCommand_;}
            inline ProcedureDefinitionIterator definition()const{return _procedureDefinition;}
            inline void setDefinition(ProcedureDefinitionIterator definition){_procedureDefinition = definition;}

            //Error createCall(Stat, Call::Parameter&&);
            inline Error newParameter(OutputCommand command = OutputCommand()){
                return newParameter(Stat::Word, command, command);
            }
            Error newParameter(Stat stat,QString rawParameter, OutputCommand outputCommand = QString());

            Error replaceLastParameter(Stat stat,QString rawParameter, OutputCommand outputCommand){
                _parameters.replace(_parameters.count() - 1, Parameter(stat, outputCommand, rawParameter));
                return Error::NoError;
            }
            inline Parameters::size_type rawParametersLength()const{return _parameters.size();}

            // -----------------------------
            //nline void nextArgument(Parameter& arg){_parameters.append(arg); _parameters.append(arg);}
            inline Parameters::size_type lastArgumentIndex()const{return parametersLength() - 1;}
            inline Parameters::size_type parametersLength()const{return (currentParameterIndex != -1)? currentParameterIndex + 1 : rawParametersLength() - 1;}
            inline QString _name(){return parameters().at(0).outputCommand();}
            //inline QString name(){return parameters().at(0).outputCommand();}
            //inline QString rawName(){return parameters().at(0).rawCommand();}
            inline Definition::RulesForArguments::Iterator lastRulesForArgument_dynamicCheck()const{
                Parameters::size_type lastIndex = lastArgumentIndex();
                for(Definition::RulesForArguments::Iterator iter = definition()->rulesForArguments.begin();
                    iter < definition()->rulesForArguments.end(); iter++)
                {
                    if(iter->index == lastIndex)
                        return iter;
                }
                return definition()->rulesForArguments.end();
                }
            inline Definition::RulesForArguments::Iterator lastRulesForArgument_onMoved()const{
                return lastRulesForArgument_dynamicCheck();}
            inline Definition::RulesForArguments::Iterator rulesForUnspecifiedArgument()const{
                return &definition()->rulesForUnspecifiedArgument;}
            inline Error isRulesInRange(Definition::RulesForArguments::Iterator rules)const{
                return (rules < definition()->rulesForArguments.end())?
                            Error::NoError :
                            Error::Error;}
            inline Definition::RulesOnEndOfCall& rulesOnEndOfCall()const{
                return definition()->rulesOnEndOfProcedureCall;
            }

            inline bool isRulesForArgumentsEmpty()const{return definition()->isRulesForArgumentsEmpty();}
            inline bool isRulesEmpty()const{return definition()->isRulesEmpty();}
            inline Call::Parameters::reference lastParameter(){return _parameters.last();}
            inline Parameters& parameters(){return _parameters;}
            //inline QString generateFunctionDefinitionExample(){return QString("// ") + name() + "(" + generatFunctionDefinitionExampleParameters() + ")";}
            inline QString generateFunctionDefinitionExample(){return QString("") + _name() + " " + generateFunctionDefinitionExampleParameters() + "";}
            inline QString generateFunctionDefinitionExampleParameters(){
                QString str;
                for(Parameters::Iterator parameter = parameters().begin() + 1; parameter < _parameters.end(); parameter++)
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
            inline Parameters&  rawParameters(){return _parameters;}
            inline bool isUserInteractionRequired()const{return /*_userInteraction == UserInteraction::Required;*/ definition()->userInteraction == UserInteractionStatus::Required;}
            inline void clearMemory(){
                using Parameter = Parameters::Iterator;
                for(Parameter param = _parameters.begin(); param < _parameters.end(); param++)
                    param->clearMemory();
            }


            bool isLastParameterEmpty() // If no parameters return true
            {
                return rawParametersLength() and /* if 0 (impossible case), returns false (no parameters to check)*/
                        rawParameters().last().isEmpty();
            }


            // End of Interface |||||||||||||||||||||||||||||||||||||||||||||
    };

    namespace Types {
        using Calls = QVector<Call>;
    };
};
#endif // COMMANDCALL_HPP
