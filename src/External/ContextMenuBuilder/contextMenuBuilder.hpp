#ifndef CONTEXTMENUBUILDER_HPP
#define CONTEXTMENUBUILDER_HPP

#include<QMenu>
#include<QAction>
#include<QContextMenuEvent>

namespace Utils::ContextMenuBuilder{


    class Configuration{
    public:
        using Actions = QList<QAction*>;
        using ActionIndex = Actions::size_type;
        Configuration(){
            _menu = new QMenu();
        }
        ~Configuration(){
            delete _menu;
            _menu = nullptr;
        }
    protected:
        QMenu* _menu = nullptr;
        Actions actions;
    public:
        QMenu& menu(){return *_menu;}
        void addMenu(QString menuName, Actions newActions){
            QMenu* menu = new QMenu(menuName);
            menu->addActions(newActions);
            _menu->addMenu(menu);
            actions.append(newActions);
        }
        void addActions(Actions newActions){
            _menu->addActions(newActions);
            actions.append(newActions);
        }
        inline ActionIndex exec(QContextMenuEvent* cev){
            return actions.indexOf(_menu->exec(cev->globalPos()));
        }
    };

}

#endif // CONTEXTMENUBUILDER_HPP
