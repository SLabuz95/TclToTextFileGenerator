#include"fileinstanceresultpanel.hpp"
#include<QApplication>
#include<QProgressBar>
#include<QDirIterator>
#include"Tcl2Capl/tcl2caplprogressevent.hpp"
#include<QMessageBox>
#include"Tcl2CaplPanels/MainApp/Interpreters/tcl2caplfileparserinstance.hpp"

FileInstanceResultPanel::FileInstanceResultPanel
(Tcl2CaplFileParserInstance& instance,
 QStringList& definitions,
 QStringList& inputPath,
 QString& outputPath,
 ProgressBarDatas progressBarDatas)
    : instance(instance),
      mainSplitter(Qt::Vertical),
      tcl2Capl(definitions, inputPath, outputPath),
      progressBars(
          // Arg1: Prepare data list, then pass list size to progressBars constructor
          /* arg1 */ ( prepareResultData(progressBarDatas), progressBarDatas.size())
          ),
      analysisPanel(*this, caplFunctionDefinitions)
{
    // Duplicate Windows title
    setWindowTitle(instance.tabName());

    // - configure progressBars
    configureProgressBars(progressBarDatas);

    // - errorsList
    errorsList.setHeaderLabels(QStringList({"File", "Errors", "Progress"}));


    mainSplitter.addWidget(&errorsList);
    mainSplitter.addWidget(&analysisPanel);

    mainLayout.addWidget(&mainSplitter);

    // - mainLayout
    setLayout(&mainLayout);

    installEventFilter(this);
}

bool FileInstanceResultPanel::eventFilter(QObject *obj, QEvent *ev){
    if(ev->type() == Tcl2CaplProgressEvent::eventType_static()
            and generatorControl.inProgress){
        Tcl2CaplProgressEvent* progressEv = Tcl2CaplProgressEvent::cast(ev);
        if(progressEv){
            if(progressEv->isReady()){
                caplFunctionDefinitions = progressEv->caplFunctionDefinitions();
                analysisPanel.reloadGui();
            }else{
                if(progressEv->isCriticalError()){
                    endGeneratorControl();
                    QMessageBox::critical(nullptr, "Generate Capl Error", progressEv->criticalError());
                    //criticalError_ = true;
                    tcl2Capl.requestInterruption();
                    delete this;
                    return false;
                }else{
                    nextGeneratorControl(progressEv->errorMsg(), progressEv->getErrorsNumber());
                }
            }


        }
    }

    return Super::eventFilter(obj, ev);
}

void FileInstanceResultPanel::prepareResultData(ProgressBarDatas& readyProgressBarDatas){
    //int currentLevel = 0;

    errorsList.addTopLevelItem(new QTreeWidgetItem(
                                   QStringList{
                                       QDir(tcl2Capl.outputDirPath()).dirName(),
                                        "0", "Pending"}));

    /* [0.9.0]
    using NameFilters = const QStringList;
    NameFilters permittedDefinitionSuffixs = QStringList{"tcl"};
    NameFilters permittedInputSuffixs = QStringList{"tc"};
    int tempLength = 0;

    ProgressBarDatas progressBarDatas;
    //QDir inputDir = tcl2Capl.getInputDir();

    {   // Main Progress - Overall
        errorsList.addTopLevelItem(new QTreeWidgetItem(
                                       QStringList{
                                           QDir(tcl2Capl.outputDirPath()).dirName(),
                                            "0", "Pending"}));
        progressBarDatas.append(ProgressBarDatas::Type{errorsList.topLevelItem(0)});
        // + Definitions - ProgressBar if exists

    }
    // + Definitions - ProgressBar if exists
    QTreeWidgetItem* sectionItem = nullptr;
    if(not tcl2Capl.definitionPaths().isEmpty()){
        errorsList.topLevelItem(0)->addChild(sectionItem = new QTreeWidgetItem(
                                       QStringList{
                                           "Definitions", "0", "Pending"}));
        progressBarDatas.append(ProgressBarDatas::Type{sectionItem});
    }

    for(QStringList::Iterator definitionPath = tcl2Capl.definitionPaths().begin();
        definitionPath < tcl2Capl.definitionPaths().end(); definitionPath++)
    {
        // Preconditions - for failed - break - if doesnt exist, add Ignored item
        QFileInfo&& fileInfo = QFileInfo(*definitionPath);
        if(not fileInfo.exists()){
            sectionItem->addChild(new QTreeWidgetItem(
                                           QStringList{
                                               fileInfo.fileName(),
                                               "0", "Ignored"}));
            progressBarDatas.last().maxLength++;
            break; // !!!!!!! BREAK LOOP
        }

        // If file
        if(QFileInfo(*definitionPath).isFile()){
            sectionItem->addChild(new QTreeWidgetItem(
                                           QStringList{
                                               fileInfo.fileName(),
                                               "0", "Not Ready"}));
            progressBarDatas.last().maxLength++;
        }else{
            // Search through definitions dir to configurate error list
            QVector<QDirIterator*> dirs({new QDirIterator(QDir(*definitionPath))});
            QVector<QTreeWidgetItem*> treeWidgetItems({sectionItem});

            while(!dirs.isEmpty()){
                while(dirs.last()->hasNext()){
                    QFileInfo fileInfo(dirs.last()->next());
                    if(not fileInfo.exists()){
                        treeWidgetItems.last()->addChild( new QTreeWidgetItem(
                                                       QStringList{
                                                           fileInfo.fileName(),
                                                            "0", "Error - File doesnt exist"}));
                        progressBarDatas.last().maxLength++;
                    }else{
                        // Directory
                        if(fileInfo.isDir()){
                            QDir dir(fileInfo.filePath());
                            dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
                            dir.setSorting(QDir::Name);
                            dirs.append(new QDirIterator(dir));
                            treeWidgetItems.last()->addChild( (treeWidgetItems += new QTreeWidgetItem(
                                        QStringList{
                                            dir.dirName(),
                                            "0", "Pending"})).last());
                            progressBarDatas.append(ProgressBarDatas::Type{treeWidgetItems.last()});
                        }else{
                            if(fileInfo.isFile() and permittedDefinitionSuffixs.contains(fileInfo.completeSuffix())){
                                treeWidgetItems.last()->addChild( new QTreeWidgetItem(
                                                                      QStringList{
                                                                          fileInfo.fileName(),
                                                                          "0", "Pending"}));
                                progressBarDatas.last().maxLength++;
                            }
                        }
                    }
                }
                delete dirs.takeLast();
                //currentLevel--;
                if(progressBarDatas.last().maxLength == 0)
                {   // No Files or Empty Directories in this directory
                    if(treeWidgetItems.size() == 1){
                        errorsList.takeTopLevelItem(errorsList.topLevelItemCount() - 1);
                    }else{
                        treeWidgetItems.last()->parent()->removeChild(treeWidgetItems.last());
                    }
                    delete treeWidgetItems.takeLast();
                }else{
                    readyProgressBarDatas.append(progressBarDatas.last());
                    treeWidgetItems.removeLast();
                }
                tempLength = progressBarDatas.takeLast().maxLength;
                if(!progressBarDatas.isEmpty())
                    progressBarDatas.last().maxLength += tempLength;

            }
        }
    }

    // + Inputs - ProgressBar if exists
    errorsList.topLevelItem(0)->addChild(sectionItem = new QTreeWidgetItem(
                                   QStringList{
                                       "Inputs", "0", "Pending"}));
    progressBarDatas.append(ProgressBarDatas::Type{sectionItem});

    for(QStringList::Iterator input = tcl2Capl.inputPaths().begin();
        input < tcl2Capl.inputPaths().end(); input++)
    {
       // if input doesnt exist - critical
        QFileInfo&& inputFile = QFileInfo(*input);
        if(not inputFile.exists()){
            sectionItem->addChild(new QTreeWidgetItem(
                                           QStringList{
                                               inputFile.fileName(),
                                               "0", "Ignored"}));
            progressBarDatas.last().maxLength++;
        }else{
            // If file
            if(inputFile.isFile()){
                sectionItem->addChild(new QTreeWidgetItem(
                                               QStringList{
                                                   inputFile.fileName(),
                                                   "0", "Not Ready"}));
                progressBarDatas.last().maxLength++;
            }else{
                // Search through input dir to configurate error list
                QVector<QDirIterator*> dirs({new QDirIterator(*input)});
                QVector<QTreeWidgetItem*> treeWidgetItems({sectionItem});

                while(!dirs.isEmpty()){
                    while(dirs.last()->hasNext()){
                        QFileInfo fileInfo(dirs.last()->next());
                        // Directory
                        if(fileInfo.isDir()){
                            QDir dir(fileInfo.filePath());
                            dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
                            dir.setSorting(QDir::Name);
                            dirs.append(new QDirIterator(dir));
                            treeWidgetItems.last()->addChild( (treeWidgetItems += new QTreeWidgetItem(
                                                           QStringList{
                                                               dir.dirName(),
                                                                "0", "Pending"})).last());
                            progressBarDatas.append(ProgressBarDatas::Type{treeWidgetItems.last()});
                        }else{
                            if(fileInfo.isFile() and permittedInputSuffixs.contains(fileInfo.completeSuffix())){
                                treeWidgetItems.last()->addChild( new QTreeWidgetItem(
                                                               QStringList{
                                                                   fileInfo.fileName(),
                                                                    "0", "Pending"}));
                                progressBarDatas.last().maxLength++;
                            }
                        }
                    }
                    delete dirs.takeLast();
                    //currentLevel--;
                    if(progressBarDatas.last().maxLength == 0)
                    {   // No Files or Empty Directories in this directory
                        if(treeWidgetItems.size() == 1){
                            errorsList.takeTopLevelItem(errorsList.topLevelItemCount() - 1);
                        }else{
                            treeWidgetItems.last()->parent()->removeChild(treeWidgetItems.last());
                        }
                        delete treeWidgetItems.takeLast();
                    }else{
                        readyProgressBarDatas.append(progressBarDatas.last());
                        treeWidgetItems.removeLast();
                    }
                    tempLength = progressBarDatas.takeLast().maxLength;
                    if(!progressBarDatas.isEmpty())
                        progressBarDatas.last().maxLength += tempLength;

                }
            }
        }
    }

    //qDebug() << formatCount;
    [0.9.0] End */
}

void FileInstanceResultPanel::configureProgressBars(ProgressBarDatas& readyProgressBarDatas)
{
    /*
    // Configurate Error List Progress Bars
    if(!readyProgressBarDatas.isEmpty()){

        using ProgressBar = ProgressBars::iterator;
        using ProgressBarData = ProgressBarDatas::Iterator;

        // Loop Init
        {
            ProgressBar curProgressBar = progressBars.begin();
            for(ProgressBarData data = readyProgressBarDatas.begin();
                data < readyProgressBarDatas.end(); data++, curProgressBar++)
            {
                curProgressBar->setMaximum(data->maxLength);
                curProgressBar->setValue(0);
                errorsList.setItemWidget(data->item, 2, curProgressBar);
            }
        }

    }*/
}

void FileInstanceResultPanel::prepareGeneratorControl(){
    generatorControl.inProgress = true;
    //generatorControl.children.append({errorsList.topLevelItem(0)});
}

void FileInstanceResultPanel::nextGeneratorControl(const QString& errorMsg, uint errorsCount){   // On End of File read
    //WARNING: Temporarly changed
    const char PROGRESS_SIGN = '-';
    const int MAX_NUMB_OF_PROGRESS_SIGNS = 10;
    constexpr int PENDING_STRING_LENGTH = QAnyStringView("Pending").size();
    // Just overrride "Pending" with "Pending" + number_of_Progress_signs
    // At 2 column (Progress info)

    if(errorMsg == "END"){
        endGeneratorControl();
        errorsList.topLevelItem(0)->setText(2, "Done");
    }else{
        if(errorsCount > 0)
            errorsList.topLevelItem(0)->setText(1,QString::number(errorsList.topLevelItem(0)->text(1).toInt() + errorsCount));
        errorsList.topLevelItem(0)->setText(2, QString("Pending") +
                    QString(((errorsList.topLevelItem(0)->text(2).size() - PENDING_STRING_LENGTH) + 1) % MAX_NUMB_OF_PROGRESS_SIGNS, PROGRESS_SIGN)
                    );
    }


    //errorsList.setUpdatesEnabled(false);
    /*GeneratorControl::ChildControl* tempChild =
            (not generatorControl.children.isEmpty())?
                &generatorControl.children.last() : nullptr;
    while(not generatorControl.children.isEmpty()){
        // Find File
        if(tempChild->childIndex < tempChild->item->childCount()){
            if(tempChild->item->child(tempChild->childIndex)->childCount() != 0){
                // Folder
                generatorControl.children.append({tempChild->item->child(tempChild->childIndex)});
                tempChild->childIndex++;
                tempChild = &generatorControl.children.last();
            }else{
                tempChild->item->child(tempChild->childIndex)->setText(1, QString::number(errorsCount));
                tempChild->item->child(tempChild->childIndex)->setText(2, "Ready");
                tempChild->childIndex++;
                break;  // !!!!!!! FILE HAS BEEN FOUND _Break_Loop
            }
        }else{
            generatorControl.children.removeLast();
            // Reinitilize
            tempChild =
                (not generatorControl.children.isEmpty())?
                    &generatorControl.children.last() : nullptr;
        }
    }

    if(not tempChild){  // Finialize
        endGeneratorControl();
    }else{
        // Update Progress Bars
        for(GeneratorControl::ChildrenControl::Iterator child = generatorControl.children.begin();
            child < generatorControl.children.end(); child++)
        {
            if(errorsCount > 0)
                child->item->setText(1, QString::number(child->item->text(1).toUInt() + errorsCount));
            // Just increment
            static_cast<QProgressBar*>(errorsList.itemWidget(child->item, 2))->setValue(static_cast<QProgressBar*>(errorsList.itemWidget(child->item, 2))->value() + 1);

        }
    }*/
    //errorsList.setUpdatesEnabled(true);
}

void FileInstanceResultPanel::endGeneratorControl(){
    generatorControl.inProgress = false;
    generatorControl.children.clear();
    //tcl2Capl.quitAndWait();
}

void FileInstanceResultPanel::generateCapl(UserProceduresConfig& userConfig){
    prepareGeneratorControl();
    tcl2Capl.generateCaplsFromFolderWithTcls(this, userConfig);
}

void FileInstanceResultPanel::generateCaplInWriteOnlyMode(UserProceduresConfig& userConfig){
    //using Mode = UserProceduresConfig::ProceduresSettings::InterpreterMode;
    prepareGeneratorControl();
    tcl2Capl.generateCaplsFromFolderWithTcls(this, userConfig, Mode::TestCaseReport);
}

