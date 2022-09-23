#ifndef SPINBOXNOWHEEL_HPP
#define SPINBOXNOWHEEL_HPP

#include<QSpinBox>
#include<QWheelEvent>

class SpinBoxNoWheel : public QSpinBox{
public:
    SpinBoxNoWheel(QWidget *parent = nullptr) : QSpinBox(parent){}
protected:
    void wheelEvent(QWheelEvent* e)override{
        e->ignore();
    }
};

#endif // SPINBOXNOWHEEL_HPP
