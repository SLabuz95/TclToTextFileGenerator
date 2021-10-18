#include"controllerconfigmanager.hpp"
#include"External/FileReader/FilesSpecificData/XML/TclCaplParserConfig/FRI_FSD_XML_TCL_CAPL_Parser_Config.hpp"
#include<QApplication>
#include"corruptedconfiginfoevent.hpp"

using InfoPtr = ControllerConfigManager::InfoPtr;

bool ControllerConfigManager::readConfig(QFileInfo& file, Info::ControllerConfigRef configRef){
    Tcl2CaplControllerConfigXmlData tempUserDefinitionsData;
    FSD_XML_TclCaplParserConfigInterpreter::Config parserConfigInterpreter(tempUserDefinitionsData);
    QStringList blackList;
    QString filePath;
    if( not(filePath = parserConfigInterpreter.readFileByFilePath(file.filePath(), QStringList())).isEmpty()){
        // Check for interpreter errors
        if(parserConfigInterpreter.isError()){   // if error true
            qDebug() << parserConfigInterpreter.error();
            return false;
        }else{
           // if Data Model != nullptr, then file has been read
            if(parserConfigInterpreter.getOnTheBlackList()){
                // BlackList PlaceHoldered
            }else{
                //xmlTestModulesByPath.insert(tempTestModule, filePath);
                qDebug() << "Config Read Success: " << filePath;
                configRef = Tcl2CaplControllerConfig(tempUserDefinitionsData);
                //infosBySubsribers.insert(returnInfo = new Info(configPath, fileInfo.lastModified(), Tcl2CaplControllerConfig(tempUserDefinitionsData)), {subsriber});
            }
        }
    }
    return true;
}
/*QString ControllerConfigManager::readRawConfig(QFileInfo file, Info::ControllerConfigRef config){
    Tcl2CaplControllerConfigXmlData tempUserDefinitionsData;
    FSD_XML_TclCaplParserConfigInterpreter::Config parserConfigInterpreter(tempUserDefinitionsData);
    QStringList blackList;
    QString filePath;
    if( not(filePath = parserConfigInterpreter.readFileByFilePath(file.filePath(), QStringList())).isEmpty()){
        // Check for interpreter errors
        if(parserConfigInterpreter.isError()){   // if error true
            qDebug() << parserConfigInterpreter.error();
            return parserConfigInterpreter.error();
        }else{
           // if Data Model != nullptr, then file has been read
            if(parserConfigInterpreter.getOnTheBlackList()){
                // BlackList PlaceHoldered
            }else{
                //xmlTestModulesByPath.insert(tempTestModule, filePath);
                qDebug() << "Config Read Success: " << filePath;
                config = Tcl2CaplControllerConfig(tempUserDefinitionsData);
                //infosBySubsribers.insert(returnInfo = new Info(configPath, fileInfo.lastModified(), Tcl2CaplControllerConfig(tempUserDefinitionsData)), {subsriber});
            }
        }
    }
    return QString();
}*/

InfoPtr ControllerConfigManager::loadConfig(EventHandlerPtr subsriber, Info::ConfigPath configPath){
    Q_ASSERT_X(subsriber != nullptr, "ControllerConfigManager::loadConfig", "Internal error: Subscriber is nullptr");
    using InfoIter = InfosBySubsribers::key_value_iterator;
    //WARNING: Any register of the same configPath but with new modification time will UPDATE current config for the config path.
    //WARNING: For empty configPath, config is temporary and depends of subsriber (stand-alone or InstructionsInterpreter ConfigEditor only)

    if(configPath.isEmpty()){
        // Load Temporary Config
        SubsribersByInfos::iterator temporaryInfo = temporaryConfigInfos.find(subsriber);
        Q_ASSERT_X(subsriber != nullptr, "ControllerConfigManager::loadConfig", "Internal error: Subscriber already exists");
        temporaryInfo = temporaryConfigInfos.insert(subsriber, new Info());
        return temporaryInfo.value();
    }else{
        // Register config from file
        for(InfoIter info = infosBySubsribers.keyValueBegin(); info != infosBySubsribers.keyValueEnd(); info++)
        {
            if(*(*info).first == configPath){     // ConfigPath value equality
                if((*info).first->isFileUpdated()){
                    //return changeConfig((*info).first, configPath);
                }
                (*info).second.append(subsriber);
                return (*info).first;
            }
        }
        InfoPtr returnInfo = nullptr;
        QFileInfo configFile;
        Tcl2CaplControllerConfig config;
        if(readConfig(configFile, config) == true)
            infosBySubsribers.insert(returnInfo = new Info(configPath, configFile.lastModified(), config), {subsriber});

        return returnInfo;
    }
    return nullptr;
}

bool ControllerConfigManager::unloadConfig(InfoPtr infoF, ControllerConfigManager::EventHandlerPtr subsriber){
    int numbOfRemovedSubscribers = 0;
    _lastErrorMsg = "Internal Error: Unsubsribe - ConfigInfo not found";
    //WARNING: Any register of the same configPath but with new modification time will UPDATE current config for the config path.

    // Check is it temporary config
    if(infoF->path().isEmpty()){
        // Temporary info
        SubsribersByInfos::iterator info = temporaryConfigInfos.find(subsriber);
        if(info != temporaryConfigInfos.end()){    // Found
            temporaryConfigInfos.erase(info);
            numbOfRemovedSubscribers = 1;
        }else{
            _lastErrorMsg = "Internal Error: Temporary Config Unsubscribe - NumbOfSubsribers doesnt match. Expected: 1. Removed: " + QString::number(numbOfRemovedSubscribers);
        } // Not Found
    }else{
        // Config from file
        InfosBySubsribers::iterator info = infosBySubsribers.find(infoF);
        if(info != infosBySubsribers.end()){    // Found
            if((numbOfRemovedSubscribers = info.value().removeAll(subsriber)) == 1){
                _lastErrorMsg = "";
                // If no subsribers, remove config info
                if(info.value().isEmpty())
                    infosBySubsribers.erase(info);
            }else{
                _lastErrorMsg = "Internal Error: File Config Unsubscribe - NumbOfSubsribers doesnt match. Expected: 1. Removed: " + QString::number(numbOfRemovedSubscribers);
            }
        } else {} // Not Found
    }

    return numbOfRemovedSubscribers == 1;
}

InfoPtr ControllerConfigManager::changeConfig(InfoPtr infoF, Info::ConfigPath configPath){
    using InfoIter = InfosBySubsribers::key_value_iterator;
     for(InfoIter info = infosBySubsribers.keyValueBegin(); info != infosBySubsribers.keyValueEnd(); info++)
     {
         if((*info).first == infoF){ // Reference Equality
             InfoPtr returnInfo = nullptr;
             QFileInfo configFile;
             Tcl2CaplControllerConfig config;
             if(readConfig(configFile, config) == true)
                 infoF->updateConfig(config);
          }
     }
     return  (_lastErrorMsg = "Internal Error", nullptr);
}

InfoPtr ControllerConfigManager::registerConfig(EventHandlerPtr subsriber, Info::ConfigPath configPath){
    /*using InfoIter = InfosBySubsribers::key_value_iterator;
    //WARNING: Any register of the same configPath but with new modification time will UPDATE current config for the config path.
    //WARNING: For empty configPath, config is temporary and depends of subsriber (stand-alone or InstructionsInterpreter ConfigEditor only)

    if(configPath.isEmpty()){
        // Load Temporary Config
        SubsribersByInfos::iterator temporaryInfo = temporaryConfigInfos.find(subsriber);
        if(temporaryInfo != temporaryConfigInfos.end()){    // Info Has been found
            // Delete old pointer
            delete temporaryInfo.value();
            // Create new pointer
            temporaryInfo.value() = new Info();
        }else{
            temporaryInfo = temporaryConfigInfos.insert(subsriber, new Info());
        }
        return temporaryInfo.value();
    }else{
        // Register config from file
        for(InfoIter info = infosBySubsribers.keyValueBegin(); info != infosBySubsribers.keyValueEnd(); info++)
        {
            if(*(*info).first == configPath){     // ConfigPath value equality
                if((*info).first->isFileUpdated()){
                    return reregisterConfig((*info).first, configPath);
                }
                (*info).second.append(subsriber);
                return (*info).first;
            }
        }
        InfoPtr returnInfo = nullptr;
        QFileInfo configFile;
        Tcl2CaplControllerConfig config;
        if(readConfig(configFile, config) == true)
            infosBySubsribers.insert(returnInfo = new Info(configPath, configFile.lastModified(), config), {subsriber});

        return returnInfo;
    }*/
    return nullptr;
}


InfoPtr ControllerConfigManager::reregisterConfig(InfoPtr infoF, Info::ConfigPath configPath){
   /*using InfoIter = InfosBySubsribers::key_value_iterator;
    for(InfoIter info = infosBySubsribers.keyValueBegin(); info != infosBySubsribers.keyValueEnd(); info++)
    {
        if((*info).first == infoF){ // Reference Equality
            InfoPtr returnInfo = nullptr;
            QFileInfo configFile;
            Tcl2CaplControllerConfig config;
            if(readConfig(configFile, config) == true)
                infoF->updateConfig(config);
         }
    }*/
    return  (_lastErrorMsg = "Internal Error", nullptr);
}

bool ControllerConfigManager::saveConfigOnly(InfoPtr infoPtr){
    QFile file(infoPtr->path());

    _lastErrorMsg = QString();

    if(file.open(QIODevice::Text | QIODevice::WriteOnly)){
        file.write(infoPtr->controllerConfig().toXmlContent().toUtf8());
    }else{
        _lastErrorMsg = QString("Open File Error: ") + file.fileName();
    }

    file.close();
    return _lastErrorMsg.length() == 0;
}

InfoPtr ControllerConfigManager::saveConfigAs(InfoPtr infoPtr, Info::ConfigPath& path){
    // Save file
    bool errorOccured = false;
    QFile file(infoPtr->path());

    _lastErrorMsg = QString();

    if(file.open(QIODevice::Text | QIODevice::WriteOnly)){
        file.write(infoPtr->controllerConfig().toXmlContent().toUtf8());
    }else{
        _lastErrorMsg = QString("Open File Error: ") + file.fileName();
    }

    file.close();

    // Update error state
    errorOccured = (_lastErrorMsg.length() != 0);

    if(not errorOccured){    // No Error
        // Reregister and assign to infoPtr
        // Dont read

    }


    return (errorOccured == true)? nullptr : infoPtr;

}

InfoPtr ControllerConfigManager::saveConfig(InfoPtr infoPtr, Info::ConfigPath path){
    Q_ASSERT_X(isInfoExist(infoPtr), "ControllerConfigManager::saveConfig", "Internal error: ConfingInfo is not registered");

    if(infoPtr->isTemporaryConfiguration() or infoPtr->path() != path){
        infoPtr = saveConfigAs(infoPtr, path);
    }else{
        if(saveConfigOnly(infoPtr) == false){   // Error
            infoPtr = nullptr; // Return null to indicate an error
        }else{}// Return the same info ptr
    }

    return infoPtr;
}

bool ControllerConfigManager::unregisterConfig(InfoPtr infoF, EventHandlerPtr subsriber){
    int numbOfRemovedSubscribers = 0;
    _lastErrorMsg = "Internal Error: Unsubsribe - ConfigInfo not found";
    //WARNING: Any register of the same configPath but with new modification time will UPDATE current config for the config path.

    // Check is it temporary config

    if(infoF->path().isEmpty()){
        // Temporary info
        SubsribersByInfos::iterator info = temporaryConfigInfos.find(subsriber);
        if(info != temporaryConfigInfos.end()){    // Found
            temporaryConfigInfos.erase(info);
            numbOfRemovedSubscribers = 1;
        }else{ } // Not Found
    }else{
        // Config from file
        InfosBySubsribers::iterator info = infosBySubsribers.find(infoF);
        if(info != infosBySubsribers.end()){    // Found
            if((numbOfRemovedSubscribers = info.value().removeAll(subsriber)) == 1){
                _lastErrorMsg = "";
                // If no subsribers, remove config info
                if(info.value().isEmpty())
                    infosBySubsribers.erase(info);
            }else{
                _lastErrorMsg = "Internal Error: Unsubscribe - NumbOfSubsribers doesnt match. Expected: 1. Removed: " + QString::number(numbOfRemovedSubscribers);
            }
        } else {} // Not Found
    }

    return numbOfRemovedSubscribers == 1;
}

void ControllerConfigManager::processCorupptedInfo( InfosBySubsribers::key_value_iterator info){
    for(EventHandlerPtrs::Iterator eventHandler = info->second.begin(); eventHandler != info->second.end(); eventHandler++)
    {
        QApplication::sendEvent((*eventHandler), new CorruptedInfoEvent());
    }
    delete info->first;
    infosBySubsribers.remove(info->first);

}
