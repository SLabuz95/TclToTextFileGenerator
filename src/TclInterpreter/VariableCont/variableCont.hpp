#ifndef VARIABLECONT_HPP
#define VARIABLECONT_HPP

#include"TclInterpreter/tclinterpretercore.hpp"
#include<QString>
#include<QMap>
/*
class VariablesController{
    // Concept --------------
    using Name = QString;
    using SetValue = QString;
    class Value{
    public:
        using InitValue = QString;
        using CurrentValue = QString;
    protected:
        InitValue _initValue;
        CurrentValue _currentValue;
    public:
        Value(InitValue initValue) : _initValue(initValue){}
        InitValue initValue(){return _initValue;}
        CurrentValue currentValue(){return (_currentValue.isEmpty())? _initValue : _currentValue;}
        void setValue(CurrentValue currentValue){_currentValue = currentValue;}
    };
    using VariablesPriv = QMap<Name, Value>;
    VariablesPriv variables;
    // ----------------------
    // Interface
 public:
    void setValue(Name name, SetValue setValue){
        VariablesPriv::Iterator variable = variables.find(name);
        if(variable == variables.end()){    // Not Found
            variables.insert(name, Value(setValue));
        }else{
            variable->setValue(setValue);
        }
    }
    // End of Interface
};*/
namespace Tcl::Interpreter{
    class VariablesController{

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

};

#endif // VARIABLECONT_HPP
