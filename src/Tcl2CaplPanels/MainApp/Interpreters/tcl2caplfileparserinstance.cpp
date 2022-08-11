#include"tcl2caplfileparserinstance.hpp"
//#include"tcl2caplfileparserpanel.hpp"
#include<QSplitter>
#include<QEvent>
#include<QMessageBox>
#include<QDesktopServices>
#include<QUrl>
#include<QFileDialog>
#include<QDockWidget>
#include"FileInstanceResultPanel/fileinstanceresultpanel.hpp"
#include"External/FileReader/FilesSpecificData/XML/TclCaplParserConfig/FRI_FSD_XML_TCL_CAPL_Parser_Config.hpp"
#include<QContextMenuEvent>
#include"Tcl2CaplPanels/MainApp/mainApp.hpp"
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"


Tcl2CaplFileParserInstance::Tcl2CaplFileParserInstance()
    : QSplitter(Qt::Horizontal),
      //panel(panel),
      filesList(*this),
      configSplitter(Qt::Vertical),
      inputConfig(INPUT_CONFIG_BOX_TITLE),
      outputBox(OUTPUT_BOX_TITLE),
      definitionsTclInputsSplitter(Qt::Horizontal),
      inputProcedureConfigTitle(INPUT_PROCEDURE_CONFIG_TITLE),
      inputProcedureConfigSelection(FILE_SELECTION_BUTTON_TEXT),
      outputDirTitle(OUTPUT_DIR_TITLE),
      outputDirSelection(FILE_SELECTION_BUTTON_TEXT),
      readDefinitionsButton(READ_DEFINITIONS_BUTTON_TEXT),
      generateCaplButton(GENERATE_CAPL_BUTTON_TEXT),
      generateCaplReportModeButton(GENERATE_CAPL_WRITE_ONLY_MODE_BUTTON_TEXT)
{

    // Setup:
    // - definitionsTclInputsSplitter
    definitionsTclInputsSplitter.addWidget(&definitionsList);
    definitionsTclInputsSplitter.addWidget(&inputsList);

    // - inputProcedureConfigPath
    inputProcedureConfigPath.setReadOnly(true);
    // [0.9.0]
    inputProcedureConfigPath.setDisabled(true);
    // [0.9.0] End

    // - inputProcedureConfigSelection
    //inputProcedureConfigSelection.installEventFilter(this);
    // [0.9.0]
    //inputProcedureConfigSelection.setDisabled(true);
    // [0.9.0] End

    // - outputDirPath
    outputDirPath.setReadOnly(true);

    // - outputDirSelection
    outputDirSelection.installEventFilter(this);

    // - readDefinitionsButton
    readDefinitionsButton.setToolTip("Aktualne");
    readDefinitionsButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton));
    // [0.9.0]
    //readDefinitionsButton.installEventFilter(this);
    readDefinitionsButton.setDisabled(true);
    // [0.9.0] End

    // - generateCaplButton
    generateCaplButton.installEventFilter(this);

    // - generateCaplReportModeButton
    generateCaplReportModeButton.installEventFilter(this);

    // - inputConfig
    inputProcedureOutputLayout.addWidget(&inputProcedureConfigTitle, 0, 0);
    inputProcedureOutputLayout.addWidget(&inputProcedureConfigPath, 0, 1);
    inputProcedureOutputLayout.addWidget(&inputProcedureConfigSelection, 0, 2, Qt::AlignRight );

    inputProcedureOutputLayout.addWidget(&outputDirTitle, 1, 0);
    inputProcedureOutputLayout.addWidget(&outputDirPath, 1, 1);
    inputProcedureOutputLayout.addWidget(&outputDirSelection, 1, 2, Qt::AlignRight );

    inputLayout.setSpacing(0);
    inputLayout.addWidget(&definitionsTclInputsSplitter, Qt::AlignTop);
    inputLayout.addLayout(&inputProcedureOutputLayout, Qt::AlignBottom);
    definitionsTclInputsSplitter.setContentsMargins(0,0,0,0);

    inputConfig.setLayout(&inputLayout);

    // - outputBox
    outputLayout.addWidget(&readDefinitionsButton, 0, 0);
    outputLayout.addWidget(&generateCaplButton, 0, 1);
    outputLayout.addWidget(&generateCaplReportModeButton, 0, 2);

    outputLayout.setAlignment(Qt::AlignTop);
    outputBox.setLayout(&outputLayout);

    // Config Splitter
    configSplitter.addWidget(&inputConfig);
    configSplitter.addWidget(&outputBox);
    //configSplitter.setContentsMargins(0,0,0,0);

    // - centralWidget
    addWidget(&filesList);
    addWidget(&configSplitter);

    // - Request New Instance for FileWatcher
    //panel.execRequest_FileWatcher_addInstance(this, QFileInfo(filesList.rootPath()));
}

Tcl2CaplFileParserInstance::~Tcl2CaplFileParserInstance(){
    // - Request Remove Instance for FileWatcher
    //panel.execRequest_FileWatcher_removeInstance(this);
}

bool Tcl2CaplFileParserInstance::eventFilter(QObject *obj, QEvent *ev){
    switch(ev->type()){
        case QEvent::MouseButtonPress:
        {
            /*if(obj == &inputDirSelectionButton){
                QString mainDirPath =  QFileDialog::getExistingDirectory(nullptr, QString("Wybierz folder z plikami do zmodyfikowania:"));
                if(!mainDirPath.isEmpty()){
                    inputDirPath.setText(mainDirPath);
                }
               break;
            }*/

            if(obj == &inputProcedureConfigSelection){                
                if(Error&& error = readNewInputConfig_dialog();
                    !error.isEmpty())
                {
                    QMessageBox::critical(nullptr, "Select config error", error);
                }
                break;
            }

            if(obj == &outputDirSelection){
                QString mainDirPath =  QFileDialog::getExistingDirectory(nullptr, QString("Wybierz folder wyjsciowy:"));
                if(QFileInfo::exists(mainDirPath))
                    outputDirPath.setText(mainDirPath);
                else
                    outputDirPath.setText(QString());
                break;
            }

            if(obj == &generateCaplButton){
                generateCapl();
                break;
            }

            if(obj == &generateCaplReportModeButton){
                generateCaplRaportMode();
                break;
            }


        }
        break;

    default:
        break;
    }

    return Super::eventFilter(obj, ev);
}

Tcl2CaplFileParserInstance::Error Tcl2CaplFileParserInstance::readNewInputConfig_dialog(){
    QFileInfo file(QFileDialog::getOpenFileName(nullptr, QString("Wybierz plik konfiguracyjny:"), QString(), "*.xml"));
    return readNewInputConfig(file);
}

Tcl2CaplFileParserInstance::Error Tcl2CaplFileParserInstance::readNewInputConfig(QFileInfo& file){

    if(not file.filePath().isEmpty()){
        if(not file.exists()){return ("File Dont Exist");}

        if( QString error = readProceduresConfig(file);
                not error.isEmpty() )
        {
            return error;
        }
        //TCLCommandsController::addDefaultProcedureDefinitionsToUserProcedureDefintions(tempUserProceduresConfig_);
    }else{
        // Reset
       // tempUserProceduresConfig_ = UserProceduresConfig();
    }

    inputProcedureConfigPath.setText(file.filePath());
    // Merge with default procedures
     return QString();
}

Tcl2CaplFileParserInstance::Error Tcl2CaplFileParserInstance::readProceduresConfig(QFileInfo &){

    /*Tcl2CaplControllerConfigXmlData tempUserDefinitionsData;
    FSD_XML_TclCaplParserConfigInterpreter::Config parserConfigInterpreter(tempUserDefinitionsData);
    QStringList blackList;
    QString filePath;
    if(not (filePath = parserConfigInterpreter.readFileByFilePath(iniConfig.filePath(), QStringList())).isEmpty()){
        // Check for interpreter errors
        if(parserConfigInterpreter.isError()){   // if error true
            qDebug() << parserConfigInterpreter.error();
            return (parserConfigInterpreter.error());
        }else{
           // if Data Model != nullptr, then file has been read
            if(parserConfigInterpreter.getOnTheBlackList()){
                // BlackList PlaceHoldered
            }else{
                //xmlTestModulesByPath.insert(tempTestModule, filePath);
                //qDebug() << "TC Read Success: " << filePath;
                //tempUserProceduresConfig_ = tempUserDefinitionsData;
            }
        }
    }
    return QString();*/
    return QString();
}

void Tcl2CaplFileParserInstance::generateCapl(){
    definitionsList.checkActive();

    inputsList.checkActive();

    if(inputsList.count() <= 0)
        return;

    if(!outputDirPath.text().isEmpty() and not QFileInfo(outputDirPath.text()).exists())
    {
        QMessageBox::critical(nullptr, "Output Dir doesnt exist", "Output has been changed to Input Parent Folder");
        outputDirPath.clear();
        return;
    }

    QStringList definitions;
    QStringList inputs;
    for(int i = 0; i < definitionsList.count(); i++){
        definitions.append(definitionsList.item(i)->toolTip());
    }
    for(int i = 0; i < inputsList.count(); i++){
        inputs.append(inputsList.item(i)->toolTip());
    }

    // Prepare Result DockWidget (FileInstanceResultPanel)
    FileInstanceResultPanel* instanceWidget = nullptr;

    instanceWidget = new FileInstanceResultPanel(*this, definitions, inputs, outputDirPath.text());
    /*QDockWidget* newInstance = new QDockWidget("tabName");
    newInstance->setWidget(instanceWidget);
    newInstance->setAttribute(Qt::WA_DeleteOnClose);
    addDockWidget(Qt::BottomDockWidgetArea, newInstance);*/
    instanceWidget->show();

    MainWindow::ConfigEditor& configEditor =  mainWindow().getConfigEditor(&subWindow());
    configEditor.syncConfig();
    instanceWidget->generateCapl(configEditor.config());


/*
    QString writeResultError;
    if(!(writeResultError = tcl2Capl.writeResults()).isEmpty()){
        if(writeResultError == "Error File"){
            switch(QMessageBox::question(nullptr,QString("Interpreter Errors"), QString("Do you want to open error report?"))){
            case QMessageBox::Yes:
                QDesktopServices::openUrl(QUrl::fromLocalFile(tcl2Capl.reportFilePath()));
                break;
            default:
                break;
            }
        }else{
            QMessageBox::critical(nullptr, "Write Result Error", tcl2Capl.error());
            tcl2Capl.clearError();
        }
        return;
    }

    switch(QMessageBox::question(nullptr,QString("Operation Complete"), QString("Do you want to open result directory?"))){
    case QMessageBox::Yes:
        QDesktopServices::openUrl( QUrl::fromLocalFile(tcl2Capl.outputDirPath()) );
        break;
    default:
        break;
    }*/
}


void Tcl2CaplFileParserInstance::generateCaplRaportMode(){
    definitionsList.checkActive();
    inputsList.checkActive();

    if(inputsList.count() <= 0)
        return;

    if(not outputDirPath.text().isEmpty() and not QFileInfo(outputDirPath.text()).exists())
    {
        QMessageBox::critical(nullptr, "Output Dir doesnt exist", "Output has been changed to Input Parent Folder");
        outputDirPath.clear();
        return;
    }

    QStringList definitions;
    QStringList inputs;
    for(int i = 0; i < definitionsList.count(); i++){
        definitions.append(definitionsList.item(i)->toolTip());
    }
    for(int i = 0; i < inputsList.count(); i++){
        inputs.append(inputsList.item(i)->toolTip());
    }

    // Prepare Result DockWidget (FileInstanceResultPanel)
    FileInstanceResultPanel* instanceWidget = nullptr;
    instanceWidget = new FileInstanceResultPanel(*this, definitions, inputs, outputDirPath.text());
    /*QDockWidget* newInstance = new QDockWidget("tabName");
    newInstance->setWidget(instanceWidget);
    newInstance->setAttribute(Qt::WA_DeleteOnClose);
    addDockWidget(Qt::BottomDockWidgetArea, newInstance);*/
    instanceWidget->show();

    MainWindow::ConfigEditor& configEditor =  mainWindow().getConfigEditor(&subWindow());
    configEditor.syncConfig();
    instanceWidget->generateCaplInWriteOnlyMode(configEditor.config());

    /*tcl2Capl.userProceduresConfig().proceduresSettings().setWriteOnlyMode(true);
    if(!tcl2Capl.generateCaplsFromFolderWithTcls(inputDirPath.text(), outputDirPath.text()).isEmpty()){
        QMessageBox::critical(nullptr, "Generate Capl Error", tcl2Capl.error());
        tcl2Capl.clearError();
        tcl2Capl.userProceduresConfig().proceduresSettings().setWriteOnlyMode(false);
        return;
    }
    tcl2Capl.userProceduresConfig().proceduresSettings().setWriteOnlyMode(false);

    QString writeResultError;
    if(!(writeResultError = tcl2Capl.writeResults()).isEmpty()){
        if(writeResultError == "Error File"){
            switch(QMessageBox::question(nullptr,QString("Interpreter Errors"), QString("Do you want to open error report?"))){
            case QMessageBox::Yes:
                QDesktopServices::openUrl(QUrl::fromLocalFile(tcl2Capl.reportFilePath()));
                break;
            default:
                break;
            }
        }else{
            QMessageBox::critical(nullptr, "Write Result Error", tcl2Capl.error());
            tcl2Capl.clearError();
        }
        return;
    }*/
/*
    switch(QMessageBox::question(nullptr,QString("Operation Complete"), QString("Do you want to open result directory?"))){
    case QMessageBox::Yes:
        //QDesktopServices::openUrl( QUrl::fromLocalFile(tcl2Capl.outputDirPath()) );
        break;
    default:
        break;
    }*/
}

template <>
void Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Tcl2CaplFileParserInstance::Request_FileInfoRef::AddToDefinitions>(QFileInfo& file){
    if(file.exists())
    {
        if(file.isDir()){
            definitionsList.newFolder(file.filePath());
        }else{
            if(file.isFile() and file.suffix() == "tcl")
                definitionsList.newFile(file.filePath());
        }
    }
}

template <>
void Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Tcl2CaplFileParserInstance::Request_FileInfoRef::AddToInputs>(QFileInfo& file){

    if(file.exists())
    {
        if(file.isDir()){
            inputsList.newFolder(file.filePath());
        }else{
            if(file.isFile() and file.suffix() == "tc")
                inputsList.newFile(file.filePath());
        }
    }    
}

template <>
void Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Tcl2CaplFileParserInstance::Request_FileInfoRef::SetAsConfiguration>(QFileInfo& file){
    if(file.exists() and file.isFile() and file.suffix() == "xml")
    {
        if(Error&& error = readNewInputConfig(file);
            !error.isEmpty())
        {
            QMessageBox::critical(nullptr, "Select config error", error);
        }
    }
}

template <>
void Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Tcl2CaplFileParserInstance::Request_FileInfoRef::SetAsOutput>(QFileInfo& file){
    if(file.exists() and file.isDir())
    {
        outputDirPath.setText(file.filePath());
    }
}

SubWindow& Tcl2CaplFileParserInstance::subWindow(){
    return *static_cast<SubWindow*>(parentWidget());
}

MainWindow& Tcl2CaplFileParserInstance::mainWindow(){
    return *static_cast<MainWindow*>(&(subWindow().mainWindow()));
}


