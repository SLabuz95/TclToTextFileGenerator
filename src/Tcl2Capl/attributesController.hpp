#ifndef ATTRIBUTESCONTROLLER_HPP
#define ATTRIBUTESCONTROLLER_HPP

#include"controllerconfiginfo.hpp"

class AttributesController{
public:
    using Attributes = ControllerConfigInfo::Attributes;

    enum class Predefined{
        ProcessModelStatus,
        FilePrefix,
        FilePostfix,
        FileName,
        DirName,

        _Size
    };

    static const Attributes PredefinedAttributes;

private:
    Attributes attributes_;

public:
    AttributesController(){}
    AttributesController(Attributes& attributes);

    inline Attributes::ConstIterator getAttribute(QString& name){
        return attributes_.find(name);
    }
    inline Attributes::ConstIterator getAttribute(QString&& name){
        return attributes_.find(name);
    }
    inline bool isAttributeValid(Attributes::ConstIterator attribute){return attribute != attributes_.constEnd();}
    inline void setAttribute(QString& name, QString& value){
        attributes_.insert(name, Attributes::mapped_type{value});
    }
    inline void setAttribute(const QString& name, QString& value){
        attributes_.insert(name, Attributes::mapped_type{value});
    }
    inline void setAttribute(const QString& name,const QString& value){
        attributes_.insert(name, Attributes::mapped_type{value});
    }

    inline void setAttribute(QString&& name, QString&& value){
        attributes_.insert(name, Attributes::mapped_type{value});
    }
};

#endif // ATTRIBUTESCONTROLLER_HPP
