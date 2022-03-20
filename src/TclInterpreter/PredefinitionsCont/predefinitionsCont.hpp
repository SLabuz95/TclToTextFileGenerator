#ifndef PREDEFINITIONSCONT_HPP
#define PREDEFINITIONSCONT_HPP

#include"TclInterpreter/tclinterpretercore.hpp"
#include"TclInterpreter/VariableCont/variableCont.hpp"
#include"TclInterpreter/SavedStatCont/savedStatCont.hpp"

namespace Tcl::Interpreter{

    class PredefinitionsController{
        using Variable_priv = VariablesController::Variable;
        using Predefinitions_priv = QList<Variable_priv>;
        using Type = Variable_priv::Type;
        using VarName = Variable_priv::VarName;
        using ArrayRanks_priv = Variable_priv::ArrayRanks;
        using VariableNames = QStringList;
        using GroupPredefinitionPositions = QVector<Predefinitions_priv::size_type>;
        using GroupVariableNamePositions = QVector<VariableNames::size_type>;
        using Group = Types::SavedStats::size_type;
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

};

#endif // PREDEFINITIONSCONT_HPP
