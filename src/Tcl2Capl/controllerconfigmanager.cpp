#include"controllerconfigmanager.hpp"
#include"controllerconfiginfo.hpp"
#include"External/FileReader/FilesSpecificData/XML/TclCaplParserConfig/FRI_FSD_XML_TCL_CAPL_Parser_Config.hpp"
#include<QApplication>
#include"corruptedconfiginfoevent.hpp"


bool ControllerConfigManager::loadConfig(ControllerConfigInfo& configRef){
    FSD_XML_TclCaplParserConfigInterpreter::Config parserConfigInterpreter(configRef);
    QStringList blackList;
    QString filePath;

    lastErrorMsg_.clear();
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
    QFile tempFile("tempConfig.xml");
    QXmlStreamWriter xmlWriter(&tempFile);
    xmlWriter.setAutoFormatting(true);
    tempFile.remove();

    lastErrorMsg_.clear();
    if(tempFile.open(QIODevice::Text | QIODevice::WriteOnly)){
        configRef.toXmlContent(xmlWriter);
        tempFile.close();

        file.remove();
        tempFile.rename(QFileInfo(file).filePath());

    }else{
        lastErrorMsg_ = QString("Open File Error: ") + tempFile.fileName();
        qDebug() << lastErrorMsg_;
    }

    return lastErrorMsg_.length() == 0;
}

bool ControllerConfigManager::saveConfigAs(ControllerConfigInfo& configRef, QString newPath){
    // Save file
    QFile tempFile("tempConfig.xml");
    tempFile.remove();

    QFile file(newPath);

    QXmlStreamWriter xmlWriter(&tempFile);
    xmlWriter.setAutoFormatting(true);

    lastErrorMsg_ = QString();

    if(tempFile.open(QIODevice::Text | QIODevice::WriteOnly)){
        configRef.toXmlContent(xmlWriter);
        tempFile.close();

        file.remove();
        tempFile.rename(QFileInfo(file).filePath());

    }else{
        lastErrorMsg_ = QString("Open File Error: ") + tempFile.fileName();
        qDebug() << lastErrorMsg_;
    }

    return lastErrorMsg_.length() == 0;
}



