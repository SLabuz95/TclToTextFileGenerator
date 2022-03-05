#ifndef TCLINTERPRETERPRIV_HPP
#define TCLINTERPRETERPRIV_HPP

#include"External/RegExpCore/regexprcore.hpp"
#include"External/Improvements/class.hpp"
#include"External/StartUpError/startuperror.hpp"
#include<QFile>

class TCLInterpreterPriv{
public:
    using TclCommand = QString;
    using Command = QString;
    using Commands = QStringList;
    enum class Error : bool{Error, NoError};
    using Preexpressions = QStringList;

    // Stat flags:
    // - Parent - parent can be saved in SavedStats stack
    // - Child - child can be saved in another SavedStat
    // - Processing - owns method for interpretation of event for that stat
    // - Prodromic - expects the occurance of specific next event, can be saved in SavedStats stack

    enum class Stat : uint{
        None,
        Word,             // {Child, Proccessing} //Old Name: String // For definition, Check Tcl concept in Github References
        //Const,              // SAVEABLE   // Check const or string is only in special cases - const and string distinction do not change interpreter behaviour
        //Operator,           // SAVEABLE // In Development
        VariableSubbing,           // {Child}
        //UnknownString, Word state substitute that state
        CommandSubbing,       // {Parent, Child} //  Old name - FunctionCall
        CommandSubbingStart,   // {Processing, Prodromic} [, SAVABLE
        CommandSubbingEnd,  // {Processing} ],
        Braces,   // {    /// {Parent, Child, Processing} Everything is a string // SAVEABLE
        BracesEnd,  // } {Processing}   // SAVEABLE (For complete list)
        DoubleQuotes, // "  {Parent, Child, Processing}  // SAVEABLE
        MainScript,  // {Parent} SAVEABLE // Previous name: MainCodeBlock
        Whitespace, // " " , "\t" {Parent, Processing}   // SAVEABLE (Special Cases)
        VariableSubbingStart, // $ {Processing, Prodromic} // SAVEABLE
        Namespace,   // :: {Processing} // SAVEABLE
        Semicolon, // ; {Processing}
        Comment,    // # {Processing} Save to Pending Stats (if tclCommand ends with \)   // check   add EndOfString Stat to processing stats
        EndOfString,    // {Processing} Deinitialize saved stats
        Script,  // {Parent, Child, Processing} SAVEABLE // Old Name: CodeBLock
        //EndOfCodeBlock,  // SAVEABLE
        //Snprintf,   //  SAVEABLE For case of procedure call for speech mark processing -> procedure call [string concat ...]
        BackslashSubbing, // {Processing, Prodromic} // Old Name: SpecialSign
        //PendingString,  // SAVEABLE
        //PendingSnprintf,    // SAVEABLE
        //StringInQuotes, // SAVEABLE
        Ignore, // {Parent, Prodromic} SAVEABLE
        Expression, // {Parent, Child} SAVEABLE
        //EndOfExpression, // SAVEABLE
        Size
    };

    class ListStatInfo{ // Special case for List stat (too avoid unnecessary virtualisations and dynamic memory management for every single stat)

        class ListInfo{
        public:
            ListInfo(QVector<ListInfo> listInfos = {}, QString strData = QString())
                : listInfos(listInfos), strData(strData){}

            QVector<ListInfo> listInfos;
            QString strData;

            QString toString(const QString&);
            QString toCaplListString();
        };

        enum class EndListResult : bool{
            NoEndOfList,
            EndOfList,
        };

    public:
        using ListInfos = QVector<ListInfo>;
        using EndListResult = EndListResult;
    protected:
        ListInfos listInfos;
        bool closeListEventOccured = false;

    public:
        using ListInfo = ListInfo;

        ListStatInfo(){

        }

        void appendCommand(Command command);
        void newList(Command command);
        EndListResult endList(Command command);
        ListInfos::Iterator lastListInfo = nullptr;
        ListInfos& getListInfos(){return listInfos;}

        QString toString(const QString&);
        QString toCaplListString();
    };

    class SavedStatPriv{
    protected:
        Stat pStat = Stat::None;

        SavedStatPriv(){}
        SavedStatPriv(Stat stat) : pStat(stat){}

    public:

        void setStat(Stat stat){pStat = stat;}
        Stat stat()const{return pStat;}

    };


    class SavedStat : public SavedStatPriv{

    protected:
        ListStatInfo* info = nullptr;
        Command _command;
        bool functionReady = false;

        void appendCommmand_listProcessing(Command command);
    public:
        SavedStat() : SavedStatPriv(){}
        SavedStat(Stat stat) : SavedStatPriv(stat){}
        SavedStat(Stat stat, Command command) : SavedStatPriv(stat), _command(command){}

        ~SavedStat(){

        }

        void clearMemory(){
            if(info){
                delete info;
                info = nullptr;
            }
        }

        void setCommand(Command command){
            _command = command;
        }
        void appendCommand(Command command){
            _command.append(command);
        }

        void hideInfoBeforeCopyConstructor(){info = nullptr;}
        inline ListStatInfo* advancedList()const{return info;}

        const Command& command()const{return _command;}

        void newListInfo(){
            if(!info)
                info = new ListStatInfo();
            /*if(not (command().isEmpty())){
                if((command().endsWith(" ") or command().endsWith("\t"))){
                    info->newList(_command);
                    _command.clear();
                }else{
                    _command += "{";
                }
            }else{*/
                info->newList(_command);
                _command.clear();
            //}
        }

        inline ListStatInfo::EndListResult endList(){
            ListStatInfo::EndListResult result = ListStatInfo::EndListResult::EndOfList;
            if(info){
                switch((result = info->endList(command()))){
                case ListStatInfo::EndListResult::EndOfList:
                    //_command = info->toCaplString(", ");
                    _command.clear();
                    break;
                default:
                    _command.clear();
                    break;
                }

            }
            return result;
        }

        QString listToCaplString();
        QString listToRawString(){return QString("{") + ((info)? info->toString("") : _command) + "}";}
        QString listToTclListString(){return ((info)? info->toString("") : _command);}
        inline bool isAdvancedList(){return info;}
        inline void setFunctionReady(){functionReady = true;}
        inline bool isFunctionReady(){return functionReady;}

    };


    using SavedStats = QVector<SavedStat>;
    class UserInteraction{
        class Info{
            using Position = QString::size_type;
            Position startIndex;
            Position endIndex;
        };
        using Infos = QVector<Info>;
        Infos infos;
    public:
        enum Status : quint8{None, NotRequired, Required};
        static constexpr Status defaultStatus(){return Status::NotRequired;}
        inline Infos::size_type nextUserInteractionNumber(){return infos.size() + 1;}
        inline void addNewInfo(){infos.append(Info());}
        inline void clearInfos(){infos.clear() ;}
    };

    class SnprintfController{
        //QStringList snprintfDefinitions;
        uint numbOfDefinitions = 1;
    public:
        QString getNextSnprintfVarName(){
            QString snVarName = QString("_snStr") + QString::number(numbOfDefinitions++);
            return snVarName;}
        void clear(){numbOfDefinitions = 1;}
    };

    class Variables{
        struct VariableInfo{
            using Type = QString;
            using VarName = QString;
            using ArrayRanks = QList<uint>;
            using Value = QString;
            Type type;
            ArrayRanks arrayRanks;
            Value value;
        };
        using Type = VariableInfo::Type;
        using VarName = VariableInfo::VarName;
        using ArrayRanks = VariableInfo::ArrayRanks;
        using Value = VariableInfo::Value;
        using Variables_priv = QMap<VarName, VariableInfo>;
        Variables_priv variables;

    public:
        using Variable = VariableInfo;
        /*void insertVariable(Type type, VarName name, Value value, ArrayRanks arrayRanks= {0}){
            Variables_priv::Iterator variable = variables.find(name);
            //TODO: Dodowanie zmiennych - jezeli juz istnieje i jesli nie zmienia sie typ lub arrayRanks, to poprostu dodaje
            // Jezeli sie zmienia
            //if()
        }*/

    };

    class PredefinitionsControl{
        using Variable_priv = Variables::Variable;
        using Predefinitions_priv = QList<Variable_priv>;
        using Type = Variable_priv::Type;
        using VarName = Variable_priv::VarName;
        using ArrayRanks_priv = Variable_priv::ArrayRanks;
        using VariableNames = QStringList;
        using GroupPredefinitionPositions = QVector<Predefinitions_priv::size_type>;
        using GroupVariableNamePositions = QVector<VariableNames::size_type>;
        using Group = SavedStats::size_type;
        using Groups = QVector<Group>;

        Predefinitions_priv predefinitions;
        VariableNames variableNames;
        Groups groups;
        GroupPredefinitionPositions groupPredefinitionPositions;
        GroupVariableNamePositions groupVariableNamePositions;

        static const QString stringOfArrayRanks(const ArrayRanks_priv& arrayRanks){
            QString retStr;
            if(not (arrayRanks.size() == 1 and arrayRanks.at(0) == 0)){
                for(ArrayRanks_priv::ConstIterator arrayRank = arrayRanks.constBegin();
                    arrayRank < arrayRanks.constEnd(); arrayRank++)
                {
                    retStr += QString("[") + QString::number(*arrayRank) + "]";
                }
            }
            return retStr;
        }

    public:
        using Predefinitions = Predefinitions_priv;
        using Variable = Variable_priv;
        using ArrayRanks = ArrayRanks_priv;
        inline void newVariable(Type type, VarName varName, ArrayRanks arrayRanks = {0}){
            predefinitions.append(Variable{type, arrayRanks, varName});
        }

        inline bool findVariable(Variable& variableRef, Variable::VarName varName){
            using Variable = Predefinitions::reverse_iterator;
            for(Variable variable = predefinitions.rbegin(); variable != predefinitions.rend(); variable++){
                if(variable->value == varName){
                    variableRef = *variable;
                    return true;
                }
            }
            return false;
        }
        inline bool findVariableGlobally(Predefinitions& predefinitions, Variable& variableRef, Variable::VarName varName){
            using Variable = Predefinitions::reverse_iterator;
            for(Variable variable = predefinitions.rbegin(); variable != predefinitions.rend(); variable++){
                if(variable->value == varName){
                    variableRef = *variable;
                    return true;
                }
            }
            return false;
        }

        /*inline void newVariableDistinct(Type type, VarName varName, ArrayRanks arrayRanks = {0}){
            if(not variableNames.contains(varName)){
                    predefinitions.append(Variable{type, arrayRanks, varName});
                    variableNames.append(varName);
            }else{

            }
        }*/
        inline void newGroup(Group newGroup){
            if(!groups.endsWith(newGroup))
            {
                groups.append(newGroup);
                groupPredefinitionPositions.append(predefinitions.size());
                groupVariableNamePositions.append(variableNames.size());
            }
        }

        inline QString getPredefinitionsGroupStr(){
            QString str;
            if(not groups.isEmpty()){
                Predefinitions tempPredefinitions = predefinitions.mid(groupPredefinitionPositions.last());
                //str = ((tempPredefinitions.isEmpty())? "" : "\n") + tempPredefinitions.join(";\n") + ((tempPredefinitions.isEmpty())? "" : ";\n");
                if(not tempPredefinitions.isEmpty()){
                    str = "\n";
                }
                for(Predefinitions::Iterator iter = tempPredefinitions.begin(); iter < tempPredefinitions.end(); iter++){
                    str+=iter->type + " " + iter->value + stringOfArrayRanks(iter->arrayRanks) + ";\n";
                }

                predefinitions = predefinitions.mid(0, groupPredefinitionPositions.takeLast());

                variableNames = variableNames.mid(0, groupVariableNamePositions.takeLast());

                groups.removeLast();
            }
            return str;
        }
        inline QString getPredefinitionsStr(){
            QString str;
            if(groups.isEmpty()){
                for(Predefinitions::Iterator iter = predefinitions.begin(); iter < predefinitions.end(); iter++){
                    str+=iter->type + " " + iter->value + stringOfArrayRanks(iter->arrayRanks) + ";\n";
                }
            }
            return str;
        }

        inline Predefinitions& getPredefinitions(){return predefinitions;}

    };

private:
    UserInteraction _userInteraction;
    Preexpressions _preexpressions;
    SnprintfController _snprintfController;
    PredefinitionsControl _predefinitionsControl;
    Command command;
    SavedStats _savedStats{{Stat::MainScript}};

protected:
    static QString _error;
    inline static Error throwError(const QString& str =  "TCL Interpreter: Unspecified Error\n"){return (_error = str).isEmpty()? Error::NoError : Error::Error;}
    inline bool isStringConstNumber(QString str)const{return isStringFloatNumber(str);}
    inline bool isStringFloatNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForFloatsNoComma)) == 0;}
    inline bool isStringIntNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForInt)) == 0;}
    inline bool isStringUIntNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForUint)) == 0;}

    using AddExpressionToCodeBlockFunction = void (TCLInterpreterPriv::*)(Commands, QString);
public:
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
                                                               }*/
    inline void addExpressionToMainCodeBlock(Commands commands, QString postfix = QString()){
        if(commands.size() > 0){
            command.append(commands.join("\n") + postfix);
        }
    }
    /*inline void addExpressionToCodeBlock(Command command){command.isEmpty() or
                                                                If not empty  ( lastSavedStat().setCommand( lastSavedStat().command() +  command + "\n"), true);
                                                               }*/
    inline void addExpressionToCodeBlock(Commands commands, QString postfix = QString()){
        (this->*addExpressionToCodeBlockFunction)(commands, postfix);
    }

    inline void addExpressionToCodeBlock_standard(Commands commands, QString postfix){
        if(commands.size() > 0)
            lastSavedStat().appendCommand(commands.join("\n") + postfix);
    }

    inline void addExpressionToCodeBlock_writeOnlyProcedure(Commands commands, QString postfix){
        addExpressionToMainCodeBlock(commands, postfix);
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


};
#endif // TCLINTERPRETERPRIV_HPP
