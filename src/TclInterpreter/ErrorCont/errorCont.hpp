#ifndef ERRORCONT_HPP
#define ERRORCONT_HPP

#include<QString>
#include"TclInterpreter/tclinterpretercore.hpp"
namespace Tcl::Interpreter{
    using namespace Tcl::Interpreter::Core;

    class ErrorController{
        static QString _error;
    public:
        class ErrorMessage{
        public:
            ErrorMessage(QString message, QString fullLineData, QString restOfLineData)
                : message(message), fullLineData(fullLineData), restOfLineData(restOfLineData){}
        private:
            QString message;
            QString fullLineData;
            QString restOfLineData;
        public:
            QString toString(){return "<b>Message:</b> " + message + "<br>\n<b>Line:</b> " + fullLineData + "<br>\n<b>Rest of Line:</b> " + restOfLineData;}
        };

        inline static Error throwError(const QString& str =  "TCL Interpreter: Unspecified Error\n"){return (_error = str).isEmpty()? Error::NoError : Error::Error;}
        inline static bool isError(){return not _error.isEmpty();}
        inline static void clearError(){_error.clear();}
        inline static const QString& error(){return _error;}

    };

};
// ----

#endif // ERRORCONT_HPP
