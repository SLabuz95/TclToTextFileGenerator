#ifndef NAVIGATIONLIST_HPP
#define NAVIGATIONLIST_HPP

#include<QTreeWidget>
#include"External/Utils/class.hpp"

namespace  Panels::Configuration::View{
    class ConfigViewPanel;
}

namespace Panels::Configuration::Navigation{

    enum class PanelType{
        AttributesList,
        WriteOnlyProceduresList,
        Procedures,
        DefaultProcedure,
        Size,
        Invalid = Size
    };

    enum class EventType{
        Added,
        Changed,
        Removed,
        Cleared,
        Loaded,
        Moved,
        Size,
        Invalid = Size
    };


    constexpr static std::underlying_type_t<PanelType> panelType2number(PanelType mode){
        return static_cast<std::underlying_type_t<PanelType>>(mode);
    }
    constexpr static PanelType number2panelType(int mode){
        return (mode >= 0 and mode < panelType2number(PanelType::Size))?
                    static_cast<PanelType>(mode) : PanelType::Invalid;
    }

    class NavigationElement : public QTreeWidgetItem{
    public:
        inline NavigationElement(QTreeWidget *parent, const QStringList &strings, int type = Type)
            : QTreeWidgetItem(parent, strings, type)
        {}

    };

    class List : public QTreeWidget{

    public:
        static const QString navigationPanelNames[panelType2number(PanelType::Size)];
        using NavigationElementChildFctPtr = void (List::*)(NavigationElement* const);
    private:
        using Super = QTreeWidget;
        static NavigationElementChildFctPtr navigationElementChildFcts[];

        void navigateMainPanel(const PanelType);
        template<PanelType>
        void navigationElementChildFct(NavigationElement* const);

        inline void callNavigationElementChildFct(PanelType const type, NavigationElement* const element){
            (this->*navigationElementChildFcts[panelType2number(type)])(element);
        }

        inline int indexOf(NavigationElement* child)const{
            return invisibleRootItem()->indexOfChild(child);
        }
        inline NavigationElement *itemAt(const QPoint &p) const{
            return static_cast<NavigationElement*>(QTreeWidget::itemAt(p));
        }
    public:
        List(View::ConfigViewPanel& parent);
        virtual ~List() override{}

        View::ConfigViewPanel& configPanel;


        WIDGET_ADD_EVENT_FILTER

    };

};


#endif // NAVIGATIONLIST_HPP
