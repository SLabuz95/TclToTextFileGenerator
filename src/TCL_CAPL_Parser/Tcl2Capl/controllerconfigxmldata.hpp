#ifndef Tcl2CaplControllerConfigXmlDataHPP
#define Tcl2CaplControllerConfigXmlDataHPP

#include"FileReader/FilesSpecificData/FSDTemplate.hpp"
#include"Tcl2Capl/controllerconfig.hpp"
//#include"tclToCAPL.hpp"

class Tcl2CaplControllerConfigXmlData : public FSD_DataModelBase<Tcl2CaplControllerConfigXmlData>{
public:
    //using TclProcedureInterpreter = TCLInterpreter::TCLProceduresInterpreter;
    using OutputConfig = Tcl2CaplControllerConfig;
    using Settings = OutputConfig::Settings;
    using WriteOnlyProcedures = Settings::WriteOnlyProcedures;
    using UserProceduresConfig = OutputConfig::Procedures;
    using UserDefaultProcedureConfig = UserProceduresConfig::Type;
    using Predefinitions = OutputConfig::Predefinitions;

protected:
    UserProceduresConfig userProcedureConfig_;
    UserDefaultProcedureConfig userDefaultProcedureConfig_;
    Settings proceduresSettings_;
    //WriteOnlyProcedures writeOnlyProcedures_;
    Predefinitions predefinitions_;

public:
    UserProceduresConfig& userProcedureConfig(){return userProcedureConfig_;}
    UserDefaultProcedureConfig& userDefaultProcedureConfig(){return userDefaultProcedureConfig_;}
    WriteOnlyProcedures& writeOnlyProcedures(){return proceduresSettings().writeOnlyProcedures();}
    // For Compatibility
    Settings& proceduresSettings(){return proceduresSettings_;}
    //ProceduresSettings& proceduresSettings(){return proceduresSettings_;}
    Predefinitions& predefinitions(){return predefinitions_;}
};

#endif // Tcl2CaplControllerConfigXmlDataHPP
