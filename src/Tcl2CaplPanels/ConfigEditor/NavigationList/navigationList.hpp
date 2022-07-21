#ifndef NAVIGATIONLIST_HPP
#define NAVIGATIONLIST_HPP

#include<QTreeWidget>
#include"External/Utils/class.hpp"
#include<QItemDelegate>
#include<QApplication>
#include"Tcl2Capl/controllerconfiginfo.hpp"

class ControllerConfigInfo;
namespace  Panels::Configuration{
    class Panel;
    namespace View{
        class ConfigViewPanel;
    }
}

namespace Panels::Configuration::Navigation{

    enum class PanelType{
        AttributesList,
        WriteOnlyProceduresList,
        Procedures,
        DefaultProcedure,
        Size,
        Invalid = Size,
        NoFound = -1
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

    class List : public QTreeWidget{

    public:
        using ConfigEditor = Configuration::Panel;
        class NavigationElement : public QTreeWidgetItem{
        using Super = QTreeWidgetItem;
        public:
            inline NavigationElement(QTreeWidget *parent, const QStringList &strings, int type = Type)
                : QTreeWidgetItem(parent, strings, type)
            {}
            inline NavigationElement(QTreeWidgetItem *parent, const QStringList &strings, int type = Type)
                : QTreeWidgetItem(parent, strings, type)
            {}
            List *treeWidget(){return static_cast<List*>(Super::treeWidget());}
            NavigationElement *parent() const{return static_cast<NavigationElement*>(Super::parent());}
            inline NavigationElement *child(int index)const{return static_cast<NavigationElement*>(Super::child(index));}
        };
        class ItemDelegate : public QItemDelegate{
            ItemDelegate() = delete;
        public:
            explicit ItemDelegate(List& list, QObject* parent = nullptr) : QItemDelegate(parent), list(list){}

        protected:
            List& list;

            QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
            void setEditorData(QWidget* editor, const QModelIndex& index) const;
            void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
            void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        };
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
        inline NavigationElement* itemFromIndex(const QModelIndex& p)const{
            return static_cast<NavigationElement*>(Super::itemFromIndex(p));
        }

        struct CurEditItemInfo{
            NavigationElement* item = nullptr;
            PanelType panelType;
            QString oldStr;
        } curEditItemInfo;

        NavigationElement* activeProcedureCategoryItem = nullptr;
        NavigationElement* activeDefaultProcedureCategoryItem = nullptr;
    public:
        List(View::ConfigViewPanel& parent);
        virtual ~List() override{}

        View::ConfigViewPanel& configPanel_;

        ConfigEditor& configEditor();
        ControllerConfigInfo& config();

        bool edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event)override;
        void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)override;
        void processEditData(CurEditItemInfo&);
        void closePersistentEditor();
        void editItem( NavigationElement* item);

        inline bool isDefaultProcedurePanel(NavigationElement* item)const{
            return item == topLevelItem(panelType2number(PanelType::DefaultProcedure));
        }

        inline bool isNavigationElementActive(NavigationElement* element)const{return activeProcedureCategoryItem == element or
                                                                              activeDefaultProcedureCategoryItem == element;}

        WIDGET_ADD_EVENT_FILTER

        void deactivateProcedureCategory(){
            if(activeProcedureCategoryItem){
                while(activeProcedureCategoryItem->parent() != nullptr){
                    activeProcedureCategoryItem->setIcon(0, QIcon());
                    activeProcedureCategoryItem = activeProcedureCategoryItem->parent();
                }
            }
        }
        void activateProcedureCategory(NavigationElement* element){
            deactivateProcedureCategory();
            activeProcedureCategoryItem = element;
            while(element->parent() != nullptr){
                element->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
                element = element->parent();
            }
        }

        void deactivateDefaultProcedureCategory(){
            if(activeDefaultProcedureCategoryItem){
                while(activeDefaultProcedureCategoryItem->parent() != nullptr){
                    activeDefaultProcedureCategoryItem->setIcon(0, QIcon());
                    activeDefaultProcedureCategoryItem = activeDefaultProcedureCategoryItem->parent();
                }
            }
        }
        void activateDefaultProcedureCategory(NavigationElement* element){
            deactivateDefaultProcedureCategory();
            activeDefaultProcedureCategoryItem = element;
            while(element->parent() != nullptr){
                element->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
                element = element->parent();
            }
        }

        void loadData(ControllerConfigInfo::ProceduresView&);

    };

};


#endif // NAVIGATIONLIST_HPP
