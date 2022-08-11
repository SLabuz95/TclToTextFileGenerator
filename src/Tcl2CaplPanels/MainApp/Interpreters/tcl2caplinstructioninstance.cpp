#include"tcl2caplinstructioninstance.hpp"
#include<QEvent>
#include<QMessageBox>
#include<QDesktopServices>
#include<QUrl>
#include<QFileDialog>
#include<QDockWidget>
#include"FileInstanceResultPanel/fileinstanceresultpanel.hpp"
#include"External/FileReader/FilesSpecificData/XML/TclCaplParserConfig/FRI_FSD_XML_TCL_CAPL_Parser_Config.hpp"
#include"TcFileModifier/tcfilemodifier.hpp"
#include<QStringListModel>
#include"Tcl2CaplPanels/MainApp/mainApp.hpp"
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"


Tcl2CaplInstructionInstance::Tcl2CaplInstructionInstance()
    : //panel(panel),
      mainSplitter(Qt::Vertical, this),
      tclCaplSplitter(Qt::Horizontal, this),
      configErrorsSplitter(Qt::Horizontal, this),
      inputTclConfigBox(INPUT_TCL_CONFIG_BOX_TEXT, this),
      outputBox(OUTPUT_BOX_TITLE, this),
      errorsBox(ERRORS_BOX_TITLE, this),
      generateCaplButton(GENERATE_CAPL_BUTTON_TEXT, this),
      generateCaplRaportModeButton(GENERATE_CAPL_WRITE_ONLY_MODE_BUTTON_TEXT, this)
{

    // Setup:
    const int TAB_NUMB_OF_CHARACKTERS = 1;

    // - outputText
    outputText.setReadOnly(true);

    // - errorsText
    errorsText.setReadOnly(true);

    // - inputTclConfig
    inputTclConfig.setAutoFormatting(InputTextEdit::AutoAll);
    inputTclConfig.setWordWrapMode(QTextOption::NoWrap);
    inputTclConfig.setAcceptRichText(false);
    inputTclConfig.setTabStopDistance(TAB_NUMB_OF_CHARACKTERS * QFontMetricsF(inputTclConfig.font()).maxWidth());

    // - inputConfig
    /*inputConfig.setAutoFormatting(InputTextEdit::AutoAll);
    inputConfig.setWordWrapMode(QTextOption::NoWrap);
    inputConfig.setAcceptRichText(false);
    inputConfig.setTabStopDistance(TAB_NUMB_OF_CHARACKTERS * QFontMetricsF(inputConfig.font()).maxWidth());*/
    QStringList wordList;
    wordList << "alpha" << "omega" << "omicron" << "zeta";

    /*inputConfigCompleter.setWidget(&inputConfig);
    inputConfigCompleter.setModel(new QStringListModel(wordList));
    completerHelper.setCompleter(&inputConfigCompleter);*/

    // - outputText
    outputText.setAutoFormatting(InputTextEdit::AutoAll);
    outputText.setWordWrapMode(QTextOption::NoWrap);    
    outputText.setTabStopDistance(TAB_NUMB_OF_CHARACKTERS * QFontMetricsF(inputTclConfig.font()).maxWidth());

    // - inputTclConfigBox
    inputTclBoxLayout.addWidget(&inputTclConfig);
    inputTclConfigBox.setLayout(&inputTclBoxLayout);   

    // - inputConfigBox
    /*inputConfigBoxLayout.addWidget(&inputConfig);
    inputConfigBox.setLayout(&inputConfigBoxLayout);*/

    // - outputBox
    outputBoxLayout.addWidget(&outputText);
    outputBox.setLayout(&outputBoxLayout);

    // - errorsBox
    errorsBoxLayout.addWidget(&errorsText);
    errorsBox.setLayout(&errorsBoxLayout);

    // - inputSplitter
    tclCaplSplitter.addWidget(&inputTclConfigBox);
    tclCaplSplitter.addWidget(&outputBox);

    // - outputSplitter
    //configErrorsSplitter.addWidget(&inputConfigBox);
    configErrorsSplitter.addWidget(&errorsBox);

    // - mainSplitter
    mainSplitter.addWidget(&tclCaplSplitter);
    mainSplitter.addWidget(&configErrorsSplitter);

    // - controlLayout
    controlLayout.addWidget(&generateCaplButton);
    controlLayout.addWidget(&generateCaplRaportModeButton);

    // - centralWidgetLayout
    centralWidgetLayout.addWidget(&mainSplitter);
    centralWidgetLayout.addLayout(&controlLayout);

    // - centralWidget
    setLayout(&centralWidgetLayout);
    installEventFilter(this);
    // - generateCaplButton
    generateCaplButton.installEventFilter(this);

    // - generateCaplReportModeButton
    generateCaplRaportModeButton.installEventFilter(this);

    show();

}

bool Tcl2CaplInstructionInstance::eventFilter(QObject *obj, QEvent *ev){
    //using Mode = UserProceduresConfig::ProceduresSettings::InterpreterMode;
    switch(ev->type()){
        case QEvent::MouseButtonPress:
        {
           if(obj == &generateCaplButton){
                generateCapl();
                break;
            }

            if(obj == &generateCaplRaportModeButton){
                generateCaplRaportMode();
                //tempUserProceduresConfig_.proceduresSettings().setMode(Mode::TestCaseReport);
                break;
            }
        }
        break;    
    default:
        break;
    }

    return Super::eventFilter(obj, ev);
}

bool
Tcl2CaplInstructionInstance::readInputConfig(){
    // Reader
    /*
    QString inputConfigText = inputConfig.toPlainText();
    if(not inputConfigText.isEmpty()){
        Tcl2CaplControllerConfigXmlData tempUserDefinitionsData;
        FSD_XML_TclCaplParserConfigInterpreter::Config parserConfigInterpreter(tempUserDefinitionsData);
        FileReaderInterpreter<FSD_XML_TclCaplParserConfigInterpreter::Parent_FSD> interpreter(parserConfigInterpreter);
        QXmlStreamReader reader(inputConfigText);
        parserConfigInterpreter.initialize();
        while(not reader.atEnd())
        {
            QXmlStreamReader::TokenType token = reader.readNext();
            int interpreterStat = static_cast<int>(FileReaderInterpreter_Priv_Stat::SIZE) + static_cast<int>(token);
            NS_FRI_COM_Manager::ChannelDatas cmds = {
                {NS_FRI_COM_Manager::Commands::EXEC_STAT, &interpreterStat},
                {NS_FRI_COM_Manager::Commands::DATA, &reader}
            };

            if(!interpreter.simulate_processCOMMessage(cmds) or parserConfigInterpreter.isError())
            {
                QMessageBox::critical(nullptr, "Błąd skryptu konfiguracyjnego", parserConfigInterpreter.error());

                qDebug() << parserConfigInterpreter.error();
;
                return false;
            }
        }
        parserConfigInterpreter.deinitialize();
        //tempUserProceduresConfig_ = tempUserDefinitionsData;
        TCLCommandsController::addDefaultProcedureDefinitionsToUserProcedureDefintions(tempUserProceduresConfig_);
    }else{
        //tempUserProceduresConfig_ = UserProceduresConfig();
    }
*/
    return true;

}

void Tcl2CaplInstructionInstance::generateCapl(){
    using TCLCommandsController = Tcl::Interpreter::Command::Controller;
    //if(not readInputConfig())
      //  return;

    FunctionDefinitions caplFunctionDefinitions;
    MainWindow::ConfigEditor& configEditor =  mainWindow().getConfigEditor(&subWindow());
    configEditor.syncConfig();
    UserInputConfig userInputConfig(configEditor.config());
    TCLCommandsController::addDefaultProcedureDefinitionsToUserProcedureDefintions(userInputConfig);
    TcFileModifier::Data data(userInputConfig, caplFunctionDefinitions);
    QFile file_DONT_USE_PH_ONLY;
    Tcl2CaplReadData resultData(QDir(), file_DONT_USE_PH_ONLY, userInputConfig, caplFunctionDefinitions);
    QString inputTclText = inputTclConfig.toPlainText();
    QString outputCaplText;
    QString errorText;
    if(inputTclText.isEmpty())
        return;
    QTextStream textStream(&inputTclText, QIODevice::ReadOnly);
    while(not textStream.atEnd())
    {
        data.lineData = textStream.readLine();
        if(data.tclToCaplInterpreter_.toCAPL(data.lineData) == Tcl::Error::Error){
            QMessageBox::critical(nullptr, QString("Tcl Interpreter"), QString("Internal Critical Error"));
            return;
        }
    }
    data.lineData = QString();
    if(data.tclToCaplInterpreter_.toCAPL(data.lineData) == Tcl::Error::Error){
        QMessageBox::critical(nullptr, QString("Tcl Interpreter"), QString("Internal Critical Error"));
        return;
    }
    errorsText.clear();
    outputText.clear();
    data.tclToCaplInterpreter_.printErrorReport(errorText);
    if(not errorText.isEmpty()){
        errorText.append(QString("</table>\n</div>\n</div>\n<hr>\n</body></html>").toUtf8());
        errorsText.setHtml(errorText);
        return;
    }

    // No Errors
    outputCaplText.append(data.tclToCaplInterpreter_.printPredefinitions() + data.tclToCaplInterpreter_.readCommand());
    outputText.setText(outputCaplText);


    /*if(inputDirPath.text().isEmpty())
        return;

    if(!QFileInfo(inputDirPath.text()).exists())
    {
        QMessageBox::critical(nullptr, "Input is missing", "Choose another input");
        inputDirPath.clear();
        return;
    }

    if(!outputDirPath.text().isEmpty() and !QFileInfo(outputDirPath.text()).exists())
    {
        QMessageBox::critical(nullptr, "Output Dir doesnt exist", "Output has been changed to Input Parent Folder");
        outputDirPath.clear();
        return;
    }*/


    /*if(!tcl2Capl.generateCaplsFromFolderWithTcls(inputDirPath.text(), outputDirPath.text()).isEmpty()){
        QMessageBox::critical(nullptr, "Generate Capl Error", tcl2Capl.error());
        tcl2Capl.clearError();
        return;
    }*/

    // Prepare Result DockWidget (FileInstanceResultPanel)
   /* FileInstanceResultPanel* instanceWidget = nullptr;
    try {
        instanceWidget = new FileInstanceResultPanel(*this, inputDirPath.text(), outputDirPath.text());
        QDockWidget* newInstance = new QDockWidget("tabName");
        newInstance->setWidget(instanceWidget);
        newInstance->setAttribute(Qt::WA_DeleteOnClose);
        addDockWidget(Qt::BottomDockWidgetArea, newInstance);
        show();

        instanceWidget->generateCapl(tempUserProceduresConfig_);
    }  catch (std::exception& e) {
        qDebug() << e.what();
        delete instanceWidget, instanceWidget = nullptr;
    }

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


void Tcl2CaplInstructionInstance::generateCaplRaportMode(){
    //using Mode = UserProceduresConfig::ProceduresSettings::InterpreterMode;
    if(not readInputConfig())
        return;
    //tempUserProceduresConfig_.proceduresSettings().setMode(Mode::TestCaseReport);

    FunctionDefinitions caplFunctionDefinitions;
    MainWindow::ConfigEditor& configEditor =  mainWindow().getConfigEditor(&subWindow());
    configEditor.syncConfig();
    UserInputConfig userInputConfig(configEditor.config());
    //TcFileModifier::Data data(userInputConfig, caplFunctionDefinitions);
    //QFile file_DONT_USE_PH_ONLY;
    //Tcl2CaplReadData resultData(QDir(), file_DONT_USE_PH_ONLY, tempUserProceduresConfig_);
    QString inputTclText = inputTclConfig.toPlainText();
    QString outputCaplText;
    QString errorText;
    if(inputTclText.isEmpty())
        return;
    QTextStream textStream(&inputTclText, QIODevice::ReadOnly);
    while(not textStream.atEnd())
    {
        //data.lineData = textStream.readLine();
        //if(data.tclToCaplInterpreter_.toCAPL(data.lineData) == TCLInterpreter::Error::Error){
            QMessageBox::critical(nullptr, QString("Tcl Interpreter"), QString("Internal Critical Error"));
            return;
        //}
    }
    errorsText.clear();
    outputText.clear();
    //data.tclToCaplInterpreter_.printErrorReport(errorText);
    if(not errorText.isEmpty()){
        errorText.append(QString("</table>\n</div>\n</div>\n<hr>\n</body></html>").toUtf8());
        errorsText.setHtml(errorText);
        return;
    }

    // No Errors
    //outputCaplText.append(data.tclToCaplInterpreter_.printPredefinitions() + "\n" + data.tclToCaplInterpreter_.readCaplCommand());
    outputText.setText(outputCaplText);


    /*if(inputDirPath.text().isEmpty())
        return;

    if(!QFileInfo(inputDirPath.text()).exists())
    {
        QMessageBox::critical(nullptr, "Input is missing", "Choose another input");
        inputDirPath.clear();
        return;
    }

    if(!outputDirPath.text().isEmpty() and !QFileInfo(outputDirPath.text()).exists())
    {
        QMessageBox::critical(nullptr, "Output Dir doesnt exist", "Output has been changed to Input Parent Folder");
        outputDirPath.clear();
        return;
    }

    // Prepare Result DockWidget (FileInstanceResultPanel)
    FileInstanceResultPanel* instanceWidget = nullptr;
    try {
        instanceWidget = new FileInstanceResultPanel(*this, inputDirPath.text(), outputDirPath.text());
        QDockWidget* newInstance = new QDockWidget("tabName");
        newInstance->setWidget(instanceWidget);
        newInstance->setAttribute(Qt::WA_DeleteOnClose);
        addDockWidget(Qt::BottomDockWidgetArea, newInstance);
        show();

        instanceWidget->generateCaplInWriteOnlyMode(tempUserProceduresConfig_);
    }  catch (std::exception& e) {
        qDebug() << e.what();
        delete instanceWidget, instanceWidget = nullptr;
    }
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

SubWindow& Tcl2CaplInstructionInstance::subWindow(){
    return *static_cast<SubWindow*>(parentWidget());
}

MainWindow& Tcl2CaplInstructionInstance::mainWindow(){
    return *static_cast<MainWindow*>(&(subWindow().mainWindow()));
}

