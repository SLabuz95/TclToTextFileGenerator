#include"listsCont.hpp"
#include"TclInterpreter/tclToCAPL.hpp"

using namespace Tcl;

ListController::ListController(TCLInterpreter& tclInterpreter) :
    tclInterpreter(tclInterpreter)
{

}


Error ListController::throwError(){return tclInterpreter.throwError();}

Error ListController::throwError(const QString str){
    return tclInterpreter.throwError(str);
}
