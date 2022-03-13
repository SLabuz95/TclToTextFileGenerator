#ifndef TCLINTERPRETERPRIV_HPP
#define TCLINTERPRETERPRIV_HPP

#include"External/RegExpCore/regexprcore.hpp"
#include"External/Improvements/class.hpp"
#include"External/StartUpError/startuperror.hpp"
#include<QFile>
/*
class TCLInterpreterPriv{
public:
    using TclCommand = QString;
    using Command = QString;
    using Commands = QStringList;
    enum class Error : bool{Error, NoError};
    using Preexpressions = QStringList;

private:
//    UserInteraction _userInteraction;
    Preexpressions _preexpressions;
    SnprintfController _snprintfController;
    PredefinitionsController _predefinitionsController;
    Command command;
    SavedStats _savedStats{{Stat::MainScript}};

protected:
    static QString _error;
    inline bool isStringConstNumber(QString str)const{return isStringFloatNumber(str);}
    inline bool isStringFloatNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForFloatsNoComma)) == 0;}
    inline bool isStringIntNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForInt)) == 0;}
    inline bool isStringUIntNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForUint)) == 0;}

    using AddExpressionToCodeBlockFunction = void (TCLInterpreterPriv::*)(Commands, QString);
public:    
    inline static Error throwError(const QString& str =  "TCL Interpreter: Unspecified Error\n"){return (_error = str).isEmpty()? Error::NoError : Error::Error;}

    AddExpressionToCodeBlockFunction addExpressionToCodeBlockFunction = &TCLInterpreterPriv::addExpressionToCodeBlock_standard;
    inline void addPreexpressionsToMainCodeBlock(){
        if(not preexpressions().isEmpty()){ // Not Empty
            addExpressionToMainCodeBlock(preexpressions(), "\n");
            //lastSavedStat().appendCommand("\n");
            preexpressions().clear();
        }
    }


    inline void addPreexpressionsToCodeBlock(){
        if(not preexpressions().isEmpty()){ // Not Empty
            //lastSavedStat().appendCommand("\n");
            addExpressionToCodeBlock(preexpressions(), "\n");
            preexpressions().clear();
        }
    }

    /*inline void addExpressionToMainCodeBlock(Command command){command.isEmpty() or
                                                                If not empty (command.append(((!command.isEmpty())? QString(";\n") : QString()) + command ), true);
                                                               }
    inline void addExpressionToMainCodeBlock(Commands commands, QString postfix = QString()){
        if(commands.size() > 0){
            command.append(commands.join("\n") + postfix);
        }
    }
    /*inline void addExpressionToCodeBlock(Command command){command.isEmpty() or
                                                                If not empty  ( lastSavedStat().setCommand( lastSavedStat().command() +  command + "\n"), true);
                                                               }


    inline Command& readCommand(){return command;}
    inline void saveStat(SavedStat newStat){_savedStats.append(newStat);}
    Error removeLastStat(){return (_savedStats.isEmpty())? throwError("removeLastStat: No elements to remove")
                                                                   : (_savedStats.removeLast(), Error::NoError);
    }
    inline SavedStat& lastSavedStat(){return _savedStats.last();}
    inline SavedStat takeLastSavedStat(){return _savedStats.takeLast();}
    inline SavedStats& savedStats(){return _savedStats;}
    inline SavedStats::size_type savedStatsSize()const{return _savedStats.size();}
    inline bool isSavedStatsEmpty()const{return _savedStats.isEmpty();}
    inline SavedStats::ConstIterator savedStatsBegin()const{return _savedStats.begin();}
    inline SavedStats::ConstIterator savedStatsEnd()const{return _savedStats.end();}
    inline UserInteraction& userInteraction(){return _userInteraction;}
    inline Preexpressions& preexpressions(){return _preexpressions;}
    inline SnprintfController& snprintfController(){return _snprintfController;}
    inline PredefinitionsControl& predefinitionsControl(){return _predefinitionsControl;}
    inline void prepareCodeBlockContent(){
        QString predefinitions = predefinitionsControl().getPredefinitionsGroupStr();
        if(not predefinitions.isEmpty())
            lastSavedStat().setCommand(predefinitions + lastSavedStat().command());
    }


};*/
#endif // TCLINTERPRETERPRIV_HPP
