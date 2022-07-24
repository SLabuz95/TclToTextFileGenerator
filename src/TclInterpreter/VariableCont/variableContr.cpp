#include"variableCont.hpp"
#include<QString>
#include<QRegularExpression>

using namespace Tcl::Interpreter;

void VariableController::processVariableSubbingParameter(Call::Parameter& parameter, QString& str)
{
    const QRegularExpression regex = QRegularExpression("(?<!:):{1}(?!:)|[^a-zA-Z0-9_:]");
    const qsizetype firstNotAlphanumericCharacter = str.indexOf(regex); // Alpha + sign '_'

    if(firstNotAlphanumericCharacter == -1){
        // Character not found -> Add whole string to parameter
        // Clear str argument
        parameter.appendCommand(str);
        str.clear();
    }else{
        // Chop off left site of string and add to parameter
        // Right site assign to str argument (output string)
        parameter.appendCommand(str.first(firstNotAlphanumericCharacter));
        str = str.right(str.size() - firstNotAlphanumericCharacter);
    }
}

bool VariableController::isFirstSignOk(QString str){
    const QRegularExpression regex = QRegularExpression("(?<!:):{1}(?!:)|[^a-zA-Z0-9_:]");
    const qsizetype firstNotAlphanumericCharacter = str.indexOf(regex); // Alpha + sign '_'

    return firstNotAlphanumericCharacter != 0;
}


QString VariableController::prepareTclVariableNameFromStr(const QString& str){
    QString procedureName = str;
    procedureName.replace(QRegularExpression(":{2,}"), QString("::"));
    if(procedureName.startsWith("::"))
        procedureName.remove(0, 2); // Remove global namespace
    return procedureName;
}
