QT += core widgets xml
TEMPLATE = app
TARGET = "TCL_CAPL_Parser"
VERSION_PE_HEADER = 0.8.0
#QMAKE_CXXFLAGS += -std=c++2a
CONFIG += c++2a
QMAKE_CXXFLAGS += -fconcepts
CFLAGS += -Wunused-function -Wswitch

INCLUDEPATH += External

HEADERS += \
    External/Factory/factory.hpp \
    External/FileReader/COMManager/FRI_COM_Manager.hpp \
    External/FileReader/FilesSpecificData/FSDConfig.hpp \
    External/FileReader/FilesSpecificData/FSDDataModelBase.hpp \
    External/FileReader/FilesSpecificData/FSDTemplate.hpp \
    External/FileReader/FilesSpecificData/FSD_Config_Control.hpp \
    External/FileReader/FilesSpecificData/FSD_Config_Priv.hpp \
    External/FileReader/FilesSpecificData/XML/FRI_FSD_XML.hpp \
    External/FileReader/FilesSpecificData/XML/TclCaplParserConfig/FRI_FSD_XML_TCL_CAPL_Parser_Config.hpp \
    External/FileReader/Interpreter/XML/xmlfilereaderinterpreter.hpp \
    External/FileReader/Interpreter/filereaderinterpreter.hpp \
    External/FileReader/Interpreter/filereaderinterpreterpriv.hpp \
    External/FileReader/Interpreter/filereaderinterpreterprivreadinfo.hpp \
    External/FileReader/Interpreter/statsconfiguration.hpp \
    External/FileReader/Reader/XMLFileReader/xmlfilereader.hpp \
    External/FileReader/Reader/filereader.hpp \
    External/FileReader/Reader/filereaderconfiguration.hpp \
    External/FileReader/Reader/filereaderpriv.hpp \
    External/FileReader/Reader/filereaderprivreadinfo.hpp \
    External/Improvements/class.hpp \
    External/RegExpCore/regexprcore.hpp \
    External/StartUpError/startuperror.hpp \
    External/Utils/class.hpp \
    TcFileModifier/tcfilemodifier.hpp \
    TcFileModifier/tcfilemodifierdata.hpp \
    TcFileModifier/tcfilereader.hpp \
    TcFileModifier/tcfilereaderinterpreter.hpp \
    Tcl2Capl/Config/actions.hpp \
    Tcl2Capl/Config/parameters.hpp \
    Tcl2Capl/Config/rules.hpp \
    Tcl2Capl/Result/tcl2caplresult.hpp \
    Tcl2Capl/caplFunctionDefiniitions.hpp \
    Tcl2Capl/controller.hpp \
    Tcl2Capl/controllerconfig.hpp \
    Tcl2Capl/controllerconfiginfo.hpp \
    Tcl2Capl/controllerconfigmanager.hpp \
    Tcl2Capl/controllerconfigxmldata.hpp \
    Tcl2Capl/corruptedconfiginfoevent.hpp \
    Tcl2Capl/tcl2caplprogressevent.hpp \
    Tcl2CaplPanels/ConfigEditor/configEditor.hpp \
    Tcl2CaplPanels/ConfigEditor/fileconfigpanel.hpp \
    Tcl2CaplPanels/ConfigEditor/loadconfigsettings.hpp \
    Tcl2CaplPanels/ConfigEditor/proceduresList.hpp \
    Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.hpp \
    Tcl2CaplPanels/ConfigEditor/writeOnlyProceduresList.hpp \
    Tcl2CaplPanels/FileInstanceResultPanel/fileinstanceresultpanel.hpp \
    Tcl2CaplPanels/FileParserInstanceLists/definitionsList.hpp \
    Tcl2CaplPanels/FileParserInstanceLists/filesList.hpp \
    Tcl2CaplPanels/FileParserInstanceLists/filesListElement.hpp \
    Tcl2CaplPanels/FileParserInstanceLists/filesListRootSelection.hpp \
    Tcl2CaplPanels/FileParserInstanceLists/inputsList.hpp \
    Tcl2CaplPanels/FileWatcher/fileWatcher.hpp \
    Tcl2CaplPanels/FileWatcher/fileWatcherInstance.hpp \
    Tcl2CaplPanels/tcl2caplfileparserinstance.hpp \
    Tcl2CaplPanels/tcl2caplfileparserpanel.hpp \
    Tcl2CaplPanels/tcl2caplinstructioninstance.hpp \
    Tcl2CaplPanels/tcl2caplinstructionpanel.hpp \
    app.hpp \
    TcFileModifier/tcfilereaderconfig.hpp \
    appwindow.hpp \
    tclToCAPL.hpp \
    tclToCAPL_errorFileConfig.hpp \
    tclinterpreterpriv.hpp

SOURCES += \
    External/FileReader/FilesSpecificData/FSDConfig.cpp \
    External/FileReader/FilesSpecificData/XML/FRI_FSD_XML.cpp \
    External/FileReader/FilesSpecificData/XML/TclCaplParserConfig/FRI_FSD_XML_TCL_CAPL_Parser_Config.cpp \
    External/FileReader/Interpreter/XML/xmlfilereaderinterpreter.cpp \
    External/FileReader/Interpreter/filereaderinterpreterpriv.cpp \
    External/FileReader/Interpreter/statsconfiguration.cpp \
    External/FileReader/Reader/XMLFileReader/xmlfilereader.cpp \
    External/FileReader/Reader/filereader.cpp \
    External/FileReader/Reader/filereaderpriv.cpp \
    External/FileReader/Reader/filereaderprivreadinfo.cpp \
    TcFileModifier/tcfilemodifier.cpp \
    TcFileModifier/tcfilemodifierdata.cpp \
    TcFileModifier/tcfilereader.cpp \
    TcFileModifier/tcfilereaderconfig.cpp \
    TcFileModifier/tcfilereaderinterpreter.cpp \
    Tcl2Capl/Config/actions.cpp \
    Tcl2Capl/Config/rules.cpp \
    Tcl2Capl/Result/tcl2caplresult.cpp \
    Tcl2Capl/caplfunctiondefinitions.cpp \
    Tcl2Capl/controller.cpp \
    Tcl2Capl/controllerconfig.cpp \
    Tcl2Capl/controllerconfiginfo.cpp \
    Tcl2Capl/controllerconfigmanager.cpp \
    Tcl2CaplPanels/ConfigEditor/configeditor.cpp \
    Tcl2CaplPanels/ConfigEditor/fileconfigpanel.cpp \
    Tcl2CaplPanels/ConfigEditor/procedureslist.cpp \
    Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.cpp \
    Tcl2CaplPanels/ConfigEditor/writeonlyprocedureslist.cpp \
    Tcl2CaplPanels/FileInstanceResultPanel/fileinstanceresultpanel.cpp \
    Tcl2CaplPanels/FileParserInstanceLists/definitionsList.cpp \
    Tcl2CaplPanels/FileParserInstanceLists/fileslist.cpp \
    Tcl2CaplPanels/FileParserInstanceLists/fileslistelement.cpp \
    Tcl2CaplPanels/FileParserInstanceLists/fileslistrootselection.cpp \
    Tcl2CaplPanels/FileParserInstanceLists/inputslist.cpp \
    Tcl2CaplPanels/FileWatcher/filewatcher.cpp \
    Tcl2CaplPanels/tcl2caplfileparserinstance.cpp \
    Tcl2CaplPanels/tcl2caplfileparserpanel.cpp \
    Tcl2CaplPanels/tcl2caplinstructioninstance.cpp \
    Tcl2CaplPanels/tcl2caplinstructionpanel.cpp \
    app.cpp \
    appwindow.cpp \
    main.cpp \
    tclToCAPL.cpp \
    test.cpp


