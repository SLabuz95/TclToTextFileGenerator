#include"controller.hpp"
#include"Result/tcl2caplresult.hpp"
#include<QFileDialog>
#include<QDirIterator>
#include"TcFileModifier/tcfilemodifier.hpp"
#include<QApplication>
#include"tcl2caplprogressevent.hpp"
#include<QStandardPaths>


UserInputConfig::UserInputConfig(UserInputConfigData& configData)
    : userProcedures_(UserProcedures(configData.getNumbOfExistingProcedures())),
      userDefaultProcedure_(configData.readDefaultProcedure()),
      settings_(configData.settings())
{
    configData.readProcedures(userProcedures_);
}

Tcl2CaplController::~Tcl2CaplController(){
    // Clear memory
    using Result = Results::Iterator;
    for(Result result = results.begin(); result < results.end(); result++){
        delete (*result); // Pointer to Pointer -> get Access to correct memory
    }
    terminate();
    //quitAndWait();
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
    TCLCommandsController::addDefaultProcedureDefinitionsToUserProcedureDefintions(userInputConfig_);
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
        outputDir.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    else
        outputDir.setPath(output);
}

void Tcl2CaplController::run(){
    generateDefinitions_impl() and
    generateCapls_impl() and
    writeResults();
    quit();
}

bool Tcl2CaplController::generateDefinitions_impl(){
    using TCLCommandsController = Tcl::Interpreter::Command::Controller;
    //TODO: Memory Leaks
    using ErrorMsg = QString;
    const ErrorMsg ERROR_PREFIX = "Definition File Error: ";
    ErrorMsg errorMsg;
    // Preconditions
    UserInputConfig::Settings::InterpreterMode savedMode = userInputConfig_.proceduresSettings().mode();
    userInputConfig_.proceduresSettings().setMode(Settings::InterpreterMode::PredefinitionsOnly);
    TCLCommandsController::addDefaultProcedureDefinitionsToUserProcedureDefintions(userInputConfig_);

    FunctionDefinitions caplFunctionDefinitions;

    for(QStringList::Iterator definitionPath = definitionPaths().begin();
        definitionPath < definitionPaths().end(); definitionPath++)
    {
        // Preconditions - for failed - break - if doesnt exist, add Ignored item
        QFileInfo&& fileInfo = QFileInfo(*definitionPath);
        if(not fileInfo.exists()){
            errorMsg = ERROR_PREFIX + fileInfo.absoluteFilePath() + " Not Found";
            QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(errorMsg));
            break; // !!!!!!! BREAK LOOP
        }

        // If file
        if(QFileInfo(*definitionPath).isFile()){
            //Use Interpreter
            Tcl2CaplReadData tcFileData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions, Tcl2CaplReadData::ReadMode::FileMode);
            TcFileModifier::Config tcFileModifier(tcFileData);
            QStringList blackList;
            QString filePath;
            //tcFileData.clearInterpreter();
            tcFileData.setCurrentTCLFileName( fileInfo.fileName());
            if(!(filePath = tcFileModifier.readFileByFilePath(fileInfo.filePath(), QStringList())).isEmpty()){
                // Check for interpreter errors
                if(tcFileModifier.isError()){   // if error true
                    qDebug() << tcFileModifier.error();                    
                    errorMsg = ERROR_PREFIX + tcFileModifier.error();
                }else{
                   // if Data Model != nullptr, then file has been read
                    if(tcFileModifier.getOnTheBlackList()){
                        // BlackList PlaceHoldered
                    }else{
                        //xmlTestModulesByPath.insert(tempTestModule, filePath);
                        //qDebug() << "TC Read Success: " << filePath;
                    }
                }
            }
            QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(errorMsg, tcFileData.testCaseErrors()));
        }else{
            if(QFileInfo(*definitionPath).isDir()){
                QDir dir(fileInfo.filePath());
                dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
                dir.setSorting(QDir::Name);
                QVector<QDirIterator*> dirs({new QDirIterator(dir)});
                QVector<Tcl2CaplReadData*> tcFileData{new Tcl2CaplReadData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions, Tcl2CaplReadData::ReadMode::DirectoryMode)};
                using NameFilters = const QStringList;
                NameFilters permittedFileSuffixes = QStringList{"tcl"};

                while(not dirs.isEmpty()){
                    while(dirs.last()->hasNext()){
                        QFileInfo fileInfo(dirs.last()->next());
                        // Directory
                        if(fileInfo.isDir()){
                            QDir dir(fileInfo.filePath());
                            dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
                            dir.setSorting(QDir::Name);
                            if(dir.dirName() != tcFileData.at(0)->dir().dirName()){ // Ignore if dir name == first output dir name
                                dirs.append(new QDirIterator(dir));
                                tcFileData.append(new Tcl2CaplReadData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions, Tcl2CaplReadData::ReadMode::DirectoryMode));
                            }
                        }else{
                            if(fileInfo.isFile() and permittedFileSuffixes.contains(fileInfo.completeSuffix())){
                                //Use Interpreter
                                TcFileModifier::Config tcFileModifier(*tcFileData.last());
                                QStringList blackList;
                                QString filePath;
                                ErrorMsg localErrorMsg;
                                //tcFileData.clearInterpreter();
                                tcFileData.last()->setCurrentTCLFileName( fileInfo.fileName());
                                if(!(filePath = tcFileModifier.readFileByFilePath(fileInfo.filePath(), QStringList())).isEmpty()){
                                    // Check for interpreter errors
                                    if(tcFileModifier.isError()){   // if error true
                                        qDebug() << tcFileModifier.error();
                                        localErrorMsg = ERROR_PREFIX + tcFileModifier.error();
                                        errorMsg = localErrorMsg;
                                    }else{
                                       // if Data Model != nullptr, then file has been read
                                        if(tcFileModifier.getOnTheBlackList()){
                                            // BlackList PlaceHoldered
                                        }else{
                                            //xmlTestModulesByPath.insert(tempTestModule, filePath);
                                            //qDebug() << "TC Read Success: " << filePath;
                                        }
                                    }
                                }
                                QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(localErrorMsg, tcFileData.last()->testCaseErrors()));
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

    }
    // Postconditions
    userInputConfig_.proceduresSettings().setMode(savedMode);
    //qDebug() << formatCount;
    for(Predefinitions::Iterator predefinition = predefinitions.begin(); predefinition < predefinitions.end(); predefinition++){
//        qDebug() << predefinition->type + " " + predefinition->value;
    }
    return errorMsg.isEmpty();
}

bool Tcl2CaplController::generateCapls_impl(){
    //TODO: Memory Leaks
    using ErrorMsg = QString;
    const ErrorMsg ERROR_PREFIX = "Definition File Error: ";
    ErrorMsg errorMsg;
    FunctionDefinitions caplFunctionDefinitions;
    QDir outputDir = this->outputDir;

    int i = 0;

    for(i = 0; i < 1000; i++){
        QString outputDirName = "FAMOut_" + QString::number(i);
        if(not QFile::exists(outputDir.path() + "/" + outputDirName)){
            if(not outputDir.mkdir(outputDirName) or not outputDir.cd(outputDirName))
            {
                errorMsg = ERROR_PREFIX + QString("Output Directory: Failed For New Directory " + QString::number(i));
                QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(errorMsg));
                return errorMsg.isEmpty();
                //throw std::runtime_error(QString("Output Directory: Failed in New Directory " + QString::number(i)).toLocal8Bit());
            }
            break;
        }
    }
    if(i == 1000){
        errorMsg = ERROR_PREFIX + QString("mkDir Failed: Max Numb of Output Directory Has been exceeded.");
        QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(errorMsg));
        return errorMsg.isEmpty();
    }

    setReportFilePath(outputDir);
    setCaplDefinitionsFilePath(outputDir);

    for(QStringList::Iterator input = inputPaths().begin();
        input < inputPaths().end(); input++)
    {
       // if input doesnt exist - critical
        QFileInfo&& inputFile = QFileInfo(*input);

        if(not inputFile.exists()){
            QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(inputFile.filePath() + " doesnt exist"));
            break;
        }else{
            // If file
            if(inputFile.isFile()){
                Tcl2CaplReadData tcFileData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions, Tcl2CaplReadData::ReadMode::FileMode);
                TcFileModifier::Config tcFileModifier(tcFileData);
                QStringList blackList;
                QString filePath;
                //tcFileData.clearInterpreter();
                tcFileData.setCurrentTCLFileName( inputFile.fileName());
                if(!(filePath = tcFileModifier.readFileByFilePath(inputFile.filePath(), QStringList())).isEmpty()){
                    // Check for interpreter errors
                    if(tcFileModifier.isError()){   // if error true
                        qDebug() << tcFileModifier.error();
                        errorMsg = ERROR_PREFIX + tcFileModifier.error();
                    }else{
                       // if Data Model != nullptr, then file has been read
                        if(tcFileModifier.getOnTheBlackList()){
                            // BlackList PlaceHoldered
                        }else{
                            //xmlTestModulesByPath.insert(tempTestModule, filePath);
                            //qDebug() << "TC Read Success: " << filePath;
                        }
                    }
                }
                if(tcFileData.isInitiliazed()){
                    addResult(tcFileData.generateResult());
                }
                QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(errorMsg, tcFileData.testCaseErrors()));
                errorMsg.clear();
            }else{
                int i = 0;
                QString dirName = inputFile.fileName();
                for(i = 0; i < 1000; i++){
                    QString outputDirName = "Out_" + dirName + "_" + QString::number(i);
                    if(not QFile::exists(outputDir.path() + "/" + outputDirName)){
                        if(!outputDir.mkdir(outputDirName) or not outputDir.cd(outputDirName))
                        {
                            errorMsg = ERROR_PREFIX + QString("Output Directory: Failed in New Directory " + QString::number(i));
                            QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(errorMsg));
                            return errorMsg.isEmpty();
                        }
                        break;
                    }
                }
                if(i == 1000){
                    errorMsg = ERROR_PREFIX + QString("mkDir Failed: Max Numb of Output Directory Has been exceeded.");
                    QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(errorMsg));
                    return errorMsg.isEmpty();
                }
                errorMsg.clear();
                QDir dir(inputFile.filePath());
                dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
                dir.setSorting(QDir::Name);

                QVector<QDirIterator*> dirs({new QDirIterator(dir)});
                QVector<Tcl2CaplReadData*> tcFileData{new Tcl2CaplReadData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions, Tcl2CaplReadData::ReadMode::DirectoryMode)};
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
                            errorMsg.clear();

                            if(dir.dirName() != tcFileData.at(0)->dir().dirName()){ // Ignore if dir name == first output dir name
                                if(!outputDir.mkdir(dir.dirName()) || !outputDir.cd(dir.dirName()))
                                {
                                    reportFile.setFileName("");
                                    caplDefinitionsFile.setFileName("");
                                    errorMsg = ERROR_PREFIX + QString("Internal Error - FileSystem Error");
                                    QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(errorMsg));
                                    return errorMsg.isEmpty();
                                }
                                dirs.append(new QDirIterator(dir));
                                tcFileData.append(new Tcl2CaplReadData(outputDir, reportFile, userInputConfig_, caplFunctionDefinitions, Tcl2CaplReadData::ReadMode::DirectoryMode));
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
                                        errorMsg = ERROR_PREFIX + tcFileModifier.error();
                                    }else{
                                       // if Data Model != nullptr, then file has been read
                                        if(tcFileModifier.getOnTheBlackList()){
                                            // BlackList PlaceHoldered
                                        }else{
                                            //xmlTestModulesByPath.insert(tempTestModule, filePath);
                                            //qDebug() << "TC Read Success: " << filePath;
                                        }
                                    }
                                }
                                QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(errorMsg, tcFileData.last()->testCaseErrors()));
                                errorMsg.clear();
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
    //WARNING: Temporary exploit (ErrorMsg == "END") to inform about end of processing - remove after taking care of Result Panel
    QApplication::postEvent(progressEventDest, new Tcl2CaplProgressEvent(caplFunctionDefinitions));

    //qDebug() << formatCount;
    return errorMsg.isEmpty();
}

bool Tcl2CaplController::writeResults(){
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
    return true;
}
