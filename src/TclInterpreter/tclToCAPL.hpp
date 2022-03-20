#ifndef TCLTOCAPL_HPP
#define TCLTOCAPL_HPP

#include"tclinterpretercore.hpp"
#include<QtGlobal>
#include<QXmlStreamWriter>
#include"External/RegExpCore/regexprcore.hpp"
#include"External/Improvements/class.hpp"
#include"External/StartUpError/startuperror.hpp"
#include<QFile>
#include"CommandsCont/commandsCont.hpp"
#include"KeywordsCont/keywordsCont.hpp"
#include"PredefinitionsCont/predefinitionsCont.hpp"
#include"UserInteractionCont/userInteractionCont.hpp"
#include"ErrorCont/errorCont.hpp"
#include"SnprintfCont/snprintfCont.hpp"
#include"Tcl2Capl/caplFunctionDefiniitions.hpp"
//#include"Tcl2Capl/caplFunctionDefiniitions.hpp


class UserInputConfig;

namespace Tcl{
    using namespace Analysis;
    using namespace Interpreter;
    class TCLInterpreter{

    private:
        // Concept ----------------------------------------------------------------------
        using Preexpressions = QStringList;
        using CommandsController = Command::Controller;
        // ----
        using InterpretFunction = Error (TCLInterpreter::*)();
        // ----
        // Aliasses
        inline static Error throwError(){return ErrorController::throwError();}
        inline static Error throwError(const QString str){return ErrorController::throwError(str);}
        inline static void clearError(){ErrorController::clearError();}
    public:
        inline static bool isError(){return ErrorController::isError();}
        inline static const QString& error(){return ErrorController::error();}
    private:
        //
        using ErrorMessage = ErrorController::ErrorMessage;
        using ErrorMessages = QVector<ErrorMessage>;
        // ----

        using AddExpressionToCodeBlockFunction = void (TCLInterpreter::*)(OutputCommands, QString);
        AddExpressionToCodeBlockFunction addExpressionToCodeBlockFunction = &TCLInterpreter::addExpressionToCodeBlock_standard;
        using RemoveProcedureCallFunction = Error (TCLInterpreter::*)();

        inline Preexpressions& preexpressions(){return _preexpressions;}

        inline void saveStat(SavedStat newStat){_savedStats.append(newStat);}

        inline SavedStat& lastSavedStat(){return _savedStats.last();}
        inline SavedStat takeLastSavedStat(){return _savedStats.takeLast();}
        inline SavedStats& savedStats(){return _savedStats;}
        inline SavedStats::size_type savedStatsSize()const{return _savedStats.size();}
        inline bool isSavedStatsEmpty()const{return _savedStats.isEmpty();}
        inline SavedStats::ConstIterator savedStatsBegin()const{return _savedStats.begin();}
        inline SavedStats::ConstIterator savedStatsEnd()const{return _savedStats.end();}
        Error removeLastStat(){return (_savedStats.isEmpty())? throwError("removeLastStat: No elements to remove")
                                                                       : (_savedStats.removeLast(), Error::NoError);
        }

        inline void prepareCodeBlockContent(){
            QString predefinitions = predefinitionsController.getPredefinitionsGroupStr();
            if(not predefinitions.isEmpty())
                lastSavedStat().setCommand(predefinitions + lastSavedStat().command());
        }

        inline void addPreexpressionsToCodeBlock(){
            if(not preexpressions().isEmpty()){ // Not Empty
                //lastSavedStat().appendCommand("\n");
                addExpressionToCodeBlock(preexpressions(), "\n");
                preexpressions().clear();
            }
        }

        /*inline void addExpressionToMainCodeBlock(OutputCommand command){command.isEmpty() or
                                                                    If not empty (command.append(((!command.isEmpty())? QString(";\n") : QString()) + command ), true);
                                                                   }*/
        inline void addExpressionToMainCodeBlock(OutputCommands commands, QString postfix = QString()){
            if(commands.size() > 0){
                command.append(commands.join("\n") + postfix);
            }
        }
        /*inline void addExpressionToCodeBlock(OutputCommand command){command.isEmpty() or
                                                                    If not empty  ( lastSavedStat().setCommand( lastSavedStat().command() +  command + "\n"), true);
                                                                   }*/

        inline void addExpressionToCodeBlock(OutputCommands commands, QString postfix = QString()){
            (this->*addExpressionToCodeBlockFunction)(commands, postfix);
        }

        inline void addExpressionToCodeBlock_standard(OutputCommands commands, QString postfix){
            if(commands.size() > 0)
                lastSavedStat().appendCommand(commands.join("\n") + postfix);
        }

        inline void addExpressionToCodeBlock_writeOnlyProcedure(OutputCommands commands, QString postfix){
            addExpressionToMainCodeBlock(commands, postfix);
        }

        inline void addPreexpressionsToMainCodeBlock(){
            if(not preexpressions().isEmpty()){ // Not Empty
                addExpressionToMainCodeBlock(preexpressions(), "\n");
                //lastSavedStat().appendCommand("\n");
                preexpressions().clear();
            }
        }
        inline QString readCurrentKeyword(){return (proccessingStats.isEmpty())? textInterpreter.readLastKeyword() : textInterpreter.readUnknownString();}
        inline bool isCurrentKeywordProcessing()const{return proccessingStats.isEmpty();}
        inline bool isUnknownStringProcessing()const{return not isCurrentKeywordProcessing();}
    public:
        using InterpretFunctions = InterpretFunction[static_cast<std::underlying_type_t<Stat>>(Stat::Size)];
        // End Of Concept ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        // Concept Definition ------------------------------------------------------------------

        inline bool isStringConstNumber(QString str)const{return isStringFloatNumber(str);}
        inline bool isStringFloatNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForFloatsNoComma)) == 0;}
        inline bool isStringIntNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForInt)) == 0;}
        inline bool isStringUIntNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForUint)) == 0;}

        // End of Concept Definition
        // Objects ----------------------------------------------------
        protected:
            FunctionDefinitionsRef functionDefinitions;
            //Variables variables;
            // Controllers

            KeywordsController textInterpreter;
            CommandsController commandsController;
            PredefinitionsController predefinitionsController;
            SnprintfController snprintfController;
            UserInteraction userInteraction;
            OutputCommand command;
            SavedStats _savedStats{{Stat::MainScript}};
            Preexpressions _preexpressions;

            UserInputConfig& userConfig;

            Stats proccessingStats{};
            Stats pendingProccessingStats{}; // Saved stats to use in next initialization

            static InterpretFunctions interpretFunctions;
            ErrorMessages ignoreMessages;
            RemoveProcedureCallFunction removeProcedureCallFunction = &TCLInterpreter::removeProcedureCall_standard;
            bool _whitespace = false;

        // End of Objects ||||||||||||||||||||||||||||||||||||||||||||
        // Functions --------------------------------------------------

            template<Stat stat>
            Error interpret();

            inline Error callInterpretFunction(Stat stat = Stat::Size){
                if(checkWhitespace() == Error::NoError){
                    if(isSavedStatsEmpty())
                        return throwError("Empty Stats after Whitespace");

                }else{
                    return Error::Error;
                }
                if(stat != Stat::Size){
                    proccessingStats.append(stat);
                }
                return (this->*(interpretFunctions[static_cast<std::underlying_type_t<Stat>>(proccessingStats.takeLast())]))();
            }

            inline Error processUnknownString(){
               return (textInterpreter.isUnknownString())? callInterpretFunction(Stat::Word) : Error::NoError;
            }

            inline void addPendingProcessingStat(Stats stats){pendingProccessingStats.append(stats);}
            inline void addPendingProcessingStat(Stat stat){pendingProccessingStats.append(stat);}
            inline Error checkWhitespace(){
                return ((_whitespace = (lastSavedStat().stat() == Stat::Whitespace)))?
                            removeLastStat() :
                            Error::NoError;
            }
            inline bool isWhitespaceOccured()const{return _whitespace;}
            //Error interpretFunctionCall(qsizetype functionCallPos);

            Error moveArgumentToFunctionCall();
            Error moveArgumentToSnprintf_priv(const Stat);
        // End of Functions |||||||||||||||||||||||||||||||||||||||||||
            inline bool isLastProcedureCallExpr(){return commandsController.numberOfProcedureCalls() > 0 and commandsController.lastProcedureName() == "expr";}

            inline Error moveArgumentToSnprintf(){/*return moveArgumentToSnprintf_priv(Stat::Snprintf);*/}
            inline Error moveArgumentToPendingSnprintf(){/*return moveArgumentToSnprintf_priv(Stat::PendingSnprintf);*/}

            inline bool isPrelastSavedStat()const{return savedStatsBegin() <= savedStatsEnd() - 2;}
            inline SavedStat& prelastSavedStat(){return *(savedStats().end() - 2);}

            void addIgnoreMessage(ErrorMessage message){ignoreMessages.append(message);}
            Error removeIgnore(){return (isSavedStatsEmpty())?
                                throwError("TCLInterpreter::removeIgnore: No stats") : (removeLastStat(), Error::NoError);}



            Error newProcedureCall(Interpreter::Command::Call::Name name);
            inline Error removeProcedureCall(){return (this->*removeProcedureCallFunction)();}
            inline Error removeProcedureCall_standard(){
                const QString ERROR_PREFIX = "TCLInterpreter::removeProcedureCall: ";
                if(commandsController.numberOfProcedureCalls() == 0)
                    return throwError(ERROR_PREFIX + "No procedure calls");

                if(commandsController.removeProcedureCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                return Error::NoError;
            }
            Error removeProcedureCall_writeOnlyProcedure();
            Error finalizeProcedureCall();
            Error finalizeSnprintfCall();

            Error processError(){addIgnoreMessage(ErrorMessage(error(), textInterpreter.readTclCommand(), textInterpreter.restOfString()));
                                 if(processSavedStatsForError() == Error::Error)
                                     return Error::Error;
                                 clearError();
                                return Error::NoError;}
            Error processSavedStatsForError();
            Error saveStatWithParsingControl(SavedStat);
            Error removeLastSavedStatWithParsingControl();
            Error takeLastSavedStatWithParsingControl(SavedStat&);

            void addEmptyLine();
        // Interface -------------------------------------------------
        public:
            TCLInterpreter(UserInputConfig& userConfig, FunctionDefinitionsRef caplFunctionDefinitions);

            inline bool isComplete()const{return savedStatsSize() == 1;}

            Error toCAPL(TclCommand&);
            //Error addReadyCommand(TclCommand&);
            inline void activateWriteOnlyProcedureMode(){
                removeProcedureCallFunction = &TCLInterpreter::removeProcedureCall_writeOnlyProcedure;
                addExpressionToCodeBlockFunction = &TCLInterpreter::addExpressionToCodeBlock_writeOnlyProcedure;
            }

            inline void deactivateWriteOnlyProcedureMode(){
                removeProcedureCallFunction = &TCLInterpreter::removeProcedureCall_standard;
                addExpressionToCodeBlockFunction = &TCLInterpreter::addExpressionToCodeBlock_standard;

            }

            static bool checkInterpretFunctions();
            inline bool anyErrors(){return ignoreMessages.size();}
            void printErrorReport(QFile& reportFile, QString inputFileName);
            void printErrorReport(QString& inputFileName);
            OutputCommand& readCommand(){return command;}
            inline OutputCommand printPredefinitions(){return predefinitionsController.getPredefinitionsStr();}

            static inline std::underlying_type_t<Stat> cast_stat(const Stat t){return static_cast<std::underlying_type_t<Stat>>(t);}
            static inline QString cast_stat_str(const Stat t){return QString::number(cast_stat(t));}
            inline ErrorMessages::size_type getErrorsNumber()const{return ignoreMessages.size();}
            inline PredefinitionsController::Predefinitions& predefinitions(){return predefinitionsController.getPredefinitions();}

            bool isPredefinitionMode();
        // End of Interface |||||||||||||||||||||||||||||||||||||||||||||

         // Interpret Functions


        // ----------------------------

        friend class Command::Controller;
        friend class Command::Call::Parameter;
    };

};



#endif // TCLTOCAPL_HPP
