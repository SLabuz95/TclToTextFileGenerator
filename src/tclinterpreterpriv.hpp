#ifndef TCLINTERPRETERPRIV_HPP
#define TCLINTERPRETERPRIV_HPP

#include"External/RegExpCore/regexprcore.hpp"
#include"External/Improvements/class.hpp"
#include"External/StartUpError/startuperror.hpp"
#include<QFile>

class TCLInterpreterPriv{
public:
    using TclCommand = QString;
    using CAPLCommand = QString;
    using CAPLCommands = QStringList;
    enum class Error : bool{Error, NoError};
    using Preexpressions = QStringList;

    enum class Stat : uint{
        None,
        String,             // SAVEABLE
        Const,              // SAVEABLE
        Operator,           // SAVEABLE
        Variable,           // SAVEABLE
        UnknownString,
        FunctionCall,       // SAVEABLE
        LeftSquareBracket,   // [, SAVABLE
        RightSquareBracket,  // ],
        List,   // {    /// Everythink is a string // SAVEABLE
        EndOfList,  // }    // SAVEABLE (For complete list)
        SpeechMark, // "    // SAVEABLE
        MainCodeBlock,  // SAVEABLE
        Whitespace, // " " , "\t"   // SAVEABLE (Special Cases)
        VariableAccess, // $ // SAVEABLE
        GlobalAccess,   // :: // SAVEABLE
        Semicolon, // ;
        Comment,    // # Save to Pending Stats (if tclCommand ends with \)   // check   add EndOfString Stat to processing stats
        EndOfString,    // Deinitialize saved stats
        CodeBlock,  // SAVEABLE
        EndOfCodeBlock,  // SAVEABLE
        Snprintf,   // SAVEABLE For case of procedure call for speech mark processing -> procedure call [string concat ...]
        SpecialSign,
        PendingString,  // SAVEABLE
        PendingSnprintf,    // SAVEABLE
        StringInQuotes, // SAVEABLE
        Ignore, // SAVEABLE
        Expression, //SAVEABLE
        EndOfExpression, // SAVEABLE
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

        void appendCAPLCommand(CAPLCommand caplCommand);
        void newList(CAPLCommand caplcommand);
        EndListResult endList(CAPLCommand caplcommand);
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
        CAPLCommand _caplCommand;
        bool functionReady = false;
        ListStatInfo* info = nullptr;

        void appendCAPLCommmand_listProcessing(CAPLCommand caplCommand);
    public:
        SavedStat() : SavedStatPriv(){}
        SavedStat(Stat stat) : SavedStatPriv(stat){}
        SavedStat(Stat stat, CAPLCommand caplCommand) : SavedStatPriv(stat), _caplCommand(caplCommand){}

        ~SavedStat(){

        }

        void clearMemory(){
            if(info){
                delete info;
                info = nullptr;
            }
        }

        void setCAPLCommand(CAPLCommand caplCommand){
            _caplCommand = caplCommand;
        }
        void appendCAPLCommand(CAPLCommand caplCommand){
            _caplCommand.append(caplCommand);
        }

        void hideInfoBeforeCopyConstructor(){info = nullptr;}
        inline ListStatInfo* advancedList()const{return info;}

        const CAPLCommand& caplCommand()const{return _caplCommand;}

        void newListInfo(){
            if(!info)
                info = new ListStatInfo();
            /*if(not (caplCommand().isEmpty())){
                if((caplCommand().endsWith(" ") or caplCommand().endsWith("\t"))){
                    info->newList(_caplCommand);
                    _caplCommand.clear();
                }else{
                    _caplCommand += "{";
                }
            }else{*/
                info->newList(_caplCommand);
                _caplCommand.clear();
            //}
        }

        inline ListStatInfo::EndListResult endList(){
            ListStatInfo::EndListResult result = ListStatInfo::EndListResult::EndOfList;
            if(info){
                switch((result = info->endList(caplCommand()))){
                case ListStatInfo::EndListResult::EndOfList:
                    //_caplCommand = info->toCaplString(", ");
                    _caplCommand.clear();
                    break;
                default:
                    _caplCommand.clear();
                    break;
                }

            }
            return result;
        }

        QString listToCaplString();
        QString listToRawString(){return QString("{") + ((info)? info->toString("") : _caplCommand) + "}";}
        QString listToTclListString(){return ((info)? info->toString("") : _caplCommand);}
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
    CAPLCommand caplCommand;
    SavedStats _savedStats{{Stat::MainCodeBlock}};

protected:
    static QString _error;
    inline static Error throwError(const QString& str =  "TCL Interpreter: Unspecified Error\n"){return (_error = str).isEmpty()? Error::NoError : Error::Error;}
    inline bool isStringConstNumber(QString str)const{return isStringFloatNumber(str);}
    inline bool isStringFloatNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForFloatsNoComma)) == 0;}
    inline bool isStringIntNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForInt)) == 0;}
    inline bool isStringUIntNumber(QString str)const{return str.lastIndexOf(QRegularExpression(RegExpCore::regExpForUint)) == 0;}

    using AddExpressionToCodeBlockFunction = void (TCLInterpreterPriv::*)(CAPLCommands);
public:
    AddExpressionToCodeBlockFunction addExpressionToCodeBlockFunction = &TCLInterpreterPriv::addExpressionToCodeBlock_standard;
    inline void addPreexpressionsToMainCodeBlock(){
        if(!preexpressions().isEmpty()){ // Not Empty
            addExpressionToMainCodeBlock(preexpressions());
            //lastSavedStat().appendCAPLCommand("\n");
            preexpressions().clear();
        }
    }


    inline void addPreexpressionsToCodeBlock(){
        if(!preexpressions().isEmpty()){ // Not Empty
            //lastSavedStat().appendCAPLCommand("\n");
            addExpressionToCodeBlock(preexpressions());
            preexpressions().clear();
        }
    }

    /*inline void addExpressionToMainCodeBlock(CAPLCommand command){command.isEmpty() or
                                                                If not empty (caplCommand.append(((!caplCommand.isEmpty())? QString(";\n") : QString()) + command ), true);
                                                               }*/
    inline void addExpressionToMainCodeBlock(CAPLCommands commands){
        if(commands.size() > 0){
            caplCommand.append(((caplCommand.size() > 0)? QString("\n") : QString()) + commands.join("\n"));
        }
    }
    /*inline void addExpressionToCodeBlock(CAPLCommand command){command.isEmpty() or
                                                                If not empty  ( lastSavedStat().setCAPLCommand( lastSavedStat().caplCommand() +  command + "\n"), true);
                                                               }*/
    inline void addExpressionToCodeBlock(CAPLCommands commands){
        (this->*addExpressionToCodeBlockFunction)(commands);
    }

    inline void addExpressionToCodeBlock_standard(CAPLCommands commands){
        if(commands.size() > 0)
            lastSavedStat().appendCAPLCommand(((lastSavedStat().caplCommand().size() > 0)? QString("\n") : QString()) +/*QString(";\n") +*/ commands.join("\n"));
    }

    inline void addExpressionToCodeBlock_writeOnlyProcedure(CAPLCommands commands){
        addExpressionToMainCodeBlock(commands);
    }

    inline CAPLCommand& readCaplCommand(){return caplCommand;}
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
        if(!predefinitions.isEmpty())
            lastSavedStat().setCAPLCommand(predefinitions + lastSavedStat().caplCommand());
    }


};
#endif // TCLINTERPRETERPRIV_HPP
