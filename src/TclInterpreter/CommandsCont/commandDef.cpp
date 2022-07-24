#include"commandDef.hpp"

using namespace Tcl::Interpreter::Command;

QString Definition::prepareTclProcedureNameFromStr(const QString& str){
    QString procedureName = str;
    procedureName.replace(QRegularExpression(":{2,}"), QString("::"));
    if(procedureName.startsWith("::"))
        procedureName.remove(0, 2); // Remove global namespace
    return procedureName;
}
