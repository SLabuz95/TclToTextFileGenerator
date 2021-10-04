#include"controllerconfiginfo.hpp"

ControllerConfigInfo::ControllerConfigInfo(ConfigPath path, RegisterFileModificationTime modificationTime, Tcl2CaplControllerConfig config)
    : configPath(path), modificationTime(modificationTime), _controllerConfig(config)
{

}
