#include"controllerconfigmanager.hpp"
#include"controllerconfiginfo.hpp"
#include"External/FileReader/FilesSpecificData/XML/TclCaplParserConfig/FRI_FSD_XML_TCL_CAPL_Parser_Config.hpp"
#include<QApplication>
#include"corruptedconfiginfoevent.hpp"


bool ControllerConfigManager::loadConfig(ControllerConfigInfo& configRef){
    Tcl2CaplControllerConfigXmlData tempUserDefinitionsData;
    FSD_XML_TclCaplParserConfigInterpreter::Config parserConfigInterpreter(tempUserDefinitionsData);
    QStringList blackList;
    QString filePath;
    if( not(filePath = parserConfigInterpreter.readFileByFilePath(configRef.filePath(), QStringList())).isEmpty()){
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
                //configRef = Tcl2CaplControllerConfig(tempUserDefinitionsData);
                //infosBySubsribers.insert(returnInfo = new Info(configPath, fileInfo.lastModified(), Tcl2CaplControllerConfig(tempUserDefinitionsData)), {subsriber});
            }
        }
    }
    return true;
}

bool ControllerConfigManager::saveConfig(ControllerConfigInfo& configRef){
    QFile file(configRef.filePath());
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);

    lastErrorMsg_.clear();
    if(file.open(QIODevice::Text | QIODevice::WriteOnly)){
        configRef.toXmlContent(xmlWriter);
    }else{
        lastErrorMsg_ = QString("Open File Error: ") + file.fileName();
    }
    file.close();

    return lastErrorMsg_.length() == 0;;
}

bool ControllerConfigManager::saveConfigAs(ControllerConfigInfo& configRef, QString newPath){
    // Save file
    QFile file(configRef.filePath());
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);

    lastErrorMsg_ = QString();

    if(file.open(QIODevice::Text | QIODevice::WriteOnly)){
        configRef.toXmlContent(xmlWriter);
    }else{
        lastErrorMsg_ = QString("Open File Error: ") + file.fileName();
    }

    file.close();

    return lastErrorMsg_.length() == 0;
}



