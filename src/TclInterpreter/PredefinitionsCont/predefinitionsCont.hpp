#ifndef PREDEFINITIONSCONT_HPP
#define PREDEFINITIONSCONT_HPP

#include"TclInterpreter/tclinterpretercore.hpp"
#include"TclInterpreter/VariableCont/variableCont.hpp"
#include"TclInterpreter/SavedStatCont/savedStatCont.hpp"

namespace Tcl::Interpreter{

    class PredefinitionsController{
    public:
        enum class Scope : int{
            CurrentBlock,
            MainScript,
            Global,
            NewBlock,
            Size,
            None = -1
        };
        static const QList<QString> scopeMap;
        inline static Scope fromStr(QString& str){return static_cast<Scope>(scopeMap.indexOf(QRegularExpression(str, QRegularExpression::CaseInsensitiveOption)));}
         inline static QString toStr(Scope type){return scopeMap.at(std::underlying_type_t<Scope>(type));}
         inline static const decltype(scopeMap)& scopeNames(){return scopeMap;}
         static inline std::underlying_type_t<Scope> cast_scope(const Scope t){return static_cast<std::underlying_type_t<Scope>>(t);}
         static inline Scope castTo_scope(const std::underlying_type_t<Scope> t){return static_cast<Scope>(t);}
    private:
//        using Variable_priv = VariableController::Variable;
         using Variable_priv = QString;
        using Predefinitions_priv = QList<Variable_priv>;
//        using Type = Variable_priv::Type;
//        using VarName = Variable_priv::VarName;
//        using ArrayRanks_priv = Variable_priv::ArrayRanks;
        using VariableNames = QStringList;
        using GroupPredefinitionPositions = QVector<Predefinitions_priv::size_type>;
        using GroupVariableNamePositions = QVector<VariableNames::size_type>;
        using Group = Calls::size_type;
        using Groups = QVector<Group>;

        Predefinitions_priv predefinitions; // Group related
        Predefinitions_priv globalPredefinitions;
        Predefinitions_priv mainScriptPredefinitions;
        Predefinitions_priv newScriptPredefinitions;


        VariableNames variableNames;
        Groups groups;
        GroupPredefinitionPositions groupPredefinitionPositions;
        GroupVariableNamePositions groupVariableNamePositions;

//        static const QString stringOfArrayRanks(const ArrayRanks_priv& arrayRanks){
//            QString retStr;
//            if(not (arrayRanks.size() == 1 and arrayRanks.at(0) == 0)){
//                for(ArrayRanks_priv::ConstIterator arrayRank = arrayRanks.constBegin();
//                    arrayRank < arrayRanks.constEnd(); arrayRank++)
//                {
//                    retStr += QString("[") + QString::number(*arrayRank) + "]";
//                }
//            }
//            return retStr;
//        }

    public:
        using Predefinitions = Predefinitions_priv;
        using Variable = Variable_priv;
//        using ArrayRanks = ArrayRanks_priv;
//        inline void newVariable(Type type, VarName varName, ArrayRanks arrayRanks = {0}){
//            predefinitions.append(Variable{type, arrayRanks, varName});
//        }

        void newVariable(Scope scope, QString predefinition){
            switch(scope){
            case Scope::Global:
                globalPredefinitions.append(predefinition);
                break;
            case Scope::MainScript:
                mainScriptPredefinitions.append(predefinition);
                break;
            case Scope::NewBlock:
                newScriptPredefinitions.append(predefinition);
                break;
            case Scope::CurrentBlock:
                predefinitions.append(predefinition);
                break;
            default:
                break;
            }
        }

//        int findPredefinition(const Predefinitions::Type& predefinition){ // Predefinitions of group

//        }

//        void findPredefinition(Predefinitions& predefinitions, const Predefinitions::Type& predefinition){
//            // Check Name of predefinition
//            QString variableName;
//            qsizetype startIndex = predefinition.indexOf(QRegularExpression("^( \\w+[\\s]*\\[\\d+\\])|( \\w+)[\\s]*(;[\\s]*)|(=.*)$"));
//            if(startIndex != -1){
//                qsizetype endIndex = predefinitions.indexOf(QRegularExpression("[[=;]"), startIndex);
//                if(endIndex != -1){ // End of variable?

//                }else{

//                }
//            }
//        }

//        inline bool findVariable(Variable& variableRef, Variable::VarName varName){
//            using Variable = Predefinitions::reverse_iterator;
//            for(Variable variable = predefinitions.rbegin(); variable != predefinitions.rend(); variable++){
//                if(variable->value == varName){
//                    variableRef = *variable;
//                    return true;
//                }
//            }
//            return false;
//        }
//        inline bool findVariableGlobally(Predefinitions& predefinitions, Variable& variableRef, Variable::VarName varName){
//            using Variable = Predefinitions::reverse_iterator;
//            for(Variable variable = predefinitions.rbegin(); variable != predefinitions.rend(); variable++){
//                if(variable->value == varName){
//                    variableRef = *variable;
//                    return true;
//                }
//            }
//            return false;
//        }

        /*inline void newVariableDistinct(Type type, VarName varName, ArrayRanks arrayRanks = {0}){
            if(not variableNames.contains(varName)){
                    predefinitions.append(Variable{type, arrayRanks, varName});
                    variableNames.append(varName);
            }else{

            }
        }*/
        inline void newGroup(Group newGroup){
            if( not groups.endsWith(newGroup))
            {
                groups.append(newGroup);
                groupPredefinitionPositions.append(predefinitions.size());
                groupVariableNamePositions.append(variableNames.size());
            }
        }

//        inline QString getPredefinitionsGroupStr(Scope scope){
//            QString str;
//            if(not groups.isEmpty()){
//                Predefinitions tempPredefinitions = predefinitions.mid(groupPredefinitionPositions.last());
//                //str = ((tempPredefinitions.isEmpty())? "" : "\n") + tempPredefinitions.join(";\n") + ((tempPredefinitions.isEmpty())? "" : ";\n");
//                if(not tempPredefinitions.isEmpty()){
//                    str = "\n";
//                }
//                for(Predefinitions::Iterator iter = tempPredefinitions.begin(); iter < tempPredefinitions.end(); iter++){
////                    str+=iter->type + " " + iter->value + stringOfArrayRanks(iter->arrayRanks) + ";\n";
//                }

//                predefinitions = predefinitions.mid(0, groupPredefinitionPositions.takeLast());

//                variableNames = variableNames.mid(0, groupVariableNamePositions.takeLast());

//                groups.removeLast();
//            }
//            return str;
//        }
        inline QString getPredefinitionsStr(){
            QString str;
            if(groups.isEmpty()){
                for(Predefinitions::Iterator iter = predefinitions.begin(); iter < predefinitions.end(); iter++){
//                    str+=iter->type + " " + iter->value + stringOfArrayRanks(iter->arrayRanks) + ";\n";
                }
            }
            return str;
        }

        inline Predefinitions& getPredefinitions(){return predefinitions;}

    };

};

#endif // PREDEFINITIONSCONT_HPP
