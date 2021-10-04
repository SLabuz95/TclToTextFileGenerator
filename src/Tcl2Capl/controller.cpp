#include"controller.hpp"
#include<QFileDialog>
#include<QDirIterator>
#include"TcFileModifier/tcfilemodifier.hpp"
#include<QApplication>
#include"tcl2caplprogressevent.hpp"


UserInputConfig::UserInputConfig(UserInputConfigData& configData)
    : userProcedures_(UserProcedures(configData.getNumbOfUserProcedures())),
      userDefaultProcedure_(configData.defaultProcedure().toProcedureWithRawRules())
{
    UserInputConfigData::Procedures& userConfigProcedures = configData.userProcedures();
    UserInputConfigData::Procedures::Iterator configProcedure = userConfigProcedures.begin();
    UserProcedures::Iterator userProcedure = userProcedures_.begin();
    Q_ASSERT_X(userConfigProcedures.size() == userProcedureConfig().size(), "InputConfig Procedures Size have to be the same as InputConfigData Procedures size", "");
    for(;
        configProcedure != userConfigProcedures.end(); configProcedure++, userProcedure++)
    {
        (*userProcedure) = configProcedure->toProcedureWithRawRules();
    }
    TCLInterpreter::TCLProceduresInterpreter::addDefaultProcedureDefinitionsToUserProcedureDefintions(*this);
}

//#include"userdefinitions_helper.hpp"
//#include"External/FileReader/FilesSpecificData/XML/TclCaplParserConfig/FRI_FSD_XML_TCL_CAPL_Parser_Config.hpp"
Tcl2CaplController::Tcl2CaplController(QStringList& definitions, QStringList& inputPaths, QString& outputPath)
    : definitionPaths_(definitions), inputPaths_(inputPaths)
{
    prepareOutputDirs(outputPath);
    //moveToThread(this);
}

/*
Tcl2CaplController::Error Tcl2CaplController::readNewInputConfig(){
    QString mainPath =  QFileDialog::getOpenFileName(nullptr, QString("Wybierz plik konfiguracyjny:"), QString(), "*.xml");

    if(mainPath.isEmpty())   return QString();
    if(!QFile::exists(mainPath)){return throwError("File Dont Exist");}

    if(!readProceduresConfig(mainPath).isEmpty())
        return throwError("Read Config Error: ");

    configFilePath_ = mainPath;
    // Merge with default procedures
    TCLInterpreter::TCLProceduresInterpreter::addDefaultProcedureDefinitionsToUserProcedureDefintions(userInputConfig_);
    return QString();
}

Tcl2CaplController::Error Tcl2CaplController::readProceduresConfig(QString &iniConfigPath){

    UserDefinitionsData tempUserDefinitionsData;
    FSD_XML_TclCaplParserConfigInterpreter::Config parserConfigInterpreter(tempUserDefinitionsData);
    QStringList blackList;
    QString filePath;
    if(!(filePath = parserConfigInterpreter.readFileByFilePath(iniConfigPath, QStringList())).isEmpty()){
        // Check for interpreter errors
        if(parserConfigInterpreter.isError()){   // if error true
            qDebug() << parserConfigInterpreter.getErrorStr();
            return throwError(parserConfigInterpreter.getErrorStr());
        }else{
           // if Data Model != nullptr, then file has been read
            if(parserConfigInterpreter.getOnTheBlackList()){
                // BlackList PlaceHoldered
            }else{
                //xmlTestModulesByPath.insert(tempTestModule, filePath);
                qDebug() << "TC Read Success: " << filePath;
                userInputConfig_ = tempUserDefinitionsData;
            }
        }
    }
    return QString();
}*/


void Tcl2CaplController::prepareOutputDirs(QString output){
    /*QDir inputDir;
    for(QStringList::Iterator input = inputs.begin() ; input < inputs.end(); input++){
        if(QFileInfo(*input).isDir()){
            inputDir.setPath(*input);
            inputDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
            inputDir.setSorting(QDir::Name);



            outputDir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
            outputDir.setSorting(QDir::Name);
            // Prepare output
            {
                int i = 0;
                QString dirName = (output.isEmpty())? outputDir.dirName() : inputDir.dirName();
                if(output.isEmpty()){
                    dirName = outputDir.dirName();
                    outputDir.cdUp();
                }else{
                    dirName = inputDir.dirName();
                }

                for(i = 0; i < 1000; i++){
                    QString outputDirName = "FAMOut_" + dirName + "_" + QString::number(i);
                    if(!QFile::exists(outputDir.path() + "/" + outputDirName)){
                        if(!outputDir.mkdir(outputDirName) || !outputDir.cd(outputDirName))
                        {
                            throw std::runtime_error(QString("Output Directory: Failed in New Directory " + QString::number(i)).toLocal8Bit());
                        }
                        break;
                    }
                }
                if(i == 1000){
                    throw std::runtime_error("mkDir Failed: Max Numb of Output Directory Has been exceeded.");
                }
            }
        }

    }*/
    if(output.isEmpty())
        outputDir.setPath(QDir::homePath());
    else
        outputDir.setPath(output);
    setReportFilePath(outputDir);
    setCaplDefinitionsFilePath(outputDir);
}

void Tcl2CaplController::run(){
    generateDefinitions_impl();
    generateCapls_impl();

    writeResults();
}

void Tcl2CaplController::generateDefinitions_impl(){
    // Preconditions
    userInputConfig_.proceduresSettings().setMode(Settings::InterpreterMode::PredefinitionsOnly);

    CAPLFunctionDefinitions caplFunctionDefinitions;

    for(QStringList::Iterator definitionPath = definitionPaths().begin();
        definitionPath < definitionPaths().end(); definitionPath++)
    {
        // Preconditions - for failed - break - if doesnt exist, add Ignored item
        QFileInfo&& fileInfo = QFileInfo(*definitionPath);
        if(not fileInfo.exists()){
            QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent("No File"));

            break; // !!!!!!! BREAK LOOP
        }

        // If file
        if(QFileInfo(*definitionPath).isFile()){
            //Use Interpreter
            Tcl2CaplResult::Tcl2CaplReadData tcFileData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions);
            TcFileModifier::Config tcFileModifier(tcFileData);
            QStringList blackList;
            QString filePath;
            //tcFileData.clearInterpreter();
            tcFileData.setCurrentTCLFileName( fileInfo.fileName());
            if(!(filePath = tcFileModifier.readFileByFilePath(fileInfo.filePath(), QStringList())).isEmpty()){
                // Check for interpreter errors
                if(tcFileModifier.isError()){   // if error true
                    qDebug() << tcFileModifier.error();
                }else{
                   // if Data Model != nullptr, then file has been read
                    if(tcFileModifier.getOnTheBlackList()){
                        // BlackList PlaceHoldered
                    }else{
                        //xmlTestModulesByPath.insert(tempTestModule, filePath);
                        qDebug() << "TC Read Success: " << filePath;
                    }
                }
            }
            QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(tcFileData.testCaseErrors()));
        }else{
            QDir dir(fileInfo.filePath());
            dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
            dir.setSorting(QDir::Name);
            QVector<QDirIterator*> dirs({new QDirIterator(dir)});
            QVector<Tcl2CaplResult::Tcl2CaplReadData*> tcFileData{new Tcl2CaplResult::Tcl2CaplReadData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions)};
            using NameFilters = const QStringList;
            NameFilters permittedFileSuffixes = QStringList{"tcl"};

            while(!dirs.isEmpty()){
                while(dirs.last()->hasNext()){
                    QFileInfo fileInfo(dirs.last()->next());
                    // Directory

                    if(fileInfo.isDir()){
                        QDir dir(fileInfo.filePath());
                        dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
                        dir.setSorting(QDir::Name);
                        if(dir.dirName() != tcFileData.at(0)->dir().dirName()){ // Ignore if dir name == first output dir name
                            dirs.append(new QDirIterator(dir));
                            tcFileData.append(new Tcl2CaplResult::Tcl2CaplReadData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions));
                        }
                    }else{
                        if(fileInfo.isFile() and permittedFileSuffixes.contains(fileInfo.completeSuffix())){
                            //Use Interpreter
                            TcFileModifier::Config tcFileModifier(*tcFileData.last());
                            QStringList blackList;
                            QString filePath;
                            //tcFileData.clearInterpreter();
                            tcFileData.last()->setCurrentTCLFileName( fileInfo.fileName());
                            if(!(filePath = tcFileModifier.readFileByFilePath(fileInfo.filePath(), QStringList())).isEmpty()){
                                // Check for interpreter errors
                                if(tcFileModifier.isError()){   // if error true
                                    qDebug() << tcFileModifier.error();
                                }else{
                                   // if Data Model != nullptr, then file has been read
                                    if(tcFileModifier.getOnTheBlackList()){
                                        // BlackList PlaceHoldered
                                    }else{
                                        //xmlTestModulesByPath.insert(tempTestModule, filePath);
                                        qDebug() << "TC Read Success: " << filePath;
                                    }
                                }
                            }
                            QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(tcFileData.last()->testCaseErrors()));
                        }
                    }
                }
                delete dirs.takeLast();
                if(not tcFileData.isEmpty()){
                    if(tcFileData.last()->isInitiliazed()){
                        addResult(tcFileData.last()->generateResult());
                        predefinitions.append(tcFileData.last()->predefinitions());
                    }
                    delete tcFileData.takeLast();
                }
            }
        }

    }
    // Postconditions
    userInputConfig_.proceduresSettings().setMode(Settings::InterpreterMode::TestCase);
    //qDebug() << formatCount;
    for(Predefinitions::Iterator predefinition = predefinitions.begin(); predefinition < predefinitions.end(); predefinition++){
        qDebug() << predefinition->type + " " + predefinition->value;
    }
}

void Tcl2CaplController::generateCapls_impl(){
    CAPLFunctionDefinitions caplFunctionDefinitions;

    int i = 0;
    QString dirName = outputDir.dirName();
    if(userOutputDir.isEmpty()){
        dirName = outputDir.dirName();
    }

    for(i = 0; i < 1000; i++){
        QString outputDirName = "FAMOut_" + QString::number(i);
        if(!QFile::exists(outputDir.path() + "/" + outputDirName)){
            if(!outputDir.mkdir(outputDirName) || !outputDir.cd(outputDirName))
            {
                throw std::runtime_error(QString("Output Directory: Failed in New Directory " + QString::number(i)).toLocal8Bit());
            }
            break;
        }
    }
    if(i == 1000){
        throw std::runtime_error("mkDir Failed: Max Numb of Output Directory Has been exceeded.");
    }

    for(QStringList::Iterator input = inputPaths().begin();
        input < inputPaths().end(); input++)
    {
       // if input doesnt exist - critical
        QFileInfo&& inputFile = QFileInfo(*input);

        if(not inputFile.exists()){
            QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent("File doesnt exist"));
        }else{
            // If file
            if(inputFile.isFile()){

                throw std::runtime_error("Not implemented");

                Tcl2CaplResult::Tcl2CaplReadData tcFileData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions);
                TcFileModifier::Config tcFileModifier(tcFileData);
                QStringList blackList;
                QString filePath;
                //tcFileData.clearInterpreter();
                tcFileData.setCurrentTCLFileName( inputFile.fileName());
                if(!(filePath = tcFileModifier.readFileByFilePath(inputFile.filePath(), QStringList())).isEmpty()){
                    // Check for interpreter errors
                    if(tcFileModifier.isError()){   // if error true
                        qDebug() << tcFileModifier.error();
                    }else{
                       // if Data Model != nullptr, then file has been read
                        if(tcFileModifier.getOnTheBlackList()){
                            // BlackList PlaceHoldered
                        }else{
                            //xmlTestModulesByPath.insert(tempTestModule, filePath);
                            qDebug() << "TC Read Success: " << filePath;
                        }
                    }
                }
                QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(tcFileData.testCaseErrors()));
            }else{
                int i = 0;
                QString dirName = inputFile.fileName();

                for(i = 0; i < 1000; i++){
                    QString outputDirName = "FAMOut_" + dirName + "_" + QString::number(i);
                    if(!QFile::exists(outputDir.path() + "/" + outputDirName)){
                        if(!outputDir.mkdir(outputDirName) || !outputDir.cd(outputDirName))
                        {
                            throw std::runtime_error(QString("Output Directory: Failed in New Directory " + QString::number(i)).toLocal8Bit());
                        }
                        break;
                    }
                }
                if(i == 1000){
                    throw std::runtime_error("mkDir Failed: Max Numb of Output Directory Has been exceeded.");
                }
                QDir dir(inputFile.filePath());
                dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
                dir.setSorting(QDir::Name);

                QVector<QDirIterator*> dirs({new QDirIterator(dir)});
                QVector<Tcl2CaplResult::Tcl2CaplReadData*> tcFileData{new Tcl2CaplResult::Tcl2CaplReadData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions)};
                using NameFilters = const QStringList;
                NameFilters permittedFileSuffixes = QStringList{"tc"};

                while(!dirs.isEmpty()){
                    while(dirs.last()->hasNext()){
                        QFileInfo fileInfo(dirs.last()->next());
                        // Directory

                        if(fileInfo.isDir()){
                            QDir dir(fileInfo.filePath());
                            dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
                            dir.setSorting(QDir::Name);

                            if(dir.dirName() != tcFileData.at(0)->dir().dirName()){ // Ignore if dir name == first output dir name
                                if(!outputDir.mkdir(dir.dirName()) || !outputDir.cd(dir.dirName()))
                                {
                                    reportFile.setFileName("");
                                    caplDefinitionsFile.setFileName("");
                                    QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent("Internal Error - FileSystem Error"));
                                    return;
                                }
                                dirs.append(new QDirIterator(dir));
                                tcFileData.append(new Tcl2CaplResult::Tcl2CaplReadData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions));
                            }
                        }else{
                            if(fileInfo.isFile() and permittedFileSuffixes.contains(fileInfo.completeSuffix())){
                                //Use Interpreter
                                TcFileModifier::Config tcFileModifier(*tcFileData.last());
                                QStringList blackList;
                                QString filePath;
                                //tcFileData.clearInterpreter();
                                tcFileData.last()->setCurrentTCLFileName( fileInfo.fileName());
                                if(!(filePath = tcFileModifier.readFileByFilePath(fileInfo.filePath(), QStringList())).isEmpty()){
                                    // Check for interpreter errors
                                    if(tcFileModifier.isError()){   // if error true
                                        qDebug() << tcFileModifier.error();
                                    }else{
                                       // if Data Model != nullptr, then file has been read
                                        if(tcFileModifier.getOnTheBlackList()){
                                            // BlackList PlaceHoldered
                                        }else{
                                            //xmlTestModulesByPath.insert(tempTestModule, filePath);
                                            qDebug() << "TC Read Success: " << filePath;
                                        }
                                    }
                                }
                                QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(tcFileData.last()->testCaseErrors()));
                            }
                        }
                    }
                    outputDir.cdUp();
                    delete dirs.takeLast();
                    if(not tcFileData.isEmpty()){
                        if(tcFileData.last()->isInitiliazed()){
                            addResult(tcFileData.last()->generateResult());
                        }
                        delete tcFileData.takeLast();

                    }
                }
            }
        }
    }
    caplFunctionDefinitions.writeCaplFunctionDefinitions(caplDefinitionsFile);

    QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(0));

    //qDebug() << formatCount;

}

void Tcl2CaplController::writeResults(){
    using Result = Results::Iterator;
    Error error;
    if(reportFile.isOpen()){
        reportFile.write(QString("</table>\n</div>\n</div>\n<hr>\n</body></html>").toUtf8());
        reportFile.close();
        throwError( QString("Error File"));
    }
    for(Result result = results.begin(); result < results.end(); result++)
    {
        if(!(error = (*result)->writeResult()).isEmpty())
            throwError(error);
    }

}
