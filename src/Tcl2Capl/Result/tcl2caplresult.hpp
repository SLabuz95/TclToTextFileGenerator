#ifndef TCL2CAPLRESULT_HPP
#define TCL2CAPLRESULT_HPP

#include<QDir>
//#include"TclInterpreter/tclToCAPL.hpp"
//#include"Tcl2Capl/controller.hpp"
//#include"Tcl2Capl/caplFunctionDefiniitions.hpp"


class Tcl2CaplResult{
    // Concept Definition

    class Script{
        QString _content;

    public:
        Script(QString& content) : _content(content){}
        inline QString& content(){return _content;}
    };
    // ====
public:
    using Error = QString;
    using ScriptFile = QFile;


private:

    // ------------------

    // Properties -------------
    Error _error;
    ScriptFile _scriptFile;
    Script _script;
    // ------------------------

    // Functions --------------
    inline Error throwError(Error error){return _error = error;}

    // -----------------------

    // Interface -------------
public:
    Tcl2CaplResult(ScriptFile& scriptFile, Script script)
        : _script(script)
    {
        _scriptFile.setFileName(scriptFile.fileName());
    }

    inline bool isError()const{return !_error.isEmpty();}
    inline Error error()const{return _error;}

    Error writeResult();
};

#endif // TCL2CAPLRESULT_HPP
