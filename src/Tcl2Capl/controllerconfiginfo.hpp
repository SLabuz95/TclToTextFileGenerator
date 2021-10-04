#ifndef CONTROLLERCONFIGINFO_HPP
#define CONTROLLERCONFIGINFO_HPP

#include<QString>
#include<QDateTime>
#include"Tcl2Capl/controllerconfig.hpp"
#include<QFileInfo>


class ControllerConfigInfo{
public:
    using ConfigPath = QString;
    using RegisterFileModificationTime = QDateTime;
    using ControllerConfigRef = Tcl2CaplControllerConfig&;

protected:
    ConfigPath configPath;
    RegisterFileModificationTime modificationTime;
    Tcl2CaplControllerConfig _controllerConfig;

public:
    ControllerConfigInfo(){}
    ControllerConfigInfo(ConfigPath, RegisterFileModificationTime, Tcl2CaplControllerConfig);

    using Self = ControllerConfigInfo;
    using SelfPtr = ControllerConfigInfo*;
    using SelfRef = ControllerConfigInfo&;

    inline bool operator==(QString rhs){return this->configPath == rhs;}
    inline ControllerConfigRef controllerConfig(){return _controllerConfig;}
    inline void updateConfig(Tcl2CaplControllerConfig& cnf){_controllerConfig = cnf;}
    inline void updateModificationTime(){modificationTime = QFileInfo(configPath).lastModified();}
    inline bool isFileUpdated(){return QFileInfo(configPath).lastModified() > modificationTime;}
    inline bool isFileConfiguration(){return not configPath.isEmpty();}
    inline bool isTemporaryConfiguration(){return configPath.isEmpty();}
    inline ConfigPath path(){return configPath;}
    inline bool isDefaultConfig(){
        return _controllerConfig.isDefault();
    }
};

#endif // CONTROLLERCONFIGINFO_HPP
