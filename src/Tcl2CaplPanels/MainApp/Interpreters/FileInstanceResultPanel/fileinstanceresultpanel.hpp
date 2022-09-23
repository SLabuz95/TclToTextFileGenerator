#ifndef FILEINSTANCERESULTPANEL_HPP
#define FILEINSTANCERESULTPANEL_HPP

#include<QWidget>
#include<QVBoxLayout>
#include<QTreeWidget>
//#include"Tcl2Capl/Tcl2Capl.hpp"
#include"Tcl2Capl/controller.hpp"
#include<QProgressBar>
#include<QSplitter>
#include<Tcl2Capl/caplFunctionDefiniitions.hpp>
#include"analysisPanel.hpp"

class Tcl2CaplFileParserInstance;
class FileInstanceResultPanel : public QWidget{
    using UserProceduresConfig = UserInputConfig::UserInputConfigData;
    struct ProgressBarData{
        QTreeWidgetItem* item = nullptr;
        int maxLength = 0;
    };
    using ProgressBarDatas = QList<ProgressBarData>;

    struct GeneratorControl{
        bool inProgress = false;
        struct ChildControl{
            QTreeWidgetItem* item = nullptr;
            int childIndex = 0;
        };
        using ChildrenControl = QVector<ChildControl>;
        ChildrenControl children;
    } generatorControl;

    using Super = QWidget;
    using MainSplitter = QSplitter;
    using MainLayout = QVBoxLayout;
    using ErrorsList = QTreeWidget;
    using ProgressBars = QVarLengthArray<QProgressBar>;
    using CriticalError = bool;

    FileInstanceResultPanel(Tcl2CaplFileParserInstance& instance,
                            QStringList& definitions,
                            QStringList& inputPath,
                            QString& outputPath,
                            ProgressBarDatas progressBarDatas);

public:
    inline FileInstanceResultPanel(Tcl2CaplFileParserInstance& instance,
                                   QStringList& definitions,
                                   QStringList& inputPath,
                                   QString&& outputPath)
        : FileInstanceResultPanel(instance, definitions, inputPath, outputPath, ProgressBarDatas{})
    {}
    ~FileInstanceResultPanel(){
        tcl2Capl.requestInterruption();
        endGeneratorControl();
    }

protected:
    Tcl2CaplFileParserInstance& instance;

    Tcl2CaplController tcl2Capl;

    MainLayout mainLayout;
    MainSplitter mainSplitter;
    ErrorsList errorsList;
    AnalysisPanel analysisPanel;
    ProgressBars progressBars;
    FunctionDefinitions caplFunctionDefinitions;
    CriticalError criticalError_ = false;
    //QProgressBar* progressBars_data = nullptr;
    //int progressBars_size = 0;

    bool eventFilter(QObject *obj, QEvent *ev)override;

    void prepareResultData(ProgressBarDatas&);
    void configureProgressBars(ProgressBarDatas&);

    void prepareGeneratorControl();
    void nextGeneratorControl(const QString&, uint errorsCount);
    void endGeneratorControl();

public:
    void generateCapl(UserProceduresConfig&);
    void generateCaplInWriteOnlyMode(UserProceduresConfig&);
    inline CriticalError criticalError()const{return criticalError_;}

};

#endif // FILEINSTANCERESULTPANEL_HPP
