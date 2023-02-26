#include"fileWatcher.hpp"
#include"Tcl2CaplPanels/MainApp/Interpreters/FileParserInstanceLists/filesList.hpp"
FileWatcher::Instance::Instance(Tcl2CaplFileParserInstance* instance, QFileInfo& rootFile)
    : instance_(instance)
{    
    directories = {new DirInfo{rootFile, rootFile.lastModified()}};
}


bool FileWatcher::event(QEvent *ev){
    switch(ev->type()){
    case NewInstanceEvent::eventType_static():
    {
        NewInstanceEvent* myEv = NewInstanceEvent::cast(ev);
        Q_ASSERT_X(myEv and myEv->instance() and myEv->rootFile().exists(), "FileWatcher", "NewInstanceEvent Error");
        newInstance(myEv->instance(), myEv->rootFile());
    }
        break;
    case RemoveInstanceEvent::eventType_static():
    {
        RemoveInstanceEvent* myEv = RemoveInstanceEvent::cast(ev);
        Q_ASSERT_X(myEv and myEv->instance(), "FileWatcher", "RemoveInstanceEvent Error");
        removeInstance(myEv->instance());
    }
        break;
    case NewFileEvent::eventType_static():
    {
        NewFileEvent* myEv = NewFileEvent::cast(ev);
        Q_ASSERT_X(myEv and myEv->instance() and myEv->newFile().exists(), "FileWatcher", "NewFileEvent Error");
        instance(myEv->instance())->addFile(myEv->newFile(), myEv->filesList());
    }
        break;
    case EditFileEvent::eventType_static():
    {
        EditFileEvent* myEv = EditFileEvent::cast(ev);
        Q_ASSERT_X(myEv and myEv->instance() and myEv->newFile().exists() and myEv->oldFile().exists(), "FileWatcher", "EditFileEvent Error");
        instance(myEv->instance())->editFile(myEv->oldFile(), myEv->newFile(), myEv->filesList());
    }
        break;
    case RemoveFileEvent::eventType_static():
    {
        RemoveFileEvent* myEv = RemoveFileEvent::cast(ev);
        Q_ASSERT_X(myEv and myEv->instance() and myEv->file().exists(), "FileWatcher", "RemoveFileEvent Error");
        instance(myEv->instance())->removeFile(myEv->file(), myEv->filesList());
    }
        break;
    case UpdateInstanceEvent::eventType_static():
    {
        UpdateInstanceEvent* myEv = UpdateInstanceEvent::cast(ev);
        Q_ASSERT_X(myEv , "FileWatcher", "RemoveFileEvent Error");
        execCheckTimer.stop();
        check(myEv->instance());
        execCheckTimer.start();
    }
        break;
    default:
        break;
    }
    return QThread::event(ev);
}


bool FileWatcher::eventFilter(QObject *obj, QEvent *ev){
    if(obj == &execCheckTimer and ev->type() == QEvent::Timer){
        check();
    }

    return QThread::eventFilter(obj, ev);
}

void FileWatcher::check(Tcl2CaplFileParserInstance* panel){
    static int i = 0;
    //qDebug() << "Check" << i++;
    if(panel){ // Specific Panel Instance
        instance(panel)->check();
    }else{
        for(Instances::Iterator insIter = instances.begin(); insIter < instances.end(); insIter++)
        {
            if((*insIter)->isActive())
                (*insIter)->check();
        }
    }
}

void FileWatcher::Instance::check(){
    bool requested_Update = false;
    for(DirInfos::Iterator dir = directories.begin(); dir < directories.end(); dir++){
        (*dir)->dir.refresh();
        if(not (*dir)->dir.exists() or (*dir)->dir.lastModified() != (*dir)->lastModified){
            (*dir)->lastModified = (*dir)->dir.lastModified();
            if(not requested_Update){
                //TODO: Post Request
                requested_Update = true;
            }
        }
    }
    for(DirInfos::Iterator file = filesList.begin(); file < filesList.end(); file++){
        (*file)->dir.refresh();
        if(not (*file)->dir.exists() or (*file)->dir.lastModified() != (*file)->lastModified){
            (*file)->lastModified = (*file)->dir.lastModified();
            if(not requested_Update){
                //TODO: Post Request
                requested_Update = true;
            }
        }
    }
}
