#ifndef INTERPRETERINTERFACE_HPP
#define INTERPRETERINTERFACE_HPP

class InterpreterInterface {
public:
    enum class Type{
        FileInterpreter,
        TextInterpreter
    };

    virtual constexpr Type type()const = 0;
};

#endif // INTERPRETERINTERFACE_HPP
