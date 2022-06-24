#ifndef FILEWATCHER_HPP
#define FILEWATCHER_HPP

#include<QThread>
#include<QDir>
#include<QFileInfoList>
#include<QTimer>
#include<QEvent>
#include"Tcl2CaplPanels/MainApp/Interpreters//tcl2caplfileparserinstance.hpp"

class FileWatcher : public QThread{
public:
    class Instance;
    using Instances = QList<Instance*>;
    /*
    using QThread::start;
    using QThread::quit;
    using QThread::wait;
    using QThread::QObject;*/
    class Instance{
        struct DirInfo{
            QFileInfo dir;
            QDateTime lastModified;
        };
        using DirInfos = QList<DirInfo*>;

        Tcl2CaplFileParserInstance* instance_ = nullptr;
        DirInfos directories;
        DirInfos filesList;

    public:

        Instance(Tcl2CaplFileParserInstance* instance, QFileInfo& rootFile);

        inline void addFile(QFileInfo& file, bool directory){
            Q_ASSERT_X(file.exists(), "FileWatcher", "addFile method: File doesnt exist");
            DirInfos::Iterator dirInfo = nullptr;
            DirInfos::Iterator end = nullptr;
            if(directory){
                dirInfo = directories.begin();
                end = directories.end();
            }else{
                dirInfo = filesList.begin();
                end = filesList.end();
            }
            for( ; dirInfo < end; dirInfo++){
                if((*dirInfo)->dir == file){
                    if(directory){
                        Q_ASSERT_X(false, "FileWatcher", "addFile method: Directory already registered");
                    }else{
                        Q_ASSERT_X(false, "FileWatcher", "addFile method: File already registered");
                    }
                    return;
                }
            }
            if(directory)
                directories.append(new DirInfo{file, file.lastModified()});
            else{
                filesList.append(new DirInfo{file, file.lastModified()});
            }
        }

        inline void editFile(QFileInfo& oldFile, QFileInfo& newFile, bool directory){
            if(directory){

            }else{

            }
        }
        inline void removeFile(QFileInfo& file, bool directory){
            Q_ASSERT_X(file.exists(), "FileWatcher", "removeFile method: File doesnt exist");
            DirInfos::Iterator dirInfo = nullptr;
            DirInfos::Iterator end = nullptr;
            if(directory){
                dirInfo = directories.begin();
                end = directories.end();
            }else{
                dirInfo = filesList.begin();
                end = filesList.end();
            }
            for( ; dirInfo < end; dirInfo++){
                if((*dirInfo)->dir == file){
                    delete (*dirInfo);
                    if(directory){
                        directories.removeAt(dirInfo - directories.begin());
                    }else{
                        filesList.removeAt(dirInfo - filesList.begin());
                    }
                    return;
                }
            }
            Q_ASSERT_X(dirInfo == end, "FileWatcher", "removeFile method: File not removed (not registered)");
        }
        inline Tcl2CaplFileParserInstance* instance()const{return instance_;}
        inline bool isInstance(Tcl2CaplFileParserInstance* instancePtr)const{return instancePtr == instance();}
        inline bool isActive(){return instance_->isVisible() and not instance_->nativeParentWidget()->isMinimized() ;}
        void check();
    };

    FileWatcher() : QThread()
    {
        execCheckTimer.installEventFilter(this);
        execCheckTimer.setInterval(1000);
        execCheckTimer.start();
    }

    ~FileWatcher(){
        execCheckTimer.stop();
        for(Instances::Iterator instance = instances.begin(); instance < instances.end(); instance++)
        {
            delete (*instance);
        }
    }


protected:   

    QTimer execCheckTimer;
    Instances instances;

    bool event(QEvent* ev)override;
    bool eventFilter(QObject* obj, QEvent* ev)override;

    inline Instance* instance(Tcl2CaplFileParserInstance* instance){
        Q_ASSERT_X(instance, "FileWatcher", "Instance method: Argument = nullptr");
        for(Instances::Iterator iter = instances.begin(); iter < instances.end(); iter++){
            if((*iter)->instance() == instance)   return (*iter);
        }
        return nullptr;
    }

    inline void newInstance(Tcl2CaplFileParserInstance* instance, QFileInfo& rootFile){
        Q_ASSERT_X(instance , "FileWatcher", "newInstance method: Argument = nullptr");
        Q_ASSERT_X(FileWatcher::instance(instance) == nullptr, "FileWatcher", "newInstance method: Instance already exists");
        instances.append(new Instance(instance, rootFile));
    }
    void removeInstance(Tcl2CaplFileParserInstance* instancePtr){
        Q_ASSERT_X(instancePtr, "FileWatcher", "RemoveInstance method: Argument = nullptr");
        for(Instances::Iterator instance = instances.begin(); instance < instances.end(); instance++){
            if((*instance)->isInstance(instancePtr)) {delete instance; instances.removeAt(instance - instances.begin());  break;}
        }
        Q_ASSERT_X(false, "FileWatcher", "RemoveInstance method: Not Found");
    }


    void check(Tcl2CaplFileParserInstance* = nullptr);


public:
    class NewInstanceEvent : public QEvent{

    public:
        using ErrorsNumber = const uint;
        using CriticalError = const QString;
        NewInstanceEvent(Tcl2CaplFileParserInstance* instance, QFileInfo& rootFile)
            : QEvent(eventType_static()), instance_(instance), rootFile_(rootFile) {}

        static constexpr QEvent::Type eventType_static(){return static_cast<QEvent::Type>(2000);}
        static NewInstanceEvent* cast(QEvent* ev){
            return (ev->type() == eventType_static())?
                        static_cast<NewInstanceEvent*>(ev)
                      :
                        nullptr;
        }
        inline Tcl2CaplFileParserInstance* instance()const{return instance_;}
        inline QFileInfo& rootFile(){return rootFile_;}

    protected:
        Tcl2CaplFileParserInstance* instance_ = nullptr;
        QFileInfo rootFile_;

    };

    class RemoveInstanceEvent : public QEvent{

    public:
        using ErrorsNumber = const uint;
        using CriticalError = const QString;
        RemoveInstanceEvent(Tcl2CaplFileParserInstance* instance)
            : QEvent(eventType_static()), instance_(instance) {}

        static constexpr QEvent::Type eventType_static(){return static_cast<QEvent::Type>(2001);}
        static RemoveInstanceEvent* cast(QEvent* ev){
            return (ev->type() == eventType_static())?
                        static_cast<RemoveInstanceEvent*>(ev)
                      :
                        nullptr;
        }
        inline Tcl2CaplFileParserInstance* instance()const{return instance_;}

    protected:
        Tcl2CaplFileParserInstance* instance_ = nullptr;

    };


    class NewFileEvent : public QEvent{

    public:
        using ErrorsNumber = const uint;
        using CriticalError = const QString;
        NewFileEvent(Tcl2CaplFileParserInstance* instance, QFileInfo& file, bool filesList)
            : QEvent(eventType_static()), instance_(instance), file_(file), filesList_(filesList) {}

        static constexpr QEvent::Type eventType_static(){return static_cast<QEvent::Type>(2002);}
        static NewFileEvent* cast(QEvent* ev){
            return (ev->type() == eventType_static())?
                        static_cast<NewFileEvent*>(ev)
                      :
                        nullptr;
        }
        inline Tcl2CaplFileParserInstance* instance()const{return instance_;}
        inline QFileInfo& newFile(){return file_;}
        inline bool filesList(){return filesList_;}

    protected:
        Tcl2CaplFileParserInstance* instance_ = nullptr;
        QFileInfo file_;
        bool filesList_;

    };

    class EditFileEvent : public QEvent{

    public:
        EditFileEvent(Tcl2CaplFileParserInstance* instance, QFileInfo& oldFile, QFileInfo& newFile, bool filesList)
            : QEvent(eventType_static()), instance_(instance), oldFile_(oldFile), newFile_(newFile), filesList_(filesList)  {}

        static constexpr QEvent::Type eventType_static(){return static_cast<QEvent::Type>(2003);}
        static EditFileEvent* cast(QEvent* ev){
            return (ev->type() == eventType_static())?
                        static_cast<EditFileEvent*>(ev)
                      :
                        nullptr;
        }
        inline Tcl2CaplFileParserInstance* instance()const{return instance_;}
        inline QFileInfo& oldFile(){return oldFile_;}
        inline QFileInfo& newFile(){return newFile_;}
        inline bool filesList(){return filesList_;}

    protected:
        Tcl2CaplFileParserInstance* instance_ = nullptr;
        QFileInfo oldFile_;
        QFileInfo newFile_;
        bool filesList_;

    };

    class RemoveFileEvent : public QEvent{

    public:
        RemoveFileEvent(Tcl2CaplFileParserInstance* instance, QFileInfo& file, bool filesList)
            : QEvent(eventType_static()), instance_(instance), file_(file) , filesList_(filesList) {}

        static constexpr QEvent::Type eventType_static(){return static_cast<QEvent::Type>(2004);}
        static RemoveFileEvent* cast(QEvent* ev){
            return (ev->type() == eventType_static())?
                        static_cast<RemoveFileEvent*>(ev)
                      :
                        nullptr;
        }
        inline Tcl2CaplFileParserInstance* instance()const{return instance_;}
        inline QFileInfo& file(){return file_;}
        inline bool filesList(){return filesList_;}

    protected:
        Tcl2CaplFileParserInstance* instance_ = nullptr;
        QFileInfo file_;
        bool filesList_;

    };


    class UpdateInstanceEvent : public QEvent{

    public:
        using ErrorsNumber = const uint;
        using CriticalError = const QString;
        UpdateInstanceEvent(Tcl2CaplFileParserInstance* instance = nullptr)
            : QEvent(eventType_static()), instance_(instance){}

        static constexpr QEvent::Type eventType_static(){return static_cast<QEvent::Type>(2006);}
        static UpdateInstanceEvent* cast(QEvent* ev){
            return (ev->type() == eventType_static())?
                        static_cast<UpdateInstanceEvent*>(ev) : nullptr;
        }
        inline Tcl2CaplFileParserInstance* instance()const{return instance_;}

    protected:
        Tcl2CaplFileParserInstance* instance_ = nullptr;

    };
};


#endif // FILEWATCHER_HPP
