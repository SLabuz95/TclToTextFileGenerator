#include"predefinitionsCont.hpp"

using namespace Tcl::Interpreter;

const QList<QString> PredefinitionsController::scopeMap{
    QString("CurrentBlock"),
    QString("MainScript"),
    QString("Global"),
    QString("NewBlock"),
};
