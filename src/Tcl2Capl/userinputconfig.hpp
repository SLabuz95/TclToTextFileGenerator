#ifndef USERINPUTCONFIG_HPP
#define USERINPUTCONFIG_HPP
//#include"Result/tcl2caplresult.hpp"
#include<QDir>
//#include"controllerconfig.hpp"
#include"controllerconfiginfo.hpp"
//#include"Tcl2Capl/caplFunctionDefiniitions.hpp"
#include<QThread>

class UserInputConfig{
public:
    using TclProcedureInterpreter = Tcl::Interpreter::Command::Controller;
    using Settings = Tcl::Interpreter::Command::Settings;
    using WriteOnlyProcedures = Settings::WriteOnlyProcedures;
    using Attributes = ControllerConfigInfo::Attributes;
    using Phases = TcFileModifierConfigBase::ModifierPhases;
    using UserProcedure = Tcl::Interpreter::Command::Definition;
    using UserProcedures = Tcl::Interpreter::Command::CommandDefinitions;
    using UserDefaultProcedure = UserProcedure;
    using Predefinitions = Tcl::Interpreter::PredefinitionsController::Predefinitions;

    using UserInputConfigData = ControllerConfigInfo;
    UserInputConfig(){}
    UserInputConfig(UserInputConfigData& configData);
    ~UserInputConfig(){}
protected:
    Settings settings_;
    Attributes attributes_;
    Phases modifierPhases_;
    UserProcedures userProcedures_;
    UserDefaultProcedure userDefaultProcedure_;
    Predefinitions _predefinitions;

public:
    inline Settings& proceduresSettings(){return settings_;}
    inline Attributes& attributes(){return attributes_;}
    inline Phases& modifierPhases(){return modifierPhases_;}
    inline UserProcedures& userProcedureConfig(){return userProcedures_;}
    inline UserDefaultProcedure& userDefaultProcedureConfig(){return userDefaultProcedure_;}
    inline Predefinitions& predefinitions(){return _predefinitions;}
};
#endif // USERINPUTCONFIG_HPP
