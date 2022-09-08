#ifndef CONFIGVIEWPANEL_HPP
#define CONFIGVIEWPANEL_HPP

#include<QSplitter>
#include"Tcl2CaplPanels/ConfigEditor/NavigationList/navigationList.hpp"
#include"Tcl2CaplPanels/ConfigEditor/ConfigTabsPanel/configTabsPanel.hpp"
#include"Tcl2CaplPanels/ConfigEditor/loadconfigsettings.hpp"
#include"Tcl2Capl/controllerconfiginfo.hpp"

namespace Panels::Configuration{
    using namespace Navigation;

    class Panel;
    namespace View{
        class ConfigViewPanelSplitter : public QSplitter{
            using Splitter = ConfigViewPanelSplitter;
            class Handle : public QSplitterHandle{
                public:
                static constexpr int NonTextModeHandelWidth = 10;
                private:
                static constexpr int TextModeHandelWidth = 20;
                static constexpr int NonTextModeHandelMargin = 1;
                static constexpr int NonTextModeHandelTriangleWidth = NonTextModeHandelWidth - 2*NonTextModeHandelMargin;
                public:
                inline bool isNotActive(){if(splitter()) return splitter()->isNotActive(); return false;}
                bool textMode = false;
                Handle(Qt::Orientation orientation, Splitter *parent) : QSplitterHandle(orientation, parent){
                    if(splitter())
                        splitter()->setHandleWidth(NonTextModeHandelWidth);
                }
                protected:
                inline Splitter* splitter()const{return static_cast<Splitter*>(QSplitterHandle::splitter());}
                void paintEvent(QPaintEvent *event)override;
                void enterEvent(QEnterEvent *event)override;
                void leaveEvent(QEvent *event)override;

            };
        public:
            inline bool isNotActive()const{return sizes().at(0) == 0;}
            static constexpr int NonTextModeHandelWidth = Handle::NonTextModeHandelWidth;
            using QSplitter::moveSplitter;
            using QSplitter::closestLegalPosition;
            QSplitterHandle *createHandle() override{
                return new Handle(orientation(), this);
            }

        };

        enum class ViewPanelType{
            Navigation,
            ConfigTabs,
            EndOfPanels
        };

        enum class EventType{
            Notify,
            EndOfTypes
        };


        template<typename TypeName, ViewPanelType ViewId>
        struct ViewDef{
            static constexpr ViewPanelType Id = ViewId;
            using Type = TypeName;
        };

        template<typename ...Defs>
        struct ViewPanel{
            void get(){}
        };

        template <typename Def, typename ...Rest>
        struct ViewPanel<Def, Rest...> : public ViewPanel<Rest...>{
        private:
            typename Def::Type panel;
         public:
            template<class Arg>
            inline ViewPanel(Arg& arg) : ViewPanel<Rest...>(arg), panel(arg){}

            using Super = ViewPanel<Rest...>;
            using Super::get;
            typename Def::Type& get() {
                return panel;
            }
        };
        template<class...ViewPanels>
        struct Panels : public ViewPanel<ViewPanels...>{
        public:
            using Super = ViewPanel<ViewPanels...>;
            using Super::get;
            template<class Arg>
            inline Panels(Arg& arg) : Super(arg){}
        };

        using NavigationList = Navigation::List;
        using NavigationPanel = ViewDef<NavigationList, ViewPanelType::Navigation>;
        using ConfigTabs = ConfigTabsPanel;
        using ConfigTabsPanel = ViewDef<ConfigTabs, ViewPanelType::ConfigTabs>;


        class ConfigViewPanel : public ConfigViewPanelSplitter, public Panels<NavigationPanel, ConfigTabsPanel>{
            using Super = ConfigViewPanelSplitter;
            using ConfigEditor = Configuration::Panel;
        public:
            using Panels = Panels<NavigationPanel, ConfigTabsPanel>;
            using Panels::get;
            ConfigViewPanel(ConfigEditor& parent);
            virtual ~ConfigViewPanel() override{}

            ConfigEditor& configEditor;

            inline void navigationListAppeared(){
                ConfigTabs& configTabsPanel = this->Panels::Super::ViewPanel::Super::get();
                update();
                setHandleWidth(5);
                configTabsPanel.tabBar()->hide();
            }
            inline void navigationListDisappeared(){
                ConfigTabs& configTabsPanel = this->Panels::Super::ViewPanel::Super::get();
                update();
                configTabsPanel.tabBar()->show();

            }
            void loadConfigData(QStringList& writeOnlyProcedures, ControllerConfigInfo::ProceduresView& view);
        };
    }



}

#endif // CONFIGVIEWPANEL_HPP
