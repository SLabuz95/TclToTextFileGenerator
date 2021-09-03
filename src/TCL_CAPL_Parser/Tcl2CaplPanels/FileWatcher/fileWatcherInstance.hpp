#ifndef FILEWATCHERINSTANCE_HPP
#define FILEWATCHERINSTANCE_HPP

#include<QEvent>

class FileWatcherInstancePriv{
public:
    enum class SectionType;
    
    enum class BasicEventType : uint{
        Register,
        Unregister,
        AddFWObject,
        EditFWObject,
        RemoveFWObject,
        Update,
        
        _Count_
    };
    
    
    
    static constexpr std::underlying_type_t<BasicEventType> cast2Base(BasicEventType type){
        return static_cast<std::underlying_type_t<BasicEventType>>(type);
    }
    /*
    template<BasicEventType eventType>
    class FWEventImpl{
    public:
        using EventType = BasicEventType;
        FWEventBase() : QEvent(eventType_static()){}
        static constexpr QEvent::Type eventType_static(){
            return static_cast<QEvent::Type>(2000 + cast2Base(eventType));
        }
    };
    
    
    template<FWEvent<BasicEventType>>
    class FWEventConfig : public QEvent{

    public:
        BasicEventType(Tcl2CaplFileParserInstance* instance, QFileInfo& rootFile)
            : QEvent(eventType_static()), instance_(instance), rootFile_(rootFile) {}

        static constexpr QEvent::Type eventType_static(){return static_cast<QEvent::Type>(2000 + event);}
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
    
    
    static 
    */
};

#endif // FILEWATCHERINSTANCE_HPP
