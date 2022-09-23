
#ifndef MODIFIER_RULE_VIEW_TEXTRULE_HPP
#define MODIFIER_RULE_VIEW_TEXTRULE_HPP

#include"../formattedString.hpp"
#include<QTextEdit>

namespace Panels::Configuration::View::ModifierFormattedString{
    class TextItemDataView
            : public ItemDataView
    {
        static constexpr FormatRuleType ruleType = FormatRuleType::TextItem;
    public:
        TextItemDataView(QWidget* parent)
            : ItemDataView(parent)
        {
            addRow("Tekst: ", &textLineEdit);
        }
        TextItemDataView(QWidget* parent, FormatRuleRef);
        ~TextItemDataView(){}

        class TextEdit : public QTextEdit{
        public:
            TextEdit(){
                setStyleSheet("border: 1px solid red;");
                setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
            }
            inline void setText(const QString& str){
                QTextEdit::setText(str);
                checkText();
            }
            void checkText(){
                if(toPlainText().isEmpty()){
                    setStyleSheet("border: 1px solid red;");
                }else{
                    setStyleSheet("");
                }
            }

            bool event(QEvent* ev)override{
                if(ev->type() == QEvent::InputMethodQuery){
                    checkText();
                }
                return QTextEdit::event(ev);
            }
        };

        using ItemDataView::FormatRuleRef;
        static ItemDataView* create(QWidget* parent, FormatRuleRef = nullptr);
        constexpr FormatRuleType type()const override{return ruleType;}
        using Rule = ModifierFormatParametersFactory::Product<ruleType>;
    protected:
        //LineEdit* textLineEdit = nullptr;
        TextEdit textLineEdit;// = nullptr;
        void readRule(FormatRuleBase&) override;

    };
}

#endif // MODIFIER_RULE_VIEW_TEXTITEM_HPP
