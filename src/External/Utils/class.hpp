#ifndef CLASS_PRIV_HPP
#define CLASS_PRIV_HPP

#define VARIABLE_PROTECTED(type, name) protected: type name##_;\
    public: inline type& name(){return name##_;}

#define VARIABLE_PROTECTED_WITH_INIT(type, name, init) protected: type name##_ = init;\
    public: inline type& name(){return name##_;}

#define VARIABLE_PROTECTED_WITH_SET(type, name) VARIABLE_PROTECTED(type, name)\
    public: void set_##name(type& __new){name##_ = __new;}

#define WIDGET_PARENT_METHODS_DECL(className) className* parent();\
       className* parentWidget();

#define WIDGET_PARENT_METHODS_DEF(declclass, declBaseClass, className) className* declclass::parent(){return static_cast<className*>(declBaseClass::parent());}\
    className* declclass::parentWidget(){return static_cast<className>(declBaseClass::parentWidget());}

#define WIDGET_ADD_EVENT_FILTER bool eventFilter(QObject* obj, QEvent* ev)override;

#endif // CLASS_PRIV_HPP
