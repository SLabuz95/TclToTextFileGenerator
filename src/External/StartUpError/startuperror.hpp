#ifndef STARTUPERROR_HPP
#define STARTUPERROR_HPP

#include<QtGlobal>
#include<QString>

class StartUpErrorBase{
public:
    using Message = QString;

};

template<class T>
class StartUpError : public StartUpErrorBase{ // Abstract for any class with runtime initialization (for example static arrays (limitations of c++)). If error occur, it will not work anyway
public:
    using StartUpErrorBase::Message;
    static Message __startupErrorChecking();
};

template<typename ...startupClass>
typename StartUpErrorBase::Message startupErrorChecking(){
    StartUpErrorBase::Message error;
    (... && (error = StartUpError<startupClass>::__startupErrorChecking()).isEmpty());
    return error;
}
#endif // STARTUPERROR_HPP
