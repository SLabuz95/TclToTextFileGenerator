#ifndef TCL2CAPLFILEPARSERINSTANCE_HPP
#define TCL2CAPLFILEPARSERINSTANCE_HPP

#include<QMainWindow>
#include<QGroupBox>
#include<QVBoxLayout>
#include<QGridLayout>
#include<QLineEdit>
#include<QPushButton>
#include<QLabel>
//#include"Tcl2Capl/userdefinitions_helper.hpp"
#include"Tcl2Capl/controller.hpp"
#include<QDockWidget>
#include<QTreeWidget>
#include<QFileSystemModel>
#include<QFileIconProvider>
#include<QSplitter>
#include"FileParserInstanceLists/definitionsList.hpp"
#include"FileParserInstanceLists/inputsList.hpp"

//class Tcl2CaplFileParserPanel;
class SubWindow;
class MainWindow;
#include<QApplication>
#include"FileParserInstanceLists/filesList.hpp"
#include"FileParserInstanceLists/filesListElement.hpp"
class Tcl2CaplFileParserInstance : public QSplitter{
    using Super = QSplitter;

    using ConfigSplitter = QSplitter;
    using InputConfig = QGroupBox;
    using InputLayout = QVBoxLayout;
    using OutputBox = QGroupBox;
    using OutputLayout = QGridLayout;
    using DefinitionsTclInputsSplitter = QSplitter;
    using InputProcedureOutputLayout = QGridLayout;
    using InputProcedureConfigTitle = QLabel;
    using InputProcedureConfigPath = QLineEdit;
    using InputProcedureConfigSelection = QPushButton;
    using OutputDirTitle = QLabel;
    using OutputDirPath = QLineEdit;
    using OutputDirSelection = QPushButton;
    using ReadDefinitionsButton = QPushButton;
    using GenerateCaplButton = QPushButton;
    using GenerateCaplReportModeButton = QPushButton;

    const QString FILE_SELECTION_BUTTON_TEXT = QStringLiteral("..");
    const QString INPUT_CONFIG_BOX_TITLE = QStringLiteral("Ustawienia wejściowe");
    const QString OUTPUT_BOX_TITLE = QStringLiteral("Dane wyjściowe");
    const QString INPUT_DIR_TITLE = QStringLiteral("Folder TCL:");
    const QString INPUT_PROCEDURE_CONFIG_TITLE = QStringLiteral("Konfiguracja:");
    const QString OUTPUT_DIR_TITLE = QStringLiteral("Folder CAPL:");
    const QString READ_DEFINITIONS_BUTTON_TEXT = QStringLiteral("Odczytaj definicje");
    const QString GENERATE_CAPL_BUTTON_TEXT = QStringLiteral("Generuj skrypty CAPL");
    const QString GENERATE_CAPL_WRITE_ONLY_MODE_BUTTON_TEXT = QStringLiteral("Generuj skrypty CAPL - Tryb raportowy");


    //Tcl2CaplFileParserPanel& panel;
    //AppWindow& panel;
    FilesList filesList;
    ConfigSplitter configSplitter;
    InputConfig inputConfig;
    InputLayout inputLayout;
    OutputBox outputBox;
    OutputLayout outputLayout;
    DefinitionsTclInputsSplitter definitionsTclInputsSplitter;
    DefinitionsList definitionsList;
    InputsList inputsList;
    InputProcedureOutputLayout inputProcedureOutputLayout;
    InputProcedureConfigTitle inputProcedureConfigTitle;
    InputProcedureConfigPath inputProcedureConfigPath;
    InputProcedureConfigSelection inputProcedureConfigSelection;
    OutputDirTitle outputDirTitle;
    OutputDirPath outputDirPath;
    OutputDirSelection outputDirSelection;

    ReadDefinitionsButton readDefinitionsButton;
    GenerateCaplButton generateCaplButton;
    GenerateCaplReportModeButton generateCaplReportModeButton;

    // Temp Data from ConfigFile
    using UserProceduresConfig = UserInputConfig::UserInputConfigData;
   // UserProceduresConfig tempUserProceduresConfig_;

public:
    using Request_FileInfoRef_Func = void (Tcl2CaplFileParserInstance::*)(QFileInfo&);

    using Error = QString;
    Tcl2CaplFileParserInstance();
    ~Tcl2CaplFileParserInstance();

    void generateCapl();
    void generateCaplRaportMode();
    void setDefaultOutput();

    enum class Request_FileInfoRef{
        AddToDefinitions,
        AddToInputs,
        SetAsConfiguration,
        SetAsOutput,
        Size
    };

    template<Request_FileInfoRef>
    void execRequest_FileInfoRef(QFileInfo&);

    inline QString tabName(){return static_cast<QDockWidget*>(parentWidget())->windowTitle();}

protected:
    bool eventFilter(QObject*, QEvent*) override;
    Error readNewInputConfig(QFileInfo&);
    Error readNewInputConfig_dialog();
    Error readProceduresConfig(QFileInfo&);

    SubWindow& subWindow();
    MainWindow& mainWindow();
};

#endif // TCL2CAPLFILEPARSERINSTANCE_HPP
