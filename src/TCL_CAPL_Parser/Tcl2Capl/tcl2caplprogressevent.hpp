#ifndef TCL2CAPLPROGRESSEVENT_HPP
#define TCL2CAPLPROGRESSEVENT_HPP

#include<QEvent>
#include<QString>

class Tcl2CaplProgressEvent : public QEvent{

public:
    using ErrorsNumber = const uint;
    using CriticalError = const QString;
    Tcl2CaplProgressEvent(CriticalError criticalError) : QEvent(eventType_static()), criticalError_(criticalError) {}   // End of File Event
    Tcl2CaplProgressEvent(ErrorsNumber errorsNumber) : QEvent(eventType_static()), numbOfErrors_(errorsNumber) {}   // End of File Event

    static constexpr QEvent::Type eventType_static(){return static_cast<QEvent::Type>(2000);}
    static Tcl2CaplProgressEvent* cast(QEvent* ev){
        return (ev->type() == eventType_static())?
                    static_cast<Tcl2CaplProgressEvent*>(ev)
                  :
                    nullptr;
    }
    ErrorsNumber getErrorsNumber()const{return numbOfErrors_;}
    bool isCriticalError()const{return not criticalError_.isEmpty();}
    CriticalError criticalError()const{return criticalError_;}

protected:
    CriticalError criticalError_;
    ErrorsNumber numbOfErrors_ = 0;


};

#endif // TCL2CAPLPROGRESSEVENT_HPP
