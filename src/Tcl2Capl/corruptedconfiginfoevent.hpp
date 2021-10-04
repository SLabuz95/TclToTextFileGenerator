#ifndef CORRUPTEDCONFIGINFOEVENT_HPP
#define CORRUPTEDCONFIGINFOEVENT_HPP

#include<QEvent>
#include<QString>

class CorruptedConfigInfoEvent : public QEvent{

public:
    CorruptedConfigInfoEvent() : QEvent(eventType_static()){}   // End of File Event

    static constexpr QEvent::Type eventType_static(){return static_cast<QEvent::Type>(2001);}
    static CorruptedConfigInfoEvent* cast(QEvent* ev){
        return (ev->type() == eventType_static())?
                    static_cast<CorruptedConfigInfoEvent*>(ev)
                  :
                    nullptr;
    }

};

#endif // CORRUPTEDCONFIGINFOEVENT_HPP
